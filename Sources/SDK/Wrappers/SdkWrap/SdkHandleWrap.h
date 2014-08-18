/*
 * SdkHandleWrap.h: SDK handle wrapper incapsulates
 * PrlHandle_AddRef/PrlHandle_Free calls. Note: Non thread safe
 * implementation!
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


#ifndef __PARALLELS_SDK_HANDLE_WRAP_H__
#define __PARALLELS_SDK_HANDLE_WRAP_H__

#ifdef DYN_API_WRAP
#include "SdkWrap.h"
#else
#include "Parallels.h"
#endif

/**
 * Wrapper implementation. Implemented as template to be it possible use it
 * with SdkWrap or not.
 */
template <int>
class t_SdkHandleWrap
{
public:
	/**
	 * Constructor - to perform basic initialization.
	 */
	t_SdkHandleWrap()
	: m_Handle(PRL_INVALID_HANDLE)
	{}

	/**
	 * Constructor - initializes with specified handle.
	 * @param handle to storing in wrapper
	 *
	 * Keyword explicit here is on purpose - this will prevent errors like this:
	 *     void foo(const SdkHandleWrap &h)
	 *     {
	 *         // some code here
	 *         // note, that h will be released on return
	 *     }
	 *     void bar()
	 *     {
	 *         const PRL_HANDLE h = PrlFoo_GetBar();
	 *         foo(h); // compilation error here if explicit is used
	 *         // h will be released by foo() if explicit is NOT used
	 *     }
	 */
	explicit t_SdkHandleWrap(const PRL_HANDLE _handle)
	: m_Handle(_handle)
	{
	}

	/**
	 * Copy constructor.
	 * @param reference to the copying object
	 */
	t_SdkHandleWrap(const t_SdkHandleWrap &_obj)
	{
		assignHandle(_obj.get());
	}

	/**
	 * Destructor - PrlHandle_Free calling here.
	 */
	~t_SdkHandleWrap()
	{
		freeHandle();
	}

	/**
	 * Class assign operator implementation.
	 * @param reference to the copying object
	 */
	t_SdkHandleWrap &operator=(const t_SdkHandleWrap &_obj)
	{
		// Right implementation of self-assignment check.
		// Not a problem if this and _obj refers to the same handle,
		// since if they do, handle has minimum 2 references and after
		// decrementing ref count via this it will incremented back via _obj
		// inside reset() call.
		// Example of correct code:
		//     void bar()
		//     {
		//         const PRL_HANDLE h = PrlFoo_GetBar();
		//         const SdkHandleWrap w1(h); // h ref count is 1
		//         const SdkHandleWrap w2(w1); // h ref count is 2
		//         w1 = w2;
		//         // this.freeHandle(); // h ref count is 1
		//         // this.assigneHandle(_obj.get()); // h ref count is back to 2
		//         // Everything is OK.
		//     }
		// Example of incorrect code:
		//     void bar()
		//     {
		//         const PRL_HANDLE h = PrlFoo_GetBar();
		//         const SdkHandleWrap w1(h); // h ref count is 1
		//         const SdkHandleWrap w2(h); // h ref count is still 1
		//         // Here we have 2 references to h, but ref count is still 1,
		//         // so this code is WRONG. On return PrlHandle_Free will be called
		//         // 2 times instead of 1.
		//     }
		// Self-assigment check is important since
		//     w1 = w1;
		// must not free owned handle.
		// Check for this->m_Handle == _obj.m_Handle is completely useless.
		if (this != &_obj)
		{
			freeHandle();
			assignHandle(_obj.get());
		}
		return (*this);
	}

	/**
	 * Non const version of returning reference to the storing handle method
	 * @param reference to the storing handle
	 *
	 * This function calls PrlHandle_Free() on previously owned pointer.
	 * Value at returned pointer always will be PRL_INVALID_HANDLE.
	 * Read name of that function as ResetAndGetPtr().
	 */
	PRL_HANDLE* GetHandlePtr()
	{
		reset();
		return &m_Handle;
	}

	/**
	 * Const version of returning reference to the storing handle method
	 * @param reference to the storing handle
	 */
	PRL_HANDLE GetHandle() const
	{
		return m_Handle;
	}

	/**
	 * returns stored handle.
	 * Confirms to STL "smart" pointers interface.
	 */
	PRL_HANDLE get() const
	{
		return m_Handle;
	}

	/**
	 * Cast operator implementation
	 */
	operator PRL_HANDLE () const
	{
		return m_Handle;
	}

	bool valid() const
	{
		return m_Handle != PRL_INVALID_HANDLE;
	}

	/**
	 * release stored handle, without decrementing it count
	 * and return it. Confirms to STL "smart" pointers interface.
	 */
	PRL_HANDLE release()
	{
	   const PRL_HANDLE tmp = m_Handle;
	   m_Handle = PRL_INVALID_HANDLE;
	   return tmp;
	}

	/**
	 * resets stored handle, decrementing it count.
	 * Confirms to STL "smart" pointers interface.
	 */
	void reset(const PRL_HANDLE _handle = PRL_INVALID_HANDLE)
	{
		freeHandle();
		m_Handle = _handle;
	}
private:
	void freeHandle()
	{
		if (m_Handle != PRL_INVALID_HANDLE)
			PrlHandle_Free(m_Handle);
	}
	void assignHandle(const PRL_HANDLE _handle)
	{
		m_Handle = _handle;
		if (m_Handle != PRL_INVALID_HANDLE)
			PrlHandle_AddRef(m_Handle);
	}
	PRL_HANDLE m_Handle;
};

typedef t_SdkHandleWrap<0> SdkHandleWrap;

#endif // __SDK_MACRO_WRAPSPARALLELS_SDK_WRAP_H__
