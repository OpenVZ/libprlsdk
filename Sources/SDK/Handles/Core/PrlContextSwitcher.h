/*
 * PrlContextSwitcher.h
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


#ifndef __PRL_CONTEXT_SWITCHER_H__
#define __PRL_CONTEXT_SWITCHER_H__

#include <QObject>
#include <QMutex>

#include "Libraries/ContextSwitcher/ContextSwitcher.h"
#include "SDK/Include/PrlTypes.h"
#include "SDK/Include/PrlErrors.h"
#include "SDK/Include/PrlEnums.h"
#include "PrlHandleJob.h"


#define ARGS_APPLY_I(macro, args) macro args
#define ARGS_APPLY(macro, args) ARGS_APPLY_I(macro, args)
#define ARGS_STRIP_PARENS_I(...) __VA_ARGS__
#define ARGS_STRIP_PARENS(x) ARGS_APPLY(ARGS_STRIP_PARENS_I, x)

#define CALL_THROUGH_CTXT_SWITCHER(ctxt_switcher, methodname, values)\
	return ctxt_switcher->Invoke(methodname##_Impl, ARGS_STRIP_PARENS(values));

typedef PRL_METHOD_PTR( PRL_EVENT_HANDLER_PTR ) (
		PRL_HANDLE hEvent,
		PRL_VOID_PTR data
		);

/**
 * Class lets to execute Qt-based code through SDK lib event loop to prevent issues
 * with signals-slots and event processing mech.
 */
class PrlContextSwitcher : public ContextSwitcher
{
Q_OBJECT

signals:
	/**
	 * Special signal that notifies to a thread that it should finalize work
	 */
	void LetFinishWorkNow();

protected slots:
	/**
	 * Slot that fireups from SDK event loop when last one started.
	 * It notifies all interesting persons (who call PrlApi_Init() at first) that SDK event loop was really started
	 */
	void SdkEventLoopStarted();

public:
	/** Singleton instance getter */
	static PrlContextSwitcher* Instance ();
	/** Singleton instance finalizator */
	static void DeinitInstance ();

	/** Event handler */
	bool event ( QEvent* evt );

public:
	/**
	 * Notifies all listeners that they should to finalize their work
	 */
	void NotifyWorkFinalization();

private:
	/**
	 * Default class constructor
	 */
	PrlContextSwitcher();

	/** Singleton instance */
	static PrlContextSwitcher* s_pContextSwitcher;
	/** Singleton instance mutex */
	static QMutex s_pContextSwitcherMutex;
};

#endif	// __PRL_CONTEXT_SWITCHER_H__
