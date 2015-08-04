/*
 * IOServer.cpp
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

#include "Socket/SocketServer_p.h"
#include "IOServer.h"

#include "Libraries/Logging/Logging.h"

using namespace IOService;

/*****************************************************************************/

IOServer::IOServer ( const IORoutingTable& routingTable,
					 IOSender::Type type,
					 const IOCredentials& credentials) :
    IOConnection( routingTable, type, IOSender::DirectConnectionMode, "", 0 ),
	m_sockImpl( new SocketServerPrivate(this, Srv_AttachingContext, false, credentials) )
{
    init();
}

IOServer::IOServer ( const IORoutingTable& routingTable,
                     IOSender::Type type,
                     const QString& host,
                     quint32 port,
					 bool useUnixSockets,
					 const IOCredentials& credentials) :
    IOConnection( routingTable, type, IOSender::DirectConnectionMode,
                  host, port ),
	m_sockImpl( new SocketServerPrivate(this, Srv_ListeningContext, useUnixSockets, credentials) )
{
    init();
}

void IOServer::init ()
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

IOServer::~IOServer ()
{
    delete m_sockImpl;
}

IOSender::State IOServer::state () const
{
    return m_sockImpl->state();
}

IOSender::Handle IOServer::senderHandle () const
{
    return m_sockImpl->currentConnectionUuid();
}

IOSender::State IOServer::listen ()
{
    return m_sockImpl->startServer();
}

bool IOServer::serverBindingInfo ( IPvBindingInfo& info) const
{
    return m_sockImpl->serverBindingInfo(info);
}

void IOServer::disconnectServer ()
{
    m_sockImpl->stopServer();
}

quint32 IOServer::countClients () const
{
    return m_sockImpl->countClients();
}

QList<IOSender::Handle> IOServer::getClientsHandles () const
{
    return m_sockImpl->getClientsHandles();
}

IOSender::Type IOServer::clientSenderType ( const IOSender::Handle& h ) const
{
    return m_sockImpl->clientSenderType( h );
}

IOSender::SecurityMode IOServer::clientSecurityMode(const IOSender::Handle& h) const
{
    return m_sockImpl->clientSecurityMode( h );
}

IOSender::State IOServer::clientState ( const IOSender::Handle& h ) const
{
    return m_sockImpl->clientState( h );
}

QString IOServer::clientHostName ( const IOSender::Handle& h ) const
{
    return m_sockImpl->clientHostName( h );
}

bool IOServer::clientProtocolVersion (
    const IOSender::Handle& h,
    IOCommunication::ProtocolVersion& ver ) const
{
    return m_sockImpl->clientProtocolVersion( h, ver );
}

bool IOServer::detachClient (
    const IOSender::Handle& cliHandle,
    int specificArg,
    const SmartPtr<IOPackage>& additionalPkg,
    bool detachBothSides )
{
    return m_sockImpl->detachClient(cliHandle, specificArg, additionalPkg, detachBothSides);
}

bool IOServer::attachClient ( IOCommunication::DetachedClient cli )
{
    return m_sockImpl->attachClient( cli );
}

IOCommunication::SocketHandle IOServer::createDetachedClientSocket ()
{
    return m_sockImpl->createDetachedClientSocket();
}

void IOServer::setCredentials(const IOCredentials& credentials)
{
	return m_sockImpl->setCredentials(credentials);
}

bool IOServer::disconnectClient ( const IOSender::Handle& h )
{
    return m_sockImpl->stopClient( h );
}

IOSendJob::Handle IOServer::sendPackage ( const IOSender::Handle& h,
                                          const SmartPtr<IOPackage>& p )
{
    return m_sockImpl->sendPackage( h, p );
}

IOSendJob::Handle IOServer::sendDetachedClient (
    const IOSender::Handle& h,
    const IOCommunication::DetachedClient& detachedClient,
    const SmartPtr<IOPackage>& request )
{
    return m_sockImpl->sendDetachedClient( h, detachedClient, request );
}

/*****************************************************************************
 * Callbacks
 *****************************************************************************/

void IOServer::onBeforeSend ( SocketClientPrivate*,
                              const IOSender::Handle& h,
                              const SmartPtr<IOPackage>& p )
{
    emit IOServerInterface::onBeforeSend( this, h, p );
}

void IOServer::onAfterSend ( SocketClientPrivate*,
                             const IOSender::Handle& h,
                             IOSendJob::Result r,
                             const SmartPtr<IOPackage>& p )
{
    emit IOServerInterface::onAfterSend( this, h, r, p );
}

void IOServer::onDetachedClientReceived (
    SocketClientPrivate*,
    const IOSender::Handle& h,
    const SmartPtr<IOPackage>& p,
    const IOCommunication::DetachedClient& s )
{
    emit IOServerInterface::onDetachedClientReceived( h, p, s );
    emit IOServerInterface::onDetachedClientReceived( this, h, p, s );
}

void IOServer::onPackageReceived ( SocketClientPrivate*,
                                   const IOSender::Handle& h,
                                   const SmartPtr<IOPackage>& p )
{
    emit IOServerInterface::onPackageReceived( h, p );
    emit IOServerInterface::onPackageReceived( this, h, p );
}

void IOServer::onResponsePackageReceived ( SocketClientPrivate*,
                                           const IOSender::Handle& h,
                                           const IOSendJob::Handle& j,
                                           const SmartPtr<IOPackage>& p )
{
    emit IOServerInterface::onResponsePackageReceived( h, j, p );
    emit IOServerInterface::onResponsePackageReceived( this, h, j, p );
}

void IOServer::srv_onDetachClient ( SocketClientPrivate*,
                                    const IOSender::Handle& h,
                                    const IOCommunication::DetachedClient& s )
{
    emit IOServerInterface::onDetachClient( h, s );
    emit IOServerInterface::onDetachClient( this, h, s );
}

/*****************************************************************************/
