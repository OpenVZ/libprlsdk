/*
 * PrlHandleLoginLocalHelperJob.cpp
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


#include "PrlHandleLoginLocalHelperJob.h"
#include <prlcommon/Std/PrlAssert.h>

#ifdef _WIN_
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace Parallels;

#define DELAYED_CONNECTION_TIMEOUT_MSECS	50

PrlHandleLoginLocalHelperJob::PrlHandleLoginLocalHelperJob(
		PrlHandleServerPtr pServer,
		qlonglong nUserId,
		const char *strPrevSessionUuid,
		PRL_APPLICATION_MODE appMode,
		quint32 flags)
: PrlHandleLocalJob( Uuid::createUuid().toString(), PJOC_SRV_LOGIN_LOCAL )
, m_pServer(pServer)
, m_nUserId(nUserId)
, m_qsPrevSessionUuid(UTF8_2QSTR(strPrevSessionUuid))
, m_nAppMode(appMode)
, m_nFlags(flags)
, m_iLoginStage(LegacyLogin)
{
	m_pServer->AddJobToResponseAwaitingList(PrlHandleServerJobPtr(this));

	if (isDelayedConnectionMode())
	{
		bool bConnect = connect(this, SIGNAL(doSingleShot()), SLOT(onSingleShot()));
		PRL_ASSERT(bConnect);
	}
}

PRL_HANDLE PrlHandleLoginLocalHelperJob::Cancel()
{
	IOClient *pClient = m_pServer->GetPveControl()->GetIoClient();
	PRL_HANDLE hJob = PRL_INVALID_HANDLE;
	if ( pClient )
	{
		QMutexLocker _lock( &m_mutex );
		if ( IOSender::Connected == pClient->state() && m_iLoginStage )
			return m_pServer->DspCmdUserCancelOperation(QSTR2UTF8(GetJobUuid()))->GetHandle();
		else
		{
			hJob = PrlHandleLocalJob::Cancel();
			pClient->disconnectClient();
		}
	}
	PrlHandleBasePtr pThisJob = PRL_OBJECT_BY_HANDLE<PrlHandleBase>( GetHandle() );
	if ( pThisJob )
		m_pServer->Notify( pThisJob );
	return hJob;
}

void PrlHandleLoginLocalHelperJob::doJob()
{
	if ( wasCanceled() )
		return;

	QMutexLocker _lock( &m_mutex );

	IOClient *pClient = m_pServer->GetPveControl()->GetIoClient();
	if (pClient && pClient->state() != IOSender::Connected
		&& isDelayedConnectionMode())
	{
		if ( pClient )
			pClient->setLimitErrorLogging(true);

		emit doSingleShot();
		return;
	}

	if ( pClient )
		pClient->setLimitErrorLogging(false);

	CProtoCommandPtr pRequest;
	switch(m_iLoginStage)
	{
	case NoLogin:
		pRequest = CProtoSerializer::CreateDspCmdUserLoginLocalCommand(
				m_nUserId,
				m_nAppMode,
#ifdef _WIN_
				(quint64 )GetCurrentProcessId(),
#else
				getpid(),
#endif
				m_nFlags);
		break;
	case LegacyLogin:
		pRequest = CProtoSerializer::CreateDspCmdUserEasyLoginLocalCommand(
				m_nAppMode,
				m_qsPrevSessionUuid,
				m_nFlags);
		break;
	case LoginStage1:
		pRequest = CProtoSerializer::CreateProtoCommandWithOneStrParam(
				PVE::DspCmdUserLoginLocalStage2, m_qsPrevSessionUuid);
		break;
	default:
		PRL_ASSERT(0);
		return;
	}

	SmartPtr<IOPackage> pPackage
		= DispatcherPackage::createInstance(
			pRequest->GetCommandId(), pRequest->GetCommand()->toString());

	//Replace package UUID with already created one before
	Uuid _uuid( GetJobUuid() );
	_uuid.dump( pPackage->header.uuid );

	m_pServer->GetPveControl()->SendRequestToServer( pPackage );
	m_iLoginStage = (_LoginStage )(m_iLoginStage + 1);
}

bool PrlHandleLoginLocalHelperJob::prepareDataForValidation(const QString& sFilePath,
															const QString& sCheckData,
															CResult* pResult)
{
	QFile _file(sFilePath);
	if ( ! _file.open(QIODevice::WriteOnly) )
	{
		WRITE_TRACE(DBG_FATAL, "Could't to open checking legacy file '%s'", sFilePath.toUtf8().data());
		pResult->setReturnCode(PRL_ERR_NOT_ENOUGH_PERMS_TO_OPEN_AUTHORIZATION_FILE);
		return false;
	}

	QTextStream _stream(&_file);
	_stream << sCheckData;
	_stream.flush();
	if (_file.size() != sCheckData.size())
	{
		// Couldn't to write check legacy data
		pResult->setReturnCode(PRL_ERR_OUT_OF_DISK_SPACE);
		return false;
	}
	_file.close();

	doJob();

	return true;
}

void PrlHandleLoginLocalHelperJob::switchToCompatibilityMode()
{
	m_iLoginStage = NoLogin;
	doJob();
}

bool PrlHandleLoginLocalHelperJob::isDelayedConnectionMode() const
{
	return (m_nFlags & PLLF_LOGIN_WITH_DELAYED_CONNECTION);
}

void PrlHandleLoginLocalHelperJob::onSingleShot()
{
	QTimer::singleShot(
		DELAYED_CONNECTION_TIMEOUT_MSECS,
		this, SLOT(onTryConnect()));
}

void PrlHandleLoginLocalHelperJob::onTryConnect()
{
	IOClient *pClient = m_pServer->GetPveControl()->GetIoClient();
	if (pClient)
		pClient->connectClient(PVE_CONNECTION_TIMEOUT);
}
