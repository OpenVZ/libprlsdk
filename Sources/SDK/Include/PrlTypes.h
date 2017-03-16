/*
 * PrlTypes.h
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


#ifndef __PARALLELS_API_TYPES_H__
#define __PARALLELS_API_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Based on the discussion in Issue #108325
 */
#if !defined(_WIN_) && !defined(_LIN_)
    /**
     * This is probably an external build (with 3d party SDK)
     * and we don't have our build environment - trying to guess
     * it ourselves here
     */
    #if defined(_MSC_VER)
        #define _WIN_
    #elif defined(__GNUC__)
        #define _LIN_
    #else
        #error "Unknown compiler detected"
    #endif
#endif

/// All strings are UTF8 encoded
typedef char					PRL_CHAR;
typedef PRL_CHAR*				PRL_CHAR_PTR;
typedef PRL_CHAR_PTR			PRL_STR;
typedef const char*				PRL_CONST_STR;
typedef PRL_CONST_STR*			PRL_CONST_STR_PTR;

/// Unsigned 8 bit integer value
typedef unsigned char			PRL_UINT8;
typedef PRL_UINT8*				PRL_UINT8_PTR;
typedef PRL_UINT8**				PRL_UINT8_PTR_PTR;
typedef const PRL_UINT8*	 	PRL_CONST_UINT8_PTR;

/// Signed 8 bit integer value
typedef signed char				PRL_INT8;
typedef PRL_INT8*				PRL_INT8_PTR;
typedef PRL_INT8**				PRL_INT8_PTR_PTR;
typedef const PRL_INT8* 		PRL_CONST_INT8_PTR;

/// Unsigned 16 bit integer value
typedef unsigned short			PRL_UINT16;
typedef PRL_UINT16*				PRL_UINT16_PTR;
typedef PRL_UINT16**			PRL_UINT16_PTR_PTR;
typedef const PRL_UINT16*		PRL_CONST_UINT16_PTR;

/// Signed 16 bit integer value
typedef short					PRL_INT16;
typedef PRL_INT16*				PRL_INT16_PTR;
typedef PRL_INT16**				PRL_INT16_PTR_PTR;
typedef const PRL_INT16*		PRL_CONST_INT16_PTR;

/// Unsigned 32 bit integer value
typedef unsigned int			PRL_UINT32;
typedef PRL_UINT32*				PRL_UINT32_PTR;
typedef PRL_UINT32**			PRL_UINT32_PTR_PTR;
typedef const PRL_UINT32*		PRL_CONST_UINT32_PTR;

/// Signed 32 bit integer value
typedef int						PRL_INT32;
typedef PRL_INT32*				PRL_INT32_PTR;
typedef PRL_INT32**				PRL_INT32_PTR_PTR;
typedef const PRL_INT32*		PRL_CONST_INT32_PTR;

/// Unsigned 64 bit integer value
/// Use _MSC_VER macros for MinGW use
#if defined _WIN_  && defined _MSC_VER
    typedef unsigned __int64			PRL_UINT64;
#else
    typedef unsigned long long 			PRL_UINT64;
#endif

typedef PRL_UINT64*				PRL_UINT64_PTR;
typedef PRL_UINT64**			PRL_UINT64_PTR_PTR;
typedef const PRL_UINT64*		PRL_CONST_UINT64_PTR;

/// Signed 64 bit integer value
/// Use _MSC_VER macros for MinGW use
#if defined _WIN_  && defined _MSC_VER
    typedef __int64				PRL_INT64;
#else
    typedef long long 			PRL_INT64;
#endif

typedef PRL_INT64*				PRL_INT64_PTR;
typedef PRL_INT64**				PRL_INT64_PTR_PTR;
typedef const PRL_INT64*		PRL_CONST_INT64_PTR;

/// Boolean value (32 bit integer), zero means FALSE or TRUE otherwise
typedef unsigned int			PRL_BOOL;
typedef PRL_BOOL*				PRL_BOOL_PTR;
typedef const PRL_BOOL*			PRL_CONST_BOOL_PTR;

#define PRL_TRUE	(1)
#define PRL_FALSE	(0)

#define PRL_TERNARY_UNDEF (0)
#define PRL_TERNARY_TRUE (2)
#define PRL_TERNARY_FALSE (3)

/// Void pointers (for unified naming)
typedef void				PRL_VOID;
typedef PRL_VOID*			PRL_VOID_PTR;
typedef const PRL_VOID*		PRL_CONST_VOID_PTR;
typedef PRL_VOID_PTR*		PRL_VOID_PTR_PTR;

/// Unsigned integer type of the result of the sizeof operator
typedef unsigned int		PRL_SIZE;
typedef PRL_SIZE*			PRL_SIZE_PTR;
typedef PRL_SIZE_PTR*		PRL_SIZE_PTR_PTR;
typedef const PRL_SIZE*		PRL_CONST_SIZE_PTR;

/// Double type
typedef double				PRL_DOUBLE;
typedef PRL_DOUBLE*			PRL_DOUBLE_PTR;
typedef PRL_DOUBLE_PTR*		PRL_DOUBLE_PTR_PTR;
typedef const PRL_DOUBLE*	PRL_CONST_DOUBLE_PTR;

/* Default calling conversion for every supported platform. */
#ifndef PRL_CALL
#if !defined(PRL_64_BIT_BUILD) && !defined(_AMD64_) && !defined(__x86_64__)
#ifdef _WIN_
#define PRL_CALL __cdecl
#else
#define PRL_CALL __attribute__((cdecl, regparm(0)))
#endif
#else
#define PRL_CALL
#endif
#endif

/// Default extern "C" if cplusplus is used
#ifndef PRL_EXTERN
#ifdef __cplusplus
#define PRL_EXTERN extern "C"
#else
#define PRL_EXTERN
#endif
#endif

/// Default method decoration
#ifdef _WIN_

#define PRL_METHOD( func ) PRL_EXTERN __declspec(dllexport) PRL_RESULT PRL_CALL func
#define PRL_ASYNC_METHOD( func ) PRL_EXTERN __declspec(dllexport) PRL_HANDLE PRL_CALL func

#else

#ifndef PRL_GCC_DISABLE_VISIBILITY_ATTR
#define PRL_DLLEXPORT __attribute__ ((visibility("default")))
#else
#define PRL_DLLEXPORT
#endif

#define PRL_METHOD( func ) PRL_EXTERN PRL_DLLEXPORT PRL_RESULT PRL_CALL func
#define PRL_ASYNC_METHOD( func ) PRL_EXTERN PRL_DLLEXPORT PRL_HANDLE PRL_CALL func

#endif

#define PRL_METHOD_PTR( func_type ) PRL_RESULT ( PRL_CALL * func_type )
#define PRL_ASYNC_METHOD_PTR( func_type ) PRL_HANDLE ( PRL_CALL * func_type )

#ifdef DYN_API_WRAP

/// Default method declaration for the header files
#define PRL_METHOD_DECL( version, name, args )	\
	PRL_METHOD( wrap##name ) args;	\
	typedef PRL_METHOD_PTR( name##wrap##_Ptr ) args;

#define PRL_SRV_METHOD_DECL( version, name, args )	\
	PRL_METHOD( wrap##name ) args;	\
	typedef PRL_METHOD_PTR( name##wrap##_Ptr ) args;

#define PRL_ASYNC_METHOD_DECL( version, name, args )	\
	PRL_ASYNC_METHOD( wrap##name ) args;	\
	typedef PRL_ASYNC_METHOD_PTR( name##wrap##_Ptr ) args;

#define PRL_ASYNC_SRV_METHOD_DECL( version, name, args )	\
	PRL_ASYNC_METHOD( wrap##name ) args;	\
	typedef PRL_ASYNC_METHOD_PTR( name##wrap##_Ptr ) args;

#else // DYN_API_WRAP

/// Default method declaration for the header files
#define PRL_METHOD_DECL( version, name, args )	\
	PRL_METHOD( name ) args;	\
	typedef PRL_METHOD_PTR( name##_Ptr ) args;

#define PRL_SRV_METHOD_DECL( version, name, args )	\
	PRL_METHOD( name ) args;	\
	typedef PRL_METHOD_PTR( name##_Ptr ) args;

#define PRL_ASYNC_METHOD_DECL( version, name, args )	\
	PRL_ASYNC_METHOD( name ) args;	\
	typedef PRL_ASYNC_METHOD_PTR( name##_Ptr ) args;

#define PRL_ASYNC_SRV_METHOD_DECL( version, name, args )	\
	PRL_ASYNC_METHOD( name ) args;	\
	typedef PRL_ASYNC_METHOD_PTR( name##_Ptr ) args;

#endif // DYN_API_WRAP

/// Default structure definition
#ifdef _WIN_
#define PRL_STRUCT( struct_name ) struct_name
#else
#define PRL_STRUCT( struct_name ) __attribute__((packed)) struct_name
#endif

/**
 * For platforms compatibility, we need to to have some
 * very common macros one each platform.
 */
#ifdef _MSC_VER
	#define PRL_UNUSED
	#define PRL_PACKED
	#define PRL_UNUSED_PARAM(x) (void)x
#elif defined(__GNUC__)
	#define PRL_UNUSED __attribute__((unused))
	#define PRL_PACKED __attribute__((packed))
	#define PRL_UNUSED_PARAM(x) (void)x
#else
	#error SDK detected unknown compiler (!)
#endif

/// Handle representation
typedef int*				PRL_HANDLE;
typedef PRL_HANDLE* 		PRL_HANDLE_PTR;
typedef const PRL_HANDLE* 	PRL_CONST_HANDLE_PTR;

/// NULL (invalid) handle
#define PRL_NULL ( (PRL_VOID_PTR) 0 )
#define PRL_INVALID_HANDLE ( 0 )

/*
 * On windows platform we're taking advantage of the #pack directive
 * to ensure internal structures alignment by 1 - on unixes we achive
 * the same results by using the __attribute__((packed)) directive
 * -> changing alignment
 */
#ifdef _WIN_
    #pragma pack(push, save_api_pack, 1)
#endif

/**
 * API method
 **/
typedef struct _PRL_API_METHOD
{
    PRL_CONST_STR    MethodName;
    PRL_UINT32       MethodAPIVersion;
    PRL_VOID_PTR     MethodPtr;
    PRL_BOOL         MethodIsDeprecated;
    PRL_BOOL         MethodIsAsync;
    PRL_BOOL         MethodIsSrv;

    // Reserved for future use
    PRL_UINT32       ReservedA;
    PRL_UINT32       ReservedB;
    PRL_UINT32       ReservedC;
    PRL_UINT32       ReservedD;

} PRL_STRUCT( PRL_API_METHOD );

typedef PRL_API_METHOD*        PRL_API_METHOD_PTR;
typedef const PRL_API_METHOD*  PRL_CONST_API_METHOD_PTR;

/**
 * Global unique identifier
 **/
typedef struct _PRL_GUID
{
    PRL_UINT32       Data1;
    PRL_UINT16       Data2;
    PRL_UINT16       Data3;
    PRL_UINT8        Data4[8];
} PRL_STRUCT( PRL_GUID );

typedef PRL_GUID* 		PRL_GUID_PTR;
typedef const PRL_GUID* PRL_CONST_GUID_PTR;

//File descriptor type
#ifdef _WIN_
typedef PRL_VOID_PTR PRL_FILE_DESC;
#else
typedef PRL_INT32 PRL_FILE_DESC;
#endif

/// Unified result error code for each and every method
typedef int PRL_RESULT;
typedef PRL_RESULT* PRL_RESULT_PTR;

/**
 * All events (either commands, input/output, etc.) are recieved by the client
 * in this 'callback' functions, provided by the client to the library.
 */
typedef PRL_METHOD_PTR( PRL_EVENT_HANDLER_PTR ) (
		PRL_HANDLE hEvent,
		PRL_VOID_PTR data
		);

//Invalid file descriptor
#ifndef _WIN_
#define PRL_INVALID_FILE_DESCRIPTOR (-1)
#else
#define PRL_INVALID_FILE_DESCRIPTOR (0)
#endif

#ifdef _WIN_
    #pragma pack(pop, save_api_pack)
#endif

#ifdef __cplusplus
}
#endif

#endif // __PARALLELS_API_TYPES_H__
