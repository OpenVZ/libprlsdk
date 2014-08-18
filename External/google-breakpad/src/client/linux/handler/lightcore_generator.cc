//
// Author: Roman Pen
//

#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <link.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/procfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "lightcore_generator.h"
#include "client/linux/log/log.h"
#include "common/linux/linux_libc_support.h"
#include "client/linux/handler/exception_handler.h"
#include "client/linux/minidump_writer/linux_ptrace_dumper.h"

#if __i386__
#define MACHINE EM_386
#define ELF_CLASS ELFCLASS32
#elif __x86_64__
#define MACHINE EM_X86_64
#define ELF_CLASS ELFCLASS64
#else
#error "Unsupported platform!"
#endif

#define ELF_EXEC_PAGESIZE 4096
#define ELF_EXEC_PAGE_MASK (~(ELF_EXEC_PAGESIZE-1))
#define ELF_ENDIAN ELFDATA2LSB; //Little endian

#define roundup(x, y) (((x) + (y) - 1) & ~((y) - 1))

namespace
{
enum
{
	MAX_PATH = 257
};
using namespace google_breakpad;

namespace Lightcore
{
///////////////////////////////////////////////////////////////////////////////
// struct Log

struct Log
{
	static void greet(pid_t pid_, const ExceptionHandler::CrashContext& context_)
	{
		put("parallels_crash_handler: [pid=");
		number(pid_);
		put(", tid=");
		number(context_.tid);
		put("] crash is catched (signal ");
		number(context_.siginfo.si_signo);
		put(") at ip ");
		address(context_.siginfo.si_addr);
		put("\n");
	}
	template<size_t N>
	static bool ropen(const char (&name_)[N])
	{
		error();
		put("can't open '");
		put(name_);
		put("'\n");
		return true;
	}
	static bool wopen(const char *name_)
	{
		error();
		put("can't open '");
		logger::write(name_, my_strlen(name_));
		put("' for writing!\n");
		return true;
	}
	template<size_t N>
	static bool read(const char (&name_)[N])
	{
		error();
		put("read of from '");
		put(name_);
		put("'\n");
		return true;
	}
	static bool write(size_t cb_, int errno_)
	{
		error();
		put("write failed. res ");
		number(cb_);
		put(", errno ");
		number(errno_);
		put("\n");
		return true;
	}
	static bool stack(uintptr_t address_)
	{
		error();
		put("stack bottom is not found for SP=");
		address(address_);
		put("\n");
		return true;
	}
	static bool threads(unsigned threads_, unsigned stacks_)
	{
		warning();
		put("threads number is ");
		number(threads_);
		put(" but stacks number is ");
		number(stacks_);
		put("!\n");
		return true;
	}
	template<size_t N>
	static bool say(const char (&text_)[N])
	{
		error();
		put<N>(text_);
		put("\n");
		return true;
	}
private:
	template<size_t N>
	static void put(const char (&text_)[N])
	{
		logger::write(text_, N - 1);
	}
	static void prefix()
	{
		put("(lightcore_generator) ");
	}
	static void error()
	{
		prefix();
		put("Error: ");
	}
	static void warning()
	{
		prefix();
		put("Warning: ");
	}
	static void number(unsigned value_)
	{
		char b[32] = {};
		unsigned cb = my_uint_len(value_);
		my_uitos(b, value_, cb);
		logger::write(b, cb);
	}
	static void address(const void* value_)
	{
		address(reinterpret_cast<uintptr_t>(value_));
	}
	static void address(uintptr_t value_)
	{
		union {uintptr_t p; unsigned i; } x;
		x.p = value_;
		number(x.i);
	}
};

///////////////////////////////////////////////////////////////////////////////
// struct File

struct File
{
	explicit File(int fd_);
	~File();

	bool pad(size_t cb_);
	bool write(uintptr_t data_, size_t cb_)
	{
		return this->write(reinterpret_cast<const void* >(data_), cb_);
	}
	bool write(const void* data_, size_t cb_);
private:
	int m_fd;
};

File::File(int fd_): m_fd(fd_)
{
}

File::~File()
{
	sys_close(m_fd);
}

bool File::write(const void* data_, size_t cb_)
{
	if (0 == cb_)
		return false;

	ssize_t x = sys_write(m_fd, data_, cb_);
	if (ssize_t(cb_) == x)
		return false;

	return Log::write(cb_, errno);
}

bool File::pad(size_t cb_)
{
	if (0 == cb_)
		return false;

	off_t o = sys_lseek(m_fd, cb_, SEEK_CUR);
	if (-1 < o)
		return false;

	return Log::write(cb_, errno);
}

namespace Note
{
///////////////////////////////////////////////////////////////////////////////
// struct Unit

struct Unit
{
	Unit(int type_, const void* data_, size_t cb_);

	size_t size() const;
	bool operator()(File& dst_) const;
private:
	static const char s_NAME[];

	int m_type;
	size_t m_size;
	const void* m_data;
};
const char Unit::s_NAME[] = "CORE";

Unit::Unit(int type_, const void* data_, size_t cb_):
	m_type(type_), m_size(cb_), m_data(data_)
{
}

size_t Unit::size() const
{
	return sizeof(ElfW(Nhdr)) + roundup(sizeof(s_NAME), 4) + roundup(m_size, 4);
}

bool Unit::operator()(File& dst_) const
{
	ElfW(Nhdr) en;
	en.n_namesz = sizeof(s_NAME);
	en.n_descsz = m_size;
	en.n_type = m_type;
	if (dst_.write(&en, sizeof(en)))
		return true;
	if (dst_.write(s_NAME, en.n_namesz))
		return true;

	size_t r = sizeof(en) + en.n_namesz;
	size_t u = roundup(r, 4);
	if (dst_.pad(u - r))
		return true;
	r = u;
	if (dst_.write(m_data, m_size))
		return true;

	u = roundup(r += m_size, 4);
	return dst_.pad(u - r);
}

///////////////////////////////////////////////////////////////////////////////
// struct Command

struct Command
{
	Command(): m_note(0, 0, 1)
	{
	}

	const Unit& note() const
	{
		return m_note;
	}
protected:
	Unit m_note;
};

} // namespace Note

///////////////////////////////////////////////////////////////////////////////
// struct Simple

template<typename T>
struct Simple: private T
{
	typedef Simple<T> impl_type;

	Simple()
	{
		my_memset(this, 0, size());
	}

	size_t size() const
	{
		return sizeof(T);
	}
	bool operator()(File& dst_) const
	{
		return dst_.write(this, size());
	}
protected:
	T& data()
	{
		return *this;
	}
};

///////////////////////////////////////////////////////////////////////////////
// struct Memory

struct Memory
{
	Memory(const void* address_, size_t size_, const void* data_);
	Memory(const void* address_, size_t size_, LinuxPtraceDumper& dumper_);
	Memory(uintptr_t address_, size_t size_, LinuxPtraceDumper& dumper_);

	size_t size() const
	{
		return m_size;
	}
	uintptr_t address() const
	{
		return m_address;
	}
	void thread(pid_t thread_)
	{
		m_thread = thread_;
	}
	bool operator()(File& dst_) const;
private:
	size_t m_size;
	pid_t m_thread;
	uintptr_t m_address;
	const void* m_data;
	LinuxPtraceDumper* m_dumper;
};

Memory::Memory(const void* address_, size_t size_, const void* data_):
	m_size(size_), m_thread(-1), m_address(reinterpret_cast<uintptr_t>(address_)),
	m_data(data_), m_dumper(NULL)
{
}

Memory::Memory(const void* address_, size_t size_, LinuxPtraceDumper& dumper_):
	m_size(size_), m_thread(dumper_.crash_thread()),
	m_address(reinterpret_cast<uintptr_t>(address_)), m_data(NULL),
	m_dumper(&dumper_)
{
}
Memory::Memory(uintptr_t address_, size_t size_, LinuxPtraceDumper& dumper_):
	m_size(size_), m_thread(dumper_.crash_thread()), m_address(address_),
	m_data(NULL), m_dumper(&dumper_)
{
}

bool Memory::operator()(File& dst_) const
{
	if (NULL == m_dumper)
	{
		// NB. data is already in the local address space.
		// there is no need to use dumper to copy it.
		return dst_.write(m_data, m_size);
	}
	enum
	{
		SIZE = 4096
	};
	void *b = m_dumper->allocator()->Alloc(SIZE);
	size_t z = size();
	const char* p = reinterpret_cast<const char* >(m_address);
	while (0 < z)
	{
		loff_t d = (std::min<loff_t>)(z, SIZE);
		m_dumper->CopyFromProcess(b, m_thread, p, d);
		if (dst_.write(b, d))
			return Log::say("write of mem failed!");

		p += d;
		z -= d;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// struct Mappings

struct Mappings
{
	typedef wasteful_vector<Memory> list_type;

	explicit Mappings(LinuxPtraceDumper& dumper_);

	const list_type& data() const
	{
		return m_data;
	}
	bool operator()();
private:
	bool rest();
	pid_t tid() const
	{
		return m_dumper->crash_thread();
	}
	ElfW(Dyn)* dynamic();

	list_type m_data;
	LinuxPtraceDumper* m_dumper;
};

Mappings::Mappings(LinuxPtraceDumper& dumper_):
	m_data(dumper_.allocator()), m_dumper(&dumper_)
{
}

ElfW(Dyn)* Mappings::dynamic()
{
        ElfW(Phdr)* h = reinterpret_cast<ElfW(Phdr)*>(m_dumper->auxv()[AT_PHDR]);
        int n = m_dumper->auxv()[AT_PHNUM];
        if (0 == n || h == NULL)
                return NULL;

        // Assume the program base is at the beginning of the same page as the
        // PHDR
        char* b = reinterpret_cast<char *>(reinterpret_cast<uintptr_t>(h) & ~0xfff);
        // Search for the program PT_DYNAMIC segment
	ElfW(Phdr) m;
	my_memset(&m, 0, sizeof(m));
	for (; n >= 0; --n, ++h)
	{
		ElfW(Phdr) z;
		m_dumper->CopyFromProcess(&z, tid(), h, sizeof(z));
		// Adjust base address with the virtual address of the PT_LOAD
		// segment corresponding to offset 0
		if (z.p_type == PT_LOAD && z.p_offset == 0)
			b -= z.p_vaddr;
		if (z.p_type == PT_DYNAMIC)
			m = z;
	}
	if (0 == m.p_vaddr)
		return NULL;

	// Mmap full .dynamic section
	m_data.push_back(Memory(b + m.p_vaddr, m.p_memsz, *m_dumper));
	return reinterpret_cast<ElfW(Dyn)* >(m_data.back().address());
}

bool Mappings::rest()
{
	r_debug y;
	char n[MAX_PATH] = {};
	m_dumper->CopyFromProcess(&y, tid(), (r_debug* )m_data.back().address(), sizeof(y));
	for (link_map* p = y.r_map; p != NULL; )
	{
		struct link_map m;
		m_data.push_back(Memory(p, sizeof(link_map), *m_dumper));
		m_dumper->CopyFromProcess(&m, tid(), p, sizeof(m));
		p = m.l_next;
		if (NULL == m.l_name)
			continue;

                // Extract this shared object's name.
		m_dumper->CopyFromProcess(n, tid(), m.l_name, sizeof(n) - 1);
		if (NULL != ::strstr(n, "/libpthread.so"))
			n[4 + (my_strrchr(n, '/') - n)] = 'a';

		size_t z = my_strlen(n) + 1;
		void* d = m_dumper->allocator()->Alloc(z);
		my_memcpy(d, n, z);
		m_data.push_back(Memory(m.l_name, z, d));
	}
	return false;
}

bool Mappings::operator()()
{
	ElfW(Dyn)* x = dynamic();
	if (NULL == x)
		return true;

	r_debug* d = NULL;
	for (int i = 0;; ++i)
	{
		ElfW(Dyn) e;
		m_dumper->CopyFromProcess(&e, tid(), x + i, sizeof(e));
		if (e.d_tag == DT_DEBUG)
			d = reinterpret_cast<struct r_debug*>(e.d_un.d_ptr);
		else if (e.d_tag == DT_NULL)
			break;
	}
	if (NULL == d)
		return false;

	// Mmap dyntag_ptr
	m_data.push_back(Memory(d, sizeof(r_debug), *m_dumper));
	return rest();
}

namespace Thread
{
///////////////////////////////////////////////////////////////////////////////
// struct Unit

struct Unit: Note::Command
{
	explicit Unit(int index_);
	Unit(int index_, const ucontext& context_);

	const Memory& stack() const
	{
		return m_stack;
	}
	bool operator()(LinuxPtraceDumper& dumper_);
private:
	uintptr_t registers();

	int m_index;
	Memory m_stack;
        elf_prstatus m_data;
	const ucontext* m_context;
};

Unit::Unit(int index_):
	m_index(index_), m_stack(0, 0, 0), m_context(NULL)
{
        my_memset(&m_data, 0, sizeof(m_data));
}

Unit::Unit(int index_, const ucontext& context_):
	m_index(index_), m_stack(0, 0, 0), m_context(&context_)
{
        my_memset(&m_data, 0, sizeof(m_data));
}

bool Unit::operator()(LinuxPtraceDumper& dumper_)
{
	ThreadInfo t;
	if (!dumper_.GetThreadInfoByIndex(m_index, &t))
		return true;

	uintptr_t s = t.stack_pointer;
        // NT_PRSTATUS is the one special case, because the regset data
        // goes into the pr_reg field inside the note contents, rather
        // than being the whole note contents.  We fill the reset in here.
        // We assume that regset 0 is NT_PRSTATUS.
        m_data.pr_info.si_signo = m_data.pr_cursig = dumper_.crash_signal();
	m_data.pr_pid = dumper_.threads()[m_index];
        m_data.pr_ppid = t.ppid;
        m_data.pr_pgrp = t.tgid;
        my_memcpy(&m_data.pr_reg, &t.regs, sizeof(t.regs));
	if (NULL != m_context)
		s = registers();

	const MappingInfo* m = dumper_.FindMapping(reinterpret_cast<void* >(s));
	if (NULL == m)
		return Log::stack(s);

        // New size should be also aligned to page size
	s = (std::max)(m->start_addr, s & ELF_EXEC_PAGE_MASK);
	size_t z = m->size - (s - m->start_addr);
        // Stack not greater than 16k
	m_stack = Memory(s, z, dumper_);
	m_stack.thread(dumper_.threads()[m_index]);
//	m_stack = Memory(s, (std::min<loff_t>)(16 << 10, z), dumper_);

        m_note = Note::Unit(NT_PRSTATUS, &m_data, sizeof(m_data));
	return false;
}

#if defined(__x86_64)
uintptr_t Unit::registers()
{
	user_regs_struct *x = reinterpret_cast<user_regs_struct* >(&m_data.pr_reg);
	x->r15 = m_context->uc_mcontext.gregs[REG_R15];
	x->r14 = m_context->uc_mcontext.gregs[REG_R14];
	x->r13 = m_context->uc_mcontext.gregs[REG_R13];
	x->r12 = m_context->uc_mcontext.gregs[REG_R12];
	x->rbp = m_context->uc_mcontext.gregs[REG_RBP];
	x->rbx = m_context->uc_mcontext.gregs[REG_RBX];
	x->r11 = m_context->uc_mcontext.gregs[REG_R11];
	x->r10 = m_context->uc_mcontext.gregs[REG_R10];
	x->r9 = m_context->uc_mcontext.gregs[REG_R9];
	x->r8 = m_context->uc_mcontext.gregs[REG_R8];
	x->rax = m_context->uc_mcontext.gregs[REG_RAX];
	x->rcx = m_context->uc_mcontext.gregs[REG_RCX];
	x->rdx = m_context->uc_mcontext.gregs[REG_RDX];
	x->rsi = m_context->uc_mcontext.gregs[REG_RSI];
	x->rdi = m_context->uc_mcontext.gregs[REG_RDI];
	x->orig_rax  = 0;
	x->rip = m_context->uc_mcontext.gregs[REG_RIP];
	x->cs = m_context->uc_mcontext.gregs[REG_CSGSFS] & 0xffff;
	x->eflags = m_context->uc_mcontext.gregs[REG_EFL];
	x->rsp = m_context->uc_mcontext.gregs[REG_RSP];
	x->ss = 0;
	x->fs_base = 0;
	x->gs_base = 0;
	x->ds = 0;
	x->es = 0;
	x->fs = (m_context->uc_mcontext.gregs[REG_CSGSFS] >> 32) & 0xffff;
	x->gs = (m_context->uc_mcontext.gregs[REG_CSGSFS] >> 16) & 0xffff;

	return x->rsp;
}

#elif defined(__i386)
uintptr_t Unit::registers()
{
	user_regs_struct *x = reinterpret_cast<user_regs_struct* >(&m_data.pr_reg);
	x->ebx = m_context->uc_mcontext.gregs[REG_EBX];
	x->ecx = m_context->uc_mcontext.gregs[REG_ECX];
	x->edx = m_context->uc_mcontext.gregs[REG_EDX];
	x->esi = m_context->uc_mcontext.gregs[REG_ESI];
	x->edi = m_context->uc_mcontext.gregs[REG_EDI];
	x->ebp = m_context->uc_mcontext.gregs[REG_EBP];
	x->eax = m_context->uc_mcontext.gregs[REG_EAX];
	x->xds = m_context->uc_mcontext.gregs[REG_DS];
	x->xes = m_context->uc_mcontext.gregs[REG_ES];
	x->xfs = m_context->uc_mcontext.gregs[REG_FS];
	x->xgs = m_context->uc_mcontext.gregs[REG_GS];
	x->orig_eax = 0;
	x->eip = m_context->uc_mcontext.gregs[REG_EIP];
	x->xcs = m_context->uc_mcontext.gregs[REG_CS];
	x->eflags = m_context->uc_mcontext.gregs[REG_EFL];
	x->esp = m_context->uc_mcontext.gregs[REG_ESP];
	x->xss = m_context->uc_mcontext.gregs[REG_SS];

	return x->esp;
}

#else
#error "Unsupported platform!"
#endif // __x86_64

///////////////////////////////////////////////////////////////////////////////
// struct List

struct List
{
	typedef wasteful_vector<Unit> list_type;

	explicit List(LinuxPtraceDumper& dumper_);

	const list_type& data() const
	{
		return m_data;
	}
	bool operator()(const ucontext& context_);
private:
	list_type m_data;
	LinuxPtraceDumper* m_dumper;
};

List::List(LinuxPtraceDumper& dumper_):
	m_data(dumper_.allocator(), dumper_.threads().size()), m_dumper(&dumper_)
{
}

bool List::operator()(const ucontext& context_)
{
	// NB. order does matter.
	const unsigned c = m_dumper->threads().size();
	m_data.resize(1, Unit(0));
	for (unsigned i = 0; i < c; ++i)
	{
		Unit* u = NULL;
		if (m_dumper->crash_thread() == m_dumper->threads()[i])
		{
			m_data[0] = Unit(i, context_);
			u = &m_data[0];
		}
		else
		{
			m_data.push_back(Unit(i));
			u = &m_data.back();
		}
		if ((*u)(*m_dumper))
			return Log::threads(c, m_data.size());
	}
	return false;
}

} // namespace Thread

///////////////////////////////////////////////////////////////////////////////
// struct Cmd

struct Cmd: Note::Command
{
	explicit Cmd(pid_t pid_);

	bool operator()(LinuxPtraceDumper& dumper_);
private:
	pid_t m_pid;
	elf_prpsinfo m_data;
};

Cmd::Cmd(pid_t pid_): m_pid(pid_)
{
        my_memset(&m_data, 0, sizeof(m_data));
}

bool Cmd::operator()(LinuxPtraceDumper& dumper_)
{
	char b[MAX_PATH] = {};
	if (!dumper_.BuildProcPath(b, m_pid, "cmdline"))
		return true;

	int h = sys_open(b, O_RDONLY, 0);
	if (0 > h)
		return Log::ropen(b);

	enum
	{
		SIZE = 4024
	};
	void *d = dumper_.allocator()->Alloc(SIZE);
        my_memset(d, 0, SIZE);
	ssize_t r = sys_read(h, d, SIZE);
	sys_close(h);
	if (0 >= r)
		return Log::read(b);

	size_t cb = (std::min<size_t>)(r, sizeof(m_data.pr_psargs));
	m_data.pr_pid = m_pid;
	my_memcpy(m_data.pr_psargs, d, cb);
	my_memcpy(m_data.pr_fname, d, sizeof(m_data.pr_fname));
	m_note = Note::Unit(NT_PRPSINFO, &m_data, sizeof(m_data));
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// struct Auxv

struct Auxv: Note::Command
{
	Auxv();

	bool operator()(LinuxPtraceDumper& dumper_);
private:
	void *m_data;
};

Auxv::Auxv(): m_data(NULL)
{
}

bool Auxv::operator()(LinuxPtraceDumper& dumper_)
{
	char b[MAX_PATH] = {};
	if (!dumper_.BuildProcPath(b, dumper_.crash_thread(), "auxv"))
		return true;

	int h = sys_open(b, O_RDONLY, 0);
	if (0 > h)
		return Log::ropen(b);

	enum
	{
		SIZE = 4024
	};
	m_data = dumper_.allocator()->Alloc(SIZE);
        my_memset(m_data, 0, SIZE);
	ssize_t r = sys_read(h, m_data, SIZE);
	sys_close(h);
	if (0 >= r)
		return Log::read(b);

	m_note = Note::Unit(NT_AUXV, m_data, r);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// struct Vdso

struct Vdso
{
	Vdso();

	const Memory* data() const
	{
		return m_data;
	}
	bool operator()(LinuxPtraceDumper& dumper_);
private:
	Memory* m_data;
};

Vdso::Vdso(): m_data(NULL)
{
}

bool Vdso::operator()(LinuxPtraceDumper& dumper_)
{
#ifdef __i386__
	uintptr_t p = dumper_.auxv()[AT_SYSINFO];
	if (0 == p)
		return !Log::say("vdso should be found, but AT_SYSINFO is zero!");

	const MappingInfo* x = dumper_.FindMapping(reinterpret_cast<const void*>(p));
	if (NULL == x)
		return !Log::say("vdso is not found!");

	void* a = dumper_.allocator()->Alloc(sizeof(Memory));
	m_data = new(a)Memory(x->start_addr, x->size, dumper_);
#else // __i386__
	(void)dumper_;
#endif // __i386__
        return false;
}

///////////////////////////////////////////////////////////////////////////////
// struct Crash declaration

struct Crash: private File
{
	Crash(pid_t pid_, int fd_, LinuxPtraceDumper& dumper_);

	const Memory* vdso() const
	{
		return m_vdso.data();
	}
	const Note::Unit& cmd() const
	{
		return m_cmd.note();
	}
	const Note::Unit& auxv() const
	{
		return m_auxv.note();
	}
	const Mappings::list_type& mappings() const
	{
		return m_mappings.data();
	}
	const Thread::List::list_type& threads() const
	{
		return m_threads.data();
	}

	bool operator()(const ucontext& context_);
private:
	Cmd m_cmd;
	Auxv m_auxv;
	Vdso m_vdso;
	Mappings m_mappings;
	Thread::List m_threads;
	LinuxPtraceDumper* m_dumper;
};

namespace Header
{
///////////////////////////////////////////////////////////////////////////////
// struct Elf

struct Elf: Simple<ElfW(Ehdr)>
{
	explicit Elf(size_t mappings_);

	bool operator()(File& dst_) const;
};

Elf::Elf(size_t mappings_)
{
	my_memcpy(data().e_ident, ELFMAG, SELFMAG);
	data().e_ident[EI_CLASS] = ELF_CLASS;
	data().e_ident[EI_DATA] = ELF_ENDIAN;
	data().e_ident[EI_VERSION] = EV_CURRENT;
	data().e_ident[EI_OSABI] = ELFOSABI_LINUX;
	data().e_type = ET_CORE;
	data().e_machine = MACHINE;
	data().e_version = EV_CURRENT;
	data().e_phoff = size();
	data().e_flags = 0; // ???
	data().e_ehsize = size();
	data().e_phentsize = sizeof(ElfW(Phdr));
	data().e_phnum = mappings_;
}

bool Elf::operator()(File& dst_) const
{
	if (!impl_type::operator()(dst_))
		return false;

	return Log::say("core write failed!");
}

///////////////////////////////////////////////////////////////////////////////
// struct Notes

struct Notes: Simple<ElfW(Phdr)>
{
	explicit Notes(size_t size_);

	Notes& offset(loff_t offset_)
	{
		data().p_offset = offset_;
		return *this;
	}
	bool operator()(File& dst_) const;
};

Notes::Notes(size_t size_)
{
	data().p_type = PT_NOTE;
	data().p_filesz = size_;
}

bool Notes::operator()(File& dst_) const
{
	if (!impl_type::operator()(dst_))
		return false;

	return Log::say("core write failed!");
}

///////////////////////////////////////////////////////////////////////////////
// struct Program

struct Program: Simple<ElfW(Phdr)>
{
	explicit Program(const Memory& data_);
	Program(uintptr_t address_, size_t size_);

	Program& offset(loff_t offset_)
	{
		data().p_offset = offset_;
		return *this;
	}
private:
	void init(uintptr_t address_, size_t size_);
};

Program::Program(const Memory& data_)
{
	init(data_.address(), data_.size());
}

Program::Program(uintptr_t address_, size_t size_)
{
	init(address_, size_);
}

void Program::init(uintptr_t address_, size_t size_)
{
	data().p_type = PT_LOAD;
	data().p_vaddr = (ElfW(Addr))address_;
	data().p_paddr = 0;
	data().p_filesz = size_;
	data().p_memsz = size_;
	data().p_flags = PF_W | PF_R | PF_X;
	data().p_align = ELF_EXEC_PAGESIZE;
}

///////////////////////////////////////////////////////////////////////////////
// struct Core

struct Core
{
	explicit Core(const Crash& crash_);

	bool operator()(File& dst_) const;
private:
	const Crash* m_crash;
	loff_t m_notes;
	loff_t m_headers;
};

Core::Core(const Crash& crash_): m_crash(&crash_), m_notes(0), m_headers(0)
{
	m_notes = crash_.auxv().size() + crash_.cmd().size();
	for (unsigned i = 0, c = crash_.threads().size(); i < c; ++i)
	{
		m_notes += crash_.threads()[i].note().size();
		m_headers += Program(crash_.threads()[i].stack()).size();
	}
	for (unsigned i = 0, c = crash_.mappings().size(); i < c; ++i)
		m_headers += Program(crash_.mappings()[i]).size();
}

bool Core::operator()(File& dst_) const
{
	const Memory* v = m_crash->vdso();
	Elf e(1 + m_crash->threads().size() + m_crash->mappings().size() + (NULL != v));
	if (e(dst_))
		return true;

	// write program headers.
	Notes n(m_notes);
	loff_t o = e.size() + n.size() + m_headers;
	if (n.offset(o)(dst_))
		return true;

	loff_t b = m_notes + o;
	o = roundup(b, ELF_EXEC_PAGESIZE);
	for (unsigned i = 0, c = m_crash->mappings().size(); i < c; ++i)
	{
		const Memory& m = m_crash->mappings()[i];
		Program h(m);
		if (h.offset(o)(dst_))
			return true;
		o += m.size();
	}
	for (unsigned i = 0, c = m_crash->threads().size(); i < c; ++i)
	{
		const Memory& s = m_crash->threads()[i].stack();
		Program h(s);
		if (h.offset(o)(dst_))
			return true;
		o += s.size();
	}
	if (NULL != v)
	{
		Program h(*v);
		if (h.offset(o)(dst_))
			return true;
	}
	// write thread notes.
	if (m_crash->threads()[0].note()(dst_))
		return true;
	if (m_crash->cmd()(dst_) || m_crash->auxv()(dst_))
		return true;
	for (unsigned i = 1, c = m_crash->threads().size(); i < c; ++i)
	{
		if (m_crash->threads()[i].note()(dst_))
			return true;
	}
	return dst_.pad(roundup(b, ELF_EXEC_PAGESIZE) - b);
}

} // namespace Header

///////////////////////////////////////////////////////////////////////////////
// struct Crash definition

Crash::Crash(pid_t pid_, int fd_, LinuxPtraceDumper& dumper_): File(fd_),
	m_cmd(pid_), m_mappings(dumper_), m_threads(dumper_),
	m_dumper(&dumper_)
{
}

bool Crash::operator()(const ucontext& context_)
{
	m_threads(context_);
	if (m_mappings())
		return !Log::say("module iteration failed!");

	if (m_cmd(*m_dumper) || m_auxv(*m_dumper) || m_vdso(*m_dumper))
		return false;

	if (Header::Core(*this)(*this))
		return false;

	for (unsigned i = 0, c = mappings().size(); i < c; ++i)
	{
		if (mappings()[i](*this))
			return false;
	}
	for (unsigned i = 0, c = threads().size(); i < c; ++i)
	{
		if (threads()[i].stack()(*this))
			return false;
	}
	return NULL == vdso() || !(*vdso())(*this);
}

} // namespace Lightcore
} // namespace

namespace google_breakpad
{
bool WriteLightcore(const char* path_, pid_t pid_, const void* blob_, size_t cb_)
{
	if (cb_ != sizeof(ExceptionHandler::CrashContext))
		return false;

	using namespace Lightcore;
	const ExceptionHandler::CrashContext* context = reinterpret_cast<const ExceptionHandler::CrashContext*>(blob_);
	Log::greet(pid_, *context);
	LinuxPtraceDumper d(pid_);
	d.set_crash_address(reinterpret_cast<uintptr_t>(context->siginfo.si_addr));
	d.set_crash_signal(context->siginfo.si_signo);
	d.set_crash_thread(context->tid);
	if (!d.Init())
		return false;

	int h = sys_open(path_, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (-1 == h)
		return !Log::wopen(path_);

	Crash u(pid_, h, d);
	bool output = d.ThreadsSuspend();
	if (output || d.threads().empty())
		output = u(context->context);
	d.ThreadsResume();
	return output;
}

} // namespace google_breakpad

