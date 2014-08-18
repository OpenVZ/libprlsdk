/*
 * IOServerPool.cpp
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


#include <QList>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>
#include <Libraries/Logging/Logging.h>
#include "IOServerPool.h"

using namespace IOService;

class IOServerPool::Imp
{
public:
    typedef QList<SmartPtr<IOServerInterface> > ServerList;
    void addServer(const SmartPtr<IOServerInterface> &server);
    void removeServer(const SmartPtr<IOServerInterface> &server);
    void addClient(const IOServerInterface *const server,
                   const IOSender::Handle &h);
    void removeClient(const IOSender::Handle &h);
    SmartPtr<IOServerInterface> getServer(const IOSender::Handle &h) const;
    ServerList getServerList() const;
private:
    typedef QHash<IOSender::Handle, SmartPtr<IOServerInterface> > ClientMap;
    mutable QMutex m_mutex;
    ServerList m_servers;
    ClientMap m_clients;
};

void IOServerPool::Imp::addServer(const SmartPtr<IOServerInterface> &server)
{
    const QMutexLocker locker(&m_mutex);
    Q_ASSERT(!m_servers.contains(server));
    m_servers.append(server);
}

void IOServerPool::Imp::removeServer(const SmartPtr<IOServerInterface> &server)
{
    const QMutexLocker locker(&m_mutex);
    Q_ASSERT(m_servers.contains(server));
    Q_ASSERT(!m_clients.values().contains(server));
    m_servers.removeOne(server);
}

void IOServerPool::Imp::addClient(const IOServerInterface *const server,
                                  const IOSender::Handle &h)
{
    const QMutexLocker locker(&m_mutex);
    Q_ASSERT(!m_clients.contains(h));
    for (int i = m_servers.size(); 0 < i--;)
    {
        const SmartPtr<IOServerInterface> &serverPtr = m_servers[i];
        if (serverPtr.get() == server)
        {
            m_clients.insert(h, serverPtr);
            return;
        }
    }
    Q_ASSERT(0);
}

void IOServerPool::Imp::removeClient(const IOSender::Handle &h)
{
    const QMutexLocker locker(&m_mutex);
    Q_ASSERT(m_clients.contains(h));
    m_clients.remove(h);
}

SmartPtr<IOServerInterface> IOServerPool::Imp::getServer(const IOSender::Handle &h) const
{
    const QMutexLocker locker(&m_mutex);
    const ClientMap::const_iterator i = m_clients.find(h);
    SmartPtr<IOServerInterface> server;
    if (m_clients.end() != i)
    {
        server = i.value();
    }
    return server;
}

IOServerPool::Imp::ServerList IOServerPool::Imp::getServerList() const
{
    const QMutexLocker locker(&m_mutex);
    return m_servers;
}

IOServerPool::IOServerPool () : m_imp(new Imp)
{}

IOServerPool::~IOServerPool ()
{
    const IOServerPool::Imp::ServerList servers = m_imp->getServerList();
    Q_ASSERT(servers.empty());
    for (int i = servers.size(); 0 < i--;)
    {
        servers[i]->disconnect(this);
    }
}

bool IOServerPool::addServer ( SmartPtr<IOServerInterface> server )
{
    Q_ASSERT(server);
    Q_ASSERT(server->state() == IOSender::Disconnected);
    if (server->state() != IOSender::Disconnected)
        return false;

    bool res = false;
    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onClientConnected(IOSender::Handle)),
                           SIGNAL(onClientConnected(IOSender::Handle)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onClientAttached(IOSender::Handle,
                                                   const SmartPtr<IOPackage>)),
                           SIGNAL(onClientAttached(IOSender::Handle,
                                                   const SmartPtr<IOPackage>)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onClientDisconnected(IOSender::Handle)),
                           SIGNAL(onClientDisconnected(IOSender::Handle)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onPackageReceived(IOSender::Handle,
                                                    const SmartPtr<IOPackage>)),
                           SIGNAL(onPackageReceived(IOSender::Handle,
                                                    const SmartPtr<IOPackage>)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onPackageReceived(IOServerInterface*, IOSender::Handle,
                                                    const SmartPtr<IOPackage>)),
                           SIGNAL(onPackageReceived(IOServerInterface*, IOSender::Handle,
                                                    const SmartPtr<IOPackage>)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onResponsePackageReceived(IOSender::Handle,
                                                            IOSendJob::Handle,
                                                            const SmartPtr<IOPackage>)),
                           SIGNAL(onResponsePackageReceived(IOSender::Handle,
                                                            IOSendJob::Handle,
                                                            const SmartPtr<IOPackage>)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onResponsePackageReceived(IOServerInterface*,
                                                            IOSender::Handle,
                                                            IOSendJob::Handle,
                                                            const SmartPtr<IOPackage>)),
                           SIGNAL(onResponsePackageReceived(IOServerInterface*,
                                                            IOSender::Handle,
                                                            IOSendJob::Handle,
                                                            const SmartPtr<IOPackage>)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onDetachedClientReceived(IOSender::Handle,
                                                           const SmartPtr<IOPackage>,
                                                           const IOCommunication::DetachedClient)),
                           SIGNAL(onDetachedClientReceived(IOSender::Handle,
                                                           const SmartPtr<IOPackage>,
                                                           const IOCommunication::DetachedClient)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onDetachedClientReceived(IOServerInterface*,
                                                           IOSender::Handle,
                                                           const SmartPtr<IOPackage>,
                                                           const IOCommunication::DetachedClient)),
                           SIGNAL(onDetachedClientReceived(IOServerInterface*,
                                                           IOSender::Handle,
                                                           const SmartPtr<IOPackage>,
                                                           const IOCommunication::DetachedClient)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onDetachClient(IOSender::Handle,
                                                 const IOCommunication::DetachedClient)),
                           SIGNAL(onDetachClient(IOSender::Handle,
                                                 const IOCommunication::DetachedClient)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onDetachClient(IOServerInterface*,
                                                 IOSender::Handle,
                                                 const IOCommunication::DetachedClient)),
                           SIGNAL(onDetachClient(IOServerInterface*,
                                                 IOSender::Handle,
                                                 const IOCommunication::DetachedClient)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onClientStateChanged(IOServerInterface*,
                                                       IOSender::Handle,
                                                       IOSender::State)),
                           SLOT(slot_onClientStateChanged(IOServerInterface*,
                                                          IOSender::Handle,
                                                          IOSender::State)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onBeforeSend(IOServerInterface*,
                                               IOSender::Handle,
                                               const SmartPtr<IOPackage>)),
                           SIGNAL(onBeforeSend(IOServerInterface*,
                                               IOSender::Handle,
                                               const SmartPtr<IOPackage>)),
                           Qt::DirectConnection);
    Q_ASSERT(res);

    //
    res = QObject::connect(server.getImpl(),
                           SIGNAL(onAfterSend(IOServerInterface*,
                                              IOSender::Handle,
                                              IOSendJob::Result,
                                              const SmartPtr<IOPackage>)),
                           SIGNAL(onAfterSend(IOServerInterface*,
                                              IOSender::Handle,
                                              IOSendJob::Result,
                                              const SmartPtr<IOPackage>)),
                           Qt::DirectConnection);
    Q_ASSERT(res);
	(void)res;

    m_imp->addServer(server);
    return true;
}

bool IOServerPool::removeServer ( SmartPtr<IOServerInterface> server )
{
    Q_ASSERT( server->state() == IOSender::Disconnected );
    if ( server->state() != IOSender::Disconnected )
        return false;
    server->disconnect(this);
    m_imp->removeServer(server);
    return true;
}

QList< SmartPtr<IOServerInterface> > IOServerPool::getServers () const
{
	return m_imp->getServerList();
}

SmartPtr<IOServerInterface> IOServerPool::getServerByClientHandle ( const IOSender::Handle& h ) const
{
    return m_imp->getServer(h);
}

IOSender::Type IOServerPool::clientSenderType (
    const IOSender::Handle& h ) const
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return IOSender::UnknownType;
    }
    return server->clientSenderType(h);
}

IOSender::SecurityMode IOServerPool::clientSecurityMode(
        const IOSender::Handle& h) const
{
	const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return IOSender::UnknownMode;
    }
    return server->clientSecurityMode(h);
}

IOSender::State IOServerPool::clientState ( const IOSender::Handle& h ) const
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return IOSender::Disconnected;
    }
    return server->clientState(h);
}

QString IOServerPool::clientHostName ( const IOSender::Handle& h ) const
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return QString();
    }
    return server->clientHostName(h);
}

bool IOServerPool::clientProtocolVersion (
    const IOSender::Handle& h,
    IOCommunication::ProtocolVersion& p ) const
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return false;
    }
    return server->clientProtocolVersion(h, p);
}

bool IOServerPool::detachClient ( const IOSender::Handle& h,
                                  int specificArg,
                                  const SmartPtr<IOPackage>& additionalPkg )
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return false;
    }
    return server->detachClient(h, specificArg, additionalPkg);
}

bool IOServerPool::disconnectClient ( const IOSender::Handle& h )
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return false;
    }
    return server->disconnectClient(h);
}

IOSendJob::Handle IOServerPool::sendPackage ( const IOSender::Handle& h,
                                              const SmartPtr<IOPackage>& p )
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return IOSendJob::Handle();
    }
    return server->sendPackage(h, p);
}

IOSendJob::Handle IOServerPool::sendDetachedClient (
                                  const IOSender::Handle& h,
                                  const IOCommunication::DetachedClient& d,
                                  const SmartPtr<IOPackage>& p )
{
    const SmartPtr<IOServerInterface> server = m_imp->getServer(h);
    if (0 == server.get())
    {
        return IOSendJob::Handle();
    }
    return server->sendDetachedClient(h, d, p);
}

void IOServerPool::slot_onClientStateChanged ( IOServerInterface* srv,
                                               IOSender::Handle cliHandle,
                                               IOSender::State state )
{
    // Connect state
    if ( state == IOSender::Connected ) {
        // Add client to internal hash
        m_imp->addClient( srv, cliHandle );
        // Forward signals
        emit onClientStateChanged( cliHandle, state );
        emit onClientStateChanged( srv, cliHandle, state );
    }
    // Disconnect state
    else {
        // Forward signals
        emit onClientStateChanged( cliHandle, state );
        emit onClientStateChanged( srv, cliHandle, state );
        // Remove client from internal hash
        m_imp->removeClient( cliHandle );
    }
}
