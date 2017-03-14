/*
 * PveControl.cpp: Main interface for processing Client<->Dispatcher
 * protocol messages
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

//#define FORCE_LOGGING_ON
//#define FORCE_LOGGING_LEVEL DBG_DEBUG

#include <prlcommon/Interfaces/ParallelsQt.h>
#include <prlcommon/PrlUuid/Uuid.h>
#include "PveControl.h"
#include <prlcommon/Interfaces/ParallelsNamespace.h>
#include <prlcommon/Messaging/CResult.h>
#include <prlcommon/Messaging/CVmEvent.h>
#include <prlcommon/Messaging/CVmEventParameter.h>
#include <prlcommon/Messaging/CVmBinaryEventParameter.h>
#include <prlcommon/Std/SmartPtr.h>
#include <prlcommon/PrlCommonUtilsBase/ParallelsDirs.h>
#include <prlcommon/IOService/IOCommunication/IORoutingTableHelper.h>
#include <prlcommon/IOService/IOCommunication/IOSSLInterface.h>
#include "PrlHandleLoginHelperJob.h"
#include "PrlHandleLoginLocalHelperJob.h"
#include <prlcommon/Std/PrlAssert.h>
#include "PrlCommon.h"

#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QThread>

#ifdef _WIN_
#include <windows.h>
#else
#include <unistd.h>
#endif

// By adding this interface we enable allocations tracing in the module
#include "Interfaces/Debug.h"

using namespace Parallels;

CPveControl::CPveControl(QObject *pEventReceiverObj)
: m_bUseSSL(IOService::initSSLLibrary()),
  m_pEventReceiverObj(pEventReceiverObj),
  m_JobsHandlesHashMutex(QMutex::Recursive),
  m_ioClient(0)
{
	m_rl.rate = 1;
	m_rl.last = -1;

	LOG_MESSAGE(DBG_DEBUG, "CPveControl::CPveControl() this=%p", this);
	bool bRes = connect(this, SIGNAL(finalizeTransportWork()), SLOT(onFinalizeTransportWork()),
							Qt::BlockingQueuedConnection);
	Q_ASSERT(bRes);
	bRes = connect(this, SIGNAL(cleanupLoginHelperJob()), SLOT(onCleanupLoginHelperJob()),
							Qt::QueuedConnection);
	Q_ASSERT(bRes);
	Q_UNUSED(bRes);
}

CPveControl::~CPveControl ()
{
	delete m_ioClient;
	if (m_bUseSSL)
		IOService::deinitSSLLibrary();
}

void CPveControl::stopTransport()
{
	WRITE_TRACE(DBG_DEBUG, "QThread::currentThread()=%p thread()=%p QCoreApplication::instance()->thread()=%p m_ioClient->thread()=%p",
		QThread::currentThread(),
		thread(),
		QCoreApplication::instance() ? QCoreApplication::instance()->thread() : NULL,
		m_ioClient ? m_ioClient->thread() : NULL);
	if ( m_ioClient && thread() && m_ioClient->thread() == thread() )
	{
		if (QThread::currentThread() != thread())
			emit finalizeTransportWork();
		else
			onFinalizeTransportWork();
	}
}

void CPveControl::onFinalizeTransportWork()
{
    Q_ASSERT(QThread::currentThread() == thread());
    // Destroy transport
    if ( m_ioClient ) {
        delete m_ioClient;
        m_ioClient = 0;
    }
}

void CPveControl::onCleanupLoginHelperJob()
{
    Q_ASSERT(QThread::currentThread() == thread());
	m_pLoginHelperJob = NULL;
	m_pLoginLocalHelperJob = NULL;
	m_rl.last = (unsigned int )-1;
}

bool CPveControl::WaitForRequestComplete(const QString &sReqId, quint32 nWaitTimeout)
{
	if (!m_ioClient)
		return (false);
	IOSendJob::Handle hJob;
	{
		QMutexLocker _lock(&m_JobsHandlesHashMutex);
		hJob = m_JobsHandlesHash.value(sReqId);
	}
	return (m_ioClient->waitForResponse(hJob, nWaitTimeout) == IOSendJob::Success);
}

void CPveControl::handleEventPackage ( const SmartPtr<IOPackage> p )
{
	WRITE_TRACE(DBG_DEBUG, "event package=[Type=(%#x)%s, Sender=%s Parent=%s Reciever=%s]",
		p->header.type, PVE::DispatcherCommandToString( p->header.type ),
		QSTR2UTF8( Uuid::toString( p->header.senderUuid ) ),
		QSTR2UTF8( Uuid::toString( p->header.parentUuid ) ),
		QSTR2UTF8( Uuid::toString( p->header.receiverUuid ) )
		);
	LOG_MESSAGE(DBG_DEBUG, "PveControl::handleEventPackage() package=[%s]", p->buffers[0].getImpl());

    if ( p->header.type == PVE::DspVmEvent )
	{
		CVmEvent *pEvent = new CVmEvent(UTF8_2QSTR(p->buffers[0].getImpl()));
		if (p->isResponsePackage())//Event was inited on some request let add info about request into event
			pEvent->setInitRequestId(Uuid::toString(p->header.parentUuid));
		pEvent->setEventId(p->header.numericId);

		if (pEvent->getEventType() == PET_DSP_EVT_VM_QUESTION)//Storing request question info now
		{
			if (Uuid::toUuid(p->header.parentUuid).isNull())
				pEvent->setInitRequestId(Uuid::toString(p->header.uuid));
			else
				pEvent->setInitRequestId(Uuid::toString(p->header.parentUuid));
			QMutexLocker _lock(&m_JobsHandlesHashMutex);
			m_QuestionsRequestPkgsHash[pEvent->getInitRequestId()] = p;
		}
		// Pass event to client
		if (m_pEventReceiverObj)
			QCoreApplication::postEvent(m_pEventReceiverObj, pEvent);
		else
			QCoreApplication::postEvent(QCoreApplication::instance(), pEvent);
    }
	else if ( p->header.type == PVE::DspVmBinaryEvent )
	{
		Q_ASSERT(p->header.buffersNumber == 1);
		QBuffer _buffer;
		_buffer.setData(p->buffers[0].getImpl(), p->data[0].bufferSize);
		bool bRes = _buffer.open(QIODevice::ReadOnly);
		Q_ASSERT(bRes);
		if (!bRes)
		{
			WRITE_TRACE(DBG_FATAL, "Fatal error - couldn't to open binary event buffer for read");
			return;
		}
		QDataStream _data_stream(&_buffer);\
		_data_stream.setVersion(QDataStream::Qt_4_0);
		CVmEvent *pEvent = new CVmEvent;
		pEvent->Deserialize(_data_stream);
		if (p->isResponsePackage())//Event was inited on some request let add info about request into event
			pEvent->setInitRequestId(Uuid::toString(p->header.parentUuid));
		else if (pEvent->getEventType() == PET_DSP_EVT_VM_QUESTION)//Storing request question info now
		{
			pEvent->setInitRequestId(Uuid::toString(p->header.uuid));
			QMutexLocker _lock(&m_JobsHandlesHashMutex);
			m_QuestionsRequestPkgsHash[Uuid::toString(p->header.uuid)] = p;
		}

		pEvent->setEventId(p->header.numericId);

		// Pass event to client
		if (m_pEventReceiverObj)
			QCoreApplication::postEvent(m_pEventReceiverObj, pEvent);
		else
			QCoreApplication::postEvent(QCoreApplication::instance(), pEvent);
    }
}

void CPveControl::handleResponsePackage ( IOSendJob::Handle hJob, const SmartPtr<IOPackage> p )
{
	LOG_MESSAGE(DBG_INFO, "response package=[Type=(%#x)%s, Sender=%s Parent=%s Reciever=%s]",
		p->header.type, PVE::DispatcherCommandToString( p->header.type ),
		QSTR2UTF8( Uuid::toString( p->header.senderUuid ) ),
		QSTR2UTF8( Uuid::toString( p->header.parentUuid ) ),
		QSTR2UTF8( Uuid::toString( p->header.receiverUuid ) )
		);
	LOG_MESSAGE(DBG_DEBUG, "PveControl::handleResponsePackage() received response=[%s]",
					m_ioClient->getJobUuid(hJob).toString().toUtf8().data());

    if ( p->header.type == PVE::DspWsResponse )
	{
		if (!p->isResponsePackage())
			LOG_MESSAGE(DBG_FATAL, "Response package received that not response with type %d '%s'", p->header.type,
						PVE::DispatcherCommandToString(p->header.type));
		CProtoCommandPtr pCmd = CProtoSerializer::ParseCommand(PVE::DspWsResponse,
									UTF8_2QSTR(p->buffers[0].getImpl()));
		CResult *pResult = new CResult;
		CProtoCommandDspWsResponse *pResponseCmd
			= CProtoSerializer::CastToProtoCommand<CProtoCommandDspWsResponse>(pCmd);
		pResponseCmd->FillResult(pResult);
		pResult->setRequestId(m_ioClient->getJobUuid(hJob));
		PVE::IDispatcherCommands nOpCode = pResult->getOpCode();
		pResult->setPackageId(p->header.numericId);

		if (m_pLoginLocalHelperJob)
		{
			bool bDoLoginLocalStage2 = (nOpCode == PVE::DspCmdUserLoginLocal
				&& PRL_SUCCEEDED(pResult->getReturnCode()));
			switch(nOpCode)
			{
			case PVE::DspCmdUserEasyLoginLocal:
				if (!PRL_SUCCEEDED(pResult->getReturnCode()))
				{
					m_pLoginLocalHelperJob->switchToCompatibilityMode();
					return;
				}
				break;
			case PVE::DspCmdUserLoginLocal:
				if (bDoLoginLocalStage2)
				{
					QString sFilePath = pResponseCmd->GetStandardParam(0);
					QString sCheckData = pResponseCmd->GetStandardParam(1);

					bDoLoginLocalStage2
						= m_pLoginLocalHelperJob->prepareDataForValidation(sFilePath, sCheckData, pResult);
				}
				break;
			default:
				break;
			}

			if (m_pLoginLocalHelperJob->isCompatibilityMode())
				emit cleanupLoginHelperJob();

			if (bDoLoginLocalStage2)
				return;
		}

		// Pass event to client
		if (m_pEventReceiverObj)
			QCoreApplication::postEvent(m_pEventReceiverObj, pResult);
		else
			QCoreApplication::postEvent(QCoreApplication::instance(), pResult);
		// it is hack - need to process job after reattach to lost task
		// job afeter DspCmdAttachToLostTask == job of lost task
		if(nOpCode != PVE::DspCmdAttachToLostTask)
			UnregisterJobHandle(hJob);
		// manually close connection if logoff succeed
		if(nOpCode == PVE::DspCmdUserLogoff)
			m_ioClient->disconnectClient();
    }
	else if ( p->header.type == PVE::DspWsBinaryResponse )
	{
		Q_ASSERT(p->header.buffersNumber == 1);
		QBuffer _buffer;
		_buffer.setData(p->buffers[0].getImpl(), p->data[0].bufferSize);
		bool bRes = _buffer.open(QIODevice::ReadOnly);
		Q_ASSERT(bRes);
		if (!bRes)
		{
			WRITE_TRACE(DBG_FATAL, "Fatal error - couldn't to open binary event buffer for read");
			return;
		}
		QDataStream _data_stream(&_buffer);
		_data_stream.setVersion(QDataStream::Qt_4_0);
		SmartPtr<CVmEvent> pResponseEvent(new CVmEvent);
		pResponseEvent->Deserialize(_data_stream);

		if (!p->isResponsePackage())
			WRITE_TRACE(DBG_FATAL, "Response package received that not response with type %d '%s'", p->header.type,
						PVE::DispatcherCommandToString(p->header.type));
		CProtoCommandPtr pCmd = CProtoSerializer::ParseCommand(PVE::DspWsResponse, pResponseEvent->toString());
		CResult *pResult = new CResult;
		CProtoCommandDspWsResponse *pResponseCmd
			= CProtoSerializer::CastToProtoCommand<CProtoCommandDspWsResponse>(pCmd);
		pResponseCmd->FillResult(pResult);
		pResult->setRequestId(m_ioClient->getJobUuid(hJob));
		pResult->setPackageId(p->header.numericId);

		//Extract and assign response binary data
		CVmEvent *pBinaryData = new CVmEvent;
		pBinaryData->Deserialize(_data_stream);
		pResult->setBinaryContent( pBinaryData );

		// Pass event to client
		if (m_pEventReceiverObj)
			QCoreApplication::postEvent(m_pEventReceiverObj, pResult);
		else
			QCoreApplication::postEvent(QCoreApplication::instance(), pResult);
		UnregisterJobHandle(hJob);
    }
	//To support old problem reports scheme
	else if (p->header.type == PVE::DspVmBinaryResponse)
	{
		Q_ASSERT(p->header.buffersNumber == 1);
		QBuffer _buffer;
		_buffer.setData(p->buffers[0].getImpl(), p->data[0].bufferSize);
		bool bRes = _buffer.open(QIODevice::ReadOnly);
		Q_ASSERT(bRes);
		if (!bRes)
		{
			WRITE_TRACE(DBG_FATAL, "Fatal error - couldn't to open binary event buffer for read");
			return;
		}
		QDataStream _data_stream(&_buffer);\
		_data_stream.setVersion(QDataStream::Qt_4_0);
		CVmEvent *pEvent = new CVmEvent;
		pEvent->Deserialize(_data_stream);
		if (p->isResponsePackage())//Event was inited on some request let add info about request into event
			pEvent->setInitRequestId(Uuid::toString(p->header.parentUuid));
		// fix me it temporal check for problem report binary as response post
		if (pEvent->getEventType() == PET_DSP_EVT_VM_PROBLEM_REPORT_CREATED)
		{
			CResult *pResult = new CResult;
			QString strReport;
			CPrlStringDataSerializer serializer(strReport);
			CVmBinaryEventParameter * lpcParam =
				dynamic_cast<CVmBinaryEventParameter *>(pEvent->getEventParameter(EVT_PARAM_VM_PROBLEM_REPORT));
			serializer.Deserialize(*lpcParam->getBinaryDataStream().getImpl());
			pResult->setReturnCode(pEvent->getEventCode());
			pResult->setRequestId(pEvent->getInitRequestId());
			pResult->setOpCode(PVE::DspCmdVmGetProblemReport);
			pResult->m_hashResultSet[PVE::DspCmdVmGetProblemReport_strReport] = strReport;
			pResult->setRequestId(m_ioClient->getJobUuid(hJob));
			pResult->setPackageId(p->header.numericId);
			// Pass event to client
			if (m_pEventReceiverObj)
				QCoreApplication::postEvent(m_pEventReceiverObj, pResult);
			else
				QCoreApplication::postEvent(QCoreApplication::instance(), pResult);
			UnregisterJobHandle(hJob);
		}
		delete pEvent;

	}
	else if (p->header.type != PVE::DspVmEvent)
		WRITE_TRACE(DBG_FATAL, "Unsupported package type %u '%s' received as response",
						p->header.type, PVE::DispatcherCommandToString(p->header.type));
}

void CPveControl::connectionStateChanged (IOSender::State _state)
{
	WRITE_TRACE( DBG_DEBUG, "PveControl::connectionStateChanged() _state=[%d]", _state);

	if (IOSender::Connected == _state)
	{
		if (m_ioClient->isConnectionViaProxy ())
		{
			QString proxyHost = m_ioClient->remoteHostName();
			WRITE_TRACE(DBG_WARNING, "Connected via proxy %s",  QSTR2UTF8(proxyHost));
		}
		else
		{
			WRITE_TRACE(DBG_WARNING, "Connected directly");
		}
	}

	if ( m_pLoginHelperJob )
	{

		if ( ! m_pLoginHelperJob->wasCanceled()
			 && CheckConnectionStatusHelper(
					_state, m_pLoginHelperJob->GetJobUuid(), false, m_pLoginHelperJob->GetHost()))
		{
			m_pLoginHelperJob->doJob();
		}

		emit cleanupLoginHelperJob();
	}
	else if (m_pLoginLocalHelperJob)
	{
		if (   m_pLoginLocalHelperJob->wasCanceled()
			|| ! CheckConnectionStatusHelper(
						_state, m_pLoginLocalHelperJob->GetJobUuid(),
						m_pLoginLocalHelperJob->isDelayedConnectionMode())
			)
		{
			emit cleanupLoginHelperJob();
		}
		else
		{
			m_pLoginLocalHelperJob->doJob();
		}
	}
	else
	{
		switch (_state)
		{
			case IOSender::Disconnected:
			{
				CVmEvent *pEvent = new CVmEvent(PET_DSP_EVT_DISP_CONNECTION_CLOSED,
										Uuid().toString(), PIE_DISPATCHER, PRL_ERR_WS_DISP_CONNECTION_CLOSED);
				// Pass event to client
				if (m_pEventReceiverObj)
					QCoreApplication::postEvent(m_pEventReceiverObj, pEvent);
				else
					QCoreApplication::postEvent(QCoreApplication::instance(), pEvent);
				NotifyJobsThatConnectionWasLost();
				ClearJobHandles();
			}
			break;

			default:
			break;
		}
	}
}

void CPveControl::UnregisterJobHandle(IOSendJob::Handle hJob)
{
	QMutexLocker _lock(&m_JobsHandlesHashMutex);
	m_JobsHandlesHash.remove(m_ioClient->getJobUuid(hJob));
}

void CPveControl::RegisterJobHandle(IOSendJob::Handle hJob)
{
	QMutexLocker _lock(&m_JobsHandlesHashMutex);
	m_JobsHandlesHash.insert(m_ioClient->getJobUuid(hJob), hJob);
}

IOSendJob::Handle CPveControl::GetJobHandleByUuid(const QString &sJobUuid)
{
	QMutexLocker _lock(&m_JobsHandlesHashMutex);
	return (m_JobsHandlesHash.value(sJobUuid));
}

void CPveControl::ClearJobHandles()
{
	QMutexLocker _lock(&m_JobsHandlesHashMutex);
	m_JobsHandlesHash.clear();
}

void CPveControl::NotifyJobsThatConnectionWasLost()
{
	QMutexLocker _lock(&m_JobsHandlesHashMutex);
	foreach(const QString &sJobUuid, m_JobsHandlesHash.keys())
	{
		PostFailedResult(sJobUuid, "transport layer", PRL_ERR_WS_DISP_CONNECTION_CLOSED);
	}
}

void CPveControl::PostFailedResult(const QString &strUuid,
                                   const QString &strErrorSource,
                                   PRL_RESULT event_code,
                                   QStringList params )
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::PostFailedResult()");

    /// Create result set
    CResult *pResult = new CResult();
    pResult->setRequestId(strUuid);
    pResult->setResultObjectType(PVE::GenericResult);
    pResult->setReturnCode(event_code);

    CVmEvent *pEvent = new CVmEvent(PET_DSP_EVT_ERROR_MESSAGE,
        Uuid().toString(),
        PIE_DISPATCHER,
		event_code,
        PVE::EventRespNotRequired,
        strErrorSource);

    pResult->setError(pEvent);

    if ( params.size() > 0 ) {
        QStringList templates = QStringList() <<
            EVT_PARAM_MESSAGE_PARAM_0 <<
            EVT_PARAM_MESSAGE_PARAM_1 <<
            EVT_PARAM_MESSAGE_PARAM_2;

        for ( int i = 0; i < qMin(params.size(), 3); ++i ) {
            if ( i < templates.size() )
                pEvent->addEventParameter( new CVmEventParameter(
                                         PVE::String,
                                         params[i],
                                         templates[i]
                                         ) );
        }
    }

    if (m_pEventReceiverObj)
        QCoreApplication::postEvent(m_pEventReceiverObj, pResult);
    else
        QCoreApplication::postEvent(QCoreApplication::instance(), pResult);
}

bool CPveControl::CheckConnectionStatus()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::CheckConnectionStatus()");

    return (m_ioClient && m_ioClient->state() == IOSender::Connected);
}

bool CPveControl::CheckConnectionStatusHelper(IOSender::State _state,
											  const QString& strUuid,
											  bool bDelayedConnection,
											  const QString& qsHost)
{
	if ( _state == IOSender::Connected )
		return true;

	if ( ! bDelayedConnection || LogCheckModifyRate(&m_rl))
		WRITE_TRACE(DBG_FATAL, "Can't connect to dispatcher!");

	if ( m_ioClient->error() == IOSender::ProtocolVersionError ) {
		IOCommunication::ProtocolVersion ver;
		m_ioClient->serverProtocolVersion( ver );
		PostFailedResult(strUuid, __FUNCTION__,
						 PRL_ERR_WRONG_PROTOCOL_VERSION,
						 QStringList() << qsHost <<
						 QString(IOService::IOProtocolVersion.whatWeAre) <<
						 QString(ver.whatWeAre) );
	}else if ( m_ioClient->error() == IOSender::HandshakeError )
		PostFailedResult(strUuid, __FUNCTION__,
						 PRL_ERR_HANDSHAKE_FAILED, QStringList() << qsHost);
	else if ( m_ioClient->error() == IOSender::SSLHandshakeError )
		PostFailedResult(strUuid, __FUNCTION__,
						 PRL_ERR_SSL_HANDSHAKE_FAILED,
						 QStringList() << qsHost);
	else if ( m_ioClient->error() == IOSender::RoutingTableAcceptError )
		PostFailedResult(strUuid, __FUNCTION__,
						 PRL_ERR_WRONG_CONNECTION_SECURITY_LEVEL);
	else if (bDelayedConnection)
		return true;
	else
		PostFailedResult(strUuid, __FUNCTION__,
						 PRL_ERR_CANT_CONNECT_TO_DISPATCHER);

	return false;
}

QString CPveControl::SendRequestToServer(CProtoCommandPtr pRequest)
{
	SmartPtr<IOPackage> pPackage = DispatcherPackage::createInstance(pRequest->GetCommandId(),
																		pRequest->GetCommand()->toString());
	return (SendRequestToServer(pPackage));
}

QString CPveControl::PostNotConnected(const QString &strErrorSource)
{
	QString strUuid = (Uuid::createUuid()).toString();
	QString rem = m_ioClient->remoteHostName();
	QStringList p;

	if (rem.startsWith('/'))
		p.append("localhost");
	else
		p.append(rem);

	PostFailedResult(strUuid, strErrorSource, PRL_ERR_NOT_CONNECTED_TO_DISPATCHER, p);

	return strUuid;
}

QString CPveControl::SendRequestToServer(const SmartPtr<IOPackage> &pPackage)
{
	if (!CheckConnectionStatus())
		return PostNotConnected(PVE::DispatcherCommandToString(pPackage->header.type));

	IOSendJob::Handle hJob = m_ioClient->sendPackage ( pPackage );

	if (hJob == IOSendJob::InvalidHandle)
		return PostNotConnected("SendRequestToServer");

	if (IOSendJob::SendQueueIsFull == m_ioClient->getSendResult(hJob))
	{
		QString strUuid = (Uuid::createUuid()).toString();
		PostFailedResult(strUuid, "SendRequestToServer", PRL_ERR_TRY_AGAIN);
		return strUuid;
	}
	RegisterJobHandle(hJob);
    LOG_MESSAGE(DBG_DEBUG, "Sent request [%s]", m_ioClient->getJobUuid(hJob).toString().toUtf8().data());
	return (m_ioClient->getJobUuid(hJob));
}

bool CPveControl::IsDisplayEncodingsSupported () const
{
	if ( ! m_ioClient || m_ioClient->state() != IOSender::Connected )
		return false;
	IOCommunication::ProtocolVersion ver;
    return ( m_ioClient->serverProtocolVersion(ver) &&
             IOPROTOCOL_EXT_IO_DISPLAY_ENCODINGS_SUPPORT(ver) );
}

bool CPveControl::IsYUVDisplayEncodingsSupported () const
{
	if ( ! m_ioClient || m_ioClient->state() != IOSender::Connected )
		return false;
	IOCommunication::ProtocolVersion ver;
    return ( m_ioClient->serverProtocolVersion(ver) &&
             IOPROTOCOL_NEW_EXT_IO_DISPLAY_ENCODINGS_SUPPORT(ver) );
}

void CPveControl::DspCmdUserLogin(
									PrlHandleLoginHelperJobPtr pJob,
									const char *strRemoteHost,
                                    PRL_SECURITY_LEVEL connSec,
                                    quint32 nConnectionTimeout,
                                    PRL_UINT32& outPortNumber,
                                    QString& outHostName,
                                    PRL_CONST_STR certificate,
                                    PRL_CONST_STR privateKey)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserLogin()");

    // Check connection status
    if ( CheckConnectionStatus() || m_pLoginHelperJob )
    {
		PostFailedResult(pJob->GetJobUuid(), "CPveControl::DspCmdUserLogin()",
						 PRL_ERR_ALREADY_CONNECTED_TO_DISPATCHER);
		return;
    }

	m_pLoginHelperJob = pJob;

    // Extract host name and port number
    QString host = strRemoteHost;
    host.remove(QChar(' '), Qt::CaseInsensitive);
	//For further error paths notifications
	m_pLoginHelperJob->SetHost( host );

    int port;

    port = PrlGetDefaultListenPort();

    int index = host.lastIndexOf("#");

    if (index != -1)
    {
        bool bOk;
        int tmp = QString(host.right(host.length() - index - 1)).toInt(&bOk);
        if (bOk)
            port = tmp;

        host = host.left(index);
    }

	outPortNumber = port;
	outHostName = host;

	// Create IO certificate
	IOCredentials credentials = IOCredentials::fromPrivateKeyAndCertificatesChain( privateKey, certificate );
	if ( ! credentials.isNull() && ! credentials.isValid() )
	{
		PostFailedResult(pJob->GetJobUuid(), "CPveControl::DspCmdUserLogin()",
							 PRL_ERR_INVALID_ARG, QStringList() << host);
		WRITE_TRACE(DBG_FATAL, "Wrong certificate");
		return;
	}

    // Create IO client
    m_ioClient = new IOClient(
                        IORoutingTableHelper::GetClientRoutingTable(connSec),
                        IOSender::Client, host, port, false, credentials);

    ClearJobHandles();

    // Connect for incomming input packages
    bool res;
    res = QObject::connect( m_ioClient,
                      SIGNAL(onPackageReceived(const SmartPtr<IOPackage>)),
                      SLOT(handleEventPackage(const SmartPtr<IOPackage>)),
                      Qt::DirectConnection );
    Q_ASSERT(res);
    res = QObject::connect( m_ioClient,
                      SIGNAL(onResponsePackageReceived(IOSendJob::Handle, const SmartPtr<IOPackage>)),
                      SLOT(handleResponsePackage(IOSendJob::Handle, const SmartPtr<IOPackage>)),
                      Qt::DirectConnection );
    Q_ASSERT(res);
	res = QObject::connect( m_ioClient,
                      SIGNAL(onStateChanged(IOSender::State)),
                      SLOT(connectionStateChanged(IOSender::State)),
                      Qt::DirectConnection );
	Q_ASSERT(res);
	(void)res;

    m_ioClient->connectClient( nConnectionTimeout );
}

QString CPveControl::DspCmdUserRelogin(	const char *sUser,
						const char *sPassword,
						const char *sPrevSessionUuid,
						PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserRelogin()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateDspCmdUserLoginCommand(
											UTF8_2QSTR(sUser),
											UTF8_2QSTR(sPassword),
											UTF8_2QSTR(sPrevSessionUuid),
											nFlags);

	return SendRequestToServer(pRequest);
}


QString CPveControl::DspCmdUserLogoff()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserLogoff()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdUserLogoff);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSetNonInteractiveSession(bool bNonInteractive, quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSetNonInteractiveSession()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
													PVE::DspCmdSetNonInteractiveSession,
													QString("%1").arg(bNonInteractive),
													false,
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSetSessionConfirmationMode(
			bool bEnableConfirmationMode,
			const char *strUserName,
			const char *strPassword,
			quint32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSetSessionConfirmationMode()");

		CProtoCommandPtr pRequest = CProtoSerializer::CreateSetSessionConfirmationModeProtoCommand(
			bEnableConfirmationMode,
			UTF8_2QSTR(strUserName),
			UTF8_2QSTR(strPassword),
		nFlags);

	return SendRequestToServer(pRequest);
}

void CPveControl::DspCmdUserLoginLocal ( PrlHandleLoginLocalHelperJobPtr pJob,
                                            PRL_SECURITY_LEVEL connSec,
                                            quint32 nPort,
                                            PRL_UINT32& outPortNumber)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserLoginLocal()");

    // Check connection status
    if ( CheckConnectionStatus() || m_pLoginLocalHelperJob )
    {
		PostFailedResult(pJob->GetJobUuid(), "CPveControl::DspCmdUserLoginLocal()",
							PRL_ERR_ALREADY_CONNECTED_TO_DISPATCHER);
		return;
    }

	m_pLoginLocalHelperJob = pJob;

    // Create IO client
#ifndef _WIN_
    Q_UNUSED(nPort);
	outPortNumber = 0;
    m_ioClient = new IOClient(
			IORoutingTableHelper::GetClientRoutingTable(connSec),
			IOSender::Client,
			ParallelsDirs::getDispatcherLocalSocketPath(),
			0, true );
#else
    quint32 port = (nPort ? nPort : PrlGetDefaultListenPort());
	outPortNumber = port;
    m_ioClient = new IOClient(
                        IORoutingTableHelper::GetClientRoutingTable(connSec),
                        IOSender::Client, IOService::LoopbackAddr, port );
#endif

	ClearJobHandles();

	// Connect for incomming input packages
	bool res;
	res = QObject::connect( m_ioClient,
                      SIGNAL(onPackageReceived(const SmartPtr<IOPackage>)),
                      SLOT(handleEventPackage(const SmartPtr<IOPackage>)),
                      Qt::DirectConnection );
	Q_ASSERT(res);
	res = QObject::connect( m_ioClient,
                      SIGNAL(onResponsePackageReceived(IOSendJob::Handle, const SmartPtr<IOPackage>)),
                      SLOT(handleResponsePackage(IOSendJob::Handle, const SmartPtr<IOPackage>)),
                      Qt::DirectConnection );
	Q_ASSERT(res);
	res = QObject::connect( m_ioClient,
                      SIGNAL(onStateChanged(IOSender::State)),
                      SLOT(connectionStateChanged(IOSender::State)),
                      Qt::DirectConnection );
	Q_ASSERT(res);
	(void)res;

	m_ioClient->connectClient(PVE_CONNECTION_TIMEOUT);
}

QString CPveControl::DspCmdDirVmCreate(const char *strVmConfig, const char *strVmHomePath, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirVmCreate()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmCreateProtoCommand(UTF8_2QSTR(strVmConfig),
														UTF8_2QSTR(strVmHomePath), nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmStart(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmStart()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmStart,
															UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmStartEx(const char *strVmUUID, quint32 nStartMode, quint32 nReserved)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmStartEx()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmStartExProtoCommand(UTF8_2QSTR(strVmUUID),
																			  nStartMode,
																			  nReserved);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmLoginInGuest(const char *strVmUUID, const char *sUserName,
								const char *sUserPassword, quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmLoginInGuest()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmLoginInGuestProtoCommand(
													UTF8_2QSTR(strVmUUID),
													UTF8_2QSTR(sUserName),
													sUserPassword ? UTF8_2QSTR(sUserPassword) : QString(),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmAuthWithGuestSecurityDb(const char *strVmUUID, const char *sUserName,
								const char *sUserPassword, quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmAuthWithGuestSecurityDb()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmAuthWithGuestSecurityDbProtoCommand(
													UTF8_2QSTR(strVmUUID),
													UTF8_2QSTR(sUserName),
													sUserPassword ? UTF8_2QSTR(sUserPassword) : QString(),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmStartVncServer(const char *strVmUUID,  quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmStartVncServer()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
													PVE::DspCmdVmStartVNCServer,
													UTF8_2QSTR(strVmUUID),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmStopVncServer(const char *strVmUUID,  quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmStopVncServer()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
													PVE::DspCmdVmStopVNCServer,
													UTF8_2QSTR(strVmUUID),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmLock(const char *strVmUUID,  quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmLock()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
													PVE::DspCmdVmLock,
													UTF8_2QSTR(strVmUUID),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmUnlock(const char *strVmUUID,  quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmUnlock()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
													PVE::DspCmdVmUnlock,
													UTF8_2QSTR(strVmUUID),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGuestLogout(const char *strVmUuid, const char *strVmGuestUuid, quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGuestLogout()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateBasicVmGuestProtoCommand(
													PVE::DspCmdVmGuestLogout,
													UTF8_2QSTR(strVmUuid),
													UTF8_2QSTR(strVmGuestUuid),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGuestGetNetworkSettings(const char *strVmUuid,
													 const char *strVmGuestUuid,
													 quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGuestGetNetworkSettings()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateBasicVmGuestProtoCommand(
													PVE::DspCmdVmGuestGetNetworkSettings,
													UTF8_2QSTR(strVmUuid),
													UTF8_2QSTR(strVmGuestUuid),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGuestRunProgram(const char *strVmUuid,
											 const char *strVmGuestUuid,
											 const char *strProgramName,
											 const QStringList &lstArgs,
											 const QStringList &lstVars,
											 quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGuestRunProgram()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmGuestRunProgramProtoCommand(
													UTF8_2QSTR(strVmUuid),
													UTF8_2QSTR(strVmGuestUuid),
													UTF8_2QSTR(strProgramName),
													lstArgs,
													lstVars,
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGuestSetUserPasswd(const char *strVmUuid,
												const char *strVmGuestUuid,
												const char *strUserLoginName,
												const char *strUserPassword,
												quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGuestSetUserPasswd()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmGuestSetUserPasswdProtoCommand(
													UTF8_2QSTR(strVmUuid),
													UTF8_2QSTR(strVmGuestUuid),
													UTF8_2QSTR(strUserLoginName),
													UTF8_2QSTR(strUserPassword),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmResizeDiskImage(const char *strVmUuid, const QString &sDiskImage,
								quint32 nNewSize, quint32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmResizeDiskImageProtoCommand(
										UTF8_2QSTR(strVmUuid),
										sDiskImage,
										nNewSize,
										nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmRestartGuest(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmRestartGuest()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmRestartGuest,
																			UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmStop(const char *strVmUUID, PRL_UINT32 nStopMode, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmStop()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoVmCommandStop(UTF8_2QSTR(strVmUUID), nStopMode, nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmPause(const char *strVmUUID, bool bAcpi)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmPause()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoVmCommandWithAcpiSign(PVE::DspCmdVmPause,
													UTF8_2QSTR(strVmUUID), bAcpi);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmCompact(const char *strVmUUID, quint32 uMask, quint32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmCompact()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam(
													PVE::DspCmdVmCompact,
													UTF8_2QSTR(strVmUUID),
													QString("%1").arg(uMask),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmCancelCompact(const char *strVmUUID)
{
       LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmCancelCompact()");


       CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmCancelCompact,
																					UTF8_2QSTR(strVmUUID));

       return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmConvertDisks(const char *strVmUUID, quint32 uMask, quint32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmConvertDisks()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam(
													PVE::DspCmdVmConvertDisks,
													UTF8_2QSTR(strVmUUID),
													QString("%1").arg(uMask),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmChangeSid(const char *strVmUUID, PRL_UINT32 nFlags)
{
       LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmChangeSid()");


       CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmChangeSid,
																					UTF8_2QSTR(strVmUUID),
																					nFlags);

       return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmResetUptime(const char *strVmUUID, PRL_UINT32 nFlags)
{
       LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmResetUptime()");


       CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmResetUptime,
																					UTF8_2QSTR(strVmUUID),
																					nFlags);

       return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmSuspend(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmSuspend()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmSuspend,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGetSuspendedScreen(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGetSuspendedScreen()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmGetSuspendedScreen,
																			UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmResume(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmResume()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmResume,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmDropSuspendedState(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmDropSuspendedState()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmDropSuspendedState,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmCreateSnapshot(const char *strVmUUID,
		const char *strName,
		const char *strDescription,
		const char *strStateUuid,
		quint32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmCreateSnapshot()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateCreateSnapshotProtoCommand(UTF8_2QSTR(strVmUUID),
			UTF8_2QSTR(strName),
			UTF8_2QSTR(strDescription),
			UTF8_2QSTR(strStateUuid),
			QString(),
			QString(),
			nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmSwitchToSnapshot(const char *strVmUUID, const char *strStateUuid,
		PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmSwitchToSnapshot()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateSwitchToSnapshotProtoCommand(UTF8_2QSTR(strVmUUID),
						UTF8_2QSTR(strStateUuid), nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmDeleteSnapshot(const char *strVmUUID, const char *strStateUuid, const bool bMerge)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmDeleteSnapshot()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateDeleteSnapshotProtoCommand(UTF8_2QSTR(strVmUUID),
													UTF8_2QSTR(strStateUuid), bMerge);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGetSnapshotsTree(const char *strVmUUID, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGetSnapshotsTree()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmGetSnapshotsTree,
													UTF8_2QSTR(strVmUUID), nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmUpdateSnapshotData(const char *strVmUUID, const char *strStateUuid,
												const char *strNewName, const char *strNewDescription)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmUpdateSnapshotData()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateUpdateSnapshotDataProtoCommand(
		UTF8_2QSTR(strVmUUID), UTF8_2QSTR(strStateUuid),
		UTF8_2QSTR(strNewName), UTF8_2QSTR(strNewDescription));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmReset(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmReset()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmReset,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmInternal(const char *strVmUUID,
								   const char *strCmd,
								   const QStringList &lstArgs
								   )
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmInternal()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmInternalProtoCommand(
											UTF8_2QSTR(strVmUUID), UTF8_2QSTR(strCmd), lstArgs);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmSubscribeToGuestStatistics(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmSubscribeToGuestStatistics()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmSubscribeToGuestStatistics,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmUnsubscribeFromGuestStatistics(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmUnsubscribeFromGuestStatistics()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
													PVE::DspCmdVmUnsubscribeFromGuestStatistics,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirRegVm(const char *strVmDirPath, const char *strVmUuid, quint32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirRegVmEx()");


	CProtoCommandPtr
		pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam(
					PVE::DspCmdDirRegVm,
					UTF8_2QSTR(strVmUuid ? strVmUuid : ""),
					UTF8_2QSTR(strVmDirPath),
					nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirReg3rdPartyVm(const char *strVmConfigPath, const char *strVmRootDirPath, quint32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirReg3rdPartyVm()");


	CProtoCommandPtr
		pRequest = CProtoSerializer::CreateDspCmdDirReg3rdPartyVmProtoCommand(
					UTF8_2QSTR(strVmConfigPath),
					UTF8_2QSTR(strVmRootDirPath),
					nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirUnregVm(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirUnregVm()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdDirUnregVm,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirRestoreVm(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirRestoreVm()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdDirRestoreVm,
																			UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirVmDelete(const char *sVmUuid,const QStringList & lstDevices)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirVmDelete()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmDeleteProtoCommand(UTF8_2QSTR(sVmUuid), lstDevices);

	return SendRequestToServer(pRequest);

}

QString CPveControl::DspCmdDirGetVmList(PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirGetVmList()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdDirGetVmList, false, nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdStartSearchConfig(const QStringList &searchDirsList)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdStartSearchConfig()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateStartSearchConfigProtoCommand(searchDirsList);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdConvertOldHdd(const QStringList &lstHddPathsToConvert)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdConvertOldHdd()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateConvertHddProtoCommand(lstHddPathsToConvert);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUpdateUsbAssocList(const QStringList &lstUsbList,
								 quint32 nListVersion,
								 quint32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUpdateUsbAssocList()");


	CProtoCommandPtr pRequest =
			CProtoSerializer::CreateUpdateUsbAssocListProtoCommand(lstUsbList,nListVersion,nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserGetHostHwInfo()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserGetHostHwInfo()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdUserGetHostHwInfo);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsGetDiskList()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsGetDiskList()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdFsGetDiskList);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsGetCurrentDirectory()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsGetCurrentDirectory()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdFsGetCurrentDirectory);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsGetDirectoryEntries(const char *strSourceDir)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsGetDirectoryEntries()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
													PVE::DspCmdFsGetDirectoryEntries,
													UTF8_2QSTR(strSourceDir));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsCreateDirectory(const char *strDir)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsCreateDirectory()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdFsCreateDirectory,
													UTF8_2QSTR(strDir));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsRenameEntry(const char *strOldEntryName, const char *strNewEntryName)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsRenameEntry()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateFsRenameEntryProtoCommand(
													UTF8_2QSTR(strOldEntryName),
													UTF8_2QSTR(strNewEntryName));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsRemoveEntry(const char *strEntryName)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsRemoveEntry()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdFsRemoveEntry,
													UTF8_2QSTR(strEntryName));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsCanCreateFile(const char *strEntryName)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsCanCreateFile()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdFsCanCreateFile,
													UTF8_2QSTR(strEntryName));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserGetProfile()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserGetProfile()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdUserGetProfile);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserInfoList()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserInfoList()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdUserInfoList);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserInfo(const char* strUserId)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserInfo()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdUserInfo,
																					UTF8_2QSTR(strUserId));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdPrepareForHibernate(PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdPrepareForHibernate()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdPrepareForHibernate,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdAfterHostResume(PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdAfterHostResume()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
	PVE::DspCmdAfterHostResume,
	false,
	nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetVirtualNetworkList(PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetVirtualNetworkList()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdGetVirtualNetworkList,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdAddVirtualNetwork(const char* strVirtNet, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdAddVirtualNetwork()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdAddVirtualNetwork,
									strVirtNet,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUpdateVirtualNetwork(const char* strVirtNet, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUpdateVirtualNetwork()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdUpdateVirtualNetwork,
									strVirtNet,
									nFlags
									);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDeleteVirtualNetwork(const char* strVirtNet, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDeleteVirtualNetwork()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdDeleteVirtualNetwork,
									strVirtNet,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUpdateOfflineService(const char* str, PRL_UINT32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdUpdateOfflineService,
									str,
									false,
									nFlags
									);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDeleteOfflineService(const char* str, PRL_UINT32 nFlags)
{


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdDeleteOfflineService,
									str,
									false,
									nFlags
									);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetOfflineServicesList(PRL_UINT32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdGetOfflineServicesList,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUpdateNetworkClassesConfig(const char* str, PRL_UINT32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdUpdateNetworkClassesConfig,
									str,
									false,
									nFlags
									);
	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetNetworkClassesConfig(PRL_UINT32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdGetNetworkClassesConfig,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}


QString CPveControl::DspCmdUpdateNetworkShapingConfig(const char* str, PRL_UINT32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdUpdateNetworkShapingConfig,
									str,
									false,
									nFlags
									);
	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetNetworkShapingConfig(PRL_UINT32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdGetNetworkShapingConfig,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdRestartNetworkShaping(PRL_UINT32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdRestartNetworkShaping,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdConfigureGenericPci(const char* strDevList, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdConfigureGenericPci()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdConfigureGenericPci,
									strDevList,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdAllHostUsers()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdAllHostUsers()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdAllHostUsers);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserProfileBeginEdit()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserProfileBeginEdit()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdUserProfileBeginEdit);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserProfileCommit(const char *strUserProfile)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserProfileCommit()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdUserProfileCommit,
													UTF8_2QSTR(strUserProfile));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSubscribeToHostStatistics()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSubscribeToHostStatistics()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdSubscribeToHostStatistics);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUnsubscribeFromHostStatistics()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUnsubscribeFromHostStatistics()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
													PVE::DspCmdUnsubscribeFromHostStatistics);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetHostCommonInfo()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetHostCommonInfo()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdGetHostCommonInfo);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetHostStatistics()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetHostStatistics()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdGetHostStatistics);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdHostCommonInfoBeginEdit()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdHostCommonInfoBeginEdit()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdHostCommonInfoBeginEdit);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdHostCommonInfoCommit(const char *strHostCommonInfo, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdHostCommonInfoCommit()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
		PVE::DspCmdHostCommonInfoCommit, UTF8_2QSTR(strHostCommonInfo), false, nFlags );

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserCancelOperation(const char *strCancellingRequestUuid)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserCancelOperation()");


	CProtoCommandPtr pRequest
		= CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdUserCancelOperation,
																UTF8_2QSTR(strCancellingRequestUuid));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmAnswer(const char *strEvent)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmAnswer()");


	//Extract request question package info from sending VM event
	CVmEvent answerEvent(UTF8_2QSTR(strEvent));
	SmartPtr<IOPackage> pRequestPkg;
	{
		QMutexLocker _lock(&m_JobsHandlesHashMutex);
		pRequestPkg = m_QuestionsRequestPkgsHash.take(answerEvent.getInitRequestId());
	}

	SmartPtr<IOPackage> pPackage = DispatcherPackage::createInstance(PVE::DspCmdVmAnswer,
																	 UTF8_2QSTR(strEvent),
																	 pRequestPkg);

	return SendRequestToServer(pPackage);
}

QString CPveControl::DspCmdVmDevConnect(const char *strVmUUID,
													PRL_DEVICE_TYPE nDeviceType,
													int nDeviceIndex,
													const char *strDeviceConfig)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmDevConnect()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmDeviceProtoCommand(PVE::DspCmdVmDevConnect,
																			 UTF8_2QSTR(strVmUUID),
																			 nDeviceType,
																			 nDeviceIndex,
																			 UTF8_2QSTR(strDeviceConfig));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmDevDisconnect(const char *strVmUUID,
                                           PRL_DEVICE_TYPE nDeviceType,
                                           int nDeviceIndex,
										   const char *strDeviceConfig)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmDevDisconnect()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmDeviceProtoCommand(PVE::DspCmdVmDevDisconnect,
																			 UTF8_2QSTR(strVmUUID),
																			 nDeviceType,
																			 nDeviceIndex,
																			 UTF8_2QSTR(strDeviceConfig));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGetProblemReport(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGetProblemReport()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
													PVE::DspCmdVmGetProblemReport,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGetPackedProblemReport(const char *strVmUUID,
    PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGetPackedProblemReport()");
    CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
													PVE::DspCmdVmGetPackedProblemReport,
													UTF8_2QSTR(strVmUUID),
													nFlags);
	return SendRequestToServer(pRequest);
}


QString CPveControl::DspCmdSendProblemReport(const QString& report,
    const QString &vmUuid,
    PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSendProblemReport()");
    CProtoCommandPtr pPackage = CProtoSerializer::CreateSendProblemReportProtoCommand(
                report, vmUuid, nFlags);

    return SendRequestToServer(pPackage);
}


QString CPveControl::DspCmdDirVmClone(const char *strVmUuid,
		const char *strVmName,
		const char *strNewVmUuid,
		const char *strVmHomePath,
		PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirVmClone()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmCloneProtoCommand(
			UTF8_2QSTR(strVmUuid),
			UTF8_2QSTR(strVmName),
			UTF8_2QSTR(strNewVmUuid),
			UTF8_2QSTR(strVmHomePath),
			nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirVmMigrate(const char *strVmUuid,
		const char *strTargetServerHostname,
		quint32 nTargetServerPort,
		const char *strTargetServerSessionUuid,
		const char *sTargetVmName,
		const char *strTargetVmHomePath,
		quint32 nMigrationFlags,
		quint32 nReservedFlags,
		bool bForceOperation)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirVmMigrate()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmMigrateProtoCommand(UTF8_2QSTR(strVmUuid),
																	UTF8_2QSTR(strTargetServerHostname),
																	nTargetServerPort,
																	UTF8_2QSTR(strTargetServerSessionUuid),
																	UTF8_2QSTR(sTargetVmName),
																	UTF8_2QSTR(strTargetVmHomePath),
																	nMigrationFlags,
																	nReservedFlags,
																	bForceOperation);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdCreateVmBackup(
		const char *strVmUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		const char *strDescription,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::%s", __FUNCTION__);


	CProtoCommandPtr pRequest = CProtoSerializer::CreateCreateVmBackupCommand(
		UTF8_2QSTR(strVmUuid),
		UTF8_2QSTR(strServerHostname),
		nServerPort,
		UTF8_2QSTR(strServerSessionUuid),
		UTF8_2QSTR(strDescription),
		nBackupFlags,
		nReservedFlags,
		bForceOperation);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdRestoreVmBackup(
		const char *strVmUuid,
		const char *strBackupUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		const char *strTargetVmHomePath,
		const char *strTargetVmName,
		quint32 nRestoreFlags,
		quint32 nReservedFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::%s", __FUNCTION__);


	CProtoCommandPtr pRequest = CProtoSerializer::CreateRestoreVmBackupCommand(
		UTF8_2QSTR(strVmUuid),
		UTF8_2QSTR(strBackupUuid),
		UTF8_2QSTR(strServerHostname),
		nServerPort,
		UTF8_2QSTR(strServerSessionUuid),
		UTF8_2QSTR(strTargetVmHomePath),
		UTF8_2QSTR(strTargetVmName),
		nRestoreFlags,
		nReservedFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetBackupTree(
		const char *strVmUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::%s", __FUNCTION__);


	CProtoCommandPtr pRequest = CProtoSerializer::CreateGetBackupTreeCommand(
		UTF8_2QSTR(strVmUuid),
		UTF8_2QSTR(strServerHostname),
		nServerPort,
		UTF8_2QSTR(strServerSessionUuid),
		nBackupFlags,
		nReservedFlags,
		bForceOperation);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdRemoveVmBackup(
		const char *strVmUuid,
		const char *strBackupUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		quint32 nRestoreFlags,
		quint32 nReservedFlags,
		bool bForceOperation)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::%s", __FUNCTION__);


	CProtoCommandPtr pRequest = CProtoSerializer::CreateRemoveVmBackupCommand(
		UTF8_2QSTR(strVmUuid),
		UTF8_2QSTR(strBackupUuid),
		UTF8_2QSTR(strServerHostname),
		nServerPort,
		UTF8_2QSTR(strServerSessionUuid),
		nRestoreFlags,
		nReservedFlags,
		bForceOperation);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirVmEditBegin(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirVmEditBegin()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdDirVmEditBegin,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirVmEditCommit(const char *strVmConfig, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirVmEditCommit()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
		PVE::DspCmdDirVmEditCommit,
		UTF8_2QSTR(strVmConfig),
		false,
		nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirCreateImage(const char *strVmUUID,
										  const char *strImageConfig,
										  bool bRecreateIsAllowed,
										  bool bNonInteractiveMode)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirCreateImage()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateImageCreateProtoCommand(UTF8_2QSTR(strVmUUID),
																				UTF8_2QSTR(strImageConfig),
																				bRecreateIsAllowed,
																				bNonInteractiveMode);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirCopyImage(const char *strVmUUID,
										const char *strImageConfig,
										const char *strNewImageName,
										const char *strTargetPath,
										quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirCopyImage()");


	CProtoCommandPtr pRequest = CProtoSerializer::CopyImageCreateProtoCommand(UTF8_2QSTR(strVmUUID),
																			  UTF8_2QSTR(strImageConfig),
																			  UTF8_2QSTR(strNewImageName),
																			  UTF8_2QSTR(strTargetPath),
																			  nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUpdateDeviceInfo(const char * strImageConfig)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUpdateDeviceInfo()");


		CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdUpdateDeviceInfo,
							UTF8_2QSTR(strImageConfig));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirInstallGuestOS(const char *strUserName,
											 const char *strCompanyName,
											 const char *strSerialNo)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirInstallGuestOS()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateSerialNumProtoCommand(PVE::DspCmdDirInstallGuestOS,
														UTF8_2QSTR(strUserName),
														UTF8_2QSTR(strCompanyName),
														UTF8_2QSTR(strSerialNo));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGetConfig(const char *strVmUUID, quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGetConfig()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmGetConfig,
													UTF8_2QSTR(strVmUUID),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetDefaultVmConfig(const QString &sParam, quint32 nFlags)
{

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
				PVE::DspCmdGetDefaultVmConfig,
				sParam, nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmGetStatistics(const char *strVmUUID, PRL_UINT32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmGetStatistics()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmGetStatistics,
													UTF8_2QSTR(strVmUUID),
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmInitiateDevStateNotifications(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmInitiateDevStateNotifications()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
		PVE::DspCmdVmInitiateDevStateNotifications, UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmUpdateSecurity(const char *strVmUUID, const char *strVmSecurity)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmUpdateSecurity()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmUpdateSecurityCommand(UTF8_2QSTR(strVmUUID),
													UTF8_2QSTR(strVmSecurity));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmSectionValidateConfig(const char * strVmConfig, PRL_VM_CONFIG_SECTIONS nSection)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmSectionValidateConfig()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmValidateConfigProtoCommand(
													UTF8_2QSTR(strVmConfig),
													nSection);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmMigrateCancel(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmMigrateCancel()");


	CProtoCommandPtr pRequest =
		CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmMigrateCancel, UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetVmInfo(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetVmInfo()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdGetVmInfo,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetVmToolsInfo(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetVmToolsInfo()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdGetVmToolsInfo,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCGetDispatcherRTInfo()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCGetDispatcherRTInfo()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdSMCGetDispatcherRTInfo);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCGetCommandHistoryByVm(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCGetCommandHistoryByVm()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdSMCGetCommandHistoryByVm,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCGetCommandHistoryByUser(const char *strUserUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCGetCommandHistoryByUser()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
													PVE::DspCmdSMCGetCommandHistoryByUser,
													UTF8_2QSTR(strUserUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCShutdownDispatcher(PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCShutdownDispatcher()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
		PVE::DspCmdSMCShutdownDispatcher
		, QString::number( (nFlags&PSHF_FORCE_SHUTDOWN)? 1 : 0)
		, false // bForceQuestionsSign
		, nFlags );

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCRestartDispatcher()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCRestartDispatcher()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdSMCRestartDispatcher);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCDisconnectUser(const char *strUserUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCDisconnectUser()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdSMCDisconnectUser,
													UTF8_2QSTR(strUserUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCDisconnectAllUsers()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCDisconnectAllUsers()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdSMCDisconnectAllUsers);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCCancelUserCommand(const char *strCancelRequestUuid)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCCancelUserCommand()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
													PVE::DspCmdSMCCancelUserCommand,
													UTF8_2QSTR(strCancelRequestUuid));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCShutdownVm(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCShutdownVm()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdSMCShutdownVm,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSMCRestartVm(const char *strVmUUID)
{
   LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSMCRestartVm()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdSMCRestartVm,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdNetPrlNetworkServiceStart()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdNetPrlNetworkServiceStart()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdNetPrlNetworkServiceStart);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdNetPrlNetworkServiceStop()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdNetPrlNetworkServiceStop()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdNetPrlNetworkServiceStop);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdNetPrlNetworkServiceRestart()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdNetPrlNetworkServiceRestart()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
													PVE::DspCmdNetPrlNetworkServiceRestart);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdNetPrlNetworkServiceRestoreDefaults()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdNetPrlNetworkServiceRestoreDefaults()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
													PVE::DspCmdNetPrlNetworkServiceRestoreDefaults);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetNetServiceStatus()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetNetServiceStatus()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdGetNetServiceStatus);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdAddNetAdapter(const char *strNetAdapter)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdAddNetAdapter()");


	CProtoCommandPtr pRequest =
		CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdAddNetAdapter, UTF8_2QSTR(strNetAdapter));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDeleteNetAdapter(unsigned int nIndex)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDeleteNetAdapter()");


	CProtoCommandPtr pRequest =
		CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdDeleteNetAdapter, QString::number(nIndex));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUpdateNetAdapter(const char *strNetAdapter)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUpdateNetAdapter()");


	CProtoCommandPtr pRequest =
		CProtoSerializer::CreateProtoCommandWithOneStrParam(PVE::DspCmdUpdateNetAdapter,
															UTF8_2QSTR(strNetAdapter));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserUpdateLicense(
	/* [in] */ const char *strKey,
	/* [in] */ const char *strUser,
	/* [in] */ const char *strCompany,
	/* [in] */ PRL_UINT32 nFlags )
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserUpdateLicense()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateSerialNumProtoCommand(PVE::DspCmdUserUpdateLicense,
																			  UTF8_2QSTR(strUser),
																			  UTF8_2QSTR(strCompany),
																			  UTF8_2QSTR(strKey),
																			  nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUserGetLicenseInfo()
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUserGetLicenseInfo()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(PVE::DspCmdUserGetLicenseInfo);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmCreateUnattendedFloppy(
	/* [in] */ const char *strVmUUID,
	/* [in] */ PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType,
	/* [in] */ const char *strKey,
	/* [in] */ const char *strUser,
	/* [in] */ const char *strCompany)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmCreateUnattendedFloppy()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmCreateUnattendedProtoCommand(UTF8_2QSTR(strVmUUID),
																						UTF8_2QSTR(strUser),
																						UTF8_2QSTR(strCompany),
																						UTF8_2QSTR(strKey),
																						nGuestDistroType);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdCreateUnattendedCd(
		/* [in] */ PRL_UINT32 nGuestType,
		/* [in] */ const char *sUserName,
		/* [in] */ const char *sPasswd,
		/* [in] */ const char *sFullUserName,
		/* [in] */ const char *sOsDistroPath,
		/* [in] */ const char *sOutImagePath)


{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdCreateUnattendedCd()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateSrvCreateUnattendedCdProtoCommand(nGuestType,
		UTF8_2QSTR(sUserName), UTF8_2QSTR(sPasswd),
	        UTF8_2QSTR(sFullUserName), UTF8_2QSTR(sOsDistroPath),
		UTF8_2QSTR(sOutImagePath));

	return SendRequestToServer(pRequest);
}

IOSender::State CPveControl::GetState()
{
	if (m_ioClient)
		return (m_ioClient->state());
	return (IOSender::Disconnected);
}

QString CPveControl::DspCmdAttachToLostTask(const char *strTaskId )
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdAttachToLostTask()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
		PVE::DspCmdAttachToLostTask,
		UTF8_2QSTR(strTaskId)
		);
	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmInstallUtility(const char *strVmUUID, const char *strId)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmInstallUtility()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam(
													PVE::DspCmdVmInstallUtility,
													UTF8_2QSTR(strVmUUID),
													UTF8_2QSTR(strId));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmInstallTools(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmInstallTools()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmInstallTools,
													UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmRunCompressor(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmRunCompressor()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmRunCompressor,
																			UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmCancelCompressor(const char *strVmUUID)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmCancelCompressor()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(PVE::DspCmdVmCancelCompressor,
																			UTF8_2QSTR(strVmUUID));

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmUpdateToolsSection(const char *strVmUUID, const char* strToolsSection)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmUpdateToolsSection()");


	CProtoCommandPtr
		pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam(
				PVE::DspCmdVmUpdateToolsSection
				, UTF8_2QSTR(strVmUUID)
				, UTF8_2QSTR(strToolsSection)
			);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdFsGenerateEntryName(
			const char *strDirPath,
			const char *strFilenamePrefix,
			const char *strFilenameSuffix,
			const char *strIndexDelimiter
			)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdFsGenerateEntryName()");


	CProtoCommandPtr pRequest =
			CProtoSerializer::CreateDspCmdFsGenerateEntryNameCommand(
				UTF8_2QSTR(strDirPath),
				UTF8_2QSTR(strFilenamePrefix),
				UTF8_2QSTR(strFilenameSuffix),
				UTF8_2QSTR(strIndexDelimiter)
				);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdSubscribePerfStats(const char *filter, const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSubscribePerfStats(%s, %s)", filter, strVmUUID);

	return SendRequestToServer( CProtoSerializer::CreatePerfStatsProtoCommand(
                                    CProtoPerfStatsCommand::PSA_SUBSCRIBE,
                                    UTF8_2QSTR(filter), UTF8_2QSTR(strVmUUID)) ) ;
}

QString CPveControl::DspCmdUnsubscribePerfStats(const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUnsubscribePerfStats(%s)", strVmUUID);

	return SendRequestToServer( CProtoSerializer::CreatePerfStatsProtoCommand(
                                    CProtoPerfStatsCommand::PSA_UNSUBSCRIBE,
                                    QString(), UTF8_2QSTR(strVmUUID)) ) ;
}

QString CPveControl::DspCmdGetPerfStats(const char *filter, const char *strVmUUID)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetPerfStats(%s, %s)", filter, strVmUUID);

	return SendRequestToServer( CProtoSerializer::CreatePerfStatsProtoCommand(
                                    CProtoPerfStatsCommand::PSA_GET,
                                    UTF8_2QSTR(filter), UTF8_2QSTR(strVmUUID)) ) ;
}

QString CPveControl::DspCmdStorageSetValue(
	const char *sKey, const char *sValue, PRL_UINT32 nFlags )
{

	return SendRequestToServer( CProtoSerializer::CreateDspCmdStorageSetValueCommand(
		UTF8_2QSTR(sKey),
		UTF8_2QSTR(sValue),
		nFlags)) ;
}

QString CPveControl::DspCmdVmStorageSetValue(
	const char *sVmUuid, const char *sKey, const char *sValue, PRL_UINT32 nFlags )
{

	return SendRequestToServer( CProtoSerializer::CreateDspCmdVmStorageSetValueCommand(
		UTF8_2QSTR(sVmUuid),
		UTF8_2QSTR(sKey),
		UTF8_2QSTR(sValue),
		nFlags)) ;
}

QString CPveControl::DspCmdSendClientStatistics( const char *sStatistics, PRL_UINT32 nFlags )
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdSendClientStatistics()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
				PVE::DspCmdSendClientStatistics,
				UTF8_2QSTR( sStatistics ),
				false,
				nFlags
				);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdInstallAppliance(const char* strAppCfg, quint32 nFlags)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdInstallAppliance()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
													PVE::DspCmdInstallAppliance,
													UTF8_2QSTR(strAppCfg),
													false,
													nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmSetProtection( const char* strVmUUID, const char *strProtectionPassword,
											quint32 nReserved )
{

	CProtoCommandPtr
		pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam( PVE::DspCmdVmSetProtection,
				UTF8_2QSTR( strVmUUID ),
				UTF8_2QSTR( strProtectionPassword ),
				nReserved
				);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmRemoveProtection( const char* strVmUUID, const char *strProtectionPassword,
											   quint32 nReserved )
{

	CProtoCommandPtr
		pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam( PVE::DspCmdVmRemoveProtection,
				UTF8_2QSTR( strVmUUID ),
				UTF8_2QSTR( strProtectionPassword ),
				nReserved
				);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetCtTemplateList(PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetCtTemplateList()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdGetCtTemplateList,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdRemoveCtTemplate(PRL_CONST_STR sName,
	PRL_CONST_STR sOsTmplName, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdRemoveCtTemplate()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithTwoStrParams(
									PVE::DspCmdRemoveCtTemplate,
									UTF8_2QSTR(sName),
									UTF8_2QSTR(sOsTmplName),
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdCopyCtTemplate(
			const char *sName,
			const char *sOsTmplName,
			const char *sTargetServerHostname,
			quint32 nTargetServerPort,
			const char *sTargetServerSessionUuid,
			quint32 nFlags,
			quint32 nReservedFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdCopyCtTemplate()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateCopyCtTemplateProtoCommand(
				UTF8_2QSTR(sName),
				(sOsTmplName) ? UTF8_2QSTR(sOsTmplName) : QString(),
				UTF8_2QSTR(sTargetServerHostname),
				nTargetServerPort,
				UTF8_2QSTR(sTargetServerSessionUuid),
				nFlags,
				nReservedFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdAddIPPrivateNetwork(const char *strPrivNet, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdAddIPPrivateNetwork()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdAddIPPrivateNetwork,
									strPrivNet,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdRemoveIPPrivateNetwork(const char *strPrivNet, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdRemoveIPPrivateNetwork()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdRemoveIPPrivateNetwork,
									strPrivNet,
									false,
									nFlags);
	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdUpdateIPPrivateNetwork(const char *strPrivNet, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdUpdateIPPrivateNetwork()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdUpdateIPPrivateNetwork,
									strPrivNet,
									false,
									nFlags);
	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetIPPrivateNetworksList(PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetIPPrivateNetworksList()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdGetIPPrivateNetworksList,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdRefreshPlugins(PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdRefreshPlugins()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdRefreshPlugins,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetPluginsList(const char *strClassId, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetPluginsList()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdGetPluginsList,
									strClassId,
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmMount(PRL_CONST_STR strVmUUID, PRL_CONST_STR sMntPath, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmMount()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoVmCommandWithOneStrParam(
			PVE::DspCmdVmMount,
			strVmUUID,
			sMntPath,
			nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmUmount(PRL_CONST_STR strVmUUID, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmUmount()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoBasicVmCommand(
			PVE::DspCmdVmUmount,
			strVmUUID,
			nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetDiskFreeSpace(PRL_CONST_STR strPath, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetDiskFreeSpace()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
			PVE::DspCmdGetDiskFreeSpace,
			UTF8_2QSTR(strPath),
			false,
			nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdDirVmMove(PRL_CONST_STR sVmUuid, PRL_CONST_STR sNewHomePath, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdDirVmMove()");


	CProtoCommandPtr pRequest = CProtoSerializer::CreateVmMoveProtoCommand(
				UTF8_2QSTR(sVmUuid), UTF8_2QSTR(sNewHomePath), nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetVmConfigById(PRL_CONST_STR sSearchId, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetVmConfigById()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdGetVmConfigById,
									UTF8_2QSTR(sSearchId),
									false,
									nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetCPUPoolsList()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetCPUPoolsList()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdGetCPUPoolsList);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdJoinCPUPool()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdJoinCPUPool()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdJoinCPUPool);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdLeaveCPUPool()
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdLeaveCPUPool()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmdLeaveCPUPool);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdMoveToCPUPool(PRL_CONST_STR strPoolName)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdMoveToCPUPool()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdMoveToCPUPool,
									strPoolName);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdRecalculateCPUPool(PRL_CONST_STR strPoolName)
{
    LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdRecalculateCPUPool()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
									PVE::DspCmdRecalculateCPUPool,
									strPoolName);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdVmCaptureScreen(PRL_CONST_STR sVmUuid, PRL_UINT32 nWidth,
		PRL_UINT32 nHeight, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdCaptureScreen()");

	return SendRequestToServer(CProtoSerializer::CreateVmCaptureScreenProtoCommand(
			UTF8_2QSTR(sVmUuid), nWidth, nHeight, nFlags));
}

QString CPveControl::DspCmdVmCommitEncryption(PRL_CONST_STR strVmConfig, PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdVmCommitEncryption()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
		PVE::DspCmdVmCommitEncryption,
		UTF8_2QSTR(strVmConfig),
		false,
		nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdGetVcmmdConfig(PRL_UINT32 nFlags)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmdGetVcmmdConfig()");

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
		PVE::DspCmdGetVcmmdConfig,
		false,
		nFlags);

	return SendRequestToServer(pRequest);
}

QString CPveControl::DspCmdCtReinstall(PRL_CONST_STR sVmUuid,
		PRL_CONST_STR sOs, PRL_UINT32 nFlags)
{
	CProtoCommandPtr pRequest =
		CProtoSerializer::CreateProtoCommandWithTwoStrParams(
				PVE::DspCmdCtReinstall,
				UTF8_2QSTR(sVmUuid),
				UTF8_2QSTR(sOs),
				false,
				nFlags);

	return SendRequestToServer(pRequest);
}
