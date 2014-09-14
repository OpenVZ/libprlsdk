/*
 * parallels_breakpad.cpp: Provides common google-breakpad
 * functionaliry for Parallels usage.
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


#include <QDir>

#include "parallels_breakpad.h"

#include "Interfaces/ParallelsQt.h"
#include "Libraries/Logging/Logging.h"
#include "Libraries/PrlCommonUtilsBase/ParallelsDirs.h"
#include "Build/Current.ver"


#ifndef _WIN_ // Unix
  #include <limits.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
#else // Windows
  #ifndef PATH_MAX
    #define PATH_MAX 4096 // 4k should be enough
  #endif
  #include <direct.h>
#endif

#include <stdio.h>

/*****************************************************************************/

namespace {
    google_breakpad::ExceptionHandler* g_crashHandler = 0;
    bp_char_t* g_cwd = 0;
    size_t g_cwdSz = 0;
    bp_char_t* g_appName = 0;
    size_t g_appNameSz = 0;
    bp_char_t* g_cmdLine = 0;
    size_t g_cmdLineSz = 0;
    char* g_extraDumpName = 0;
    size_t g_extraDumpNameSz = 0;

    /// Const helpers
    const bp_char_t* g_crashReporterAppName =
#ifdef _WIN_
        L"prl_crash_reporter";
#else
        "prl_crash_reporter";
#endif

#if defined(_LIN_)
static struct Arguments
{
	void* context;
	char* dirname;
	char* basename;
	bool (* filter)(void*);
	bool (* minidump)(const char*, const char*, void*, bool);
} g_arguments;

bool ProxyFilter(void* context_)
{
	Arguments* a = (Arguments* )context_;
	if (NULL == a)
		return false;

	return NULL == a->filter || a->filter(a->context);
}

bool ProxyMinidump(const google_breakpad::MinidumpDescriptor& , void* context_, bool succeeded_)
{
	Arguments* a = (Arguments* )context_;
	if (NULL == a || a->minidump == NULL)
		return false;

	return a->minidump(a->dirname, a->basename, a->context, succeeded_);
}

#endif // _LIN_
}

/*****************************************************************************/

google_breakpad::ExceptionHandler* ParallelsBreakpad::CREATE_CRASH_HANDLER (
    const char* extra_dump_name,
    GenFilenameCallback_type gen_callback,
    MinidumpCallback_type minidump_callback,
    google_breakpad::ExceptionHandler::FilterCallback filter_callback )
{
    void *context;

    QByteArray envCrashHandler = qgetenv("PARALLELS_CRASH_HANDLER");
    if ( QString(envCrashHandler).contains("OFF", Qt::CaseInsensitive) ||
         QString(envCrashHandler).contains("0") ) {
        WRITE_TRACE(DBG_FATAL, "Parallels crash handler is turned off!");
        return 0;
    }

    if ( ! g_crashHandler ) {
#ifndef _WIN_ // Unix
        ::setlocale(LC_ALL, "");        // use correct char set mapping
        ::setlocale(LC_NUMERIC, "C");   // make sprintf()/scanf() work
#endif
        //
        // Create extra dump name
        //
        {
            g_extraDumpName = 0;
            g_extraDumpNameSz = 0;
            if ( extra_dump_name ) {
                g_extraDumpNameSz = ::strlen(extra_dump_name);
                g_extraDumpName =
                    reinterpret_cast<char*>(::malloc(g_extraDumpNameSz + 1));
                if ( g_extraDumpName == 0 ) {
                    WRITE_TRACE(DBG_FATAL, "Error: can't allocate enough for crash "
                                "handler!");
                    goto error;
                }
                ::memcpy(g_extraDumpName, extra_dump_name, g_extraDumpNameSz);
                g_extraDumpName[g_extraDumpNameSz] = '\0';
            }
        }

        //
        // Get and try to create dumps path
        //
        {
            QString dumpsPath = ParallelsDirs::getCrashDumpsPath();
#ifndef _WIN_ // Unix
            QDir dir = QDir( dumpsPath );
            if ( ! dir.exists() ) {
                mode_t old_mask = ::umask(0);
                bool res = dir.mkpath(dumpsPath);
                ::umask(old_mask);

                if ( ! res ) {
                    WRITE_TRACE(DBG_FATAL, "Error: can't create directory '%s' "
                                "for Parallels dumps",
                                dumpsPath.toLocal8Bit().constData());
                    goto error;
                }
            }
#else // Windows
            // Create crash dumps directory for everyone

            SECURITY_ATTRIBUTES sa;
            SECURITY_DESCRIPTOR sd;

            ::InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
            ::SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
            ::SetSecurityDescriptorGroup(&sd,NULL, FALSE );
            ::SetSecurityDescriptorSacl(&sd, FALSE, NULL, FALSE );

            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.lpSecurityDescriptor = &sd;
            sa.bInheritHandle = TRUE;

            BOOL res = ::CreateDirectory( dumpsPath.utf16(), &sa );
            if ( res == 0 && ::GetLastError() != ERROR_ALREADY_EXISTS ) {
                WRITE_TRACE(DBG_FATAL, "Error: can't create directory '%s' "
                            "for Parallels dumps",
                            dumpsPath.toLocal8Bit().constData());
                goto error;
            }
#endif
        }

        //
        // Get current working directory
        //
        g_cwdSz = PATH_MAX;
        g_cwd = reinterpret_cast<bp_char_t*>(
            ::malloc(g_cwdSz * sizeof(bp_char_t)));
        if ( g_cwd == 0 ) {
            WRITE_TRACE(DBG_FATAL, "Error: can't allocate enough for crash handler!");
            goto error;
        }
        else {
#ifdef _WIN_
            bp_char_t* cwd = ::_wgetcwd( g_cwd, (int)g_cwdSz );
#else
            bp_char_t* cwd = ::getcwd( g_cwd, g_cwdSz );
#endif
            if ( cwd == 0 ) {
                WRITE_TRACE(DBG_FATAL, "Error: can't get current working directory!");
                goto error;
            }
            // Count correct size for cwd
#ifdef _WIN_
            QString qcwd = UTF16_2QSTR( cwd );
#else
            QString qcwd = QString::fromLocal8Bit( cwd );
#endif
            g_cwdSz = qcwd.size();
        }

        //
        // Create big cmd line buffer
        //
        g_cmdLineSz = PATH_MAX * 2;
        g_cmdLine = reinterpret_cast<bp_char_t*>(
            ::malloc(g_cmdLineSz * sizeof(bp_char_t)));
        if ( g_cmdLine == 0 ) {
            WRITE_TRACE(DBG_FATAL, "Error: can't allocate enough for crash handler!");
            goto error;
        }


        //
        // Get current application name
        //
        g_appNameSz = PATH_MAX;
        g_appName = reinterpret_cast<bp_char_t*>(
            ::malloc(g_appNameSz * sizeof(bp_char_t)));
        if ( g_appName == 0 ) {
            WRITE_TRACE(DBG_FATAL, "Error: can't allocate enough for crash handler!");
            goto error;
        }
        else {
#if defined(_WIN_)
            g_appNameSz = ::GetModuleFileName( NULL, g_appName, (DWORD)g_appNameSz );
            if ( g_appNameSz == 0 ) {
                WRITE_TRACE(DBG_FATAL, "Error: can't get current application name!");
                goto error;
            }
#else // LIN_
            g_appNameSz = ::readlink("/proc/self/exe", g_appName,
                                     g_appNameSz-1);
            if ( (ssize_t)g_appNameSz < 0 ) {
                WRITE_TRACE(DBG_FATAL, "Error: can't get current executable name!");
                goto error;
            }
            g_appName[g_appNameSz] = '\0';
#endif
        }

	/* will use context as crash counter in filter */
	context = (void *)malloc(sizeof(int));
	memset(context, 0, sizeof(int));

        //
        // Create google_breakpad handler
        //
#if defined(_LIN_)
	std::string f;
	g_arguments.context = context;
	g_arguments.filter = filter_callback;
	g_arguments.minidump = minidump_callback;
	if (gen_callback(g_arguments.dirname, g_arguments.basename))
		f.append(g_arguments.dirname).append("/").append(g_arguments.basename);

	google_breakpad::MinidumpDescriptor d(f);
        g_crashHandler = new(std::nothrow) google_breakpad::ExceptionHandler(
				google_breakpad::ExceptionHandler::LightCoreDump,
				d,
				&ProxyFilter,
				&ProxyMinidump,
				&g_arguments,
				true,
				-1);
#elif defined(_WIN_)
        g_crashHandler = new(std::nothrow) google_breakpad::ExceptionHandler(
			   L".",
			   gen_callback,
			   filter_callback, //FilterCallback
			   minidump_callback,
			   context,
			   google_breakpad::ExceptionHandler::HANDLER_ALL );
#else // _WIN_
        g_crashHandler = new(std::nothrow) google_breakpad::ExceptionHandler(
			   ".",
			   gen_callback,
			   filter_callback, //FilterCallback
			   minidump_callback,
			   context,
			   true );
#endif // _LIN_
        if ( g_crashHandler == 0 ) {
            WRITE_TRACE(DBG_FATAL, "Error: can't allocate enough for crash handler!");
            goto error;
        }
    }
    return g_crashHandler;

error:
    WRITE_TRACE(DBG_FATAL, "Error: crash handler will be disabled!");

    // Free all allocated global buffers
    ::free( g_cwd );
    g_cwd = 0;
    g_cwdSz = 0;
    ::free( g_appName );
    g_appName = 0;
    g_appNameSz = 0;
    ::free( g_cmdLine );
    g_cmdLine = 0;
    g_cmdLineSz = 0;
    ::free( g_extraDumpName );
    g_extraDumpName = 0;
    g_extraDumpNameSz = 0;

    return 0;
}


bool ParallelsBreakpad::MinidumpCallback ( const bp_char_t *dump_path,
                                           const bp_char_t *minidump_id,
                                           void *context,
#ifdef _WIN_
                                           EXCEPTION_POINTERS*,
                                           MDRawAssertionInfo*,
#endif
                                           bool succeeded )
{
    (void)context;

    if ( ! succeeded )
        // Nothing we can do
        return false;

    size_t cmdSz;
    bp_char_t* cmd = ParallelsBreakpad::GetCMDLine(cmdSz);
#ifdef _WIN_
    ::_snwprintf(cmd, cmdSz, L"%s/%s.desc", dump_path, minidump_id);
#else
    ::snprintf(cmd, cmdSz, "%s/%s.desc", dump_path, minidump_id);
#endif


#ifdef _WIN_
    HANDLE h =::CreateFile( cmd, GENERIC_WRITE, FILE_SHARE_READ |
                            FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL );
    ::CloseHandle(h);
#else
    int h = ::open( cmd, O_WRONLY | O_CREAT, 0600 );
    ::close(h);
#endif

    return true;
}

bool ParallelsBreakpad::MinidumpCallback_StartCrashReporter (
    const bp_char_t *dump_path,
    const bp_char_t *minidump_id,
    void *context,
#ifdef _WIN_
    EXCEPTION_POINTERS* ep,
    MDRawAssertionInfo* inf,
#endif
    bool succeeded )
{
    bool res = ParallelsBreakpad::MinidumpCallback( dump_path,
                                                    minidump_id,
                                                    context,
#ifdef _WIN_
                                                    ep,
                                                    inf,
#endif
                                                    succeeded );
    if ( ! res )
        return false;

    const char* productName = VER_SHORTPRODUCTNAME_STR;
    const char* clientVersion = VER_FULL_BUILD_NUMBER_STR;

    size_t cmdSz;
    bp_char_t* cmd = ParallelsBreakpad::GetCMDLine(cmdSz);

    size_t appNameSz;
    const bp_char_t* appName = ParallelsBreakpad::GetAppName(appNameSz);

    // Create args:
    // prl_crash_reporter CRASHED_APP_NAME DUMP_FILE

#ifdef _WIN_ // Windows
    ::_snwprintf(cmd, cmdSz, L"\"%s\" \"%S\" \"%S\" \"%s\" \"%s\\%s\"",
                 g_crashReporterAppName,
                 productName,
                 clientVersion,
                 appName,
                 dump_path,
                 minidump_id);
#else // Unix
    ::snprintf(cmd, cmdSz, "%s/%s", dump_path, minidump_id);

    char crashRepAppName[PATH_MAX] = {0};
    ::snprintf(crashRepAppName, sizeof(crashRepAppName), "%s", g_crashReporterAppName);

    char nconst_productName[256] = {0};
    ::snprintf(nconst_productName, sizeof(nconst_productName), "%s", productName);

    char nconst_clientVersion[256] = {0};
    ::snprintf(nconst_clientVersion, sizeof(nconst_clientVersion), "%s", clientVersion);

    char nconst_appName[PATH_MAX] = {0};
    ::snprintf(nconst_appName, sizeof(nconst_appName), "%s", appName);

    char* argv[8] = {0};
    argv[0] = crashRepAppName;
    argv[1] = nconst_productName;
    argv[2] = nconst_clientVersion;
    argv[3] = nconst_appName;
    argv[4] = cmd;
#endif

#ifdef _WIN_ // Windows

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ::ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;
    ::ZeroMemory(&pi, sizeof(pi));

    if ( ::CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0,
                         NULL, NULL, &si, &pi) ) {
        ::CloseHandle( pi.hProcess );
        ::CloseHandle( pi.hThread );
    }

    // just terminate. nothing we can do if CreateProcess failed
    ::TerminateProcess(::GetCurrentProcess(), 1);

    // Unreachable line for Windows

#else // Unix
    pid_t pid = ::vfork();
    if ( pid == -1 ) {
        // Nothing we can do here!
        return false;
    }
    else if ( pid == 0 ) {
        (void)::execv(g_crashReporterAppName, argv);
        // Nothing we can do here!
        _exit(1);
    }

    // just terminate. nothing we can do if fork or exec failed
    _exit(1);

    // Unreachable line for Unix

#endif

    return true;
}

bool ParallelsBreakpad::GenFilenameCallback ( bp_char_t*& path,
                                              bp_char_t*& file_name )
{
#ifdef _WIN_
    const char* platform = "win";
    const char* dump_ext = "dmp";
    DWORD pid = ::GetCurrentProcessId();
#else
    const char* platform = "lin";
    const char* dump_ext = "lcore";
    int pid = ::getpid();
#endif


#ifdef _WIN_
    QString appName = UTF16_2QSTR( g_appName );
#else
    QString appName = QString::fromLocal8Bit( g_appName );
#endif

    // Copy full build name, replace '.' with '-'
    const char* fullBuildName = VER_FULL_BUILD_NUMBER_STR;
    QString fullBuildNameStr( fullBuildName );
    fullBuildNameStr.replace( ".", "-" );

    QString fileName;

    if ( g_extraDumpNameSz && g_extraDumpName )
        fileName = QString("%1.%2.%3.%4.%5.%6").
            arg(QFileInfo(appName).baseName()).
            arg(fullBuildNameStr).
            arg(g_extraDumpName).
            arg(pid).
            arg(platform).
            arg(dump_ext);
    else
        fileName = QString("%1.%2.%3.%4.%5").
            arg(QFileInfo(appName).baseName()).
            arg(fullBuildNameStr).
            arg(pid).
            arg(platform).
            arg(dump_ext);

    QString dumpsPath = ParallelsDirs::getCrashDumpsPath();

    //
    // Create and fill output strings
    //

#ifdef _WIN_
    const bp_char_t* fnStr = fileName.utf16();
    const bp_char_t* dpStr = dumpsPath.utf16();

    size_t fnSz = fileName.size();
    size_t dpSz = dumpsPath.size();
#else
    QByteArray fileNameBa = fileName.toLocal8Bit();
    QByteArray dumpsPathBa = dumpsPath.toLocal8Bit();

    size_t fnSz = fileNameBa.size();
    size_t dpSz = dumpsPathBa.size();

    const bp_char_t* fnStr = fileNameBa.constData();
    const bp_char_t* dpStr = dumpsPathBa.constData();
#endif

    path = reinterpret_cast<bp_char_t*>(
        ::malloc(dpSz * sizeof(bp_char_t) + sizeof(bp_char_t)));
    if ( path == 0 ) {
        WRITE_TRACE(DBG_FATAL, "Error: can't allocate enough for path name!");
        return false;
    }
    ::memcpy(path, dpStr, dpSz * sizeof(bp_char_t));
    path[dpSz] = 0;

    file_name = reinterpret_cast<bp_char_t*>(
        ::malloc(fnSz * sizeof(bp_char_t) + sizeof(bp_char_t)));
    if ( file_name == 0 ) {
        ::free(path);
        path = 0;

        WRITE_TRACE(DBG_FATAL, "Error: can't allocate enough for file name!");
        return false;
    }
    ::memcpy(file_name, fnStr, fnSz * sizeof(bp_char_t));
    file_name[fnSz] = 0;

    return true;
}

/* will create minidump for first crash only*/
bool ParallelsBreakpad::FilterCallback(
#ifdef _WIN_
    void *context,
    EXCEPTION_POINTERS* ep,
    MDRawAssertionInfo* inf
#else
    void *context
#endif
    )
{
#ifdef _WIN_
	(void)ep;
	(void)inf;
#endif
	/* at google-breakpad filter call protected by mutex */
	if (context) {
		int *counter = (int *)context;
		return ((*counter)++ == 0);
	}
	return true;
}

const bp_char_t* ParallelsBreakpad::GetCWD ( size_t& cwdSize )
{
    cwdSize = g_cwdSz;
    return g_cwd;
}

const bp_char_t* ParallelsBreakpad::GetAppName ( size_t& appNameSize )
{
    appNameSize = g_appNameSz;
    return g_appName;
}

bp_char_t* ParallelsBreakpad::GetCMDLine ( size_t& cmdSize )
{
    cmdSize = g_cmdLineSz;
    return g_cmdLine;
}

/*****************************************************************************/
