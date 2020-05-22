/*
 * ContextSwitcher.h: Class that let to execute Qt-based code in lib
 * event loop
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#pragma once

#include <QThread>
#include <QObject>
#include <QMetaObject>

#include "PP.h"

/**
* Allows to execute function(s) in main thread context (context switch)
*/
class ContextSwitcher : public QObject
{
	Q_OBJECT

	/**
	* IInvoke interface.
	* Call() function will be called in main thread context.
	*/
	class IInvoke
	{
	public:
		virtual ~IInvoke() {}
		virtual void Call() = 0;
	};

	/**
	* IInvoker template interface.
	*
	* Provides actual context switch (see Invoke(...) function),
	*
	* Inherit this interface, reimplement Call(), that must
	* save result in m_res member.
	*/
	template<class Tret>
	class IInvoker : public IInvoke
	{
	protected:
		Tret m_res;
	public:
		Tret Invoke(ContextSwitcher* pCtx)
		{
			bool bRes = QMetaObject::invokeMethod(pCtx, "MakeCtxCall", Qt::BlockingQueuedConnection,
				Q_ARG(IInvoke*, this));
			Q_ASSERT(bRes); Q_UNUSED(bRes);

			return m_res;
		}
	};

public:
	/**
	 * Default class constructor
	 */
	ContextSwitcher();

	/**
	 * EXAMPLE : Here we define template functions this like
	 */

	/*
	template<class Tret ,class Targ0 ,class Targ1 >
	Tret Invoke(Tret(*func)(Targ0, Targ1) ,Targ0 arg0 ,Targ1 arg1)
	{
		if (QThread::currentThread() != this->thread())
		{
			class Invoker : public IInvoker<Tret>
			{
				 Tret(*func)(Targ0, Targ1);
				 Targ0& arg0; Targ1& arg1;
				 virtual void Call()	{ IInvoker<Tret>::m_res = (*func)(arg0, arg1); }
			public:
				Invoker(Tret(*func)(Targ0, Targ1) ,Targ0& arg0 ,Targ1& arg1)
					: func(func) ,arg0(arg0), arg1(arg1) {}
			};

			return Invoker(func ,arg0, arg1).Invoke(this);
		}
		else
		{
			return (*func)(arg0, arg1);
		}
	}
	*/

#define CLASS_ARG_TYPE(N, d) ,class T##arg##N
#define TYPE_VAL_REF_MEMB(N, d) T##arg##N & arg##N ;
#define TYPE_VAL_REF_PARAM(N, d) ,T##arg##N & arg##N
#define MEMB_VAL_INIT(N, d) ,arg##N(arg##N)
#define PARAM_VAL_PASS(N, d) ,arg##N
#define NAME_COMMA_SEP(N, name) NAME_COMMA_SEP_I(N, name)
#define NAME_COMMA_SEP_I(N, name) PT_PP_COMMA_IF(N) name##N

/**
* Defines macro that defines template function Invoke().
* Invoke() crates temp object, that saves passed params,
* this object passed and called in MakeCtxCall(IInvoke*) via IInvoke* interface.
*/
#define DECL_INVOKE_FUNC_N_PARAMS(N, d) \
	template<class Tret PP_MAKE_##N (CLASS_ARG_TYPE, _) > \
	Tret Invoke(Tret(*func)(PP_MAKE_##N (NAME_COMMA_SEP, Targ)) PP_MAKE_##N (TYPE_VAL_REF_PARAM, _)) \
	{ \
		if (QThread::currentThread() != this->thread()) \
		{ \
			class Invoker : public IInvoker<Tret> \
			{ \
				Tret(*func)(PP_MAKE_##N (NAME_COMMA_SEP, Targ)); \
				PP_MAKE_##N (TYPE_VAL_REF_MEMB, _); \
				virtual void Call()	{ IInvoker<Tret>::m_res = (*func)(PP_MAKE_##N (NAME_COMMA_SEP, arg)); } \
				public: \
				Invoker(Tret(*func)(PP_MAKE_##N (NAME_COMMA_SEP, Targ)) PP_MAKE_##N (TYPE_VAL_REF_PARAM, _)) \
					: func(func) PP_MAKE_##N (MEMB_VAL_INIT, _) {} \
			}; \
			\
			return Invoker(func PP_MAKE_##N (PARAM_VAL_PASS, _)).Invoke(this); \
		} \
		else \
		{ \
			return (*func)(PP_MAKE_##N (NAME_COMMA_SEP, arg)); \
		} \
	}

	/**
	* Declare template Invoke(...) functions,
	* those accept up to 12 arguments.
	*/
	PP_MAKE_13(DECL_INVOKE_FUNC_N_PARAMS, _)

#undef DECL_INVOKE_FUNC_N_PARAMS
#undef NAME_COMMA_SEP_I
#undef NAME_COMMA_SEP
#undef PARAM_VAL_PASS
#undef MEMB_VAL_INIT
#undef TYPE_VAL_REF_PARAM
#undef TYPE_VAL_REF_MEMB
#undef CLASS_ARG_TYPE

private slots:
	/**
	* Function called from lib event loop (in main thread context).
	* This function calls passed IInvoke::Call();
	*/
	Q_SLOT void MakeCtxCall(IInvoke*);
};
