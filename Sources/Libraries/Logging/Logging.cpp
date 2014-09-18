/*
 * Logging.cpp: Unified logging library for all user-space components.
 *
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

/*
 * Logging is devided into two main parts - user & developer.
 * Logging interfaces are as the following:
 *
 *  *** LOGGING METHODS ***
 *
 * 0) LOG_MESSAGE      - VISIBLE ONLY FOR DEVELOPER
 * 1) WRITE_TRACE      - VISIBLE FOR THE END USER (!)
 *
 * Signature of the every logging function is the following:
 *     <function> ( level, "format string", ... )
 *
 *  *** LOGGING LEVELS ***
 *
 * Logging levels are:
 *
 *     (0) DBG_FATAL       - most important
 *     (1) DBG_WARNING     - almost most important
 *     (2) DBG_INFO        - almost least important
 *     (3) DBG_DEBUG       - least important
 *     (4) DBG_TRACE       - detailed information about function calls
 *
 * *** CUSTOMIZATION ***
 *
 * Users can force some parameters (there will be more in the future), namely:
 *
 * #define FORCE_LOGGING_ON
 * #define FORCE_LOGGING_LEVEL     DBG_DEBUG
 * #define FORCE_LOGGING_PREFIX    " >>>>>> MY >>>>> "
 *
 * And include library header after that
 *
 * #include "Libraries/Logging/Logging.h"
 *
 * Do not reinvent the wheel - use this module.
 * Fill free to extend it if you feel that you need this.
 */

#include "Logging.h"
#include "Build/Current.ver"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <Interfaces/CommonTypes.h>
#include <Libraries/Std/MemBarrier.h>

#ifdef _WIN_
    // For OutputDebugString method of Win32 API
    #include <windows.h>
	#include "shlobj.h"
	#include "io.h"
	#include "Aclapi.h"
	#include <Sddl.h>
    #include <Libraries/PrlCommonUtils/utf82wstr_imp.h>

    // windows standard library madness
    #define strdate _strdate
    #define strdup  _strdup
    #define strtime _strtime
    #define snprintf _snprintf
	#define vsnprintf _vsnprintf
	#define close	CloseHandle
	#define STDOUT_FILENO	(1)
	#define STDERR_FILENO	(2)
#else
	// file permissions
	#include <errno.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <pwd.h>
	#include <pthread.h>
#endif

#if defined(_LIN_)
	#include <syscall.h>
#endif

/* for rate limit check */
#include "Libraries/Std/PrlTime.h"
#include "Interfaces/ParallelsTypes.h"
#include "Libraries/Std/AtomicOps.h"


#ifdef _WIN_
	#ifdef _DEBUG
	#	define TRACEDBG( msg ) printf( "===trace==: %s, errno = %d\n", msg, GetLastError() )
	#else
	#	define TRACEDBG(...) do {} while(0)
	#endif

static int write(FILE_HANDLE fd, const char *buf, unsigned len)
{
	OVERLAPPED o;
	DWORD written;
	memset(&o, 0, sizeof(o));
	o.Offset = -1; // Write to the end of file
	o.OffsetHigh = -1;
	if (!WriteFile(fd, buf, len, &written, &o))
		return -1;
	return (int)written;
}

static int write(int fd, const char *buf, unsigned len)
{
	return _write(fd, buf, len);
}
#else
static void Sleep(UINT msec)
{
	usleep(msec * 1000);
}
#endif


#if defined (_WIN_)
#define LOG_OS_NAME		"Windows"
#elif defined(_LIN_)
#define LOG_DEFAULT_DIR	"/var/log"
#define LOG_USER_DIR	"/.parallels"
#define LOG_OS_NAME		"Linux"
#else
#error Unsupported OS
#endif

/**
 * Logger variables/members that must be static.
 * 8-byte alignment is required for timestamp field by 64-bit atomic ops on ARM.
 */
struct PRL_ALIGN(8) LoggerData
{
	UINT64 timestamp;
	FILE_HANDLE fd;
	int *users;
	int is_console_enabled;
	int users1, users2;

	char logFilePath[1024];
	char logFileName[1024];
};

static struct LoggerData __logger_data = {
	0,						/* timestamp */
	INVALID_FILE_HANDLE,	/* fd */
	&__logger_data.users1,	/* users */
	1,						/* is_console_enabled */
	0, 0,					/* users1, users2 */
	"",						/* logFilePath */
	""						/* logFileName */
};

int __log_level = DBG_LEVEL;


/**
 * Returns pointer on instance of LoggerData that represents static-like data
 * for this module. Always returns valid pointer on structure.
 */
static LoggerData *g_d()
{
	return &__logger_data;
}


/**
 * The idea of the class is simple - save some state before the logging
 * operation that could be affected by logging, and restore it in the end
 * of it (e.g. GetLastError() should not change in the result call
 * to the LOG_MESSAGE method.
 */
struct LogStateSaver
{
    unsigned int m_LastError;

    LogStateSaver()
    {
		m_LastError =
			#ifdef _WIN_
				::GetLastError()
			#else
				errno
			#endif
			;
    }

    ~LogStateSaver()
    {
		#ifdef _WIN_
			::SetLastError( m_LastError );
		#else
			errno = m_LastError;
		#endif
    }
};


/**
 * Return current Time string identifier
 */
static int GetDateTimeString(char *buf)
{
#ifndef _WIN_
	int pos;
	struct timeval tv;
	gettimeofday(&tv, NULL);

	time_t stamp_time = tv.tv_sec;
	tm t;
	pos = strftime(buf, 128, "%m-%d %H:%M:%S", localtime_r(&stamp_time, &t));
	pos += sprintf(buf + pos, ".%03d ", (int)(tv.tv_usec/1000));
	return pos;
#else
	SYSTEMTIME st;
	::GetLocalTime(&st);
	return sprintf(buf, "%02d-%02d %02d:%02d:%02d.%03d ",
				   st.wMonth, st.wDay,
				   st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif
}


/**
 * This callback will be called for each message to be processed
 */
static LogMsgFunc_t	log_cb = Logger::PutMessage;

/**
 * Install default logger callback. This callback will be called
 * for each message to be processed
 */
LogMsgFunc_t Logger::SetMessageCallback(LogMsgFunc_t __log_cb)
{
	LoggerData *const d = g_d();

	LogMsgFunc_t ret = log_cb;
	log_cb = __log_cb;
	WriteMemoryBarrier();

	int *old_cb_users = d->users;
	if (old_cb_users == NULL)
		return ret;

	/* flip-flop user count. We need to know that there are no
	 * users of OLD callback */
	if (d->users == &d->users1)
		d->users = &d->users2;
	else
		d->users = &d->users1;

	while (AtomicRead(old_cb_users) != 0)
		Sleep(1);

	return ret;
}


/**
 * Check current rate and return true
 * if we should output the message.
 * Method modifies last timestamp of the structure.
 */
int LogCheckModifyRate(struct LogRateLimit *rl)
{
    LogStateSaver saver;

	unsigned curr = (unsigned) (
#ifndef _WIN_
		PrlGetTickCount64() / PrlGetTicksPerSecond()
#else
		/* FIXME: Windows tools can not have dependency from Libraries/Std */
		GetTickCount() / 1000
#endif
		);

	/**
	 * NOTE: not syncing access to m_Last here, simply not to affect
	 * performance (since the main idea of this method to be called
	 * on very frequent paths).
	 *
	 * The only possible side effect - multiple threads will
	 * output (number of threads) messages insted of single one
	 */
	if (rl->last == (unsigned)-1 || (curr - rl->last > rl->rate)) {
		rl->last = curr;
		return 1;
	}

	return 0;
}


/**
 * Perform actual output of the log message.
 */
void log_release(const char *prefix, const char *target,
				 int level, const char* fmt, ...)
{
	LogStateSaver saver;
	Logger logger(NULL, 0, NULL, prefix, target);

	va_list args;

	va_start(args, fmt);
	logger.ParseMessage(level, fmt, args);
	va_end(args);
}

void log_debug(const char *file, int line, const char *function,
			   const char *prefix, const char *target,
			   int level, const char* fmt, ...)
{
	LogStateSaver saver;
	Logger logger(file, line, function, prefix, target);

	va_list args;

	va_start(args, fmt);
	logger.ParseMessage(level, fmt, args);
	va_end(args);
}

#define SPRINTF(ptr, ...)	do {							\
	ptr += sprintf(ptr,__VA_ARGS__);						\
} while (0)

/**
 * Perform actual output of the log message.
 */
int Logger::DefaultParseMessage(int lvl, const char* fmt, va_list args)
{
	LoggerData *const d = g_d();

	// Updating levels according to the groups
	int group = lvl / DBG_GROUP_SIZE;
	int level = lvl % DBG_GROUP_SIZE;

    // Searching for the file name in the string (to prevent full path printing)
    // This means that if the file variable contains '/path/to/file.cpp'
    // we will print only the 'file.cpp' string
    if ( m_File )
    {
        const char * ptr1 = m_File;
        const char * ptr2 = m_File;

        while ( *ptr2 != 0 )
        {
            if ( *ptr2 == '\\' || *ptr2 == '/' )
                ptr1 = ptr2 + 1;

            ptr2++;
        }

        m_File = ptr1;
    }

    // Searching for the function name in the string (to prevent full name printing)
    // This means that if the func variable contains 'namespace::class::func'
    // we will print only the 'func' string
    if ( m_Func )
    {
        const char * ptr1 = m_Func;
        const char * ptr2 = m_Func;

        while ( *ptr2 != 0 )
        {
            if ( *ptr2 == ':' )
                ptr1 = ptr2 + 1;

            ptr2++;
        }

        m_Func = ptr1;
    }

	char message[4096];
	char *ptr = message;

	ptr += GetDateTimeString(ptr);

    if (group == DBG_GROUP_QT )
		SPRINTF(ptr, "QT: ");

    // Choosing the message prefix
    switch ( level )
    {
        case DBG_FATAL:
			SPRINTF(ptr, "F ");
            break;

        case DBG_WARNING:
			SPRINTF(ptr, "W ");
            break;

        case DBG_INFO:
			SPRINTF(ptr, "I ");
            break;

        case DBG_DEBUG:
			SPRINTF(ptr, "D ");
            break;

		case DBG_TRACE:
			SPRINTF(ptr, "T ");
            break;

        default:
			SPRINTF(ptr, "O(%u) ", level);
            break;
    }

	if (*m_ForcePrefix)
		SPRINTF(ptr, "%s ", m_ForcePrefix);
#ifdef _WIN_
	SPRINTF(ptr, "/%s:%u:%x/ ", m_Target,
			GetCurrentProcessId(), GetCurrentThreadId());
#else
	SPRINTF(ptr, "/%s:%u:%u/ ", m_Target, getpid(), (pid_t)syscall(__NR_gettid));
#endif

	// Adding file/line info (if needed)
	if ( group != DBG_GROUP_QT && m_File && m_Func )
		SPRINTF(ptr, "{%s @ %s:%i} ", m_Func, m_File, m_Line);

	vsnprintf(ptr, ((ULONG_PTR)message - (ULONG_PTR)ptr + sizeof(message)),
			  fmt, args);
	// Truncate the string to fit buffer even with the trailing "\n"
	message[sizeof(message)-2] = 0;
	strcat(ptr, "\n");

	LogMsgFunc_t old_cb;
	do {
		int *users = d->users;
		AtomicInc(users);
		old_cb = log_cb;
		log_cb(message, (int)strlen(message));
		AtomicDec(users);
	} while (old_cb != log_cb);
#if !defined(EXTERNALLY_AVAILABLE_BUILD) && defined(_WIN_)
	// On windows host duplicating log to the debug output
	OutputDebugStringA(message);
#endif

    return 0;
}

int Logger::ParseMessage(int lvl, const char* fmt, va_list args)
{
	return DefaultParseMessage(lvl, fmt, args);
}
#undef SPRINTF

#ifdef _WIN_
static void log_set_permissions(HANDLE hFile)
{
#define GOTO_FINALIZE( msg ) do {	\
	TRACEDBG( msg );				\
	goto finalize;					\
} while (0)

	PSECURITY_DESCRIPTOR pSd = NULL;
	PACL pDacl = NULL;
	PACL pNewDacl = NULL;
	PSID pSidIntercative = NULL;
	DWORD err;

	err = GetSecurityInfo(hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
						  NULL, NULL, &pDacl, NULL, &pSd);
	if (err != ERROR_SUCCESS)
		GOTO_FINALIZE("GetSecurityInfo failed");

	if (!pDacl)
		GOTO_FINALIZE("GetSecurityInfo return pDacl == NULL");

	// SECURITY_INTERACTIVE_RID
	if (!ConvertStringSidToSid( SDDL_INTERACTIVE, &pSidIntercative))
		GOTO_FINALIZE("ConvertStringSidToSid failed");

	// prepare new ACE
	EXPLICIT_ACCESS		ea;
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = GENERIC_READ | GENERIC_WRITE;
	ea.grfAccessMode		= SET_ACCESS;
	ea.grfInheritance		= NO_INHERITANCE;
	ea.Trustee.TrusteeForm	= TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType	= TRUSTEE_IS_GROUP;
	ea.Trustee.ptstrName	= (LPTSTR)pSidIntercative;

	// create new Dacl from current with new ace
	err = SetEntriesInAcl(1, &ea, pDacl, &pNewDacl);
	if (err != ERROR_SUCCESS)
		GOTO_FINALIZE("SetEntriesInAcl failed");

	if (!pNewDacl)
		GOTO_FINALIZE( "pNewDacl == NULL" );

	err = SetSecurityInfo(hFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
						  NULL, NULL, pNewDacl, NULL);
	if (err != ERROR_SUCCESS)
		GOTO_FINALIZE( "SetSecurityInfo failed" );

finalize:
	if (pNewDacl)
		LocalFree(pNewDacl);

	if (pSidIntercative)
		LocalFree(pSidIntercative);

	if (pSd)
		LocalFree(pSd);

#undef GOTO_FINALIZE
}

static FILE_HANDLE __log_open(const char *filename)
{
	HANDLE hFile = CreateFileW(utf82wstr(filename, strlen(filename)).c_str(),
		GENERIC_WRITE | READ_CONTROL | WRITE_DAC | FILE_APPEND_DATA,
		FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,
		NULL,
		OPEN_ALWAYS, // we will use ERROR_ALREADY_EXISTS
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_BACKUP_SEMANTICS,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile) {
		TRACEDBG( "===CreateFile failed" );
		return INVALID_FILE_HANDLE;
	}

	if (GetLastError() != ERROR_ALREADY_EXISTS)
		log_set_permissions(hFile);

	return hFile;
}
#else
static FILE_HANDLE __log_open(const char *filename)
{
	int err;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

	err = open(filename, O_WRONLY | O_APPEND | O_CREAT, mode);
	if (err < 0)
		return err;

	fchmod(err, mode);
	return err;
}
#endif

static FILE_HANDLE log_open(const char *filename)
{
	FILE_HANDLE fd = __log_open(filename);
	if (fd != INVALID_FILE_HANDLE)
		return fd;
#if defined(_LIN_)
	if (errno == ETXTBSY)	/* special: PCS, no fallback */
		return fd;
#endif

	const char *home = GetUserHomeDir();
	SetLogFileName(home, GetProdDefaultLogFileName());
	filename = GetLogFileName();
	return __log_open(filename);
}

static FILE_HANDLE log_get_fd()
{
	FILE_HANDLE fd;
	const char *filename;
	struct LoggerData *ld = g_d();
	UINT64 timestamp = (UINT64)time(NULL);

	fd = ld->fd;
	if (fd != INVALID_FILE_HANDLE) {
		if (timestamp == ld->timestamp)		// racy check, non-racy below
			return ld->fd;
		if (timestamp == AtomicSwap(&ld->timestamp, timestamp))
			return ld->fd;
	}
	filename = GetLogFileName();
	if (fd == INVALID_FILE_HANDLE)
		goto reopen;

#ifdef _WIN_
	BY_HANDLE_FILE_INFORMATION info1, info2;
	HANDLE file;
	BOOL err;

	file = CreateFileW(utf82wstr(filename, strlen(filename)).c_str(),
					   GENERIC_READ,
					   FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,
					   NULL,
					   OPEN_EXISTING,
					   FILE_ATTRIBUTE_NORMAL|FILE_FLAG_BACKUP_SEMANTICS,
					   NULL);
	if (file == INVALID_FILE_HANDLE)
		goto reopen;

	err = !GetFileInformationByHandle(file, &info1);
	CloseHandle(file);
	if (err || !GetFileInformationByHandle(fd, &info2))
		goto reopen;

	if (info1.dwVolumeSerialNumber != info2.dwVolumeSerialNumber ||
		info1.nFileIndexHigh != info2.nFileIndexHigh ||
		info1.nFileIndexLow != info2.nFileIndexLow)
		goto reopen;
#else
	struct stat st1, st2;

	if (stat(filename, &st1) < 0)
		goto reopen;
	if (fstat(ld->fd, &st2) < 0)
		goto reopen;
	if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino)
		goto reopen;
#endif

	return ld->fd;

reopen:
	fd = log_open(filename);
	if (fd == INVALID_FILE_HANDLE)
		return fd;

	fd = AtomicSwap(&ld->fd, fd);
	if (fd != INVALID_FILE_HANDLE)
		/* There is a strict race in between logging being performed from
		 * other thread with this close. Though:
		 *   - this is rare case
		 *   - no problem to lost one record, write will just return EINVAL
		 *     for the case
		 */
		close(fd);

	return ld->fd;
}

static void log_write(const char *msg, int length)
{
	FILE_HANDLE fd = log_get_fd();
	if (fd < 0)
		return;

	write(fd, msg, length);
}

void Logger::PutMessage(const char* const msg, int length)
{
	log_write(msg, length);

#ifndef EXTERNALLY_AVAILABLE_BUILD
	if (g_d()->is_console_enabled)
		write(STDERR_FILENO, msg, length);
#endif
}


/**
 * Set base path to the log file.
 */
void SetLogFileName(const char* path, const char *filename)
{
	FILE_HANDLE fd;
	LoggerData *const d = g_d();

	strncpy(d->logFilePath, path, sizeof(d->logFilePath));
	d->logFilePath[sizeof(d->logFilePath) - 1] = 0;

	snprintf(d->logFileName, sizeof(d->logFilePath), "%s/%s", path, filename);
	d->logFileName[sizeof(d->logFileName) - 1] = 0;

	fd = d->fd;
	d->fd = INVALID_FILE_HANDLE;
	close(fd);
}


const char *GetUserHomeDir()
{
//Win: C:\Documents and Settings\<username>\Application Data
//Vista: C:\Users\<username>\AppData\Roaming
//Lin, Mac: $HOME

	static char userHomeDir[1024];

	if (userHomeDir[0])
		return userHomeDir;

#ifdef _WIN_
	TCHAR   szPath[MAX_PATH];
	HRESULT hres = SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath);
	if (!SUCCEEDED(hres))
		return "";
	WideCharToMultiByte(CP_UTF8, 0, szPath, -1,
					    userHomeDir, sizeof(userHomeDir), 0, 0);
#else
	uid_t euid = geteuid();
	struct passwd* pPswd = getpwuid(euid);
	if (!pPswd || !pPswd->pw_dir)
		return "";
	snprintf(userHomeDir, sizeof(userHomeDir), "%s" LOG_USER_DIR,
			 pPswd->pw_dir);
#endif
	userHomeDir[sizeof(userHomeDir)-1] = 0;
	return userHomeDir;
}


const char *GetDefaultLogFilePath()
{
#ifdef _WIN_
	static char buf[MAX_PATH];

	if (buf[0] != 0)
		return buf;

	GetWindowsDirectoryA(buf, sizeof(buf)/sizeof(buf[0]));
	_strlwr_s(buf); // normalize to allow compare with userlogpath
	strcat(buf, "\\Logs\\");

	if (CreateDirectoryA(buf, NULL ) && GetLastError() != ERROR_ALREADY_EXISTS)
		buf[0] = 0;
	return buf;
#else
	return LOG_DEFAULT_DIR;
#endif
}


static LoggerData *getLogFiles()
{
	LoggerData *const d = g_d();
	if (*d->logFilePath == '\0') /* strlen() != 0 */
		SetLogFileName(GetDefaultLogFilePath(),
					   GetProdDefaultLogFileName());
	return d;
}


const char *GetLogFilePath()
{
	return getLogFiles()->logFilePath;
}


const char *GetLogFileName()
{
    return getLogFiles()->logFileName;
}

const char *GetProdDefaultLogFileName()
{
	static const char sLogFileName[] =
		PRL_LOG_FILE_NAME_DEFAULT;
	return sLogFileName;
}

/**
 * Enforce current loglevel for the library
 */
void SetLogLevel( int level )
{
	if (level == -1)
		level = DBG_LEVEL;
	__log_level = level;
}

/**
 * Get current loglevel for the library
 */
int GetLogLevel()
{
	return __log_level;
}

/**
 * Enable/disable logging to console. Provided for command line tools
 */
int SetConsoleLogging(int enable)
{
	LoggerData* d = g_d();

	int prev = d->is_console_enabled;
	d->is_console_enabled = enable;
	return prev;
}


/**
 * Reset file where logging is being performed.
 * This is extremely useful to be done on the first system startup.
 */
void Logger::ResetLogFile( Logger::LOG_RESET_REASON reason /* = LRR_RESTART */ )
{
	(void) reason;

	FILE_HANDLE fd = log_open(GetLogFileName());
	if (fd == INVALID_FILE_HANDLE)
		return;

	char buf[256];

#define FPUTS(fd, str)	write(fd, (str), (int)strlen(str))
#define	FPRINTF(fd, ...)	do {							\
	sprintf(buf, __VA_ARGS__);								\
	FPUTS(fd, buf);											\
} while (0)

	FPUTS(fd, "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	FPUTS(fd, "* Parallels Virtualization System Log File\n");
	FPUTS(fd, "*\n");
	FPRINTF(fd, "* Product information %s\n", VER_SHORTPRODUCTNAME_STR);
	FPRINTF(fd, "* Build information %s %s\n",
			VER_PRODUCTVERSION_STR, VER_SPECIAL_BUILD_STR);
	FPUTS(fd, "*\n");

	GetDateTimeString(buf);
	strcat(buf, "\n");
	FPUTS(fd, buf);

	FPRINTF(fd, "*\n");
	FPRINTF(fd, "* Operating System " LOG_OS_NAME "\n");
	FPUTS(fd, "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");
	FPUTS(fd, "\n");

	close(fd);

#undef FPUTS
#undef FPRINTF
}

/*
 * This is callback function to log events from userspace. For prl_vm_app
 * it can be done via shared memory. For guest applications/drivers - via CPUID
 * vmexit.
 */
void (*etrace_log_func)(PRL_UINT32 src, PRL_UINT32 type, PRL_UINT64 data) = NULL;

// #define SELF_CHECK_ENABLE

#ifdef SELF_CHECK_ENABLE

/**
 * Compilation verification function.
 */
static int verify_log_compiles()
{
#ifndef SELF_CHECK_ENABLE
    // Uncomment to ensure logging library works correctly
    return 0;
#endif // SELF_CHECK_ENABLE

    WRITE_TRACE( DBG_FATAL, "----- <LOGGING LIBRARY VERIFICATION> -----" );

    LOG_MESSAGE( DBG_FATAL,         " LOG_MESSAGE %s", "DBG_FATAL" );
    LOG_MESSAGE( DBG_WARNING,       " LOG_MESSAGE %s", "DBG_WARNING" );
    LOG_MESSAGE( DBG_INFO,          " LOG_MESSAGE %s", "DBG_INFO" );
    LOG_MESSAGE( DBG_DEBUG,         " LOG_MESSAGE %s", "DBG_DEBUG" );
	LOG_MESSAGE( DBG_TRACE,         " LOG_MESSAGE %s", "DBG_TRACE" );
    LOG_MESSAGE( DBG_FATAL,         " " );

    WRITE_TRACE(DBG_FATAL, " WRITE_TRACE %s", "DBG_FATAL" );
    WRITE_TRACE(DBG_FATAL, " WRITE_TRACE %s", "DBG_WARNING" );
    WRITE_TRACE(DBG_FATAL, " WRITE_TRACE %s", "DBG_INFO" );
    WRITE_TRACE(DBG_FATAL, " WRITE_TRACE %s", "DBG_DEBUG" );
	WRITE_TRACE(DBG_FATAL, " WRITE_TRACE %s", "DBG_TRACE" );

    WRITE_TRACE(DBG_FATAL, " " );

	LogRateLimit limit(2);
	for ( unsigned i = 0; i < 30; i++ )
	{
		sleep(1);
		WRITE_TRACE(DBG_FATAL, "Iteration %u", i );

		WRITE_TRACE( limit, DBG_FATAL, "WRITE_TRACE RateLimited=%u", i );
		LOG_MESSAGE( limit, DBG_FATAL, "LOG_MESSAGE RateLimited=%u", i );
	}

    LOG_MESSAGE( DBG_FATAL, "----- </LOGGING LIBRARY VERIFICATION> -----" );

    return 0;
}

/**
 * Compilation verification variable.
 */
static int verification_log_var = verify_log_compiles();


int main()
{
    verify_log_compiles();
}

#endif // SELF_CHECK_ENABLE
