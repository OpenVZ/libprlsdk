/*
 * PrlEventsHandler.h
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


#ifndef __PARALLELS_EVENTS_HANDLER_H__
#define __PARALLELS_EVENTS_HANDLER_H__

#include <QList>
#include <QHash>
#include <QPair>
#include <QMutex>
#include <QThread>

#include "PrlHandleBase.h"
#include "PrlCommon.h"
#include "BuiltinEventSource.h"

#include <prlcommon/Std/SmartPtr.h>

/**
 * Notification thread
 */
class CNotificationThread : public QThread, public CInstancesCounter
{
	Q_OBJECT
public:
	CNotificationThread ();
	~CNotificationThread ();
	void ExitNotification ( int retcode );

	bool isStarted() const;

signals:
	void ToNotifyVm(PrlHandleVmPtr, PrlHandleBasePtr);
	void ToNotifyServer(PrlHandleServerPtr, PrlHandleBasePtr);

public slots:
	void NotificationStarted ();
	void NotificationFinished ();
	/** Overloaded start for own purposes on start procedure (waiting for actual thread start) */
	void start ( Priority priority = InheritPriority );

private:
	void run ();

private:
	// Friend class to call signals
	friend class CEventsHandler;

	volatile bool m_bNotificationStarted;
};

/**
 * This class helps to start notification thread with QTimer
 */
class CNotificationMapper : public QObject
{
	Q_OBJECT
public:
	CNotificationMapper ( CNotificationThread* t );

public slots:
	void NotificationStarted ();

private:
	CNotificationThread* m_pThread;
};


/**
 * Server notifier object declaration
 */
class CServerNotifier : public QObject
{
	Q_OBJECT
public slots:
    void Notify(PrlHandleServerPtr, PrlHandleBasePtr);
};

/**
 * Vm notifier object declaration
 */
class CVmNotifier : public QObject
{
	Q_OBJECT
public slots:
    void Notify(PrlHandleVmPtr, PrlHandleBasePtr);
};

/**
 * Class for processing command events from the server.
 */
class CEventsHandler : public QObject
{
Q_OBJECT
public:
	/**
	 * Constructor - performs basic initialization of the object.
	 */
	CEventsHandler(PRL_HANDLE hServer = PRL_INVALID_HANDLE);

	/**
	 * Destructor
	 */
	~CEventsHandler();

	/**
	 * Stops notification thread
	 */
	void StopNotificationThread();

	/**
	 * Registers postponed notification for server object
	 */
	void RegisterNotification( PrlHandleServerPtr pServer, PrlHandleBasePtr pEvent );

	bool isStarted() const;

private:
	/**
	 * Handler of the events from the server.
	 */
	bool event(QEvent *pEvent);

public:
	/// Handle to the server object
	PRL_HANDLE m_ServerHandle;

	/** Notification thread */
	SmartPtr<CNotificationThread> m_NotificationThread;

public slots:
	/**
	 * Slot that maps on PrlContextSwitcher::LetFinishWorkNow() signal and finalize thread work
	 * directly from common finalization mech
	 */
	void FinalizeWork();

private:
	/** Notification thread removal mutex*/
	QMutex m_Mutex;
};

#endif // __PARALLELS_EVENTS_HANDLER_H__
