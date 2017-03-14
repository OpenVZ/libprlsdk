/*
 * SdkWrap.h: Really simple C++ wrapper over the DLL. Requires no
 * special libraries and works on win/lib/mac.
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __PARALLELS_DLL_WRAP_H__
#define __PARALLELS_DLL_WRAP_H__

#include "Parallels.h"

/**
 * Checking compile-time compatibility
 */
#ifndef DYN_API_WRAP
    #error This wrapping library was designed to be used transparently with  DYN_API_WRAP macro
#endif

namespace PrlSdkWrapNamespace {
/**
 * Load dynamic library pointer my the file name
 *
 * @param utf-8 encoded file name
 * @param sign whether debug mode presents (more debug output on SDK lib load fails)
 * @return PRL_RESULT
 */
extern PRL_RESULT SdkWrap_Load( PRL_CONST_STR name, bool bDebugMode = false );

/**
 * Unload previously loaded dynamic library.
 *
 * @return PRL_RESULT
 */
extern PRL_RESULT SdkWrap_Unload();

/**
 * Find symbol in the loaded library.
 *
 * @param symbol name to find
 * @return pointer to the symbol, NULL on failure
 */
extern PRL_VOID_PTR SdkWrap_GetSymbol( PRL_CONST_STR symbol );

/**
 * Return boolean status that identifies loaded state of the dll.
 *
 * @return boolean loaded status
 */
extern PRL_BOOL SdkWrap_IsLoaded();

/**
 * Tries to load SDK library from well known locations corresponding to current
 * operating system. At first method tries to load SDK library using standard
 * environment that specifies paths to all system dynamically linking libraries.
 * If it fails method tries to load library from current executable directory.
 * At last method tries to load SDK library from Parallels product installation
 * path. If all three attempts fail then method fails.
 *
 * Note: if SDK library was already loaded then it won't be reloaded. Use SdkWrap_Unload
 * at first.
 *
 * @return boolean sign whether SDK library was loaded successfully
 */
extern PRL_BOOL SdkWrap_LoadLibFromStdPaths( bool bDebugMode = false );

/**
 * Each SDK method is instantiated as an instance of the
 * variable that points to the proper method somehow
 */
#ifdef PRL_SDK_WRAP_FOR_EACH_ITERATOR
    #undef PRL_SDK_WRAP_FOR_EACH_ITERATOR
#endif
#define PRL_SDK_WRAP_FOR_EACH_ITERATOR( name ) \
	extern name##wrap##_Ptr name;

#include "PrlApiMacro.h"

PRL_SDK_WRAP_FOR_EACH()
}//namespace PrlSdkWrapNamespace

using namespace PrlSdkWrapNamespace;

// this copyed from PrlApi.h
inline const char * prl_result_to_string(PRL_RESULT value)
{
    const char * result ;
    PrlDbg_PrlResultToString(value, &result) ;
    return result ;
}

inline const char * handle_type_to_string(PRL_HANDLE_TYPE value)
{
    const char * result ;
    PrlDbg_HandleTypeToString(value, &result) ;
    return result ;
}

inline const char * event_type_to_string(PRL_EVENT_TYPE value)
{
    const char * result ;
    PrlDbg_EventTypeToString(value, &result) ;
    return result ;
}

#endif // __PARALLELS_DLL_WRAP_H__
