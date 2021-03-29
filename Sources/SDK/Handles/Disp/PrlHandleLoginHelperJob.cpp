/*
 * PrlHandleLoginHelperJob.cpp
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


#include "PrlHandleLoginHelperJob.h"
#include <prlcommon/Interfaces/VirtuozzoQt.h>
#include <prlcommon/ProtoSerializer/CProtoSerializer.h>
#include <prlcommon/PrlCommonUtilsBase/CRsaHelper.hpp>
#include <prlcommon/PrlCommonUtilsBase/CFileHelper.h>
#include "SDK/Wrappers/SdkWrap/SdkHandleWrap.h"

using namespace Virtuozzo;
enum
{
	CLIENT =
		PCF_ORIGINAL_CLIENT
};

PrlHandleLoginHelperJob::PrlHandleLoginHelperJob(
	PrlHandleServerPtr pServer,
	PRL_CONST_STR user,
	PRL_CONST_STR passwd,
	PRL_CONST_STR sPrevSessionUuid,
	PRL_UINT32 flags
)
:
PrlHandleLocalJob( Uuid::createUuid().toString(), PJOC_SRV_LOGIN ),
m_pServer(pServer), m_sUser(UTF8_2QSTR(user)), m_sPassword(UTF8_2QSTR(passwd)),
m_sPrevSessionUuid(UTF8_2QSTR(sPrevSessionUuid)), m_bLoginRequestWasSent(false),
m_flags(flags | CLIENT)
{
	m_pServer->AddJobToResponseAwaitingList(PrlHandleServerJobPtr(this));
}

PRL_HANDLE PrlHandleLoginHelperJob::Cancel()
{
	IOClient *pClient = m_pServer->GetPveControl()->GetIoClient();
	PRL_HANDLE hJob = PRL_INVALID_HANDLE;
	if ( pClient )
	{
		QMutexLocker _lock( &m_mutex );
		if ( IOSender::Connected == pClient->state() && m_bLoginRequestWasSent )
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

QString PrlHandleLoginHelperJob::EncodePassword(const QString &sPassword)
{
    IOCommunication::ProtocolVersion ver;
	IOClient *pClient = m_pServer->GetPveControl()->GetIoClient();

    if (pClient && pClient->serverProtocolVersion(ver) && IOPROTOCOL_NEW_LOGIN_SUPPORT(ver))
	{
		QByteArray b;
		b.append(sPassword.leftJustified(MAX_PASSWORD_LENGTH, QChar::Null));
		return b.toBase64();
	}
	return sPassword;
}

void PrlHandleLoginHelperJob::doJob()
{
	if ( ! wasCanceled() )
	{
		QMutexLocker _lock( &m_mutex );
		CProtoCommandPtr pRequest = CProtoSerializer::CreateDspCmdUserLoginCommand(
												m_sUser,
												EncodePassword(m_sPassword),
												m_sPrevSessionUuid,
												m_flags);
		SmartPtr<IOPackage> pPackage = DispatcherPackage::createInstance(pRequest->GetCommandId(),
																			pRequest->GetCommand()->toString());
		// Replace package UUID with already created one before
		Uuid _uuid( GetJobUuid() );
		_uuid.dump( pPackage->header.uuid );

		m_pServer->GetPveControl()->SendRequestToServer( pPackage );
		m_bLoginRequestWasSent = true;
	}
}

void PrlHandleLoginHelperJob::processPublicKeyAuth(CProtoCommandDspWsResponse* pResponseCommand)
{
	if (!pResponseCommand->IsValid() ||
		pResponseCommand->GetRetCode() != PRL_ERR_SUCCESS ||
		pResponseCommand->GetStandardParamsCount() != 1 ||
		wasCanceled())
		return;

	QString encrypted_session = pResponseCommand->GetStandardParam(0);

	auto session = CRsaHelper().Decrypt(encrypted_session);
	if (session.isFailed())
		return;

	m_flags &= ~PLLF_LOGIN_WITH_RSA_KEYS;
	auto pAuthorizeCmd = CProtoSerializer::CreateDspCmdUserLoginCommand(
		m_sUser,
		QString(),
		session.value(),
		m_flags
	);

	auto pPackage =
		DispatcherPackage::createInstance(pAuthorizeCmd->GetCommandId(),
										  pAuthorizeCmd->GetCommand()->toString());

	// Replace package UUID with already created one before
	Uuid _uuid(GetJobUuid());
	_uuid.dump(pPackage->header.uuid);

	m_pServer->GetPveControl()->SendRequestToServer(pPackage);
}
