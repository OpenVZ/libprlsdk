/*
 * ExecChannel.cpp
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


#include "Libraries/Logging/Logging.h"
#include "Libraries/IOService/src/IOCommunication/IOProtocol.h"
#include "SDK/Include/PrlIOStructs.h"

#include "ExecChannel.h"

ExecChannel::ExecChannel( const QString& sessionId,
                   const QString& vmId,
                   bool /*isLocal*/,
                   const QString& host,
                   quint32 port,
                   PRL_SECURITY_LEVEL securityLevel,
                   bool useUnixSockets ) :
	Channel( host, port, securityLevel, useUnixSockets,
			IOSender::IOCtClient),
	m_sessionId(sessionId),
	m_vmId(vmId),
	m_authenticated(false)
{
	init();
}

void ExecChannel::init()
{
	// Catch state change
	QObject::connect( &getIOClient(),
			SIGNAL(onStateChanged(IOSender::State)),
			SLOT(innerHandleStateChange(IOSender::State)),
			Qt::DirectConnection );

	// Catch packages from server
	QObject::connect( &getIOClient(),
			SIGNAL(onPackageReceived(const SmartPtr<IOPackage>)),
			SLOT(innerHandlePackage(const SmartPtr<IOPackage>)),
			Qt::DirectConnection );
}

ExecChannel::~ExecChannel()
{
}

void ExecChannel::innerHandlePackage( const SmartPtr<IOPackage> p )
{
	if ( ! isValid() ) {
		LOG_MESSAGE(DBG_FATAL, "IOChannel is invalid!");
		return;
	}

	// Check for auth package first
	if ( ! m_authenticated &&
			p->header.type != PET_IO_AUTH_RESPONSE ) {

		LOG_MESSAGE(DBG_DEBUG, "Package has been received, but it is not an "
				"auth response. Waiting for auth response package ...");
	}
	// Great, auth package has been received
	else if ( ! m_authenticated &&
			p->header.type == PET_IO_AUTH_RESPONSE ) {

		const PRL_IO_AUTH_RESPONSE* authResp =
			reinterpret_cast<const PRL_IO_AUTH_RESPONSE*>(p->buffers[0].getImpl());

		if ( p->data[0].bufferSize < sizeof(*authResp) ) {
			LOG_MESSAGE(DBG_FATAL, "Wrong auth response package!");
			m_authenticated = false;
		} else {
			m_authenticated = !!authResp->authResult;
		}


		// Our authentication has been rejected
		// Stop everything
		if ( ! m_authenticated ) {
			setValid( false );
			setState( Channel::AuthenticationFailed );
			getIOClient().disconnectClient();
		} else {
			setState(Channel::Started);
		}

		// Reject package for outer listeners
	}
	if ( p ) {
		emit onPackageReceived( p );
	}
}

void ExecChannel::innerHandleStateChange( IOSender::State st )
{
	if ( st == IOSender::Connected ) {
		setValid( true );
		// We are connected now. Send auth request.
		sendAuthRequest();
	}
	else if ( st == IOSender::Disconnected ) {
		setValid( false );
		setState( Channel::Stopped );

		m_authenticated = false;
	}
	else {
		WRITE_TRACE(DBG_WARNING, "Unknown IO client state: %d", st);
	}
}

IOSendJob::Handle ExecChannel::sendPackage ( IOPackage::Type pkgType,
		const void* data, quint32 size )
{
	LOG_MESSAGE(DBG_DEBUG, "Package : type=%d, dataSize=%d",
			pkgType, size);

	if ( ! isValid() ) {
		LOG_MESSAGE(DBG_WARNING, "Desktop is invalid!");
		return IOSendJob::InvalidHandle;
	}

	SmartPtr<IOPackage> package =
		IOPackage::createInstance( pkgType, IOPackage::RawEncoding,
				data, size );
	return getIOClient().sendPackage( package );
}

IOSendJob::Handle ExecChannel::sendPackage ( const SmartPtr<IOPackage> package)
{
	return getIOClient().sendPackage( package );
}

bool ExecChannel::sendAuthRequest ()
{
	PRL_IO_AUTH_EXEC_REQUEST authRequest;
	Uuid( m_sessionId ).dump( authRequest.sessionUuid );

	// Create auth package
	IOSendJob::Handle job =
		ExecChannel::sendPackage( PET_IO_CLI_AUTHENTICATE_EXEC_SESSION,
				&authRequest, sizeof(authRequest) );
	IOSendJob::Result res = getIOClient().getSendResult( job );
	return res == IOSendJob::SendPended || res == IOSendJob::Success;
}
