/*
 * PrlHandleServer.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2023 Virtuozzo International GmbH. All rights reserved.
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


#include "PrlHandleServer.h"
#include "PrlHandleServerJob.h"
#include "PrlHandleVm.h"
#include "PrlContextSwitcher.h"
#include "PrlHandleProblemReportBase.h"
#include "PrlHandleCpuPool.h"
#include "PrlHandleBackup.h"

#include <prlcommon/Messaging/CVmBinaryEventParameter.h>

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/PrlUuid/Uuid.h>
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/ProtoSerializer/CProtoSerializer.h>

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


/*********************************************************************************************/

Q_GLOBAL_STATIC(QMutex, RegisteredThreadsMutex)
Q_GLOBAL_STATIC(QList<SmartPtr<CNotificationThread> >, RegisteredThreadsList)

/*********************************************************************************************/

PrlHandleServer::PrlHandleServer()
: PrlHandleBase( PHT_SERVER ),
  m_isConnectionLocal( false ),
  m_isProxyConnection( false ),
  m_pPveControl(NULL),
  m_pEventsHandler( new CEventsHandler(GetHandle()) ),
  m_nManagePort(0),
  m_nSecurityLevel(PSL_LOW_SECURITY),
  m_bNonInteractiveSession(PRL_FALSE),
  m_bConfirmationModeEnabled(PRL_FALSE),
  m_nServerAppExecuteMode(PAM_UNKNOWN),
  m_nStartTime(0),
  m_nStartTimeMonotonic(0)
  , m_pSupportedOsesMatrix(0)
{
	InitializeConnection();
}

bool PrlHandleServer::isStarted() const
{
	return m_pEventsHandler->isStarted();
}

PrlHandleServer::~PrlHandleServer()
{
	if (QCoreApplication::closingDown())
		return;
	if (m_pPveControl)
	{
		m_pPveControl->stopTransport();
		m_pPveControl->deleteLater();
	}

	//CEventsHandler object should to be destroyed from native event loop
	m_pEventsHandler->StopNotificationThread();
	m_pEventsHandler->deleteLater();
}

void PrlHandleServer::InitConnectionSettings(const QString& hostName,
                            bool ioLocal,
                            bool isProxyConnection,
                            PRL_UINT32 port_cmd,
                            PRL_SECURITY_LEVEL security_level )
{
	m_sHostname = hostName;

	m_isConnectionLocal = ioLocal;
	m_isProxyConnection = isProxyConnection;
	m_nManagePort = port_cmd;

	m_nSecurityLevel = security_level;
	m_sServerUuid = "";
	m_sHostOsVersion = "";
	m_sProductVersion = "";
	m_sProxyPeerHandle = "";
	m_nStartTime = 0;
	m_nStartTimeMonotonic = 0;
}

void PrlHandleServer::Notify( PrlHandleBasePtr pEvent )
{
	if ( m_pEventsHandler )
	{
		PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>( GetHandle() );
		if ( pServer )
			m_pEventsHandler->RegisterNotification( pServer, pEvent );
	}
}

PRL_RESULT PrlHandleServer::fromString(PRL_CONST_STR sXml)
{
	QMutexLocker _lock(&m_MembersMutex);
	return PRL_FAILED(m_evtRestriction.fromString(sXml)) ? PRL_ERR_INVALID_ARG : PRL_ERR_SUCCESS;
}

QString PrlHandleServer::toString()
{
	QMutexLocker _lock(&m_MembersMutex);
	return m_evtRestriction.toString();
}

PrlHandleJobPtr PrlHandleServer::create_job(const QString& job_uuid, PRL_JOB_OPERATION_CODE job_code)
{
    return PrlHandleJobPtr(new PrlHandleServerJob(PrlHandleServerPtr(this), job_uuid, job_code)) ;
}

void PrlHandleServer::FinalizeAllTransportConnections()
{
    // Should be the main thread
    Q_ASSERT(QMainThread::Instance() == QThread::currentThread());

	QSet<PRL_HANDLE> _servers;
	QSet<PRL_HANDLE> _vms;
	{
		QMutexLocker lock(PrlHandleBase::s_pHandlesMapMutex);
		HandlesMap::const_iterator _it = s_pHandlesMap->begin();
		for(; _it != s_pHandlesMap->end(); ++_it)
		{
			if (_it.value()->GetType() == PHT_SERVER)
				_servers.insert(_it.key());
			else if (_it.value()->GetType() == PHT_VIRTUAL_MACHINE)
				_vms.insert(_it.key());
		}
	}
	foreach(PRL_HANDLE hVm, _vms)
	{
		PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>(hVm);
		if (pVm)
			pVm->VmDisconnectForcibly();
	}
	foreach(PRL_HANDLE hServer, _servers)
	{
		PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>(hServer);
		if (pServer) {
			pServer->StopTransport();
			pServer->StopNotificationThread();
		}
	}
}

void PrlHandleServer::RegisterThreadToDelete ( SmartPtr<CNotificationThread> &thread )
{
	Q_ASSERT(thread.getImpl());

	QMutex* mutex = RegisteredThreadsMutex();
	QList<SmartPtr<CNotificationThread> >* list = RegisteredThreadsList();

	// Exit event loop
	thread->ExitNotification(0);

	if ( QThread::currentThread() == thread.getImpl() ) {
		// Lock
		QMutexLocker locker(mutex);
		if (NULL != list && !list->contains(thread))
			list->append( thread );

		// Unlock
		locker.unlock();

		// SDK context switcher will catch DeferredDelete event
		QCoreApplication::postEvent( PrlContextSwitcher::Instance(),
									 new QDeferredDeleteEvent );
	}
	else {
		//Wait until thread finalized it work
		thread->wait();

		// Clean all stopped other registered threads
		PrlHandleServer::CleanRegisteredThreads();
	}
}

void PrlHandleServer::RegisterThreadToDeleteFinMech(SmartPtr<CNotificationThread> &thread)
{
	Q_ASSERT(thread.getImpl());

	QMutex* mutex = RegisteredThreadsMutex();
	QList<SmartPtr<CNotificationThread> >* list = RegisteredThreadsList();

	// Exit event loop
	thread->ExitNotification(0);

	// Lock
	QMutexLocker locker(mutex);
	if (NULL != list && !list->contains(thread))
		list->append( thread );

	// Unlock
	locker.unlock();

	// SDK context switcher will catch DeferredDelete event
	QCoreApplication::postEvent( PrlContextSwitcher::Instance(), new QDeferredDeleteEvent );
}

void PrlHandleServer::CleanRegisteredThreads()
{
	bool bCallAgain = false;

	QMutex* mutex = RegisteredThreadsMutex();
	QList<SmartPtr<CNotificationThread> >* list = RegisteredThreadsList();

	// Lock
	QMutexLocker locker(mutex);

	if (NULL != list)
	{
		QList<SmartPtr<CNotificationThread> >::Iterator it = list->begin();
		while ( it != list->end() ) {
			SmartPtr<CNotificationThread> &thread = *it;
			if ( NULL != thread.getImpl() && thread->isRunning() ) {
				bCallAgain = true;
				++it;
			}
			else {
				it = list->erase(it);
			}
		}
	}

	// Unlock
	locker.unlock();

	if ( bCallAgain )
		// SDK context switcher will catch DeferredDelete event
		QCoreApplication::postEvent( PrlContextSwitcher::Instance(), new QDeferredDeleteEvent );
}

void PrlHandleServer::StopTransport()
{
	if (m_pPveControl)
		m_pPveControl->stopTransport();
}

void PrlHandleServer::StopNotificationThread()
{
	m_pEventsHandler->StopNotificationThread();
}

void PrlHandleServer::InitializeConnection()
{
	if (m_pPveControl)
	{
		m_pPveControl->stopTransport();
		delete m_pPveControl;
	}
	m_pPveControl = new CPveControl(m_pEventsHandler);
}

bool PrlHandleServer::IsConnectionLocal () const
{
	return m_isConnectionLocal;
}

bool PrlHandleServer::IsProxyConnection () const
{
	return m_isProxyConnection;
}

QString PrlHandleServer::GetSessionUuid () const
{
	QMutexLocker _lock(&m_MembersMutex);
	return (m_sSessionUuid);
}

QString PrlHandleServer::GetRemoteHost () const
{
	QMutexLocker _lock(&m_MembersMutex);
	return (m_sHostname);
}

void PrlHandleServer::AddJobToResponseAwaitingList(const PrlHandleServerJobPtr &pJob)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_ResponseAwaitingList.append(pJob);
}

void PrlHandleServer::RemoveJobFromResponseAwaitingList(const PrlHandleServerJobPtr &pJob)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_ResponseAwaitingList.removeAll(pJob);
}

PRL_UINT32 PrlHandleServer::GetManagePort () const
{
	QMutexLocker _lock(&m_MembersMutex);
	return (m_nManagePort);
}

PRL_SECURITY_LEVEL PrlHandleServer::GetSecurityLevel () const
{
	QMutexLocker _lock(&m_MembersMutex);
	return (m_nSecurityLevel);
}

IOSender::Handle PrlHandleServer::GetProxyPeerHandle () const
{
	QMutexLocker _lock(&m_MembersMutex);
	return (m_sProxyPeerHandle);
}

void PrlHandleServer::SetRestrictionEvent(QString qsRestrictEvt)
{
	QMutexLocker _lock(&m_MembersMutex);
	m_evtRestriction.fromString(qsRestrictEvt);
}

CPveControl* PrlHandleServer::GetPveControl() const
{
    return m_pPveControl;
}

PrlHandleJobPtr PrlHandleServer::DspCmdVmInternal(PRL_CONST_STR sVmUuid, PRL_CONST_STR cmd,
												  const QStringList &lstArgs)
{
	QString job_uuid = m_pPveControl->DspCmdVmInternal(sVmUuid, cmd, lstArgs);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_CMD_INTERNAL)));
}

PrlHandleJobPtr PrlHandleServer::DspCmdVmRunCompressor(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmRunCompressor(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_RUN_COMPRESSOR)));
}

PrlHandleJobPtr PrlHandleServer::DspCmdVmCancelCompressor(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmCancelCompressor(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_CANCEL_COMPRESSOR)));
}

PrlHandleJobPtr PrlHandleServer::DspCmdVmUpdateNvram(PRL_CONST_STR sVmUuid)
{
	QString job_uuid = m_pPveControl->DspCmdVmUpdateNvram(sVmUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_VM_UPDATE_NVRAM)));
}

PrlHandleJobPtr PrlHandleServer::DspCmdVmUpdateToolsSection(PRL_CONST_STR sVmUuid, PRL_CONST_STR sToolsSection )
{
	QString job_uuid = m_pPveControl->DspCmdVmUpdateToolsSection(sVmUuid, sToolsSection);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
		PJOC_VM_UPDATE_TOOLS_SECTION)));
}

PrlHandleJobPtr PrlHandleServer::DspCmdUserCancelOperation(PRL_CONST_STR sJobUuid)
{
	QString job_uuid = m_pPveControl->DspCmdUserCancelOperation(sJobUuid);

	return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_JOB_CANCEL)));
}

PrlHandleVmPtr PrlHandleServer::GetVmHandleByUuid( const QString &vm_uuid )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	HandleVmUuidMap::iterator it = m_HandleVmUuidMap.find( vm_uuid );

	if ( it == m_HandleVmUuidMap.end() )
		return PrlHandleVmPtr((PrlHandleVm *)0);

	PrlHandleVmPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVm>(it->second);
	if (!pVm.getHandle())//Temporary record found (record of already dead VM object) let erase it
		m_HandleVmUuidMap.erase(it);
	return pVm;
}

PrlHandleVmPtr PrlHandleServer::GetVmByConfig(const QString &vm_config)
{
	CVmConfiguration _vm_config(vm_config);
	if (PRL_SUCCEEDED(_vm_config.m_uiRcInit) && _vm_config.getVmIdentification())
		return GetVmHandleByUuid(_vm_config.getVmIdentification()->getVmUuid());
	return (PrlHandleVmPtr((PrlHandleVm *)0));
}

void PrlHandleServer::ProcessLoginResponse(CResult *pResult)
{
	SmartPtr<CVmEvent> pLoginResponse(0);
	if (!pResult->getBinaryContent())//Old clients support
	{
		QString str = pResult->m_hashResultSet[ PVE::DspCmdUserLogin_strLoginInfo ];
		pLoginResponse = SmartPtr<CVmEvent>(new CVmEvent(str));
	}
	else
		pLoginResponse = SmartPtr<CVmEvent>(new CVmEvent(pResult->getBinaryContent()));

	if (PRL_SUCCEEDED(pLoginResponse->m_uiRcInit))
	{
		QMutexLocker _lock(&m_MembersMutex);
		CVmEventParameter *pParam = pLoginResponse->getEventParameter(EVT_PARAM_RESPONSE_LOGIN_CMD_SESSION);
		if (pParam)
			m_sSessionUuid = pParam->getParamValue();
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain session UUID info");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SERVER_INFO_SERVER_UUID);
		if (pParam)
			m_sServerUuid = pParam->getParamValue();
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain server UUID info");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SERVER_INFO_OS_VERSION);
		if (pParam)
			m_sHostOsVersion = pParam->getParamValue();
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain server host OS version info");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SERVER_INFO_PRODUCT_VERSION);
		if (pParam)
			m_sProductVersion = pParam->getParamValue();
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain server product version info");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SERVER_INFO_CONFIRMATION_MODE);
		if (pParam)
			m_bConfirmationModeEnabled = (bool)pParam->getParamValue().toUInt() ;
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain confirmation mode info");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SERVER_INFO_START_TIME);
		if (pParam)
			m_nStartTime = pParam->getParamValue().toULongLong();
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain service start time (wall clock time)");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SERVER_INFO_START_TIME_MONOTONIC);
		if (pParam)
			m_nStartTimeMonotonic = pParam->getParamValue().toULongLong();
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain service start time from monotonic clock");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SERVER_INFO_APP_EXECUTE_MODE);
		if (pParam)
		{
			m_nServerAppExecuteMode = (PRL_APPLICATION_MODE)pParam->getParamValue().toUInt();
			if( m_nServerAppExecuteMode > PAM_LAST )
			{
				WRITE_TRACE(DBG_INFO, "Login response contains unknown server application mode %d."
					, m_nServerAppExecuteMode);
				m_nServerAppExecuteMode = PAM_UNKNOWN;
			}
		}
		else
			WRITE_TRACE(DBG_DEBUG, "Login response doesn't contain server application mode info (old client?).");
		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_SUPPORTED_OSES_LIST);
		if (pParam)
		{
			CVmBinaryEventParameter *pBinaryParam = dynamic_cast<CVmBinaryEventParameter *>(pParam);
			PRL_ASSERT(pBinaryParam);
			if (pBinaryParam)
			{
				if (!m_pSupportedOsesMatrix.isValid())
					m_pSupportedOsesMatrix = SmartPtr<COsesMatrix>(new COsesMatrix);
				m_pSupportedOsesMatrix->Deserialize(*pBinaryParam->getBinaryDataStream().getImpl());
			}
			else
			{
				WRITE_TRACE(DBG_FATAL, "Wrong event for guest OSes matrix object initialization. Type: %.8X, code: %.8X",\
					pLoginResponse->getEventType(), pLoginResponse->getEventCode());
			}
		}
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain guest OSes matrix");
		pParam = pLoginResponse->getEventParameter(EVT_PARAM_SESSION_RESTRICT_EVENT);
		if (pParam)
			SetRestrictionEvent(pParam->getParamValue());
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain restriction event");

		pParam = pLoginResponse->getEventParameter(EVT_PARAM_PRL_FEATURES_MATRIX);
		if (pParam)
		{
			CVmBinaryEventParameter *pBinaryParam = dynamic_cast<CVmBinaryEventParameter *>(pParam);
			PRL_ASSERT(pBinaryParam);
			if (pBinaryParam)
			{
				m_FeaturesMatrix.Deserialize(*pBinaryParam->getBinaryDataStream().getImpl());
			}
			else
			{
				WRITE_TRACE(DBG_FATAL, "Wrong event for features matrix object initialization. Type: %.8X, code: %.8X",\
					pLoginResponse->getEventType(), pLoginResponse->getEventCode());
			}
		}
		else
			WRITE_TRACE(DBG_INFO, "Login response doesn't contain features matrix");

	}
	else
		WRITE_TRACE(DBG_FATAL, "Wrong login response received: error code: %.8X '%s' data: [%s]",\
			pLoginResponse->m_uiRcInit, PRL_RESULT_TO_STRING(pLoginResponse->m_uiRcInit), QSTR2UTF8(pLoginResponse->toString()));
}

PrlHandleJobPtr PrlHandleServer::DspCmdVmStorageSetValue(PRL_CONST_STR sVmUuid, PRL_CONST_STR sKey,
																   PRL_CONST_STR sValue, PRL_UINT32 nFlags)
{
	return create_job(m_pPveControl->DspCmdVmStorageSetValue(sVmUuid, sKey, sValue, nFlags),
		PJOC_VM_STORE_VALUE_BY_KEY);
}

PrlHandleJobPtr PrlHandleServer::DspCmdSendProblemReport(PRL_CONST_STR sVmUuid, PRL_HANDLE hProblemReport,
                                                         PRL_UINT32 nFlags)
{
    PrlHandleProblemReportBasePtr pPR =
        PRL_OBJECT_BY_HANDLE<PrlHandleProblemReportBase>(hProblemReport);

    QString job_uuid = m_pPveControl->DspCmdSendProblemReport(pPR->toString(), sVmUuid, nFlags);
    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob(
                           PrlHandleServerPtr(this), job_uuid,
                           PJOC_VM_SEND_PROBLEM_REPORT)));
}

PrlHandleJobPtr PrlHandleServer::GetCPUPoolsList()
{
    QString job_uuid = m_pPveControl->DspCmdGetCPUPoolsList();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_CPU_POOLS_LIST_POOLS)));
}

PrlHandleJobPtr PrlHandleServer::LeaveCPUPool()
{
    QString job_uuid = m_pPveControl->DspCmdLeaveCPUPool();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_CPU_POOLS_LEAVE)));
}

PrlHandleJobPtr PrlHandleServer::JoinCPUPool()
{
    QString job_uuid = m_pPveControl->DspCmdJoinCPUPool();

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_CPU_POOLS_JOIN)));
}

PrlHandleJobPtr PrlHandleServer::MoveToCPUPool(PRL_CONST_STR sCpuPool)
{
    QString job_uuid = m_pPveControl->DspCmdMoveToCPUPool(sCpuPool);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_CPU_POOLS_MOVE)));
}

PrlHandleJobPtr PrlHandleServer::RecalculateCPUPool(PRL_CONST_STR sCpuPool)
{
    QString job_uuid = m_pPveControl->DspCmdRecalculateCPUPool(sCpuPool);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_SRV_CPU_POOLS_RECALCULATE)));
}

PrlHandleJobPtr PrlHandleServer::BeginBackup(PrlHandleVm& vm_, PRL_UINT32 flags_)
{
	QString x;
	if (0 != (flags_ & PBMBF_CREATE_MAP))
		x = Uuid::createUuid().toString();

	QString y = m_pPveControl->DspCmdBeginVmBackup(vm_.GetUuid(), x, flags_);
	return PrlHandleJobPtr(new PrlHandleServerJob(PrlHandleServerPtr(this), y,
				PJOC_SRV_BEGIN_VM_BACKUP));
}

PrlHandleJobPtr PrlHandleServer::EndBackup(const PrlHandleBackup& backup_, PRL_UINT32 flags_)
{
	QString x = m_pPveControl->DspCmdEndVmBackup(backup_.getVm()->GetUuid(), flags_);
	return PrlHandleJobPtr(new PrlHandleServerJob(PrlHandleServerPtr(this), x,
				PJOC_VM_END_BACKUP));
}
