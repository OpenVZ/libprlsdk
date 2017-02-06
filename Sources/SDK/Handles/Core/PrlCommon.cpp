/*
 * PrlCommon.cpp: Common SDK mechanisms
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


#include <QTimer>
#include <QMetaType>

#include <QTranslator>

#include "Build/Current.ver"

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/PrlCommonUtilsBase/ParallelsDirs.h>

#include "SDK/Include/PrlTypes.h"
#include "SDK/Include/PrlEnums.h"

#include "PrlCommon.h"
#include "PrlHandleOpTypeList.h"
#include "PrlContextSwitcher.h"
#include "PrlHandleServer.h"

#include <prlcommon/HostUtils/PrlMiscellaneous.h>
#include <prlcommon/PrlCommonUtilsBase/CGuestOsesHelper.h>

#include "PrlFileDescriptorsMech.h"

#ifndef _WIN_
#include <signal.h>
#include <errno.h>
#endif

/**
* Static pointer to the instance of the main QT application thread.
*/
static QMainThread* s_QMainThread = NULL;

PrlSdkThreadsDestructor *g_pThreadsDestructor = 0;

Q_GLOBAL_STATIC(QReadWriteLock, InitStatusLocker)

namespace {
/** Sign which specifies whether SDK initialized */
bool g_bSdkInitStatus = false;
}

PrlSdkStatusReader::PrlSdkStatusReader()
: m_lock(InitStatusLocker())
{}

bool PrlSdkStatusReader::IsSdkInitialized()
{
	return (g_bSdkInitStatus);
}

PrlSdkStatusWriter::PrlSdkStatusWriter()
{
	if ( QThread::currentThread() == s_QMainThread )
	{
		while ( ! InitStatusLocker()->tryLockForWrite() )
		{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 50);
#else
			QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers | QEventLoop::DeferredDeletion, 50);
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
		}
	}
	else
		InitStatusLocker()->lockForWrite();
}

PrlSdkStatusWriter::~PrlSdkStatusWriter()
{
	InitStatusLocker()->unlock();
}

void PrlSdkStatusWriter::SetInitStatus(bool bStatus)
{
	g_bSdkInitStatus = bStatus;
}

QAtomicInt CInstancesCounter::g_nInstancesNum = 0;

CInstancesCounter::CInstancesCounter()
{
	g_nInstancesNum.ref();
}

CInstancesCounter::~CInstancesCounter()
{
	g_nInstancesNum.deref();
}

quint32 CInstancesCounter::GetInstancesNum()
{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
	return (quint32(g_nInstancesNum.loadAcquire()));
#else
	return (quint32((int)g_nInstancesNum));
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
}

PrlSdkThreadsDestructor::PrlSdkThreadsDestructor()
: m_ThreadsListMutex(QMutex::Recursive)
{}

////////////////////////////////////////////////////////////////////////////////////////////////

// Static definition
PrlContextSwitcher* PrlContextSwitcher::s_pContextSwitcher = 0;
QMutex PrlContextSwitcher::s_pContextSwitcherMutex;

PrlContextSwitcher* PrlContextSwitcher::Instance ()
{
	QMutexLocker locker( &s_pContextSwitcherMutex );
	if ( ! s_pContextSwitcher ) {
		Q_ASSERT( QMainThread::Instance() == QThread::currentThread() );
		s_pContextSwitcher = new PrlContextSwitcher;
	}
	return s_pContextSwitcher;
}

void PrlContextSwitcher::DeinitInstance ()
{
	Q_ASSERT( QMainThread::Instance() == QThread::currentThread() );
	QMutexLocker locker( &s_pContextSwitcherMutex );
	delete s_pContextSwitcher;
	s_pContextSwitcher = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

PrlContextSwitcher::PrlContextSwitcher()
{}

void PrlSdkThreadsDestructor::RegisterThreadForDeletion(Heappy *thread_)
{
	QMutexLocker g(&m_ThreadsListMutex);
	m_heappies << thread_;
	m_lstThreadObjs.append(thread_);
}

void PrlSdkThreadsDestructor::RegisterThreadForDeletion(PrlThread *pThread)
{
	QMutexLocker _lock(&m_ThreadsListMutex);
	m_lstThreadObjs.append(pThread);
}

void PrlSdkThreadsDestructor::ProcessThreadsInstances()
{
	QMutexLocker _lock(&m_ThreadsListMutex);
	QObject* s = sender();
	m_lstThreadObjs.removeAll(s);
	if (0 < m_heappies.removeAll(s))
		s->deleteLater();

	if (m_lstThreadObjs.isEmpty())
		m_event.wakeOne();
}

void PrlSdkThreadsDestructor::WaitAllThreadsAndCleanup()
{
	QMutexLocker _lock(&m_ThreadsListMutex);
	if (!m_lstThreadObjs.isEmpty())
		m_event.wait(&m_ThreadsListMutex);
}

///////////////////////////////////////////////////////////////////////////////
// class PrlThread

PrlThread::PrlThread()
{
	Q_ASSERT(g_pThreadsDestructor);
	setStackSize(PRL_STACK_SIZE);
	CHECK_CONNECT_DONE_PROPERLY(connect(this, SIGNAL(finished()), g_pThreadsDestructor, SLOT(ProcessThreadsInstances()), Qt::QueuedConnection))
}

void PrlThread::run()
{
	g_pThreadsDestructor->RegisterThreadForDeletion(this);
	concreteRun();
}

void PrlThread::concreteRun()
{
}

///////////////////////////////////////////////////////////////////////////////
// struct Heappy

Heappy::Heappy()
{
	g_pThreadsDestructor->RegisterThreadForDeletion(this);
}

Heappy::~Heappy()
{
}

PRL_RESULT PrlGetSupportedOsesTypesList(PRL_HOST_OS_TYPE nHostOsType, PRL_HANDLE_PTR phOsesTypesList)
{
	PRL_HOST_OS_TYPE nProcessedHostOsType = CGuestOsesHelper::GetHostOsType(nHostOsType);
	if (PHO_UNKNOWN == nProcessedHostOsType)
		return (PRL_ERR_INVALID_ARG);

	TOpaqueTypeList<PRL_UINT8> *pOpaqueTypeList = new TOpaqueTypeList<PRL_UINT8>(CGuestOsesHelper::GetSupportedOsesTypes(nProcessedHostOsType));
	if (!pOpaqueTypeList)
		return (PRL_ERR_OUT_OF_MEMORY);

	PrlHandleOpTypeList *pOpaqueTypeHandle = new PrlHandleOpTypeList(pOpaqueTypeList);
	if (!pOpaqueTypeHandle)
	{
		delete pOpaqueTypeList;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	*phOsesTypesList = pOpaqueTypeHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlGetSupportedOsesVersionsList(PRL_HOST_OS_TYPE nHostOsType, PRL_UINT8 nGuestOsType, PRL_HANDLE_PTR phGuestsList)
{
	PRL_HOST_OS_TYPE nProcessedHostOsType = CGuestOsesHelper::GetHostOsType(nHostOsType);
	if (PHO_UNKNOWN == nProcessedHostOsType)
		return (PRL_ERR_INVALID_ARG);

	if (!CGuestOsesHelper::CheckGuestOsType(nGuestOsType))
		return (PRL_ERR_INVALID_ARG);

	TOpaqueTypeList<PRL_UINT16> *pOpaqueTypeList = new TOpaqueTypeList<PRL_UINT16>(CGuestOsesHelper::GetSupportedOsesVersions(nProcessedHostOsType, nGuestOsType));
	if (!pOpaqueTypeList)
		return (PRL_ERR_OUT_OF_MEMORY);

	PrlHandleOpTypeList *pOpaqueTypeHandle = new PrlHandleOpTypeList(pOpaqueTypeList);
	if (!pOpaqueTypeHandle)
	{
		delete pOpaqueTypeList;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	*phGuestsList = pOpaqueTypeHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlGetDefaultOsVersion(PRL_UINT8 nGuestOsType, PRL_UINT16_PTR pnOsVersion)
{
	if (!CGuestOsesHelper::CheckGuestOsType(nGuestOsType))
		return (PRL_ERR_INVALID_ARG);

	*pnOsVersion = CGuestOsesHelper::GetDefaultOsVersion(nGuestOsType);
	return (PRL_ERR_SUCCESS);
}

QMainThread::QMainThread()
: m_bOwnerOfQApp(false), m_nFlags(0)
{
	setStackSize(PRL_STACK_SIZE);
}

void QMainThread::InitSignals()
{
#ifndef _WIN_
	//***********************************************
	struct sigaction _prev_handler;

	int ret = sigaction(SIGCHLD, NULL, &_prev_handler);
	if (ret == -1)
		WRITE_TRACE(DBG_FATAL, "An error %d occurred on sigaction() system call\n", errno);
	_prev_handler.sa_flags |= SA_RESTART;
	ret = sigaction(SIGCHLD, &_prev_handler, NULL);
	if (ret == -1)
		WRITE_TRACE(DBG_FATAL, "An error %d occurred on sigaction() system call\n", errno);
	//***********************************************
#endif
}

void QMainThread::InitContextSwitcher()
{
	PrlContextSwitcher::Instance();

	qRegisterMetaType<PrlHandleServerPtr>( "PrlHandleServerPtr" );
	qRegisterMetaType<PrlHandleVmPtr>( "PrlHandleVmPtr" );
	qRegisterMetaType<PrlHandleBasePtr>( "PrlHandleBasePtr" );

	//Registering slot notifies that SDK event loop started
	QTimer::singleShot(0, PrlContextSwitcher::Instance(), SLOT(SdkEventLoopStarted()));
}

void QMainThread::InitCommonThreadsDestructionMech()
{
	//Initialize common threads destructor mech
	g_pThreadsDestructor = new PrlSdkThreadsDestructor;
}

void QMainThread::DeinitSdkParts()
{
	PrlHandleServer::FinalizeAllTransportConnections();
	//Finalize all therads described to common finalization mech
	PrlContextSwitcher::Instance()->NotifyWorkFinalization();
	//Event loop work emulation in order to finalize correctly all SDK threads
	while (CInstancesCounter::GetInstancesNum())
	{
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 50);
#else
		QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers | QEventLoop::DeferredDeletion, 50);
#endif // QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
	}

	QMutexLocker _lock(&m_MainThreadMutex);
	m_MainThreadCondition.wakeAll();
	PrlContextSwitcher::DeinitInstance();

	//Copy Handle map under mutex to temporary list to prevent dead-locks ( Task #PM-1642 )
	//Loop until all handle spawning activities would be stopped.
	QList<SmartPtr<PrlHandleBase> > _lockFreeHandleList;
	while(true)
	{
		{
			QMutexLocker lock(PrlHandleBase::s_pHandlesMapMutex);
			if (PrlHandleBase::s_pHandlesMap->isEmpty())
				break;

			_lockFreeHandleList = PrlHandleBase::s_pHandlesMap->values();
			PrlHandleBase::s_pHandlesMap->clear();
		}
		//Destroy handles without mutex lock
		_lockFreeHandleList.clear();
	}
}

void QMainThread::InitSdkParts()
{
	InitSignals();
	InitContextSwitcher();
	InitCommonThreadsDestructionMech();
}

void QMainThread::run()
{
	if (!QCoreApplication::instance())
	{
		m_bOwnerOfQApp = true;
		//Create own instance of QCoreApplication
		int argc = 1;
		QByteArray app = "application.exe";
		char* argv[] = { app.data() };

		WRITE_TRACE( DBG_DEBUG, "Starting SDK Library QCoreApplication Event Loop." );
		QCoreApplication a(argc, argv);
		InitSdkParts();
		PrlSdkStatusWriter().SetInitStatus(true);
		QCoreApplication::exec();
		PrlSdkStatusWriter().SetInitStatus(false);
		DeinitSdkParts();
	}
	else
	{
		m_bOwnerOfQApp = false;
		InitSdkParts();
		PrlSdkStatusWriter().SetInitStatus(true);
		QThread::exec();
		PrlSdkStatusWriter().SetInitStatus(false);
		DeinitSdkParts();
	}
	WRITE_TRACE( DBG_DEBUG, "Finished SDK Library QCoreApplication Event Loop." );
}

void QMainThread::NotifyThatSdkEventLoopStarted()
{
	QMutexLocker _lock(&m_MainThreadMutex);
	m_MainThreadCondition.wakeAll();
}

void QMainThread::stop()
{
	QMutexLocker _lock(&m_MainThreadMutex);
	if (m_bOwnerOfQApp)//thread is owner of QCoreApplication event loop
		QCoreApplication::exit(0);
	else
		exit(0);
	m_MainThreadCondition.wait(&m_MainThreadMutex);
	wait();
	g_pThreadsDestructor->WaitAllThreadsAndCleanup();
	delete g_pThreadsDestructor;
	g_pThreadsDestructor = NULL;
}

void QMainThread::start(PRL_UINT32 nFlags)
{
	QMutexLocker _lock(&m_MainThreadMutex);
	m_nFlags = nFlags;
	QThread::start();
	m_MainThreadCondition.wait(&m_MainThreadMutex);
}

void PrlContextSwitcher::SdkEventLoopStarted()
{
	if (s_QMainThread)
		s_QMainThread->NotifyThatSdkEventLoopStarted();
}

bool PrlContextSwitcher::event ( QEvent* evt )
{
	if ( evt->type() == QEvent::DeferredDelete ) {
		PrlHandleServer::CleanRegisteredThreads();
		return true;
	}
	else
		return QObject::event(evt);
}

void PrlContextSwitcher::NotifyWorkFinalization()
{
	emit LetFinishWorkNow();
}

bool QMainThread::IsInitialized()
{
	return (s_QMainThread != NULL);
}

QMainThread *QMainThread::Instance()
{
	if (!s_QMainThread)
		s_QMainThread = new QMainThread;
	return (s_QMainThread);
}

void QMainThread::Deinit()
{
	if (s_QMainThread)
	{
		delete s_QMainThread;
		s_QMainThread = NULL;
	}
}

PRL_UINT32 PrlGetDefaultListenPort()
{
	return PRL_DISPATCHER_LISTEN_PORT;
}

PRL_BOOL PRL_WRONG_VM_DEVICE_HANDLE_TYPE(PRL_HANDLE_TYPE typo)
{
	switch (typo)
	{
		case PHT_VIRTUAL_DEV_FLOPPY:
		case PHT_VIRTUAL_DEV_HARD_DISK:
		case PHT_VIRTUAL_DEV_SERIAL_PORT:
		case PHT_VIRTUAL_DEV_PARALLEL_PORT:
		case PHT_VIRTUAL_DEV_NET_ADAPTER:
		case PHT_VIRTUAL_DEV_MOUSE:
		case PHT_VIRTUAL_DEV_KEYBOARD:
		case PHT_VIRTUAL_DEV_DISPLAY:
		case PHT_VIRTUAL_DEV_OPTICAL_DISK:
		case PHT_VIRTUAL_DEV_USB_DEVICE:
		case PHT_VIRTUAL_DEV_SOUND:
		case PHT_VIRTUAL_DEV_GENERIC_PCI:
		case PHT_VIRTUAL_DEV_GENERIC_SCSI:
			return (PRL_FALSE);

		default:
			return (PRL_TRUE);
	}
}

/**
 * Checks whether specified handle is a non VM device handle
 * @param some handle
 */
PRL_BOOL PRL_WRONG_VM_DEVICE_HANDLE(PRL_HANDLE handle)
{
	PrlHandleBasePtr pHandle = PRL_OBJECT_BY_HANDLE<PrlHandleBase>( handle );

	// Handles should be valid pointers
	if ( !pHandle )
		return PRL_TRUE;

	PRL_HANDLE_TYPE typo = pHandle->GetType();
	return (PRL_WRONG_VM_DEVICE_HANDLE_TYPE(typo));
}

/**
 * Checks whether specified handle type is a virtual device handle type
 */
PRL_BOOL PRL_WRONG_VM_DEVICE_TYPE(PRL_DEVICE_TYPE typo)
{
	switch (typo)
	{
		case PDE_FLOPPY_DISK:
		case PDE_HARD_DISK:
		case PDE_SERIAL_PORT:
		case PDE_PARALLEL_PORT:
		case PDE_GENERIC_NETWORK_ADAPTER:
		case PDE_OPTICAL_DISK:
		case PDE_USB_DEVICE:
		case PDE_SOUND_DEVICE:
		case PDE_PCI_VIDEO_ADAPTER:
		case PDE_GENERIC_PCI_DEVICE:
		case PDE_GENERIC_SCSI_DEVICE:
			return (PRL_FALSE);

		default:
			return (PRL_TRUE);
	}
}

PRL_HANDLE store_result(PrlHandleJob *job_obj, PRL_HANDLE hVm)
{
	if (job_obj) {
		if (hVm != PRL_INVALID_HANDLE)
			job_obj->SetVmHandle(hVm);
	} else {
		return PRL_INVALID_HANDLE;
	}

	return job_obj->GetHandle();
}
