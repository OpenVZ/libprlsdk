/*
 * parallels_breakpad.h: Provides common google-breakpad
 * functionaliry for Parallels usage
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


#ifndef PARALLELS_BREAKPAD_H__
#define PARALLELS_BREAKPAD_H__

#include "exception_handler.h"

#ifdef _WIN_
  typedef wchar_t bp_char_t;
#else
  typedef char bp_char_t;
#endif

namespace ParallelsBreakpad {
#ifdef _LIN_
typedef bool (*GenFilenameCallback_type)(char*& , char*& );
typedef bool (*MinidumpCallback_type)(const bp_char_t*, const bp_char_t*, void*, bool);
#else
typedef google_breakpad::ExceptionHandler::MinidumpCallback MinidumpCallback_type;
typedef google_breakpad::ExceptionHandler::GenFilenameCallback GenFilenameCallback_type;
#endif // _LIN_
/**
 * IMPORTANT !!!
 * After any changes in this header need fix declaration in VI/Transporter:
 * 		"Sources/VI/Sources/Transporter/ProblemReporting/ProplemReporting.cpp"
 * It is necessary to prevent compilation errors.
**/

/**
 * Creates crash handler.
 * Must be called in first line of 'main' function.
 */
google_breakpad::ExceptionHandler* CREATE_CRASH_HANDLER (
    const char* extra_dump_name,
    GenFilenameCallback_type gen_callback,
    MinidumpCallback_type minidump_callback,
    google_breakpad::ExceptionHandler::FilterCallback filter_callback = 0
 );

/**
 * Predefined minidump callback, which creates descriptor of Parallels
 * dump in temp directory with mask: '<app_name>.<pid>.<platform>.dmp.desc'
 */
bool MinidumpCallback ( const bp_char_t *dump_path,
                        const bp_char_t *minidump_id,
                        void *context,
#ifdef _WIN_
                        EXCEPTION_POINTERS*,
                        MDRawAssertionInfo*,
#endif
                        bool succeeded );


/**
 * Predefined minidump callback, which creates descriptor of Parallels
 * dump in temp directory with mask: '<app_name>.<pid>.<platform>.dmp.desc'
 * and starts crash reporter.
 */
bool MinidumpCallback_StartCrashReporter ( const bp_char_t *dump_path,
                                           const bp_char_t *minidump_id,
                                           void *context,
#ifdef _WIN_
                                           EXCEPTION_POINTERS*,
                                           MDRawAssertionInfo*,
#endif
                                           bool succeeded );


/**
 * Predefined filename callback, which generates Parallels dump names
 * in temp directory with mask: '<app_name>.<pid>.<platform>.dmp'
 */
bool GenFilenameCallback ( bp_char_t*& path, bp_char_t*& filename );

/**
 * Predefined minidump filter
 */
#ifdef _WIN_
bool FilterCallback ( void *context, EXCEPTION_POINTERS*, MDRawAssertionInfo*);
#else
bool FilterCallback ( void *context );
#endif

/**
 * Returns current working directory.
 * Is usefull for 'MinidumpCallback's.
 */
const bp_char_t* GetCWD ( size_t& cwdSize );

/**
 * Returns current application name.
 * Is usefull for 'MinidumpCallback's.
 */
const bp_char_t* GetAppName ( size_t& appNameSize );

/**
 * Prebuffered empty CMD line of ~10kb size.
 * Must be enough for even greeedy 'MinidumpCallback's.
 */
bp_char_t* GetCMDLine ( size_t& cmdSize );

} // namespace Parallels

#endif  // PARALLELS_BREAKPAD_H__

