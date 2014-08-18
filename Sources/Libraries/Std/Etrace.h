/*
 * Etrace.h: Event tracer facility. Can be used everywhere: in guest
 * (userspace, kernelspace), in monitor, in host (hypervisor,
 * application).
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __ETRACE_H__
#define __ETRACE_H__

// This header uses Parallels types - so we need to include it here
#include "Interfaces/ParallelsTypes.h"

#if !defined(_ETRACE_USE_CPUID_)
#include "Libraries/Std/AtomicOps.h"
#define EtrAtomicInc(x) AtomicIncU(x)
#endif

/*****************************************************************************
 * ETRACE CONFIGURATION
 *****************************************************************************/

/*
 * Define macro below and re-compile the project completely to activate eTrace.
 */
//#define ETRACE_ENABLED

/*
 * Etrace buffer size
 */
#define ETRACE_DEFAULT_BUF_SIZE		(16 * 1024 * 1024)

#define ETRACE_MIN_BUF_SIZE			(1 * 1024 * 1024)
#define ETRACE_MAX_BUF_SIZE			(64 * 1024 * 1024)

/*============================================================================
 * Etrace checkpoints
 *
 * Set all desired events in ETRACE_MASK below to enable tracer
 *---------------------------------------------------------------------------*/

// eTrace checkpoints
enum {
	ETRACE_CP_MON_VMEXIT	= 0,	// vmexit
	ETRACE_CP_MON_VMCALL,			// vmcall type
	ETRACE_CP_MON_PF,				// guest page fault
	ETRACE_CP_MON_MON2GST,			// monitor -> guest
	ETRACE_CP_MON_MON2HYP,			// monitor -> hypervisor (monitor side)
	ETRACE_CP_MON_HYP2MON,			// hypervisor -> monitor (monitor side)
	ETRACE_CP_MON_IDE,				// IDE (monitor-side)
	ETRACE_CP_MON_E1000,			// e1000 (monitor-side)
	ETRACE_CP_MON_IPI,				// monitor IPI's
	ETRACE_CP_MON_APIC,				// APIC access

	ETRACE_CP_HYP_HYP2MON	= 0xA,	// hypervisor -> monitor (hypervisor side)
	ETRACE_CP_HYP_MON2HYP,			// monitor -> hypervisor (hypervisor side)
	ETRACE_CP_HYP_HYP2HLT,			// switch from hypervisor to halt
	ETRACE_CP_HYP_HLT2HYP,			// wake from halt in hypervisor
	ETRACE_CP_HYP_SNDIPI2VCPU,		// send IPI to VCPU
	ETRACE_CP_PRLNET,			// prl_netbridge

	ETRACE_CP_HST_VCPU		= 0x10,	// VCPU events and hypervisor calls
	ETRACE_CP_HST_TOOLS,			// Tools requests
	ETRACE_CP_HST_SNDVCPUSIG,		// SendVcpuSignal()
	ETRACE_CP_HST_SFILTER,			// storage filter request
	ETRACE_CP_HST_IDE,				// IDE (host-side)
	ETRACE_CP_HST_E1000,			// e1000 (host-side)
	ETRACE_CP_HST_API_REQ,			// API request

	ETRACE_CP_HST_HDD_RD_BEG= 0x17,	// disk read begin
	ETRACE_CP_HST_HDD_RD_END,		// disk read end
	ETRACE_CP_HST_HDD_WR_BEG,		// disk write begin
	ETRACE_CP_HST_HDD_WR_END,		// disk write end
	ETRACE_CP_HST_HDD_PCACHE_RD_BEG,// disk pcache read begin
	ETRACE_CP_HST_HDD_PCACHE_RD_END,// disk pcache read end
	ETRACE_CP_HST_HDD_KICK,			// disk "kick"

	ETRACE_CP_GST_TG		= 0x1E,	// tool gate
	ETRACE_CP_GST_SFILTER,			// storage filter checkpoint in guest
	ETRACE_CP_GST_OTG_CMD,			// OTG command
	ETRACE_CP_GST_OTG_GENCMD,		// OTG generic command

	ETRACE_CP_USB_VM,				// USB VM event
	ETRACE_CP_USB_MON,				// USB Monitor event

	ETRACE_CP_NAT,				// prl_napt

	ETRACE_CP_GST_TST,				// used by atomic testsuite
	// USR1-USR4 checkpoints can be used for quick debugging without
	// prl_vm_app/monitor/hypervisor recompilation

	ETRACE_CP_USR1		= 0x26,		// user-defined reason
	ETRACE_CP_USR2,					// user-defined reason
	ETRACE_CP_USR3,					// user-defined reason
	ETRACE_CP_USR4,					// user-defined reason

	ETRACE_CP_PAX_BEGIN,			// PAX BEGIN phase
	ETRACE_CP_PAX_END,				// PAX END phase
	ETRACE_CP_PAX_INSTANT,			// PAX INSTANT phase
	ETRACE_CP_PAX_ASYNC_BEGIN,		// PAX (async) BEGIN phase
	ETRACE_CP_PAX_ASYNC_STEP,		// PAX (async) STEP phase
	ETRACE_CP_PAX_ASYNC_END,		// PAX (async) END phase
	ETRACE_CP_PAX_COUNTER,			// PAX COUNTER phase

	ETRACE_CP_IOSERVICE,			// IOService events

	ETRACE_CP_AIO_WORKER,			// AioWorker events

	/* 51 - 62 - free */

	ETRACE_CP_INTERNAL	= 0x3f,		// internal Etrace checkpoint

	ETRACE_CP_LAST,
	ETRACE_CP_SPECIAL	= 0xff		// reserved for special events
};

#ifdef ETRACE_ENABLED
#define ETRACE_MASK		~0ULL	/* collect all events */
#else
#define ETRACE_MASK		0ULL
#endif

//
// Param packing helpers
//
#define ETRACE_PACK_BITS(d, shift, bits)	(((ULONG64)((d) & ((1ULL<<(bits))-1))) << (shift))
#define ETRACE_UNPACK_BITS(d, shift, bits)	((((ULONG64)(d)) >> (shift)) & ((1ULL<<(bits))-1))

#define ETRACE_PACK_BYTE(d, byte)			ETRACE_PACK_BITS(d, byte << 3,  8)
#define ETRACE_UNPACK_BYTE(d, byte)			ETRACE_UNPACK_BITS(d, byte << 3,  8)

/*****************************************************************************
 * ETRACE CHECKPOING SUBCLASSES
 *****************************************************************************/

enum {
	ETRACE_IPI_WANT_SEND = 0,
	ETRACE_IPI_REAL_SEND,
};

// Tools dispatcher checkpoints types
enum {
	ETRACE_TOOLS_SUBMIT = 0,
	ETRACE_TOOLS_PROCESS,
	ETRACE_TOOLS_DONE,
	ETRACE_TOOLS_COMPLETED,
};

// Toolgate checkpoints types
enum {
	ETRACE_TG_SUBMIT = 0,
	ETRACE_TG_SUBMITTED,
	ETRACE_TG_ISR,
	ETRACE_TG_COMPLETE,
	ETRACE_TG_COMPLETED,
};

// Storage filter checkpoints types
enum {
	ETRACE_SFILTER_DPC_ENTER = 0,
	ETRACE_SFILTER_DPC_EXIT,
	ETRACE_SFILTER_SCAN_SLOT,
	ETRACE_SFILTER_INS_REQ,
	ETRACE_SFILTER_CONNECT,
	ETRACE_SFILTER_INIT,
};

enum {
	ETRACE_IDE_RESET = 0,
	ETRACE_IDE_FLUSH_CACHE,
	ETRACE_IDE_CMD,
	ETRACE_IDE_READ_DMA,
	ETRACE_IDE_READ_PIO,
	ETRACE_IDE_WRITE_DMA,
	ETRACE_IDE_WRITE_PIO,
	ETRACE_IDE_ASYNC,
	ETRACE_IDE_WAIT_BEGIN,
	ETRACE_IDE_WAIT_END,
	ETRACE_IDE_COMPLETE,
	ETRACE_IDE_GEN_IRQ,
};

enum {
	ETRACE_KICK_BEGIN = 0,
	ETRACE_KICK_END,
};

enum {
	ETRACE_E1000_ACTIVATE,
	ETRACE_E1000_RCVPKT,
	ETRACE_E1000_SNDPKT_SINGLE,
	ETRACE_E1000_SNDPKT,
	ETRACE_E1000_SNDPKT_COMPLETE,
	ETRACE_E1000_REG_WR,
	ETRACE_E1000_REG_RD
};

// USB VM Events
enum {
	ETRACE_USB_UVC_ACQUIRE,
	ETRACE_USB_UVC_RELEASE,
	ETRACE_USB_UVC_COPY_START,
	ETRACE_USB_UVC_COPY_STOP,
	ETRACE_USB_MSC_READ_CMD,
	ETRACE_USB_MSC_WRITE_CMD,
	ETRACE_USB_MSC_START_READ,
	ETRACE_USB_MSC_START_WRITE,
	ETRACE_USB_MSC_READ_CALLBACK,
	ETRACE_USB_MSC_WRITE_CALLBACK,
	ETRACE_USB_MSC_READ_CSW,
	ETRACE_USB_MSC_WRITE_CSW
};

// AioWorker events
enum {
	ETRACE_AIO_WORKER_SUBMIT,
	ETRACE_AIO_WORKER_START_IO,
	ETRACE_AIO_WORKER_STOP_IO,
	ETRACE_AIO_WORKER_WAKE_UP
};

// Atomic testsuite events
enum {
	ETRACE_TST_THREAD_STARTED,
	ETRACE_TST_THREAD_STOPPED,
	ETRACE_TST_IO_READ_SYNC_REQ_BEG,
	ETRACE_TST_IO_READ_SYNC_REQ_END,
	ETRACE_TST_IO_WRITE_SYNC_REQ_BEG,
	ETRACE_TST_IO_WRITE_SYNC_REQ_END,
};

// Internal Etrace events
enum {
	ETRACE_INTERNAL_LOG_STARTED = 0x01,
};

enum {
	/* 0..127 reserved for WebRTC events defined in trace_enevt.h */

	/* Touch event processed, scroll event created */
	ETRACE_PAX_EVENT_ScrollStart = 0x80,
	/* Scroll event is sent from Client to Agent */
	ETRACE_PAX_EVENT_ScrollSend,
	/* Scroll event is received by Agent */
	ETRACE_PAX_EVENT_ScrollReceive,
	/* Scroll event is applied to active App on host */
	ETRACE_PAX_EVENT_ScrollApply,
	/* Video capture event */
	ETRACE_PAX_EVENT_VCaptureSW,
	ETRACE_PAX_EVENT_VCaptureCGS,
	ETRACE_PAX_EVENT_VCaptureCGDS,
	ETRACE_PAX_EVENT_VCaptureGDI,
	ETRACE_PAX_EVENT_VCaptureW8,
	ETRACE_PAX_EVENT_VCaptureF,
	/* Video render event */
	ETRACE_PAX_EVENT_VRender,
	ETRACE_PAX_EVENT_VDraw,
	/* Video encode event */
	ETRACE_PAX_EVENT_VEncode,
	ETRACE_PAX_EVENT_VSetPrivate,
	ETRACE_PAX_EVENT_VBurn,
	/* Video decode event */
	ETRACE_PAX_EVENT_VDecode,
	ETRACE_PAX_EVENT_VGetPrivate,
};

enum {
	ETRACE_IOS_EVENT_SRV_LISTEN,
	ETRACE_IOS_EVENT_SRV_ACCEPT,
	ETRACE_IOS_EVENT_SRV_CLOSE,
	ETRACE_IOS_EVENT_CLI_CLOSE,
	ETRACE_IOS_EVENT_SRV_CONNECT,
	ETRACE_IOS_EVENT_SRV_CONNECT_PROXY,
	ETRACE_IOS_EVENT_CLI_CONNECT,
	ETRACE_IOS_EVENT_CLI_CONNECT_PROXY,
	ETRACE_IOS_EVENT_MNG_CONNECT,

	ETRACE_IOS_EVENT_SRV_SET_HS,
	ETRACE_IOS_EVENT_SRV_SET_HS_SSL,
	ETRACE_IOS_EVENT_SRV_SEND_HS,
	ETRACE_IOS_EVENT_SRV_SEND_HS_SSL,
	ETRACE_IOS_EVENT_SRV_SEND_REHS_SSL,
	ETRACE_IOS_EVENT_SRV_SEND_HS_PROXY,
	ETRACE_IOS_EVENT_CLI_SEND_HS,
	ETRACE_IOS_EVENT_CLI_SEND_HS_SSL,
	ETRACE_IOS_EVENT_CLI_SEND_HS_PROXY,
	ETRACE_IOS_EVENT_MNG_SEND_HS,

	ETRACE_IOS_EVENT_MNG_RECV_REQ_CMD,
	ETRACE_IOS_EVENT_MNG_RECV_REQ_DATA,
	ETRACE_IOS_EVENT_MNG_RECV_BRK_REQ_CMD,
	ETRACE_IOS_EVENT_MNG_RECV_BRK_REQ_DATA,
	ETRACE_IOS_EVENT_MNG_SEND_RES,
	ETRACE_IOS_EVENT_MNG_SEND_BRK_RES,
	ETRACE_IOS_EVENT_MNG_SEND_HBEAT,
	ETRACE_IOS_EVENT_MNG_RECV_HBEAT_CHNG,

	ETRACE_IOS_EVENT_RECV_PKG_SSL,
	ETRACE_IOS_EVENT_SEND_PKG,
	ETRACE_IOS_EVENT_SEND_PKG_SSL,
};

// Special events
enum {
	ETRACE_CP_SPECIAL_OVERFLOW = 0xff,
};


/*****************************************************************************
 * END OF ETRACE CHECKPOINT DESCRIPTION
 *****************************************************************************/

#if ETRACE_CP_LAST > 64
#error ETRACE: To many control points defined
#endif

#if (ETRACE_MASK != 0)
#define ETRACE
#else
#undef ETRACE
#endif

#ifdef ETRACE

#define CPUID_FOR_ETRACE	0xFACE0FFF

#include <Interfaces/packed.h>

typedef struct _ETRACE_DESC
{
	UINT64	mask;
	UINT32	curr;
	UINT32	size;
	UINT64	start_tsc;
	UINT64	start_time;
	UINT64	end_time;
	UINT64	pad;
} ETRACE_DESC;

typedef struct _ETRACE_DATA
{
	/*
	 * tst - Type, Source, Time
	 * Byte  7   - Type
	 * Byte  6   - Source (depends on type)
	 * Bytes 5-0 - (Tsc >> 8)
	 */
	UINT64	tst;
	UINT64	data;
} ETRACE_DATA;

#define ETRACE_DATA_MAX	((ETRACE_BUF_SIZE - sizeof(ETRACE_DESC)) / sizeof(ETRACE_DATA))

/**
 * @brief
 *		Shared between monitor and driver
 */
typedef struct _ETRACE_MEM
{
	ETRACE_DESC	desc;
	ETRACE_DATA data[1];
} ETRACE_MEM;

#include <Interfaces/unpacked.h>

#define ETRACE_CYCLES_MASK	0x0000FFFFFFFFFFFFULL

#define ETRACE_UNPACK_TYPE(d)	(((ULONG64)(d)) >> 56)
#define ETRACE_UNPACK_SRC(d)	((((ULONG64)(d)) >> 48) & 0xFF)
#define ETRACE_UNPACK_TIME(d)	((((ULONG64)(d)) & ETRACE_CYCLES_MASK) << 8)

#define ETRACE_PACK_TYPE(d)	(((ULONG64)(d)) << 56)
#define ETRACE_PACK_SRC(d)	(((ULONG64)((d) & 0xFF)) << 48)
#define ETRACE_PACK_TIME(d)	(((ULONG64)((d) >> 8)) & ETRACE_CYCLES_MASK)

/*****************************************************************************
 * ETRACE_LOG() IMPLEMENTATION
 *****************************************************************************/


#define __ETRACE_FILL_DATA(d, type, vcpu, cycles, val) \
	do {				\
		(d).tst = (ETRACE_PACK_TYPE(type)		\
			| ETRACE_PACK_SRC(vcpu)			\
			| ETRACE_PACK_TIME(cycles));		\
		(d).data = (val);				\
	} while (0)

#define __ETRACE_LOG(ptr, vcpu, type, val, cycles)					\
	do {														\
		ETRACE_MEM *__etrace_log_p = (ptr);									\
		UINT32 curr;												\
		curr = EtrAtomicInc(&__etrace_log_p->desc.curr) % __etrace_log_p->desc.size;	\
		__ETRACE_FILL_DATA(__etrace_log_p->data[curr], type, vcpu, cycles, val);	\
	} while (0)
#endif /* ETRACE */

#ifndef ETRACE
/* Used to check arguments validity in ETRACE_LOG
 * x is required to make gcc happy */
static __inline int __dummy_etrace_message__(int x, ...)
{
    (void)x;
    return 0;
}

#define ETRACE_LOG(x,...)										\
	do {														\
		(void)sizeof(__dummy_etrace_message__(0, __VA_ARGS__));	\
	} while (0)

#elif defined(_PRL_NETBRIDGE_)

	// xxx: it makes sense to log cpu-number here
	#define ETRACE_LOG(etrace_buf, src, type, val)					\
	do {															\
		if ((etrace_buf) && (ETRACE_MASK & (1ULL << (type))))		\
			__ETRACE_LOG((etrace_buf), 0, (type),					\
						(val), prl_get_tsc());						\
	} while (0)

#elif defined(_ETRACE_USE_CPUID_)

	/* guest: user-space / kernel-space */

static void etrace_log_cpuid(unsigned int _eax,
		unsigned int _ebx, unsigned int _ecx, unsigned int _edx)
{
#if   defined(_MSC_VER)
	__asm {
		push eax
		push ebx
		push ecx
		push edx

		mov eax, _eax
		mov ebx, _ebx
		mov ecx, _ecx
		mov edx, _edx

		cpuid

		pop edx
		pop ecx
		pop ebx
		pop eax
	}
#elif defined(__GNUC__)
	__asm __volatile__ (
		"cpuid\n\t"
		: : "a" (_eax), "b" (_ebx), "c" (_ecx), "d" (_edx)
	);
#else
	#error Implement me
#endif
}

	#define ETRACE_LOG(src, type, val)										\
	do {																	\
		if (ETRACE_MASK & (1ULL << (type)))									\
			etrace_log_cpuid(CPUID_FOR_ETRACE, (src), (type), (val));		\
	} while (0)

#else

	/* host: user-space (prl_vm_app) */

	#include "Libraries/Logging/Logging.h"

	#define ETRACE_LOG(src, type, val)										\
	do {																	\
		if (ETRACE_MASK & (1ULL << (type)))									\
			if (etrace_log_func)											\
				etrace_log_func((src), (type), (val));						\
	} while (0)

#endif

#endif // __PMM_H__
