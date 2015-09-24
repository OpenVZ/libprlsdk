/*
 * IOClient.cpp
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


#include <QMetaType>

#include "Socket/SocketClient_p.h"
#include "IOClient.h"

using namespace IOService;

/*****************************************************************************/

IOClient::IOClient ( const IORoutingTable& routingTable,
                     IOSender::Type type,
					 IOCommunication::SocketHandle h,
					 const IOCredentials& credentials) :
    IOConnection( routingTable, type, IOSender::DirectConnectionMode, "", 0 ),
    m_sockImpl( new SocketClientPrivate(
                                        IOConnection::getJobManager(),
                                        IOConnection::getRoutingTable(),
                                        IOConnection::senderType(),
                                        IOSender::DirectConnectionMode,
                                        IOConnection::remoteHostName(),
                                        IOConnection::remotePortNumber(),
                                        IOSender::InvalidHandle, //proxy handle

                                        Cli_ClientContext,
                                        this, this,
                                        h->takeSocketHandle(),
                                        Uuid(),
                                        IOCommunication::DetachedClient(),
										credentials,
                                        false ) )
{
    init();
}

IOClient::IOClient ( const IORoutingTable& routingTable,
                     IOSender::Type type,
                     const QString& host,
                     quint32 port,
					 bool useUnixSockets,
					 const IOCredentials& credentials) :
    IOConnection( routingTable, type, IOSender::DirectConnectionMode,
                  host, port ),
    m_sockImpl( new SocketClientPrivate(
                                        IOConnection::getJobManager(),
                                        IOConnection::getRoutingTable(),
                                        IOConnection::senderType(),
                                        IOSender::DirectConnectionMode,
                                        IOConnection::remoteHostName(),
                                        IOConnection::remotePortNumber(),
                                        IOSender::InvalidHandle, //proxy handle

                                        Cli_ClientContext,
                                        this, this, -1,
                                        Uuid(),
                                        IOCommunication::DetachedClient(),
										credentials,
                                        useUnixSockets ) )
{
    init();
}

void IOClient::init ()
{
    qRegisterMetaType< SmartPtr<IOPackage> >( "const SmartPtr<IOPackage>" );
    qRegisterMetaType< SmartPtr<IOPackage> >( "SmartPtr<IOPackage>" );
    qRegisterMetaType< IOSendJob::Handle >( "const IOSendJob::Handle" );
    qRegisterMetaType< IOSendJob::Handle >( "IOSendJob::Handle" );
    qRegisterMetaType< IOSendJob::Result >( "const IOSendJob::Result" );
    qRegisterMetaType< IOSendJob::Result >( "IOSendJob::Result" );
    qRegisterMetaType< IOSender::State >( "IOSender::State" );
    qRegisterMetaType< IOSender::State >( "const IOSender::State" );
    qRegisterMetaType< IOSender::Handle >( "IOSender::Handle" );
    qRegisterMetaType< IOSender::Handle >( "const IOSender::Handle" );
    qRegisterMetaType< IOCommunication::DetachedClient >( "const IOCommunication::DetachedClient" );
    qRegisterMetaType< IOCommunication::DetachedClient >( "IOCommunication::DetachedClient" );
}

IOClient::~IOClient ()
{
    delete m_sockImpl;
}

IOSender::State IOClient::state () const
{
    return m_sockImpl->state();
}

IOSender::Error IOClient::error () const
{
    return m_sockImpl->error();
}

IOSender::Handle IOClient::senderHandle () const
{
    return m_sockImpl->currentConnectionUuid();
}

bool IOClient::connectClient ( quint32 msecsTimeout )
{
    return m_sockImpl->startClient( msecsTimeout );
}

IOSender::State IOClient::waitForConnection ( quint32 msecs ) const
{
    return m_sockImpl->waitForConnection( msecs );
}

void IOClient::disconnectClient ()
{
    m_sockImpl->stopClient();
}

bool IOClient::serverProtocolVersion (
    IOCommunication::ProtocolVersion& ver ) const
{
    return m_sockImpl->peerProtocolVersion( ver );
}

IOSender::SecurityMode IOClient::securityMode() const
{
    return m_sockImpl->securityMode();
}

IOSendJob::Handle IOClient::sendPackage ( const SmartPtr<IOPackage>& p )
{
    return m_sockImpl->sendPackage( p );
}

IOSendJob::Handle IOClient::sendDetachedClient (
    const IOCommunication::DetachedClient& detachedClient,
    const SmartPtr<IOPackage>& request )
{
    return m_sockImpl->sendDetachedClient( detachedClient, request );
}

/*****************************************************************************
 * Callbacks
 *****************************************************************************/

void IOClient::onBeforeSend ( SocketClientPrivate*,
                              const IOSender::Handle&,
                              const SmartPtr<IOPackage>& p )
{
    emit IOClientInterface::onBeforeSend( this, p );
}

void IOClient::onAfterSend ( SocketClientPrivate*,
                             const IOSender::Handle&,
                             IOSendJob::Result r,
                             const SmartPtr<IOPackage>& p )
{
    emit IOClientInterface::onAfterSend( this, r, p );
}

void IOClient::onDetachedClientReceived (
    SocketClientPrivate*,
    const IOSender::Handle& ,
    const SmartPtr<IOPackage>& p,
    const IOCommunication::DetachedClient& c )
{
    emit IOClientInterface::onDetachedClientReceived( p, c );
    emit IOClientInterface::onDetachedClientReceived( this, p, c );
}

void IOClient::onPackageReceived ( SocketClientPrivate*,
                                   const IOSender::Handle&,
                                   const SmartPtr<IOPackage>& p )
{
    emit IOClientInterface::onPackageReceived( p );
    emit IOClientInterface::onPackageReceived( this, p );
}

void IOClient::onResponsePackageReceived ( SocketClientPrivate*,
                                           const IOSender::Handle&,
                                           const IOSendJob::Handle& j,
                                           const SmartPtr<IOPackage>& p )
{
    emit IOClientInterface::onResponsePackageReceived( j, p );
    emit IOClientInterface::onResponsePackageReceived( this, j, p );
}

void IOClient::onStateChanged ( SocketClientPrivate*,
                                const IOSender::Handle&,
                                IOSender::State,
                                IOSender::State newState )
{
    emit IOClientInterface::onStateChanged( newState );
}

void IOClient::srv_onDetachClient ( SocketClientPrivate*,
                                    const IOSender::Handle& h,
                                    const IOCommunication::DetachedClient& s )
{
    emit onDetachClient( h, s );
    emit onDetachClient( this, h, s );
}

/*****************************************************************************/

void IOClient::setLimitErrorLogging(bool bLimitErrorLogging)
{
	if (m_sockImpl)
		m_sockImpl->setLimitErrorLogging(bLimitErrorLogging);
}
