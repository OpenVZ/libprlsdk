/*
 * AtomicOps.h: This module defines methods for atomic memory
 * operations (like inc, dec, add, etc.). Important Note: for 32-bit
 * systems on new GCC 4 compilers we cannot specify '%%ebx' register
 * in clobbers - so we're forced to use them "silently". Because of
 * that in many methods we cannot rely on automatic registers
 * distribution if GCC and need to do everything manually.
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


#ifndef ATOMICOPS_H_INCLUDED
#define ATOMICOPS_H_INCLUDED

#include "Interfaces/ParallelsTypes.h"
#include "MemBarrier.h"

//
// all AtomicOps() return *dst value fetched before executing op
// NOTE: the caller MUST align operands properly
//
// generic case uses CMPXCHG/CMPXCHG8B instructions
// 32bit AtomicAdd/Inc/Dec() are optimized using XADD
// 32bit AtomicSwap() uses XCHG
//
// no explicit memory barriers are defined here,
// however all operations behave as a strong memory barrier
// for both compiler and processor
//

#if defined(__GNUC__)
# undef __inline
# define __inline static __always_inline
#endif

#if defined(ASSERT_VER) && ASSERT_VER
#ifndef WRITE_TRACE
	#include "Libraries/Logging/Logging.h"
#endif

#define TRACE_UNALIGNED do {} while (0)

#define CHECK_ALIGNMENT(ptr, sz) \
		do { \
			ULONG_PTR la = (ULONG_PTR)ptr; \
			if ((la ^ (la + sz - 1)) & ~(CACHELINE_SIZE - 1)) { \
				WRITE_TRACE(DBG_FATAL, \
							"Unaligned atomic access cross cache line at la=0x%p (sz=%u), FILE=%s LINE=%u", \
							la, sz, __FILE__, __LINE__); \
				TRACE_UNALIGNED; \
			} else if ((la ^ (la + sz - 1)) & ~(sz - 1)) { \
				WRITE_TRACE(DBG_FATAL, "Unaligned atomic access at la=0x%p (sz=%u), FILE=%s LINE=%u", \
							la, sz, __FILE__, __LINE__); \
				TRACE_UNALIGNED; \
			} \
		} while(0)
#else
#define	CHECK_ALIGNMENT(ptr, sz) do {} while(0)
#endif

//
// NOTE: volatile guarantees that value is not cached by the compiler
//
#define defineAtomicRead32(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(const argtype *src) \
{ \
	return *(volatile argtype *)src; \
}

#define defineAtomicWrite32(name, op_ignored, argtype) \
__inline void Atomic##name(argtype *dst, argtype write) \
{ \
	*(volatile argtype *)dst = write; \
}

#if defined (_AMD64_)
#define defineAtomicRead64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(const argtype *src) \
{ \
	return *(volatile argtype *)src; \
}

#define defineAtomicWrite64(name, op_ignored, argtype) \
__inline void Atomic##name(argtype *dst, argtype write) \
{ \
	*(volatile argtype *)dst = write; \
}
#endif

#if defined(_MSC_VER)
// Microsoft Visual C path

#pragma warning(push)
// suppress "no return value"
// assemply inlines do return [edx:]eax per register convention
#pragma warning(disable:4035)

#define atomicInt64 __int64

//
// NOTE: __fastcall interferes with inline assembly in VC6
//
// NOTE: xchg is always locking
//
#if defined (_X86_)

#define defineAtomicOp8(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov ecx, dst \
    __asm mov al, [ecx] \
__asm retry: \
    __asm mov dl, delta \
    __asm op dl, al \
    __asm lock cmpxchg [ecx], dl \
    __asm jnz retry \
}

#define defineAtomicSwap32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype swap) \
{ \
    __asm mov ecx, dst \
    __asm mov eax, swap \
    __asm xchg [ecx], eax \
}

#define defineAtomicCompareSwap32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
    __asm mov ecx, dst \
    __asm mov eax, compare \
    __asm mov edx, swap \
    __asm lock cmpxchg [ecx], edx \
}

#define defineAtomicOp32(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov ecx, dst \
    __asm mov eax, [ecx] \
__asm retry: \
    __asm mov edx, delta \
    __asm op edx, eax \
    __asm lock cmpxchg [ecx], edx \
    __asm jnz retry \
}

#define defineAtomicAdd32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov ecx, dst \
    __asm mov eax, delta \
    __asm lock xadd [ecx], eax \
}

#define defineAtomicInc32(name, op_delta, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm mov ecx, dst \
    __asm mov eax, op_delta \
    __asm lock xadd [ecx], eax \
}

#define defineAtomicDec32(name, op_delta, argtype) \
defineAtomicInc32(name, op_delta, argtype)

#define defineAtomicIncAndTest32(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm xor eax, eax \
    __asm mov ecx, dst \
    __asm lock inc dword ptr [ecx] \
    __asm sete al \
}

#define defineAtomicDecAndTest32(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm xor eax, eax \
    __asm mov ecx, dst \
    __asm lock dec dword ptr [ecx] \
    __asm sete al \
}

//
// base 64bit type
// not a typedef because we'll be using `unsigned`
// modifier with it
//


//
// if *dst happens to be equal to edx:eax the same value
// will be swapped in breaking nothing
//
#define defineAtomicRead64asm(src) \
    __asm mov esi, src \
    __asm mov ebx, eax \
    __asm mov ecx, edx \
    __asm lock cmpxchg8b [esi]

#define defineAtomicRead64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(const argtype *src) \
{ \
    defineAtomicRead64asm(src) \
}

#define defineAtomicSwap64asm(dst, swap) \
	defineAtomicRead64asm(dst) \
	__asm mov ebx, dword ptr swap \
	__asm mov ecx, dword ptr swap+4 \
__asm retry: \
	__asm lock cmpxchg8b [esi] \
	__asm jnz retry

#define defineAtomicSwap64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype swap) \
{ \
	defineAtomicSwap64asm(dst, swap) \
}

#define defineAtomicWrite64(name, op_ignored, argtype) \
__inline void __stdcall \
Atomic##name(argtype *dst, argtype write) \
{ \
	defineAtomicSwap64asm(dst, write) \
}

#define defineAtomicCompareSwap64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
    __asm mov esi, dst \
    __asm mov eax, dword ptr compare \
    __asm mov edx, dword ptr compare+4 \
    __asm mov ebx, dword ptr swap \
    __asm mov ecx, dword ptr swap+4 \
    __asm lock cmpxchg8b [esi] \
}

#define defineAtomicOp64(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    defineAtomicRead64asm(dst) \
__asm retry: \
    __asm mov ebx, dword ptr delta \
    __asm mov ecx, dword ptr delta+4 \
    __asm op ebx, eax \
    __asm op ecx, edx \
    __asm lock cmpxchg8b [esi] \
    __asm jnz retry \
}

//
// need to handle carry
//
#define defineAtomicAdd64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    defineAtomicRead64asm(dst) \
__asm retry: \
    __asm mov ebx, dword ptr delta \
    __asm mov ecx, dword ptr delta+4 \
    __asm add ebx, eax \
    __asm adc ecx, edx \
    __asm lock cmpxchg8b [esi] \
    __asm jnz retry \
}

#define defineAtomicInc64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    defineAtomicRead64asm(dst) \
__asm retry: \
    __asm mov ebx, eax \
    __asm mov ecx, edx \
    __asm add ebx, 1 \
    __asm adc ecx, 0 \
    __asm lock cmpxchg8b [esi] \
    __asm jnz retry \
}

#define defineAtomicDec64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    defineAtomicRead64asm(dst) \
__asm retry: \
    __asm mov ebx, eax \
    __asm mov ecx, edx \
    __asm sub ebx, 1 \
    __asm sbb ecx, 0 \
    __asm lock cmpxchg8b [esi] \
    __asm jnz retry \
}

#define defineAtomicIncAndTest64(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    defineAtomicRead64asm(dst) \
__asm retry: \
    __asm mov ebx, eax \
    __asm mov ecx, edx \
    __asm add ebx, 1 \
    __asm adc ecx, 0 \
    __asm xor edi, edi \
    __asm or  edi, ebx \
    __asm or  edi, ecx \
    __asm lock cmpxchg8b [esi] \
    __asm jnz retry \
    __asm xor edx, edx \
    __asm xor eax, eax \
    __asm test edi, edi \
    __asm sete al \
}

#define defineAtomicDecAndTest64(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    defineAtomicRead64asm(dst) \
__asm retry: \
    __asm mov ebx, eax \
    __asm mov ecx, edx \
    __asm sub ebx, 1 \
    __asm sbb ecx, 0 \
    __asm xor edi, edi \
    __asm or  edi, ebx \
    __asm or  edi, ecx \
    __asm lock cmpxchg8b [esi] \
    __asm jnz retry \
    __asm xor eax, eax \
    __asm xor edx, edx \
    __asm test edi, edi \
    __asm sete al \
}

#elif defined (_AMD64_)
// AMD 64 version

#if defined (__INTEL_COMPILER)

#define defineAtomicOp8(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov rcx, dst \
    __asm mov al, byte ptr [rcx] \
__asm retry: \
    __asm mov dl, delta \
    __asm op dl, al \
    __asm lock cmpxchg byte ptr [rcx], dl \
    __asm jnz retry \
}

#define defineAtomicSwap32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype swap) \
{ \
    __asm mov rcx, dst \
    __asm mov eax, swap \
    __asm xchg dword ptr [rcx], eax \
}

#define defineAtomicCompareSwap32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
    __asm mov rcx, dst \
    __asm mov eax, compare \
    __asm mov edx, swap \
    __asm lock cmpxchg dword ptr [rcx], edx \
}

#define defineAtomicOp32(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov rcx, dst \
    __asm mov eax, dword ptr [rcx] \
__asm retry: \
    __asm mov edx, delta \
    __asm op edx, eax \
    __asm lock cmpxchg dword ptr [rcx], edx \
    __asm jnz retry \
}

#define defineAtomicAdd32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov rcx, dst \
    __asm mov eax, delta \
    __asm lock xadd dword ptr [rcx], eax \
}

#define defineAtomicInc32(name, op_delta, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm mov rcx, dst \
    __asm mov eax, op_delta \
    __asm lock xadd dword ptr [rcx], eax \
}

#define defineAtomicDec32(name, op_delta, argtype) \
defineAtomicInc32(name, op_delta, argtype)

#define defineAtomicIncAndTest32(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm xor rax, rax \
    __asm mov rcx, dst \
    __asm lock inc dword ptr [rcx] \
    __asm sete al \
}

#define defineAtomicDecAndTest32(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm xor rax, rax \
    __asm mov rcx, dst \
    __asm lock dec dword ptr [rcx] \
    __asm sete al \
}

//
// base 64bit type
// not a typedef because we'll be using `unsigned`
// modifier with it
//

#define defineAtomicSwap64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype swap) \
{ \
	__asm mov rsi, dst \
    __asm mov rax, qword ptr swap   \
    __asm xchg [rsi], rax \
}

#define defineAtomicCompareSwap64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
    __asm mov rsi, dst \
    __asm mov rax, qword ptr compare \
    __asm mov rbx, qword ptr swap \
    __asm lock cmpxchg [rsi], rbx \
}

#define defineAtomicOp64(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov rsi, dst \
    __asm mov rax, [rsi] \
__asm retry: \
    __asm mov rbx, qword ptr delta \
    __asm op rbx, rax \
    __asm lock cmpxchg [rsi], rbx \
    __asm jnz retry \
}

//
// need to handle carry
//
#define defineAtomicAdd64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
    __asm mov rsi, dst \
    __asm mov rax, qword ptr delta \
    __asm lock xadd qword ptr [rsi], rax \
}

#define defineAtomicInc64(name, op_delta, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm mov rsi, dst \
    __asm mov rax, op_delta \
    __asm lock xadd qword ptr [rsi], rax \
}

#define defineAtomicDec64(name, op_delta, argtype) \
defineAtomicInc64(name, op_delta, argtype)

#define defineAtomicIncAndTest64(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm xor rax, rax \
    __asm mov rsi, dst \
    __asm lock inc qword ptr [rsi] \
    __asm sete al \
}

#define defineAtomicDecAndTest64(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
    __asm xor rax, rax \
    __asm mov rsi, dst \
    __asm lock dec qword ptr [rsi] \
    __asm sete al \
}

#else // __INTEL_COMPILER


// intrin.h provides definitions of _Interlock-functions.
// This header is provided by msvc2005 and it is absent in WinDDK 6000/
// _Interlock-functions are also defined in winbase.h which is included
// in our WinDDK build env somewhere before and we don't need intrin.h.
#if !defined(WINBASEAPI) && !defined(_NTDDK_)
// These defs are workaround for incompatibilities between declarations in
// intrin.h from VC and winnt.h from W7SDK. These incompatibilities break
// compilation in mixed environment.
#define _interlockedbittestandset _interlockedbittestandset_workaround
#define _interlockedbittestandreset _interlockedbittestandreset_workaround
#define _interlockedbittestandset64 _interlockedbittestandset64_workaround
#define _interlockedbittestandreset64 _interlockedbittestandreset64_workaround
#include <intrin.h>
#undef _interlockedbittestandset
#undef _interlockedbittestandreset
#undef _interlockedbittestandset64
#undef _interlockedbittestandreset64

#else

// msvc2005 doesn't have defs of InterlockedAnd8U/Or8U in winbase.h.
#if (_MSC_VER < 1500)

#ifdef __cplusplus
  extern "C" {
#endif

char _InterlockedOr8(char volatile *, char);
char _InterlockedAnd8(char volatile *, char);

#ifdef __cplusplus
  }
#endif

#endif // _MSC_VER < 1500

#endif // WINBASEAPI

#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchange64)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedOr)
#pragma intrinsic(_InterlockedOr8)
#pragma intrinsic(_InterlockedOr64)
#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedAnd8)
#pragma intrinsic(_InterlockedAnd64)
#pragma intrinsic(_InterlockedXor)
#pragma intrinsic(_InterlockedXor64)
#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedExchangeAdd64)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedIncrement64)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedDecrement64)

#if defined (_X86_)

#define defineAtomicRead64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *src) \
{ \
	return (argtype)_InterlockedCompareExchange64((volatile __int64*)src, 0, 0); \
}

#define defineAtomicWrite64(name, op_ignored, argtype) \
__inline void Atomic##name(argtype *dst, argtype write) \
{ \
	_InterlockedExchange64((volatile __int64*)dst, (__int64)write); \
}

#endif

#define defineAtomicSwapSpecial(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype swap) \
{ \
	return (argtype)_InterlockedExchange64(reinterpret_cast<volatile __int64*>(dst), reinterpret_cast<__int64>(swap)); \
}

#define defineAtomicCompareSwapSpecial(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
	return (argtype)_InterlockedCompareExchange64(reinterpret_cast<volatile __int64*>(dst), reinterpret_cast<__int64>(swap), reinterpret_cast<__int64>(compare)); \
}

#define defineAtomicOp8(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
	return (argtype)_Interlocked##op##8((volatile char*)dst, (char)delta); \
}

#define defineAtomicSwap32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype swap) \
{ \
	return (argtype)_InterlockedExchange((volatile long*)dst, (long)swap); \
}

#define defineAtomicCompareSwap32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
	return (argtype)_InterlockedCompareExchange((volatile long*)dst, (long)swap, (long)compare); \
}

#define defineAtomicOp32(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
	return (argtype)_Interlocked##op((volatile long*)dst, (long)delta); \
}

#define defineAtomicAdd32(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
	return (argtype)_InterlockedExchangeAdd((volatile long*)dst, (long)delta); \
}

#define defineAtomicInc32(name, op_delta, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)(_InterlockedIncrement((volatile long*)dst) - 1); \
}

#define defineAtomicDec32(name, op_delta, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)(_InterlockedDecrement((volatile long*)dst) + 1); \
}

#define defineAtomicIncAndTest32(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)_InterlockedIncrement((volatile long*)dst) ? 0 : 1; \
}

#define defineAtomicDecAndTest32(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)_InterlockedDecrement((volatile long*)dst) ? 0 : 1; \
}

#define defineAtomicSwap64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype swap) \
{ \
	return (argtype)_InterlockedExchange64((volatile __int64*)dst, (__int64)swap); \
}

#define defineAtomicCompareSwap64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
	return (argtype)_InterlockedCompareExchange64((volatile __int64*)dst, (__int64)swap, (__int64)compare); \
}

#define defineAtomicOp64(name, op, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
	return (argtype)_Interlocked##op##64((volatile __int64*)dst, (__int64)delta); \
}

#define defineAtomicAdd64(name, op_ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst, argtype delta) \
{ \
	return (argtype)_InterlockedExchangeAdd64((volatile __int64*)dst, (__int64)delta); \
}

#define defineAtomicInc64(name, op_delta, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)(_InterlockedIncrement64((volatile __int64*)dst) - 1); \
}

#define defineAtomicDec64(name, op_delta, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)(_InterlockedDecrement64((volatile __int64*)dst) + 1); \
}

#define defineAtomicIncAndTest64(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)_InterlockedIncrement64((volatile __int64*)dst) ? 0 : 1; \
}

#define defineAtomicDecAndTest64(name, ignored, argtype) \
__inline argtype __stdcall \
Atomic##name(argtype *dst) \
{ \
	return (argtype)_InterlockedDecrement64((volatile __int64*)dst) ? 0 : 1; \
}

#endif

#else
    #error ("Unsupported arch")
#endif





#elif defined(__GNUC__) && (defined(_X86_) || defined(_AMD64_))
// GCC path

//
// NOTE: earlyclobber outputs to avoid allocation clashes
// NOTE: output "=&r"(*dst) because it is used in a loop
// NOTE: "memory" clobber to mark memory barrier for compiler

#define defineAtomicOp8(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
    argtype oldval, newval; \
\
    __asm__ __volatile__ ( \
        "mov %2, %0\n" \
    "1:\n\t" \
        "mov %3, %1\n\t" \
        #op " %0, %1\n\t" \
        "lock \n\t" \
        "cmpxchg %1, %2\n\t" \
        "jnz 1b\n\t" \
        : "=&a"(oldval), "=&r"(newval), "+m"(*dst) \
        : "g"(delta) \
        : "memory"); \
    return oldval; \
}

//
// NOTE: xchg is always locking
//
#define defineAtomicSwap32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype swap) \
{ \
    argtype oldval; \
\
    __asm__ __volatile__ ( \
        "xchg %2, %1\n\t" \
        : "=a"(oldval), "+m"(*dst) \
        : "0"(swap) \
        : "memory"); \
    return oldval; \
}

#define defineAtomicCompareSwap32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
    argtype oldval; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "cmpxchg %3, %1\n\t" \
        : "=a"(oldval), "+m"(*dst) \
        : "0"(compare), "r"(swap) \
        : "memory"); \
    return oldval; \
}

#define defineAtomicOp32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
    argtype oldval, newval; \
\
    __asm__ __volatile__ ( \
        "mov %2, %0\n" \
    "1:\n\t" \
        "mov %3, %1\n\t" \
        #op " %0, %1\n\t" \
        "lock \n\t" \
        "cmpxchg %1, %2\n\t" \
        "jnz 1b\n\t" \
        : "=&a"(oldval), "=&r"(newval), "+m"(*dst) \
        : "g"(delta) \
        : "memory"); \
    return oldval; \
}

#define defineAtomicAdd32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
    argtype oldval; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "xadd %0, %1\n\t" \
        : "=a"(oldval), "+m"(*dst) \
        : "0"(delta) \
        : "memory"); \
    return oldval; \
}

#define defineAtomicInc32(name, op_delta, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    argtype oldval; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "xadd %0, %1\n\t" \
        : "=a"(oldval), "+m"(*dst) \
        : "0"((argtype)op_delta) \
        : "memory"); \
    return oldval; \
}

#define defineAtomicDec32(name, op_delta, argtype) \
defineAtomicInc32(name, op_delta, argtype)

#define defineAtomicIncAndTest32(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    unsigned char c; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "incl %1\n\t" \
        "sete %0\n\t" \
        : "=a"(c), "+m"(*dst) \
        : \
        : "memory"); \
    return c; \
}

#define defineAtomicDecAndTest32(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    unsigned char c; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "decl %1\n\t" \
        "sete %0\n\t" \
        : "=a"(c), "+m"(*dst) \
        : \
        : "memory"); \
    return c; \
}

//
// base 64bit type
// not a typedef because we'll be using `unsigned`
// modifier with it
//
#define atomicInt64 long long


#if defined(_X86_)

//
// if *dst happens to be equal to edx:eax the same value
// will be swapped in breaking nothing
//
#define defineAtomicRead64asm(src, val) \
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
        "movl %%eax, %%ebx\n\t" \
        "movl %%edx, %%ecx\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%%esi)\n\t" \
        "popl %%ebx\n\t" \
        : "=&A"(val) \
        : "S"(src) \
        : /*"ebx",*/ "ecx")
// NOTE: We can't mark ebx as clobbered here, cause we get an
// error: PIC register 'ebx' clobbered in 'asm' on MacOS

#define defineAtomicRead64(name, op, argtype) \
__inline argtype \
Atomic##name(const argtype *src) \
{ \
    argtype val; \
\
    defineAtomicRead64asm(src, val); \
    __asm__ __volatile__ ("" : : : "memory"); \
    return val; \
}

#define defineAtomicSwap64asm(dst, oldval, swap) \
	defineAtomicRead64asm(dst, oldval); \
	__asm__ __volatile__ ( \
		"pushl %%ebx\n\t" \
		"1:\n\t" \
		"movl (%%edi), %%ebx\n\t" \
		"movl 4(%%edi), %%ecx\n\t" \
		"lock \n\t" \
		"cmpxchg8b (%%esi)\n\t" \
		"jnz 1b\n\t" \
		"popl %%ebx\n\t" \
		: "=&A"(oldval) \
		: "0"(oldval), "S"(dst), "D"(&swap) \
		: /*"ebx",*/ "ecx", "memory")

#define defineAtomicSwap64(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype swap) \
{ \
    argtype oldval; \
	defineAtomicSwap64asm(dst, oldval, swap); \
    return oldval; \
}

#define defineAtomicWrite64(name, op, argtype) \
__inline void __stdcall \
Atomic##name(argtype *dst, argtype write) \
{ \
	argtype oldval; \
	defineAtomicSwap64asm(dst, oldval, write); \
}

#define defineAtomicCompareSwap64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
    argtype oldval; \
\
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
        "movl (%%edi), %%ebx\n\t" \
        "movl 4(%%edi), %%ecx\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%2)\n\t" \
        "popl %%ebx\n\t" \
        : "=A"(oldval)  \
        : "0"(compare), "S"(dst), "D"(&swap) \
        : /*"ebx",*/ "ecx", "memory"); \
    return oldval; \
}

#define defineAtomicOp64(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
    argtype oldval; \
\
    defineAtomicRead64asm(dst, oldval); \
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
    "1:\n\t" \
        "movl (%%edi), %%ebx\n\t" \
        "movl 4(%%edi), %%ecx\n\t" \
        #op " %%eax, %%ebx\n\t" \
        #op " %%edx, %%ecx\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%%esi)\n\t" \
        "jnz 1b\n\t" \
        "popl %%ebx\n\t" \
        : "=&A"(oldval) \
        : "0"(oldval), "S"(dst), "D"(&delta) \
        : /*"ebx",*/ "ecx", "memory"); \
    return oldval; \
}

#define defineAtomicAdd64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
    argtype oldval; \
\
    defineAtomicRead64asm(dst, oldval); \
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
    "1:\n\t" \
        "movl (%%edi), %%ebx\n\t" \
        "movl 4(%%edi), %%ecx\n\t" \
        "addl %%eax, %%ebx\n\t" \
        "adcl %%edx, %%ecx\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%%esi)\n\t" \
        "jnz 1b\n\t" \
        "popl %%ebx\n\t" \
        : "=&A"(oldval) \
        : "0"(oldval), "S"(dst), "D"(&delta) \
        : /*"ebx",*/ "ecx", "memory"); \
    return oldval; \
}

#define defineAtomicInc64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    argtype oldval; \
\
    defineAtomicRead64asm(dst, oldval); \
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
    "1:\n\t" \
        "movl %%eax, %%ebx\n\t" \
        "movl %%edx, %%ecx\n\t" \
        "addl $1, %%ebx\n\t" \
        "adcl $0, %%ecx\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%%esi)\n\t" \
        "jnz 1b\n\t" \
        "popl %%ebx\n\t" \
        : "=&A"(oldval) \
        : "0"(oldval), "S"(dst) \
        : /*"ebx",*/ "ecx", "memory"); \
    return oldval; \
}

#define defineAtomicDec64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    argtype oldval; \
\
    defineAtomicRead64asm(dst, oldval); \
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
    "1:\n\t" \
        "movl %%eax, %%ebx\n\t" \
        "movl %%edx, %%ecx\n\t" \
        "subl $1, %%ebx\n\t" \
        "sbbl $0, %%ecx\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%%esi)\n\t" \
        "jnz 1b\n\t" \
        "popl %%ebx\n\t" \
        : "=&A"(oldval) \
        : "0"(oldval), "S"(dst) \
        : /*"ebx",*/ "ecx", "memory"); \
    return oldval; \
}


#define defineAtomicIncAndTest64(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    argtype tmp; \
\
    defineAtomicRead64asm(dst, tmp); \
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
    "1:\n\t" \
        "movl %%eax, %%ebx\n\t" \
        "movl %%edx, %%ecx\n\t" \
        "addl $1, %%ebx\n\t" \
        "adcl $0, %%ecx\n\t" \
        "xor   %%edi, %%edi\n\t" \
        "or   %%ebx, %%edi\n\t" \
        "or   %%ecx, %%edi\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%%esi)\n\t" \
        "jnz 1b\n\t" \
        "xorl %%eax, %%eax\n\t" \
        "testl %%edi, %%edi\n\t" \
        "sete %%al\n\t" \
        "popl %%ebx\n\t" \
        : "=&A"(tmp) \
        : "0"(tmp), "S"(dst) \
        : /*"ebx",*/ "ecx", "edi", "memory"); \
    return (tmp & 0xff); \
}

#define defineAtomicDecAndTest64(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    argtype tmp; \
\
    defineAtomicRead64asm(dst, tmp); \
    __asm__ __volatile__ ( \
        "pushl %%ebx\n\t" \
    "1:\n\t" \
        "movl %%eax, %%ebx\n\t" \
        "movl %%edx, %%ecx\n\t" \
        "subl $1, %%ebx\n\t" \
        "sbbl $0, %%ecx\n\t" \
        "xor   %%edi, %%edi\n\t" \
        "or   %%ebx, %%edi\n\t" \
        "or   %%ecx, %%edi\n\t" \
        "lock \n\t" \
        "cmpxchg8b (%%esi)\n\t" \
        "jnz 1b\n\t" \
        "xorl %%eax, %%eax\n\t" \
        "testl %%edi, %%edi\n\t" \
        "sete %%al\n\t" \
        "popl %%ebx\n\t" \
        : "=&A"(tmp) \
        : "0"(tmp), "S"(dst) \
        : /*"ebx",*/ "ecx", "edi", "memory"); \
    return (tmp & 0xff); \
}

#elif defined(_AMD64_) // && defined(__GNUC__)


#define defineAtomicSwap64(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype swap) \
{ \
	argtype oldval; \
	__asm__ __volatile__ ( \
		"xchgq %2, %1\n\t" \
		: "=a"(oldval), "+m"(*dst) \
		: "0"(swap) \
		: "memory"); \
    return oldval; \
}

#define defineAtomicCompareSwap64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
	argtype oldval; \
\
	__asm__ __volatile__ ( \
		"lock \n\t" \
		"cmpxchgq %3, %1\n\t" \
		: "=a"(oldval), "+m"(*dst)  \
		: "0"(compare), "r"(swap) \
		: "memory"); \
	return oldval; \
}

#define defineAtomicOp64(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
    argtype oldval, newval; \
\
	__asm__ __volatile__ ( \
		"movq %2, %0\n" \
	"1:\n\t" \
		"movq %3, %1\n\t" \
		#op " %0, %1\n\t" \
		"lock \n\t" \
		"cmpxchgq %1, %2\n\t" \
		"jnz 1b\n\t" \
		: "=&a"(oldval), "=&r"(newval), "+m"(*dst) \
		: "g"(delta) \
		: "memory"); \
	return oldval; \
}

#define defineAtomicAdd64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
    argtype val; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "xaddq %%rax, %1\n\t" \
        : "=a"(val), "+m"(*dst) \
        : "0"(delta) \
        : "memory"); \
    return val; \
}

#define defineAtomicInc64(name, op_delta, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    argtype val; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "xaddq %%rax, %1\n\t" \
        : "=a"(val), "+m"(*dst) \
        : "0"((argtype)op_delta) \
        : "memory"); \
    return val; \
}

#define defineAtomicDec64(name, op_delta, argtype) \
defineAtomicInc64(name, op_delta, argtype)

#define defineAtomicIncAndTest64(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    unsigned char c; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "incq %1\n\t" \
        "sete %0\n\t" \
        : "=a"(c), "+m"(*dst) \
        : \
        : "memory"); \
    return c; \
}

#define defineAtomicDecAndTest64(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    unsigned char c; \
\
    __asm__ __volatile__ ( \
        "lock \n\t" \
        "decq %1\n\t" \
        "sete %0\n\t" \
        : "=a"(c), "+m"(*dst) \
        : \
        : "memory"); \
    return c; \
}

#else // defined(__GNUC__) && !defined(_X86_) && !defined(_AMD64_)

    #error ("Unsupported _GNUC_ arch")

#endif

#elif defined(__GNUC__) && defined(_ARM_)

#define defineAtomicSwap32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype newValue) \
{ \
    register argtype originalValue; \
    register int result; \
    asm volatile("0:\n" \
                 "ldrex %[originalValue], [%[dst]]\n" \
                 "strex %[result], %[newValue], [%[dst]]\n" \
                 "teq %[result], #0\n" \
                 "bne 0b\n" \
                 : [originalValue] "=&r" (originalValue), \
                   [result] "=&r" (result), \
                   "+m" (*dst) \
                 : [newValue] "r" (newValue), \
                   [dst] "r" (dst) \
                 : "cc", "memory"); \
    return originalValue; \
}

#define defineAtomicCompareSwap32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype expectedValue, argtype newValue) \
{ \
	register int temp; \
	register argtype result; \
	asm volatile("0: \n\t" \
				"ldrex %[result], [%[dst]]  \n\t" \
				"eors %[temp], %[result], %[expectedValue]  \n\t" \
				"itt eq \n\t" \
				"strexeq %[temp], %[newValue], [%[dst]]  \n\t" \
				"teqeq %[temp], #1  \n\t" \
				"beq 0b  \n\t" \
				: [result] "=&r" (result), \
				  [temp] "=&r" (temp), \
				"+m" (*dst) \
				: [dst] "r" (dst), \
				  [expectedValue] "r" (expectedValue), \
				  [newValue] "r" (newValue) \
				: "cc", "memory" \
	); \
	return result; \
}

// XXX
// NOT IMPLEMENTED YET
#define defineAtomicOp8(name, op, argtype)
#define defineAtomicOp32(name, op, argtype)
#define defineAtomicOp64(name, op, argtype)
#define defineAtomicDec64(name, op, argtype)
#define defineAtomicIncAndTest64(name, ignored, argtype)
#define defineAtomicDecAndTest64(name, ignored, argtype)

#define atomicInt64 long long

#if _ARMV_ >= 7

#define defineAtomicRead64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(const argtype *src) \
{ \
	CHECK_ALIGNMENT(src, 8); \
    unsigned int val0, val1; \
	asm volatile( \
				"@ AtomicRead64\n" \
				"ldrexd r0, r1, [%2]\n" \
				"mov %0, r0\n" \
				"mov %1, r1\n" \
				: "=&r" (val0), "=&r" (val1) \
				: "r" (src) \
				: "r0", "r1" \
	); \
    return (argtype)val0 | ((argtype)val1 << 32); \
}

#define defineAtomicWrite64(name, op_ignored, argtype) \
__inline void \
Atomic##name(argtype *dst, argtype write) \
{ \
	CHECK_ALIGNMENT(dst, 8); \
	unsigned int result; \
	asm volatile( \
				"@ AtomicWrite64\n" \
				"mov r0, %2\n" \
				"mov r1, %3\n" \
				"0:\n" \
				"ldrexd	r2, r3, [%1]\n" \
				"strexd	%0, r0, r1, [%1]\n" \
				"teq	%0, #0\n" \
				"bne	0b" \
				: "=&r" (result) \
				: "r" (dst), "r" ((unsigned int)write),  "r" ((unsigned int)(write>>32)) \
				: "r0", "r1", "r2", "r3", "memory"); \
}

#define defineAtomicAdd64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype delta) \
{ \
	CHECK_ALIGNMENT(dst, 8); \
    unsigned int val0, val1; \
	unsigned int result; \
\
	asm volatile( \
				"@ AtomicAdd64\n" \
				"0:\n" \
				"ldrexd	r0, r1, [%3]\n" \
				"adds	r2, r0, %4\n" \
				"adc	r3, r1, %5\n" \
				"strexd	%0, r2, r3, [%3]\n" \
				"teq	%0, #0\n" \
				"bne	0b\n" \
				"mov %1, r0\n" \
				"mov %2, r1\n" \
				: "=&r" (result), "=&r" (val0), "=&r" (val1) \
				: "r" (dst), "r" ((unsigned int)delta), "r" ((unsigned int)(delta>>32)) \
				: "r0", "r1", "r2", "r3", "memory"); \
    return (argtype)val0 | ((argtype)val1 << 32); \
}

#define defineAtomicInc64(name, op_ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
	CHECK_ALIGNMENT(dst, 8); \
    unsigned int val0, val1; \
	unsigned int result; \
\
	asm volatile( \
				"@ AtomicInc64\n" \
				"0:\n" \
				"ldrexd r0, r1, [%3]\n" \
				"adds r2, r0, #1\n" \
				"adc r3, r1, #0\n" \
				"strexd %0, r2, r3, [%3]\n" \
				"teq %0, #0\n" \
				"bne 0b\n" \
				"mov %1, r0\n" \
				"mov %2, r1\n" \
				: "=&r" (result), "=&r" (val0), "=&r" (val1) \
				: "r" (dst) \
				: "r0", "r1", "r2", "r3", "memory", "cc"); \
    return (argtype)val0 | ((argtype)val1 << 32); \
}

#define defineAtomicSwap64(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype swap) \
{ \
	CHECK_ALIGNMENT(dst, 8); \
	unsigned int val0, val1; \
\
	ArmDataMemoryBarrier(); \
\
	asm volatile( \
				"@ AtomicSwap64\n" \
				"0:\n" \
				"ldrexd r0, r1, [%2]\n" \
				"push {r0,r1}\n" \
				"mov r0, %3\n" \
				"mov r1, %4\n" \
				"strexd	%0, r0, r1, [%2]\n" \
				"teq %0, #0\n" \
				"pop {%0}\n" \
				"pop {%1}\n" \
				"bne 0b\n" \
				: "=&r" (val0), "=&r" (val1) \
				: "r" (dst), "r" ((unsigned int)swap), "r" ((unsigned int)(swap>>32)) \
				: "r0", "r1", "memory"); \
\
	ArmDataMemoryBarrier(); \
	return (argtype)val0 | ((argtype)val1 << 32); \
}

#define defineAtomicCompareSwap64(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype compare, argtype swap) \
{ \
	CHECK_ALIGNMENT(dst, 8); \
	unsigned int val0, val1; \
\
	ArmDataMemoryBarrier(); \
\
	asm volatile( \
				"@ AtomicCompareSwap64\n" \
				"0:\n" \
				"ldrexd r0, r1, [%2]\n" \
				"push {r0,r1}\n" \
				"teq r0, %3\n" \
				"teqeq r1, %4\n" \
				"mov r0, %5\n" \
				"mov r1, %6\n" \
				"strexdeq %0, r0, r1, [%2]\n" \
				"teq %0, #0\n" \
				"pop {%0}\n" \
				"pop {%1}\n" \
				"bne 0b\n" \
				: "=&r" (val0), "=&r" (val1) \
				: "r" (dst), \
				  "r" ((unsigned int)compare), "r" ((unsigned int)(compare>>32)), \
				  "r" ((unsigned int)swap), "r" ((unsigned int)(swap>>32)) \
				: "r0", "r1", "memory"); \
\
	ArmDataMemoryBarrier(); \
    return (argtype)val0 | ((argtype)val1 << 32); \
}

#else
	#error Unsupported arch
#endif

#define defineAtomicAdd32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst, argtype valueToAdd) \
{ \
    register argtype originalValue; \
    register argtype newValue; \
    register int result; \
    asm volatile("0:\n" \
                 "ldrex %[originalValue], [%[dst]]\n" \
                 "add %[newValue], %[originalValue], %[valueToAdd]\n" \
                 "strex %[result], %[newValue], [%[dst]]\n" \
                 "teq %[result], #0\n" \
                 "bne 0b\n" \
                 : [originalValue] "=&r" (originalValue), \
                   [newValue] "=&r" (newValue), \
                   [result] "=&r" (result), \
                   "+m" (*dst) \
                 : [valueToAdd] "r" (valueToAdd), \
                   [dst] "r" (dst) \
                 : "cc", "memory"); \
    return originalValue; \
}

#define defineAtomicInc32(name, op, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    register argtype originalValue; \
    register argtype newValue; \
    register int result; \
    asm volatile("0:\n" \
                 "ldrex %[originalValue], [%[dst]]\n" \
                 "add %[newValue], %[originalValue], #1\n" \
                 "strex %[result], %[newValue], [%[dst]]\n" \
                 "teq %[result], #0\n" \
                 "bne 0b\n" \
                 : [originalValue] "=&r" (originalValue), \
                   [newValue] "=&r" (newValue), \
                   [result] "=&r" (result), \
                   "+m" (*dst) \
                 : [dst] "r" (dst) \
                 : "cc", "memory"); \
    return originalValue; \
}


#define defineAtomicDec32(name, op_delta, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    register argtype originalValue; \
    register argtype newValue; \
    register int result; \
    asm volatile("0:\n" \
                 "ldrex %[originalValue], [%[dst]]\n" \
                 "sub %[newValue], %[originalValue], #1\n" \
                 "strex %[result], %[newValue], [%[dst]]\n" \
                 "teq %[result], #0\n" \
                 "bne 0b\n" \
                 : [originalValue] "=&r" (originalValue), \
                   [newValue] "=&r" (newValue), \
                   [result] "=&r" (result), \
                   "+m" (*dst) \
                 : [dst] "r" (dst) \
                 : "cc", "memory"); \
    return originalValue; \
}


#define defineAtomicIncAndTest32(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    register argtype newValue; \
    register int result; \
    asm volatile("0:\n" \
                 "ldrex %[newValue], [%[dst]]\n" \
                 "add %[newValue], %[newValue], #1\n" \
                 "strex %[result], %[newValue], [%[dst]]\n" \
                 "teq %[result], #0\n" \
                 "bne 0b\n" \
                 : [newValue] "=&r" (newValue), \
                   [result] "=&r" (result), \
                   "+m" (*dst) \
                 : [dst] "r" (dst) \
                 : "cc", "memory"); \
    return newValue == 0; \
}

#define defineAtomicDecAndTest32(name, ignored, argtype) \
__inline argtype \
Atomic##name(argtype *dst) \
{ \
    register argtype newValue; \
    register int result; \
    asm volatile("0:\n" \
                 "ldrex %[newValue], [%[dst]]\n" \
                 "sub %[newValue], %[newValue], #1\n" \
                 "strex %[result], %[newValue], [%[dst]]\n" \
                 "teq %[result], #0\n" \
                 "bne 0b\n" \
                 : [newValue] "=&r" (newValue), \
                   [result] "=&r" (result), \
                   "+m" (*dst) \
                 : [dst] "r" (dst) \
                 : "cc", "memory"); \
    return newValue == 0; \
}

#endif // __GNUC__


#ifdef __cplusplus
extern "C" {
#endif

// alignment check wrappers
#define alignment_check1_const(macro_name, func_name, arg, argtype, bits) \
macro_name(func_name##_unchecked, arg, argtype) \
\
__inline argtype Atomic##func_name(const argtype * arg0) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	return Atomic##func_name##_unchecked(arg0); \
}

#define alignment_check1(macro_name, func_name, arg, argtype, bits) \
macro_name(func_name##_unchecked, arg, argtype) \
\
__inline argtype Atomic##func_name(argtype * arg0) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	return Atomic##func_name##_unchecked(arg0); \
}

#define alignment_check2(macro_name, func_name, arg, argtype, bits) \
macro_name(func_name##_unchecked, arg, argtype) \
\
__inline argtype Atomic##func_name(argtype *arg0, argtype arg1) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	return Atomic##func_name##_unchecked(arg0, arg1); \
}

#define alignment_check2_noret(macro_name, func_name, arg, argtype, bits) \
macro_name(func_name##_unchecked, arg, argtype) \
\
__inline void Atomic##func_name(argtype *arg0, argtype arg1) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	Atomic##func_name##_unchecked(arg0, arg1); \
}

#define alignment_check3(macro_name, func_name, arg, argtype, bits) \
macro_name(func_name##_unchecked, arg, argtype) \
\
__inline argtype Atomic##func_name(argtype *arg0, argtype arg1, argtype arg2) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	return Atomic##func_name##_unchecked(arg0, arg1, arg2); \
}

// alignment check wrappers for templates
#define alignment_check1_T(macro_name, func_name, arg, T, bits) \
template<class T> \
macro_name(func_name##_unchecked, arg, T *) \
\
template<class T> \
__inline T * Atomic##func_name(const T * *arg0) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	return Atomic##func_name##_unchecked(arg0); \
}

#define alignment_check2_T(macro_name, func_name, arg, T, bits) \
template<class T> \
macro_name(func_name##_unchecked, arg, T *) \
\
template<class T> \
__inline T * Atomic##func_name(T * *arg0, T * arg1) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	return Atomic##func_name##_unchecked(arg0, arg1); \
}

#define alignment_check2_noret_T(macro_name, func_name, arg, T, bits) \
template<class T> \
macro_name(func_name##_unchecked, arg, T *) \
\
template<class T> \
__inline void Atomic##func_name(T * *arg0, T * arg1) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	Atomic##func_name##_unchecked(arg0, arg1); \
}

#define alignment_check3_T(macro_name, func_name, arg, T, bits) \
template<class T> \
macro_name(func_name##_unchecked, arg, T *) \
\
template<class T> \
__inline T * Atomic##func_name(T * *arg0, T * arg1, T * arg2) \
{ \
	CHECK_ALIGNMENT(arg0, bits/8); \
	return Atomic##func_name##_unchecked(arg0, arg1, arg2); \
}

#ifndef _ARM_

#define defineAtomicFamily(bits, argtype, suffix) \
alignment_check1_const(defineAtomicRead##bits, Read##suffix, mov, argtype, bits) \
alignment_check2_noret(defineAtomicWrite##bits, Write##suffix, mov, argtype, bits) \
alignment_check2(defineAtomicSwap##bits, Swap##suffix, xchg, argtype, bits) \
alignment_check3(defineAtomicCompareSwap##bits, CompareSwap##suffix, cmpxchg, argtype, bits) \
alignment_check2(defineAtomicOp##bits, And##suffix, And, argtype, bits) \
alignment_check2(defineAtomicOp##bits, Or##suffix, Or, argtype, bits) \
alignment_check2(defineAtomicOp##bits, Xor##suffix, Xor, argtype, bits) \
alignment_check2(defineAtomicAdd##bits, Add##suffix, xadd, argtype, bits) \
alignment_check1(defineAtomicInc##bits, Inc##suffix, +1, argtype, bits) \
alignment_check1(defineAtomicDec##bits, Dec##suffix, -1, argtype, bits) \
alignment_check1(defineAtomicIncAndTest##bits, IncAndTest##suffix, ignored, argtype, bits) \
alignment_check1(defineAtomicDecAndTest##bits, DecAndTest##suffix, ignored, argtype, bits)

#else

#define defineAtomicFamily(bits, argtype, suffix) \
defineAtomicRead##bits( Read##suffix, mov, argtype ) \
defineAtomicWrite##bits( Write##suffix, mov, argtype ) \
defineAtomicSwap##bits( Swap##suffix, xchg, argtype ) \
defineAtomicCompareSwap##bits( CompareSwap##suffix, cmpxchg, argtype ) \
defineAtomicAdd##bits( Add##suffix, xadd, argtype ) \
defineAtomicInc##bits( Inc##suffix, +1, argtype ) \
defineAtomicDec##bits( Dec##suffix, -1, argtype ) \
defineAtomicIncAndTest##bits( IncAndTest##suffix, ignored, argtype ) \
defineAtomicDecAndTest##bits( DecAndTest##suffix, ignored, argtype )

#endif

#define defineAtomicFamily2(bits, argtype) \
defineAtomicFamily(bits, argtype,)

defineAtomicFamily2(32, int)
defineAtomicFamily(32, unsigned, U)

defineAtomicOp8(And8U, And, unsigned char)
defineAtomicOp8(Or8U, Or, unsigned char)

//
// limited set of operations for `void *`
//
#if defined(_MSC_VER) && defined(_AMD64_) && !defined(__INTEL_COMPILER) && defined(__cplusplus)
alignment_check2(defineAtomicSwapSpecial, SwapPv, xchg, void *, 64)
alignment_check3(defineAtomicCompareSwapSpecial, CompareSwapPv, cmpxchg, void *, 64)
#else
alignment_check2(defineAtomicSwap32, SwapPv, xchg, void *, 32)
alignment_check3(defineAtomicCompareSwap32, CompareSwapPv, cmpxchg, void *, 32)
#endif

defineAtomicFamily(64, atomicInt64, 64)
defineAtomicFamily(64, unsigned atomicInt64, 64U)

#ifdef __cplusplus
} // extern "C"

//
// C++ overloads for types other than int
//
defineAtomicFamily2(32, unsigned)

//
// long modifier can not be used with 64bit types
//
defineAtomicFamily2(64, atomicInt64)
defineAtomicFamily2(64, unsigned atomicInt64)


//
// object pointer templated ops
//
#if defined(_AMD64_)
alignment_check1_T(defineAtomicRead64, Read, mov, T, 64)
alignment_check2_noret_T(defineAtomicWrite64, Write, mov, T, 64)
alignment_check2_T(defineAtomicSwap64, Swap, xchg, T, 64)
alignment_check3_T(defineAtomicCompareSwap64, CompareSwap, cmpxchg, T, 64)
#else
alignment_check1_T(defineAtomicRead32, Read, mov, T, 32)
alignment_check2_noret_T(defineAtomicWrite32, Write, mov, T, 32)
alignment_check2_T(defineAtomicSwap32, Swap, xchg, T, 32)
alignment_check3_T(defineAtomicCompareSwap32, CompareSwap, cmpxchg, T, 32)
#endif

template<class T>
__inline T*
AtomicAdd(T **dst, int delta)
{
    return reinterpret_cast<T*>(AtomicAdd(
        reinterpret_cast<int *>(dst), delta * sizeof(**dst)));
}
template<class T>
__inline T*
AtomicInc(T **dst)
{
    return reinterpret_cast<T*>(AtomicAdd(
        reinterpret_cast<int *>(dst), sizeof(**dst)));
}
template<class T>
__inline T*
AtomicDec(T **dst)
{
    return reinterpret_cast<T*>(AtomicAdd(
        reinterpret_cast<int *>(dst), -sizeof(**dst)));
}

#endif // __cplusplus


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#if defined(__GNUC__)
# undef __inline
#endif

#endif // ATOMICOPS_H_INCLUDED
