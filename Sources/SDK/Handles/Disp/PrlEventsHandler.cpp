/*
 * PrlEventsHandler.cpp
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


#include "PrlEventsHandler.h"
#include "PrlHandleServer.h"
#include "PrlHandleServerDisp.h"
#include "PrlHandleServerJob.h"
#include "PrlHandleVmEvent.h"
#include "PrlContextSwitcher.h"

#include <prlxmlmodel/Messaging/CVmEvent.h>
#include <prlxmlmodel/Messaging/CVmBinaryEventParameter.h>

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/PrlUuid/Uuid.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/HostUtils/HostUtils.h>

#include "PrlHandleVmCfg.h"
#include "PrlFileDescriptorsMech.h"

#ifdef _WIN_
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif

#ifdef ENABLE_MALLOC_DEBUG
    // By adding this interface we enable allocations tracing in the module
    #include "Interfaces/Debug.h"
#else
    // We're not allowed to throw exceptions from the library -
    // so we need to prevent operator ::new from doing this
    #include <new>
    using std::nothrow;
    #define new new(nothrow)
#endif

/***********************************************Notifiers classes implementation*********************************/

/** Notifier thread starter */
CNotificationMapper::CNotificationMapper ( CNotificationThread* t )
: m_pThread(t)
{}

void CNotificationMapper::NotificationStarted ()
{
    m_pThread->NotificationStarted();
}

/** Notification thread implementation */

CNotificationThread::CNotificationThread () :
	m_bNotificationStarted(false)
{
	setStackSize(PRL_STACK_SIZE);
	LOG_MESSAGE(DBG_DEBUG, "Notification thread %p created", this);
}

CNotificationThread::~CNotificationThread ()
{
	LOG_MESSAGE(DBG_DEBUG, "Notification thread %p destroyed", this);
}

void CNotificationThread::run ()
{
	CServerNotifier *pServNotifier = new CServerNotifier;
	CVmNotifier *pVmNotifier = new CVmNotifier;
	CNotificationMapper *pMapper = new CNotificationMapper( this );

	bool res = false;
	(void)res;

	// Connect server notification
	res = QObject::connect( this,
							SIGNAL(ToNotifyServer(PrlHandleServerPtr,PrlHandleBasePtr)),
							pServNotifier,
							SLOT(Notify(PrlHandleServerPtr,PrlHandleBasePtr)),
							Qt::QueuedConnection);
	Q_ASSERT(res);

	// Connect vm notification
	res = QObject::connect( this,
							SIGNAL(ToNotifyVm(PrlHandleVmPtr,PrlHandleBasePtr)),
							pVmNotifier,
							SLOT(Notify(PrlHandleVmPtr,PrlHandleBasePtr)),
							Qt::QueuedConnection);
	Q_ASSERT(res);

	res = QObject::connect( this,
							SIGNAL(finished()),
							SLOT(NotificationFinished()),
							Qt::QueuedConnection);
	Q_ASSERT(res);

	// Exec event loop
	exec();

	m_bNotificationStarted = false;

	//We should move these objects deletion to the main SDK event loop thread
	//due implicit QCoreApplication::removePostedEvents() can be called and
	//destruction of SDK objects (transport participants) can be initiated.
	QObject::disconnect(SIGNAL(ToNotifyServer(PrlHandleServerPtr,PrlHandleBasePtr)),
						pServNotifier,
						SLOT(Notify(PrlHandleServerPtr,PrlHandleBasePtr)));
	QObject::disconnect(SIGNAL(ToNotifyVm(PrlHandleVmPtr,PrlHandleBasePtr)),
						pVmNotifier,
						SLOT(Notify(PrlHandleVmPtr,PrlHandleBasePtr)));
	QObject::disconnect(SIGNAL(finished()), this, SLOT(NotificationFinished()));

	pServNotifier->moveToThread( QCoreApplication::instance()->thread() );
	pVmNotifier->moveToThread( QCoreApplication::instance()->thread() );
	pMapper->moveToThread( QCoreApplication::instance()->thread() );
	QCoreApplication::postEvent( pServNotifier, new QEvent(QEvent::DeferredDelete) );
	QCoreApplication::postEvent( pVmNotifier, new QEvent(QEvent::DeferredDelete) );
	QCoreApplication::postEvent( pMapper, new QEvent(QEvent::DeferredDelete) );
}

void CNotificationThread::NotificationStarted ()
{
	m_bNotificationStarted = true;
}

void CNotificationThread::NotificationFinished()
{
	m_bNotificationStarted = false;
}

bool CNotificationThread::isStarted() const
{
	return m_bNotificationStarted;
}

void CNotificationThread::ExitNotification ( int retcode )
{
	if ( QThread::currentThread() != this ) {
		while ( ! m_bNotificationStarted && isRunning() )
			// Sleep msecs
			QThread::wait(50);
	}
	QThread::exit(retcode);
}

void CNotificationThread::start ( Priority priority )
{
	NotificationStarted();
	QThread::start(priority);

	unsigned int i = 0;
	while (i++ < 100 && !isRunning() && m_bNotificationStarted) {
		/* The QThread::wait return false if the wait timed out,
		 * otherwise return true (if has finished or has not been started yet.
		 * Exit if timedout in 5 sec.
		 */
		if (QThread::wait(50) == true)
			HostUtils::Sleep(50);
	}
}

/** Server notifier object implementation */

void CServerNotifier::Notify ( PrlHandleServerPtr pServer, PrlHandleBasePtr pEvent )
{
	pServer->eventSource()->NotifyListeners(pEvent);
}

/** Vm notifier object implementation */
void CVmNotifier::Notify ( PrlHandleVmPtr pVm, PrlHandleBasePtr pEvent )
{
	pVm->ProcessVmEvent(pEvent);
	pVm->eventSource()->NotifyListeners(pEvent);
}

/**
 * Constructor - performs basic initialization of the object.
 */
CEventsHandler::CEventsHandler(PRL_HANDLE hServer)
:	m_ServerHandle(hServer),
	m_NotificationThread( new CNotificationThread )
{
	// Start notification thread
	m_NotificationThread->start();

	Q_ASSERT(PrlContextSwitcher::Instance());

	// Connect object at common finalization mechanizm
	bool res = QObject::connect( PrlContextSwitcher::Instance(),
							SIGNAL(LetFinishWorkNow()),
							this,
							SLOT(FinalizeWork()),
							Qt::DirectConnection);
	Q_ASSERT(res);
    (void) res; // keeping compiler happy in the release version
}

bool CEventsHandler::isStarted() const
{
	return m_NotificationThread->isStarted();
}

void CEventsHandler::FinalizeWork()
{
	SmartPtr<CNotificationThread> pNotificationThread = m_NotificationThread;
	if (pNotificationThread.getImpl())
	{
		PrlHandleServer::RegisterThreadToDeleteFinMech(pNotificationThread);
		m_NotificationThread = SmartPtr<CNotificationThread>(0);
	}
}

/**
 * Destructor
 */
CEventsHandler::~CEventsHandler ()
{
}

/**
 * Handler of the events from the server.
 */
bool CEventsHandler::event( QEvent *pEvent )
{
	if ( pEvent->type() == QEvent::DeferredDelete )
		return QObject::event(pEvent);

	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>(m_ServerHandle);
	if (!pServer)
		return (true);
	PrlHandleServerDispPtr pServerDisp = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>(m_ServerHandle);
	if (!pServerDisp)
		return (true);

	PVE::VmEventClassType type = (PVE::VmEventClassType) pEvent->type();
	PrlHandleVmPtr pVm;
	PRL_HANDLE event_handle = PRL_INVALID_HANDLE;

	if ( type == PVE::CResultType )
	{
		CResult* pResult = static_cast<CResult*>(pEvent);

		// Searching for registered job by its uuid
		PrlHandleServerJobPtr pJob = PrlHandleServerJob::GetJobByUuid( pResult->getRequestId() );

		// If job was found - setting it's result
		if ( pJob )
		{
			event_handle = pJob->GetHandle();
			if ((pResult->getOpCode() == PVE::DspCmdUserLogin || pResult->getOpCode() == PVE::DspCmdUserLoginLocalStage2)
					&& PRL_SUCCEEDED(pResult->getReturnCode()))
			{
				pServer->ProcessLoginResponse(pResult);
			}
			else if (pResult->getOpCode() == PVE::DspCmdSetNonInteractiveSession
					&& PRL_SUCCEEDED(pResult->getReturnCode()))
			{
				QString qsValue = pResult->GetParamToken(0);
				pServerDisp->SetNonInteractiveSession(qsValue.toInt() != 0);
			}
			else if (pResult->getOpCode() == PVE::DspCmdSetSessionConfirmationMode
				&& PRL_SUCCEEDED(pResult->getReturnCode()))
			{
				QString qsValue = pResult->GetParamToken(0);
				pServerDisp->SetConfirmationMode(qsValue.toInt() != 0);
			}
			pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>(pJob->GetVmHandle());
			if( NULL != pVm )
			{
				if (pResult->getOpCode() == PVE::DspCmdVmStart
					|| pResult->getOpCode() == PVE::DspCmdVmStartEx)
					pVm->StartComplete( pResult );
				else if (PRL_SUCCEEDED(pResult->getReturnCode()))
				{
					switch( pResult->getOpCode() )
					{
					case PVE::DspCmdVmGetConfig:
					case PVE::DspCmdGetVmConfigById:
						pVm->fromString(pResult->m_hashResultSet[PVE::DspCmdVmGetConfig_strVmConfig].toUtf8().data());
						break;
					case PVE::DspCmdDirVmEditBegin:
						pVm->fromString(pResult->m_hashResultSet[PVE::DspCmdDirRegVm_strVmConfig].toUtf8().data());
						break;
					case PVE::DspCmdVmUpdateSecurity:
						((PrlHandleVmCfg* )pVm.getHandle())->UpdateSecurityFromVmSecurity(pResult->m_hashResultSet[PVE::DspCmdVmUpdateSecurity_strSecurityInfo]);
						break;
					case PVE::DspCmdVmGetState:
						((PrlHandleVmCfg* )pVm.getHandle())->UpdateSecurityFromVmInfo(pResult->m_hashResultSet[PVE::DspCmdGetVmInfo_strContainer]);
						break;
					case PVE::DspCmdDirVmCreate:
					{
						QString sConfig = pResult->m_hashResultSet[PVE::DspCmdDirRegVm_strVmConfig];
						if (!sConfig.isEmpty())
							pVm->UpdateVmConfig(sConfig);
						break;
					}
					default:
						break;
					}
				}
			}
			pJob->SetCResult( *pResult );
		}
	}
	else if ( type == PVE::CVmEventType )
	{
		CVmEvent *pVmEvent = static_cast<CVmEvent*>(pEvent);

		LOG_MESSAGE( DBG_INFO, "Client recieved PVE::CVmEventType type=%s(%u) code=%u",
			PRL_EVENT_TYPE_TO_STRING(pVmEvent->getEventType()),
			pVmEvent->getEventType(),
			pVmEvent->getEventCode()
			);
		if (pVmEvent->getEventType() == PET_IO_READY_TO_ACCEPT_STDIN_PKGS)
		{
			PrlFileDescriptorsMech::MarkReadyToAcceptStdin(pVmEvent->getInitRequestId());
			//Internal event shouldn't be processed by external listeners
			return (true);
		}
		if (pVmEvent->getEventType() == PET_DSP_EVT_LICENSE_CHANGED)
		{
			CVmEventParameter* pParam = pVmEvent->getEventParameter(EVT_PARAM_SESSION_RESTRICT_EVENT);
			if (pParam)
				pServer->SetRestrictionEvent(pParam->getParamValue());
			else
				WRITE_TRACE(DBG_INFO, "License changed event doesn't contain restriction event");
		}

		PrlHandleVmEvent *vm_event = new PrlHandleVmEvent(pServer, pVmEvent->getEventType(), pVmEvent);
		if (vm_event)
			event_handle = vm_event->GetHandle();

		// NOTE: lot of vm events from dispatcher send with PIE_DISPATCHER issuer type
		//       but with vm_uuid in IssuerId field
		// For example: any type of progress vm operations, VmConfigChanged, etc.
		const PRL_EVENT_ISSUER_TYPE nIssuer = pVmEvent->getEventIssuerType();
		if ( nIssuer == PIE_VIRTUAL_MACHINE
			|| nIssuer == PIE_DISPATCHER )
		{
			pVm = pServer->GetVmHandleByUuid(pVmEvent->getEventIssuerId());
		}
		//Check whether processing event object is question and register it at questions list if necessary
		if (pVmEvent->getEventType() == PET_DSP_EVT_VM_QUESTION)
			pServerDisp->GetQuestionsList().RegisterQuestionObject(PrlHandleVmEventPtr(vm_event));
	}

	if ( event_handle != PRL_INVALID_HANDLE )
	{
		PrlHandleBasePtr pEvent = PRL_OBJECT_BY_HANDLE<PrlHandleBase>(event_handle);
		SmartPtr<CNotificationThread> pNotificationThread = m_NotificationThread;
		if (pEvent && pNotificationThread.getImpl())
		{
			emit pNotificationThread->ToNotifyServer(pServer, pEvent);

			if (pVm)
				emit pNotificationThread->ToNotifyVm(pVm, pEvent);

			if (PHT_EVENT == pEvent->GetType())
				pEvent->Release();
			else if (PHT_JOB == pEvent->GetType())
			{
				//Unregister job from response awaiting list due response was received now
				pServer->RemoveJobFromResponseAwaitingList(PRL_OBJECT_BY_HANDLE<PrlHandleServerJob>(event_handle));
			}
		}
	}

	return (true);
}

void CEventsHandler::RegisterNotification( PrlHandleServerPtr pServer, PrlHandleBasePtr pEvent )
{
	SmartPtr<CNotificationThread> pNotificationThread = m_NotificationThread;
	if ( pNotificationThread.getImpl() )
		emit pNotificationThread->ToNotifyServer( pServer, pEvent );
}

void CEventsHandler::StopNotificationThread()
{
	// Deferred thread clean
	SmartPtr<CNotificationThread> pNotificationThread = m_NotificationThread;
	if (pNotificationThread.getImpl())
	{
		PrlHandleServer::RegisterThreadToDelete(pNotificationThread);
		m_NotificationThread = SmartPtr<CNotificationThread>(0);
	}
}
