/*
 * SocketServer_p.cpp
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

#include <QCoreApplication>
#include <Libraries/Std/Etrace.h>
#include "Socket/SocketServer_p.h"
#include "IOServer.h"

using namespace IOService;

SocketServerPrivate::SocketServerPrivate ( IOServer* imp,
                                           SocketServerContext ctx,
										   bool useUnixSockets,
										   const IOCredentials& credentials) :
    m_impl(imp),
    m_ctx(ctx),
    m_state(IOSender::Disconnected),
    m_threadState(ThreadIsStopped),
#ifdef _WIN_
    m_connectEventHandle(WSA_INVALID_EVENT),
    m_cleanEventHandle(WSA_INVALID_EVENT)
#else
    m_maxFDNum(IOService::getMaxFDNumber()),
    m_rdSet(IOService::allocFDSet(m_maxFDNum))
#endif
	, m_localCredentials(credentials),
	m_useUnixSockets(useUnixSockets)
{
    INIT_IO_LOG(QString("IO server ctx [accept thr] (sender %1): ").
                arg(imp->senderType()));

#ifndef _WIN_
    // Ignore SIGPIPE signal
    struct sigaction act;
    sigemptyset( &act.sa_mask );
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigaction( SIGPIPE, &act, NULL );

    // Invalidate all pipes
    for ( uint i = 0; i < sizeof(m_eventPipes)/sizeof(m_eventPipes[0]); ++i )
        m_eventPipes[i] = -1;
#endif
}

SocketServerPrivate::~SocketServerPrivate ()
{
    stopServer();
}

IOSender::State SocketServerPrivate::state () const
{
    return m_state;
}

bool SocketServerPrivate::serverBindingInfo ( IPvBindingInfo& info ) const
{
    QMutexLocker locker( &m_eventMutex );

    if ( m_bindingInfo.isEmpty() )
        return false;

    info = m_bindingInfo;

    return true;
}

QString SocketServerPrivate::currentConnectionUuid () const
{
    QMutexLocker locker( &m_eventMutex );

    return m_currConnectionUuid;
}

quint32 SocketServerPrivate::countClients () const
{
    QMutexLocker locker( &m_eventMutex );

    return m_sockClients.size();
}

QList<IOSender::Handle> SocketServerPrivate::getClientsHandles () const
{
    QMutexLocker locker( &m_eventMutex );

    // Get uuids
    QList<IOSender::Handle> res;
    QHash<IOSender::Handle, SmartPtr<SocketClientPrivate> >::ConstIterator it =
        m_sockClients.begin();
    for ( ; it != m_sockClients.end(); ++it ) {
        QString sessUuid = it.value()->peerConnectionUuid();
        // Check that client has been stopped while
        // we are receiving peer connection uuid.
        // This is valid race, nothing terrible.
        if ( sessUuid.isEmpty() )
            continue;
        res.append( sessUuid );
    }

    return res;
}

IOSender::Type SocketServerPrivate::clientSenderType (
    const IOSender::Handle& h ) const
{
    QMutexLocker locker( &m_eventMutex );

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return IOSender::UnknownType;

    // Unlock
    locker.unlock();

    return client->peerSenderType();
}

IOSender::SecurityMode SocketServerPrivate::clientSecurityMode(
            const IOSender::Handle& h ) const
{
	QMutexLocker locker( &m_eventMutex );

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return IOSender::UnknownMode;

    // Unlock
    locker.unlock();

    return client->securityMode();
}

IOSender::State SocketServerPrivate::clientState (
    const IOSender::Handle& h ) const
{
    QMutexLocker locker( &m_eventMutex );

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return IOSender::Disconnected;

    // Unlock
    locker.unlock();

    return client->state();
}

QString SocketServerPrivate::clientHostName (
    const IOSender::Handle& h ) const
{
    QMutexLocker locker( &m_eventMutex );

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return QString();

    // Unlock
    locker.unlock();

    return client->peerHostName();
}

bool SocketServerPrivate::clientProtocolVersion (
    const IOSender::Handle& h,
    IOCommunication::ProtocolVersion& ver ) const
{
    QMutexLocker locker( &m_eventMutex );

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return false;

    // Unlock
    locker.unlock();

    return client->peerProtocolVersion( ver );
}

bool SocketServerPrivate::detachClient (
    const IOSender::Handle& h,
    int specificArg,
    const SmartPtr<IOPackage>& additionalPkg )
{
    QMutexLocker locker( &m_eventMutex );

    if ( QThread::currentThread() != this &&
         m_state != IOSender::Connected )
        return false;

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return false;

    // Unlock
    locker.unlock();

    return client->srv_detachClient( specificArg, additionalPkg );
}

bool SocketServerPrivate::attachClient (
    IOCommunication::DetachedClient detachedCli )
{
    if ( m_ctx != Srv_AttachingContext )
        return false;

    // Try to mark as attached
    bool res = detachedCli->attach();
    // Already attached?
    if ( ! res )
        return false;

    QMutexLocker locker( &m_eventMutex );

    if ( QThread::currentThread() != this &&
         m_state != IOSender::Connected )
        return false;

    m_attachQueue.enqueue( detachedCli );
    m_attachWait.wakeOne();

    return true;
}

void SocketServerPrivate::setCredentials (
        const IOCredentials& credentials )
{
	QMutexLocker locker( &m_eventMutex );

	m_localCredentials = credentials;
}

IOCommunication::SocketHandle
SocketServerPrivate::createDetachedClientSocket ()
{
    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ] = {0};

    int socks[2] = {-1, -1};
    bool res = IOService::socketpair( socks );
    if ( ! res ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't create socketpair (native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize));
        return IOCommunication::SocketHandle();
    }

    // Creates, appends and starts new client
    res = createAndStartNewSockClient( socks[0],
                                       IOCommunication::DetachedClient() );
    if ( ! res ) {
#ifdef _WIN_ // Windows
        ::closesocket(socks[0]);
        ::closesocket(socks[1]);
#else // Unix
        ::close(socks[0]);
        ::close(socks[1]);
#endif
        return IOCommunication::SocketHandle();
    }

    return IOCommunication::SocketHandle(
               new IOCommunication::SocketHandlePrivate(socks[1]) );
}

bool SocketServerPrivate::stopClient ( const IOSender::Handle& h )
{
    // Lock clients
    QMutexLocker locker( &m_eventMutex );

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return false;

    // Unlock
    locker.unlock();

    // Stop
    client->stopClient();

    return true;
}

IOSender::State SocketServerPrivate::startServer ()
{
        // Main start/stop lock
        QMutexLocker mainLocker( &m_startStopMutex );

        // Lock
        QMutexLocker locker( &m_eventMutex );

        // 'starting' state is invisible for us,
        // because other starting thread must lock start/stop mutex
        // and wait for correct result
        Q_ASSERT(m_threadState != ThreadIsStarting);

        // Already started, return success
        if ( m_threadState == ThreadIsStarted ) {
            Q_ASSERT(m_state == IOSender::Connected);
            return IOSender::Connected;
        }
        // Stopping in progress, wait for result
        else if ( m_threadState == ThreadIsStopping ) {
            m_threadStateWait.wait( &m_eventMutex );
            Q_ASSERT(m_threadState == ThreadIsStopped);

            // Wait for Qt thread
            QThread::wait();
        }
        // Already stopped, wait for Qt thread
        else if ( m_threadState == ThreadIsStopped )
            QThread::wait();

        //
        // Prelude before thread start
        //
        {
            // Generate new current connection uuid for whole server
            // and its clients which work in server context
            m_currConnectionUuid = Uuid::createUuid().toString();

            // Mark as starting
            m_threadState = ThreadIsStarting;
        }

        // Start thread
        QThread::start();
        // Check abnormal situation while thread start (out of resources or smth)
        // Note: running flag must be set in 'Qthread::start' call, not in newly
        // started thread, so this check is race-safe.
        if ( ! QThread::isRunning() ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("ERROR: thread has not been started!"));
            m_threadState = ThreadIsStopped;
            return IOSender::Disconnected;
        }

        m_threadStateWait.wait( &m_eventMutex );
        // Check if failure on start
        if ( m_threadState != ThreadIsStarted ) {
            // Unlock. Stopping thread will use locks
            locker.unlock();
            // Wait for Qt thread
            QThread::wait();
            // Lock
            locker.relock();
            Q_ASSERT(m_threadState == ThreadIsStopped);
            Q_ASSERT(m_state == IOSender::Disconnected);
            return IOSender::Disconnected;
        }

        // Great!
        Q_ASSERT(m_state == IOSender::Connected);
        return IOSender::Connected;
}

void SocketServerPrivate::stopServer ()
{
        // If call from this thread: just mark as finalized
        if ( QThread::currentThread() == this ) {
            // Lock
            QMutexLocker locker( &m_eventMutex );
            __finalizeThread();
            return;
        }

        // Main start/stop lock
        QMutexLocker mainLocker( &m_startStopMutex );

        // Lock
        QMutexLocker locker( &m_eventMutex );

        // 'starting' state is invisible for us,
        // because starting thread must lock start/stop mutex
        // and wait for correct result
        Q_ASSERT(m_threadState != ThreadIsStarting);

        // Already stopped
        if ( m_threadState == ThreadIsStopped ) {
            // Wait for Qt thread
            QThread::wait();
            return;
        }
        // Stopping in progress, wait for result
        else if ( m_threadState == ThreadIsStopping ) {
            m_threadStateWait.wait( &m_eventMutex );
            Q_ASSERT(m_threadState == ThreadIsStopped);

            // Wait for Qt thread
            QThread::wait();
            return;
        }

        //
        // Finalize server!
        //
        __finalizeThread();

        m_threadStateWait.wait( &m_eventMutex );
        Q_ASSERT(m_threadState == ThreadIsStopped);

        // Wait for Qt thread
        QThread::wait();
        return;
}

void SocketServerPrivate::__finalizeThread ()
{
    // Mark as stopping
    m_threadState = ThreadIsStopping;

    if ( m_ctx == Srv_ListeningContext ) {
        // Signal events
#ifdef _WIN_
        ::WSASetEvent( m_connectEventHandle );
#else
        char b = 0;
        if ( ::write( m_eventPipes[1], &b, 1 ) < 0 )
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Write failed while thread finalization!"));
#endif
    }
    else if ( m_ctx == Srv_AttachingContext ) {
        m_attachWait.wakeOne();
    }
    else
        Q_ASSERT(0);
}


bool SocketServerPrivate::createAndStartNewSockClient (
    int cliHandle
    , IOCommunication::DetachedClient detachedState
	)
{
    SmartPtr<SocketClientPrivate> client;

    // Safely get current connection uuid
    // Also, we do not bother about race,
    // we just want not to destroy string memory
    QString currConnUuid = currentConnectionUuid();

	// Make copy to reduce mutex lock time.
	QMutexLocker locker( &m_eventMutex );
	IOCredentials credentialsCopy( m_localCredentials );
	locker.unlock();

        // Create client in direct mode
        try { client = SmartPtr<SocketClientPrivate>(
                       new SocketClientPrivate( m_impl->getJobManager(),
                                                m_impl->getRoutingTable(),
                                                m_impl->senderType(),
                                                IOSender::DirectConnectionMode,
                                                m_impl->remoteHostName(),
                                                m_impl->remotePortNumber(),
                                                IOSender::InvalidHandle, //proxy handle

                                                Cli_ServerContext,
                                                m_impl, // rcv/snd listener
                                                this,   // state listener
                                                cliHandle,
                                                currConnUuid,
                                                detachedState,
												credentialsCopy,
                                                m_useUnixSockets ) );
        } catch ( ... ) {}


    if ( ! client.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
        return false;
    }

    // Atomic start
	locker.relock();

    if ( m_state != IOSender::Connected ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't create new client for "
                                      "not started server"));
        return false;
    }

    // We should add client to list in any case (even an error will happen)
    // Client will emit disconnected state, and we will catch it.
    m_preAppendClients[client.getImpl()] = client;

    // Unlock
    locker.unlock();

    //
    // There can be one interesting but safe case, when server is going down:
    //    1. we add client to preappend list.
    //    2. server clears all clients from list and stops them.
    //         (during stop our newly created client will do nothing,
    //          because it is not started yet)
    //    3. we start newly created client
    //    4. we return from this function: newly created will be correctly
    //         stopped and destroyed by smartptr, because nothing holds
    //         this pointer (server thread has cleared preappend list in 2)
    //

    // Start newborn client
    bool res =
        client->startClient( IOServer::ClientConnectionTimeout );
    if ( ! res ) {
        //TODO: we should handle, that thread has not been started
        //      (out of resources or smth).
        //      in this case we should clear preappend list manually and
        //      return false. i.e. I mean that now returned 'false'
        //      does not say directly that client thread has not been started
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't start new client"));
    }

    return res;
}

IOSendJob::Handle SocketServerPrivate::sendPackage (
    const IOSender::Handle& h,
    const SmartPtr<IOPackage>& p )
{
    // Lock clients
    QMutexLocker locker( &m_eventMutex );

    if ( m_state != IOSender::Connected )
        return IOSendJob::Handle();

    const SmartPtr<SocketClientPrivate> client = m_sockClients.value(h);
    if ( ! client )
        return IOSendJob::Handle();

    // Unlock
    locker.unlock();

    // Generate numeric identifier for the packet just before sending it
    p->generateNumericId();

    // Send pkg
    return client->sendPackage( p );
}

// Does final dereference of detached client.
// This is package destructor callback.
static void FreeDetachedClient ( void* context )
{
    IOCommunication::DetachedClient* cli =
        reinterpret_cast<IOCommunication::DetachedClient*>(context);
    delete cli;
}

IOSendJob::Handle SocketServerPrivate::sendDetachedClient (
    const IOSender::Handle& h,
    const IOCommunication::DetachedClient& detachedClient,
    const SmartPtr<IOPackage>& request )
{
    if ( ! detachedClient.isValid() ||
         ! detachedClient->attach() ) {
        LOG_MESSAGE(DBG_WARNING, IO_LOG("Client is already attached!"));
        return IOSendJob::Handle();
    }

    SmartPtr<IOPackage> pkg =
        IOCommunication::createAttachClientPackage( detachedClient,
                                                    request );
    if ( ! pkg.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't create attach client package!"));
        return IOSendJob::Handle();
    }

    // Create wrapper around smartptr to increment reference
    IOCommunication::DetachedClient* cliWrapper =
        new(std::nothrow) IOCommunication::DetachedClient(detachedClient);
    if ( cliWrapper == 0 || ! cliWrapper->isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't create attach client package!"));
        delete cliWrapper;
        return IOSendJob::Handle();
    }

    // We must deref detached client smartptr only after send has been completed.
    // Does this final deref in package destructor.
    pkg->callback.destructorCall = FreeDetachedClient;
    pkg->callback.destructorContext = cliWrapper;

    return sendPackage( h, pkg );
}

void SocketServerPrivate::run ()
{
    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];
    // Dual socks: IPv4, IPv6
    const uint MaxSocks = 2;
    int servHandles[ MaxSocks ];
    for ( uint i = 0; i < MaxSocks; ++i )
        servHandles[i] = -1;
    addrinfo* addrInfo = 0;
#ifndef _WIN_
    struct sockaddr_un * pAddr = 0;
#endif
    // Binding info for listening server
    IPvBindingInfo bindingInfo;

#ifdef _WIN_ // Windows

    // Events for all socks + connection event + clean event
    WSAEVENT wsaEvents[ MaxSocks + 2 ];
    for ( uint i = 0; i < sizeof(wsaEvents)/sizeof(wsaEvents[0]); ++i )
        wsaEvents[i] = WSA_INVALID_EVENT;

    // Init WSA
    if ( ! WSAInitHelper::InitWSA() ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Winsock 2.2 does not supported (native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize) );
        goto cleanup_and_disconnect;
    }

#endif

    // Create listen socket if server is listening
    if ( m_ctx == Srv_ListeningContext ) {
        QString host = m_impl->remoteHostName();
        QString port = QString("%1").arg(m_impl->remotePortNumber());
        bool isAnyAddr = (host == IOService::AnyAddr);
        bool isLoopbackAddr = (host == IOService::LoopbackAddr);
        addrinfo hints;
        quint16 portNumber = 0;
        ::memset( &hints, 0, sizeof (hints) );
        hints.ai_flags = AI_ADDRCONFIG | (isAnyAddr ? AI_PASSIVE : 0);
#ifndef _WIN_
        if (!m_useUnixSockets)
        {
#endif
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int res = ::getaddrinfo( (isAnyAddr || isLoopbackAddr
                                  ? 0
                                  : qPrintable(host)),
                                 qPrintable(port),
                                 &hints,
                                 &addrInfo );
        if ( res != 0 ) {
            int err = native_error();
            const char* errP = 0;
#ifdef _WIN_ // Windows
            errP = native_strerror(err, errBuff, ErrBuffSize);
#else // Unix
            if ( res == EAI_SYSTEM )
                errP = native_strerror(err, errBuff, ErrBuffSize);
            else
                errP = ::gai_strerror(res);
#endif
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Can't do getaddrinfo for remote host name "
                               "'%s', (native error: %s)"),
                        qPrintable(host),
                        errP);

            goto cleanup_and_disconnect;
        }
#ifndef _WIN_
        }
        else
        {
            addrInfo = (addrinfo *)::malloc(sizeof(addrinfo));
            if ( !addrInfo )
            {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Failed to allocate memory"));
                goto cleanup_and_disconnect;
            }
            ::memset(addrInfo, 0, sizeof(addrinfo));
            addrInfo->ai_family = AF_UNIX;
            addrInfo->ai_socktype = SOCK_STREAM;

            size_t sockaddr_size = PATH_MAX
                + ( sizeof(struct sockaddr_un) - sizeof(pAddr->sun_path)  );
            pAddr = (struct sockaddr_un *)::malloc(sockaddr_size);
            if ( !pAddr )
            {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Failed to allocate memory"));
                goto cleanup_and_disconnect;
            }
            ::memset(pAddr, 0, sockaddr_size );
            pAddr->sun_family = AF_UNIX;

            QByteArray path = host.toUtf8();
            if( path.size() > PATH_MAX - 1 )
            {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Too large unix socket path."));
                goto cleanup_and_disconnect;
            }
            ::strncpy(pAddr->sun_path, path.constData(), PATH_MAX - 1);
        }
#endif

        bool listened = false;
        QList<ErrDesc> errList;
        addrinfo* listenAddrInfo = 0;
        const quint32 MaxBindingTries = 50;
        quint32 bindingTry = 0;

        // Order addrinfo with IPv6 priority
        QList<addrinfo*> addrInfoList =
            orderAddrInfo( addrInfo, OP_PreferIPv6 );

    iterate_from_the_beginning:

        // Reinit values
        listened = false;
        listenAddrInfo = 0;
        errList.clear();
        bindingInfo.clear();
        portNumber = m_impl->remotePortNumber();
        ++bindingTry;

        // Check binding tries
        if ( bindingTry >= MaxBindingTries ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Server tried '%d' times to bind, but could not "
                               "find unique free port for '%d' addresses. "
                               "Will stop everything."),
                        bindingTry, addrInfoList.size());
            goto cleanup_and_disconnect;
        }

        // Get suitable addr info in loop
        for ( int i = 0; i < addrInfoList.size() && i < (int)MaxSocks; ++i ) {

            addrinfo* ai = addrInfoList[i];

#ifndef _WIN_
            if ( !m_useUnixSockets )
#endif
                servHandles[i] = ::socket( ai->ai_family,
                                       ai->ai_socktype,
                                       ai->ai_protocol );
#ifndef _WIN_
            else
                servHandles[i] = ::socket( AF_LOCAL, SOCK_STREAM, 0 );
#endif

            if ( servHandles[i] < 0 ) {
                // Create error
                ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                    ai->ai_protocol, native_error(),
                                    __LINE__ };
                errList.append( errDesc );

                // Try next address
                continue;
            }

            // Do not use IPv4 -> IPv6 mapping.
            //
            // On some OSes this flag is turned off, so
            // IPv4->IPv6 mapping is used by default, and
            // second binding can fail with errno = EADDRINUSE.
            // So we just always try to use IPV6_V6ONLY.
            if ( ai->ai_family == AF_INET6 ) do {
#ifdef _WIN_
                // IPV6_V6ONLY is supported on >= Windows Vista
                OSVERSIONINFO osvi;
                ::memset(&osvi, 0, sizeof(OSVERSIONINFO));
                osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
                ::GetVersionEx(&osvi);
                if ( osvi.dwMajorVersion < 6 )
                    break;
#endif
                int v6only = 1;
                if ( ::setsockopt(servHandles[i], IPPROTO_IPV6, IPV6_V6ONLY,
                                  reinterpret_cast<char*>(&v6only),
                                  sizeof(v6only)) < 0 ) {
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Can't set IPV6_V6ONLY "
                                       "(native error: %s)"),
                                native_strerror(errBuff, ErrBuffSize) );
                    goto cleanup_and_disconnect;
                }
            } while(0);

            // Actually, port number will be changed further.
            // So, at first iteration it will be equal to remotePortNumber call.
            if ( m_impl->remotePortNumber() == 0 && portNumber != 0 ) {
                if ( ai->ai_family == AF_INET6 )
                    ((sockaddr_in6*)(ai->ai_addr))->sin6_port = htons(portNumber);
                else
                    ((sockaddr_in*)(ai->ai_addr))->sin_port = htons(portNumber);
            }

            // Unix:    ensure that listening socket is closed on exec*()
            // Windows: prevent from inheritance
            if ( ! IOService::setCloseOnExec(servHandles[i]) ) {
#ifdef _WIN_ // Windows
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't duplicate listening socket "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
#else // Unix
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't set descriptor flags "
                                   "for listening socket (native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
#endif
                goto cleanup_and_disconnect;
            }


#ifndef _WIN_ // Unix

            // Reuse addr to avoid annoying timeout after server disconnect
            int reuse = 1;
            if ( ::setsockopt(servHandles[i], SOL_SOCKET, SO_REUSEADDR,
                              (char*)&reuse, sizeof(reuse)) < 0 ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't set socket options "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
                goto cleanup_and_disconnect;
            }

            // Set non-blocking mode
            int flags = 0;
            if ( (flags = ::fcntl(servHandles[i], F_GETFL, 0)) < 0 ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't get flags for socket "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
                goto cleanup_and_disconnect;
            }
            flags |= O_NONBLOCK;

            if ( ::fcntl(servHandles[i], F_SETFL, flags) < 0 ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't set flags for socket "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
                goto cleanup_and_disconnect;
            }

#else
            // Set non-blocking mode
            u_long mode = 1;
            if ( ::ioctlsocket(servHandles[i], FIONBIO, &mode) ==
                 SOCKET_ERROR ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't set socket options "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
                goto cleanup_and_disconnect;
            }
#endif
            int res = 0;
#ifndef _WIN_
            if (!m_useUnixSockets)
#endif
                res = ::bind(servHandles[i], ai->ai_addr, (int)ai->ai_addrlen);
#ifndef _WIN_
            else
            {
                ::unlink( host.toUtf8().constData() );
                res = ::bind(servHandles[i], (struct sockaddr *)pAddr, SUN_LEN(pAddr));
                if ( 0 == res )
                {
                    // Allow access for all to UNIX socket
                    struct stat _stat;
                    ::memset( &_stat, 0, sizeof( struct stat ) );
                    if ( ::stat( host.toUtf8().constData(), &_stat ) == 0 )
                    {
                        mode_t _new_mode = _stat.st_mode | S_IRWXU | S_IRWXG | S_IRWXO;
                        if ( ::chmod( host.toUtf8().constData(), _new_mode ) != 0 )
                        {
                            int nErrno = errno;
                            WRITE_TRACE(DBG_FATAL, IO_LOG("Failed to setup permissions on socket '%s' with error: %d"), host.toUtf8().constData(), nErrno);
                            goto cleanup_and_disconnect;
                        }
                    }
                    else
                    {
                        int nErrno = errno;
                        WRITE_TRACE(DBG_FATAL, IO_LOG("Failed to get stat on socket '%s' with error: %d"), host.toUtf8().constData(), nErrno);
                        goto cleanup_and_disconnect;
                    }
                }
            }
#endif

            if ( res < 0 ) {
                if ( m_impl->remotePortNumber() == 0 &&
#ifdef _WIN_
                    native_error() == WSAEADDRINUSE ) {
#else
                    native_error() == EADDRINUSE ) {
#endif
                    // Close all opened sockets
                    for ( int j = 0; j <= i; j++ ) {
                        if ( servHandles[j] > 0 ) {
#ifdef _WIN_
                            ::closesocket(servHandles[j]);
#else
                            ::close(servHandles[j]);
#endif
                            servHandles[j] = -1;
                        }
                    }

                    // Try again
                    goto iterate_from_the_beginning;
                }
                else {
                    // Create error
                    ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                        ai->ai_protocol, native_error(),
                                        __LINE__ };
                    errList.append( errDesc );

                    // Close socket before next try
#ifdef _WIN_
                    ::closesocket(servHandles[i]);
#else
                    ::close(servHandles[i]);
#endif
                    servHandles[i] = -1;

                    // Try next address
                    continue;
                }
            }

            if ( ::listen(servHandles[i], SOMAXCONN) < 0 ) {
                // Create error
                ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                    ai->ai_protocol, native_error(),
                                    __LINE__ };
                errList.append( errDesc );

#ifdef _WIN_
                ::closesocket(servHandles[i]);
#else
                ::close(servHandles[i]);
#endif
                servHandles[i] = -1;

                // Try next address
                continue;
            }

            // If this call of dynamic port, retrieve and set it real value
            QString hostAddress;
            QString errStr;
            if ( ! IOService::getHostInfo(servHandles[i], hostAddress,
                                          portNumber, errStr) ) {
                hostAddress = host;
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Getting of numeric host name failed, error: %s"),
                            qPrintable(errStr));

            }

            // Store binding information
            bindingInfo.insert((ai->ai_family == AF_INET6) ? IPv6 : IPv4,
                               QPair<QString, quint16>(hostAddress, portNumber));

            // Successfully listened. Try next.
            if ( ! listened ) {
                listened = true;
                listenAddrInfo = ai;
            }

			ETRACE_LOG(servHandles[i] & 0xFF, ETRACE_CP_IOSERVICE,
				(((quint16)m_impl->senderType() & 0xF) << 6) |
				(ETRACE_IOS_EVENT_SRV_LISTEN & 0x3F));
        }

        // Done.

        // Log out all errors
        if ( ! errList.isEmpty() ) {
            // Success log header if we are listening even only one socket
            if ( listened ) {
                Q_ASSERT(listenAddrInfo);
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Successfully listening to socket "
                                   "ai_family = %d, ai_socktype = %d, "
                                   "ai_protocol = %d"),
                            listenAddrInfo->ai_family,
                            listenAddrInfo->ai_socktype,
                            listenAddrInfo->ai_protocol);
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("But there were some troubles:"));
                WRITE_TRACE(DBG_FATAL, IO_LOG("WARNINGS BEGIN"));
            }
            // Error header
            else {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Can't listen to socket:"));
                WRITE_TRACE(DBG_FATAL, IO_LOG("ERRORS BEGIN"));
            }

            foreach ( ErrDesc errDesc, errList )
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Error for socket ai_family = %d, "
                                   "ai_socktype = %d, ai_protocol = %d "
                                   "in code line #%d, native error: %s"),
                            errDesc.ai_family,
                            errDesc.ai_socktype,
                            errDesc.ai_protocol,
                            errDesc.codeLine,
                            native_strerror(errDesc.error, errBuff,
                                            ErrBuffSize));
            if ( listened )
                WRITE_TRACE(DBG_FATAL, IO_LOG("WARNINGS END"));
            else
                WRITE_TRACE(DBG_FATAL, IO_LOG("ERRORS END"));
        }

        // Cleanup if not listened
        if ( ! listened )
            goto cleanup_and_disconnect;

#ifdef _WIN_ // Windows

        // Init events for every sock handle + connection event + clean event
        for ( uint i = 0; i < sizeof(wsaEvents)/sizeof(wsaEvents[0]); ++i ) {
            wsaEvents[i] = ::WSACreateEvent();
            if ( wsaEvents == WSA_INVALID_EVENT ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't create connect event "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
                goto cleanup_and_disconnect;
            }

            // Associate sock event with valid socket
            if ( i < MaxSocks && servHandles[i] != -1 ) {
                if ( ::WSAEventSelect(servHandles[i],
                                      wsaEvents[i],
                                      FD_ACCEPT)== SOCKET_ERROR ){
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Can't associate event with FD_ACCEPT"
                                       " (native error: %s)"),
                                native_strerror(errBuff, ErrBuffSize) );
                    goto cleanup_and_disconnect;
                }
            }
        }
        // Connection event and clean event must be the last
        m_connectEventHandle = wsaEvents[ MaxSocks ];
        m_cleanEventHandle = wsaEvents[ MaxSocks + 1 ];

#else // Unix

        // Create pipes for finalization
        if ( ::pipe(m_eventPipes) < 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Can't create event pipes (native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize) );
            goto cleanup_and_disconnect;
        }
        // Create pipes for client cleaning
        if ( ::pipe(m_eventPipes + 2) < 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Can't create event pipes (native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize) );
            goto cleanup_and_disconnect;
        }
        // Set non-blocking mode for all pipes
        for ( uint i = 0;
              i < sizeof(m_eventPipes)/sizeof(m_eventPipes[0]); ++i ) {
            if ( ::fcntl(m_eventPipes[i], F_SETFL, O_NONBLOCK) < 0 ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't set O_NONBLOCK for event pipe #%d "
                                   "(native error: %s)"),
                            i, native_strerror(errBuff, ErrBuffSize) );
                goto cleanup_and_disconnect;
            }
        }
#endif
    }
    // Attaching context
    else if ( m_ctx == Srv_AttachingContext ) {
        // Nothing to do
    }
    // Other contexts
    else
        Q_ASSERT(0);

    //
    // Great. We are absolutely connected now.
    // We can't handle errors from here, because after connected state emition
    // we should emit disconnected state, not goto error handler.
    //
    {
        // Lock
        QMutexLocker locker( &m_eventMutex );

        // Set binding information for listening server
        if ( m_ctx == Srv_ListeningContext )
            m_bindingInfo = bindingInfo;

        // Set connected state
        m_state = IOSender::Connected;

        // We should mark as started just after event pipes/handle creation
        // and before looooong operations to have a possibility to safely
        // stop this thread from any place.
        // For server we don't have any looong jobs, so we mark as started
        // just before main loop.
        m_threadState = ThreadIsStarted;
        m_threadStateWait.wakeOne();

        // Unlock
        locker.unlock();

        // Emit connected state
        emit m_impl->onServerStateChanged( IOSender::Connected );
    }

    while ( 1 ) {

        // For listening server
        if ( m_ctx == Srv_ListeningContext ) {
#ifndef _WIN_ // Unix

            // Create params for select
            int stoppingPipe = m_eventPipes[0];
            int cleaningPipe = m_eventPipes[2];
            int maxpipe = (stoppingPipe > cleaningPipe ?
                               stoppingPipe : cleaningPipe);
            int maxsock = -1;

            // Find max sock
            for ( quint32 i = 0; i < MaxSocks; ++i ) {
                maxsock = (servHandles[i] > maxsock ? servHandles[i] : maxsock);
            }
            // Get max fd
            int maxfd = (maxsock > maxpipe ? maxsock : maxpipe) + 1;
            if ( maxfd > m_maxFDNum ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Out of resources! Descriptor "
                                   "'%d' exceeds max possible '%d'."),
                            maxfd, m_maxFDNum);
                break;
            }

            // We can't use FD_ZERO here, because
            // of not original fd_set size
            ::memset(m_rdSet.getImpl(), 0, FDNUM2SZ(m_maxFDNum));
            FD_SET(stoppingPipe, m_rdSet.getImpl());
            FD_SET(cleaningPipe, m_rdSet.getImpl());

            // Set handles
            for ( quint32 i = 0; i < MaxSocks; ++i ) {
                if ( servHandles[i] > 0 )
                    FD_SET(servHandles[i], m_rdSet.getImpl());
            }

            // Do select
            int res = ::select( maxfd, m_rdSet.getImpl(), 0, 0, 0 );
            if ( res == 0 ) {
                // Timeout
                WRITE_TRACE(DBG_FATAL, IO_LOG("Timeout? Try again."));
                continue;
            }
            else if ( res < 0 && errno == EINTR ) {
                LOG_MESSAGE(DBG_INFO, IO_LOG("Select has been interrupted"));
                continue;
            }
            else if ( res < 0 ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Can't accept client socket "
                                              "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));
                break;
            }

            // Check stop in progress
            if ( FD_ISSET(stoppingPipe, m_rdSet.getImpl()) ) {
                WRITE_TRACE(DBG_INFO, IO_LOG("Stop in progress for server"));
                break;
            }

            // Check clean request
            if ( FD_ISSET(cleaningPipe, m_rdSet.getImpl()) ) {
                char buff[ 64 ];
                int err = 0;
                // Read all data
                for (;;) {
                    int res = ::read( cleaningPipe, buff, sizeof(buff) );
                    if ( res < 0 && errno == EAGAIN )
                        break;
                    else if ( res < 0 && errno != EINTR ) {
                        err = errno;
                        WRITE_TRACE(DBG_FATAL,
                                    IO_LOG("Read from cleaning pipe failed "
                                           "(native error: %s)"),
                                    native_strerror(errBuff, ErrBuffSize));
                        break;
                    }
                }
                if ( err == 0 ) {
                    cleanStoppedClients();
                    continue;
                }
                else
                    break;

            }


#else // Windows

            // Wait for accept
            DWORD res = ::WSAWaitForMultipleEvents(
                                         // All events
                                         sizeof(wsaEvents)/sizeof(wsaEvents[0]),
                                         wsaEvents,
                                         FALSE,
                                         WSA_INFINITE,
                                         FALSE ); // not alertable

            // Check wait result
            if ( res == WSA_WAIT_FAILED ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Error while waiting for new clients "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));
                break;
            }

            // Get signaled event index
            DWORD iEvent = res - WSA_WAIT_EVENT_0;

            // Check bounds
            if ( iEvent >= sizeof(wsaEvents)/sizeof(wsaEvents[0]) ||
                 iEvent < 0 ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't accept, wrong event index! "
                                   "Server will be stopped!"));
                break;
            }

            // Reset signaled event
            ::WSAResetEvent( wsaEvents[iEvent] );

            // Check stop in progress
            if ( m_threadState == ThreadIsStopping ) {
                break;
            }

            // Check clean request
            if ( wsaEvents[iEvent] == m_cleanEventHandle ) {
                cleanStoppedClients();
                continue;
            }

            // Connection event and clean event must be handled before
            Q_ASSERT( iEvent < MaxSocks );

            // Ok. Get result
            WSANETWORKEVENTS wsaEventsRes;
            res = ::WSAEnumNetworkEvents( servHandles[iEvent],
                                          wsaEvents[iEvent],
                                          &wsaEventsRes );
            if ( res == SOCKET_ERROR ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't accept, WSAEnumNetworkEvents failed "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
                break;
            }

            DWORD err = wsaEventsRes.iErrorCode[ FD_ACCEPT_BIT ];
            if ( err != 0 ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't accept client socket "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));
                continue;
            }
#endif

            // Check every sock
            for ( quint32 i = 0; i < MaxSocks; ++i ) {
                // If not valid
                if ( servHandles[i] == -1 )
                    continue;
#ifndef _WIN_ // Unix
                // Check if sock is active
                else if ( ! FD_ISSET(servHandles[i], m_rdSet.getImpl()) )
                    continue;
#else // Windows
                // Check if event is signaled
                else if ( iEvent != i )
                    continue;
#endif

                // Client addr
                sockaddr_storage cli;
                ::memset(&cli, 0, sizeof(cli));
                socklen_t cliLen = sizeof(cli);

                // Accepting client
                int handle = ::accept( servHandles[i],
                                       reinterpret_cast<sockaddr*>(&cli),
                                       &cliLen );

#ifndef _WIN_ // Unix

                if ( handle < 0 && errno == EINTR ) {
                    LOG_MESSAGE(DBG_INFO,
                                IO_LOG("Accept has been interrupted by EINTR"));
                    continue;
                }
                else
#endif
                if ( handle < 0 ) {
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Accept error (native error: %s)"),
                                native_strerror(errBuff, ErrBuffSize));
                    continue;
                }

				ETRACE_LOG(servHandles[i] & 0xFF, ETRACE_CP_IOSERVICE,
					((quint64)handle << 32) |
					(((quint16)m_impl->senderType() & 0xF) << 6) |
					(ETRACE_IOS_EVENT_SRV_ACCEPT & 0x3F));

                // Create, append and start client
                createAndStartNewSockClient(
                                        handle,
                                        IOCommunication::DetachedClient() );
            }
        }
        // Attaching server
        else if ( m_ctx == Srv_AttachingContext ) {
            // Lock
            QMutexLocker locker( &m_eventMutex );

            // Server stop in progress
            if ( m_threadState == ThreadIsStopping )
                break;

            // Check clean request
            if ( m_stoppedSockClients.size() > 0 ) {
                // Unlock
                locker.unlock();
                cleanStoppedClients();
                continue;
            }

            if ( m_attachQueue.isEmpty() ) {
                // Wait if empty
                m_attachWait.wait( &m_eventMutex );

                // Server stop in progress
                if ( m_threadState == ThreadIsStopping )
                    break;

                // Check clean request
                if ( m_stoppedSockClients.size() > 0 ) {
                    // Unlock
                    locker.unlock();
                    cleanStoppedClients();
                    continue;
                }
            }

            // One more time
            if ( m_attachQueue.isEmpty() )
                continue;

            // Dequeue state
            IOCommunication::DetachedClient state = m_attachQueue.dequeue();

            // Unlock
            locker.unlock();

            // Create, append and start client
            createAndStartNewSockClient( -1, state );
        }
        // Other contexts
        else
            Q_ASSERT(0);
    }

 cleanup_and_disconnect:
    //
    // Finalize server thread
    //

    //
    // We are disconnecting now. Set state to disconneted.
    //
    IOSender::State oldState = m_state;
    {
        // Lock
        QMutexLocker locker( &m_eventMutex );

        // Set disconneted state
        // This should prevent access from outside,
        // e.g. if smb wants to attach new client
        m_state = IOSender::Disconnected;

        // Mark as stopping
        m_threadState = ThreadIsStopping;

        // Unlock
        locker.unlock();

        // Emit disconnected state
        emit m_impl->onServerStateChanged( IOSender::Disconnected );
    }

    // If we were successfully connected:
    //   cleanup all clients
    if ( oldState == IOSender::Connected ) {
        cleanAllClients();
    }

    // Atomic stop. Actually, we do not need any locks for
    // sock handle close (we need locks only for closing event objects)
    // but in such case we will have more code.
    m_eventMutex.lock();

    // Do correct close if listening server
    if ( m_ctx == Srv_ListeningContext ) {

        // Free addr info
        if ( addrInfo ) {
            ::freeaddrinfo( addrInfo );
            addrInfo = 0;
        }
#ifndef _WIN_
        // Free UNIX addr data
        if ( m_useUnixSockets && pAddr )
        {
            ::free( pAddr );
            pAddr = 0;
        }
#endif

#ifdef _WIN_ // Windows

        // Close handles
        for ( uint i = 0; i < MaxSocks; ++i ) {
            if ( servHandles[i] != -1 ) {
                ::closesocket(servHandles[i]);
				ETRACE_LOG(servHandles[i] & 0xFF, ETRACE_CP_IOSERVICE,
					(((quint16)m_impl->senderType() & 0xF) << 6) |
					(ETRACE_IOS_EVENT_SRV_CLOSE & 0x3F));
                servHandles[i] = -1;
            }
        }

        // Close all events
        m_connectEventHandle = WSA_INVALID_EVENT;
        m_cleanEventHandle = WSA_INVALID_EVENT;
        for ( uint i = 0; i < sizeof(wsaEvents)/sizeof(wsaEvents[0]); ++i ) {
            if ( wsaEvents[i] != WSA_INVALID_EVENT ) {
                ::WSACloseEvent(wsaEvents[i]);
                wsaEvents[i] = WSA_INVALID_EVENT;
            }
        }

        // Deinit WSA
        WSAInitHelper::DeinitWSA();

#else // Unix

        // Close handles
        for ( uint i = 0; i < MaxSocks; ++i ) {
            if ( servHandles[i] != -1 ) {
                ::close(servHandles[i]);
				ETRACE_LOG(servHandles[i] & 0xFF, ETRACE_CP_IOSERVICE,
					(((quint16)m_impl->senderType() & 0xF) << 6) |
					(ETRACE_IOS_EVENT_SRV_CLOSE & 0x3F));
                servHandles[i] = -1;
            }
        }
        if ( m_useUnixSockets && m_impl )
            ::unlink( m_impl->remoteHostName().toUtf8().constData() );

        // Close all pipes
        for ( uint i = 0;
              i < sizeof(m_eventPipes)/sizeof(m_eventPipes[0]); ++i ) {
            if ( m_eventPipes[i] != -1 ) {
                ::close( m_eventPipes[i] );
                m_eventPipes[i] = -1;
            }
        }
#endif
    }

    // Mark as stopped
    m_threadState = ThreadIsStopped;
    m_threadStateWait.wakeOne();

    // Unlock
    m_eventMutex.unlock();

    return;
}

void SocketServerPrivate::cleanAllClients ()
{
    // All clients
    QList< SmartPtr<SocketClientPrivate> > clients;

    // Lock
    QMutexLocker locker( &m_eventMutex );

    clients += m_sockClients.values();
    clients += m_preAppendClients.values();
    clients += m_stoppedSockClients;

    // Unlock
    locker.unlock();

    // Stop all clients: we do not need any locks here
    QList< SmartPtr<SocketClientPrivate> >::Iterator it = clients.begin();
    while ( it != clients.end() ) {
        SmartPtr<SocketClientPrivate>& client = *it;
        client->stopClient();
        it = clients.erase(it);
    }

    // Lock
    locker.relock();

    // Clear all lists
    m_sockClients.clear();
    m_preAppendClients.clear();
    m_stoppedSockClients.clear();
    m_clientsUuids.clear();
    m_attachQueue.clear();

    // Unlock
    locker.unlock();
}

void SocketServerPrivate::cleanStoppedClients ()
{
    // Lock
    QMutexLocker locker( &m_eventMutex );

    QList< SmartPtr<SocketClientPrivate> > stoppedClients =
        m_stoppedSockClients;
    m_stoppedSockClients.clear();

    // Unlock
    locker.unlock();

    // Stop and delete all clients
    QList< SmartPtr<SocketClientPrivate> >::Iterator it;
    it = stoppedClients.begin();
    while ( it != stoppedClients.end() ) {
        SmartPtr<SocketClientPrivate>& client = *it;
        client->stopClient();
        it = stoppedClients.erase(it);
    }
}

void SocketServerPrivate::wakeupClientsCleaner ()
{
    // Lock
    QMutexLocker locker( &m_eventMutex );

    if ( m_ctx == Srv_ListeningContext ) {
        // Signal events
#ifdef _WIN_
        ::WSASetEvent( m_cleanEventHandle );
#else
        char b = 0;
        if ( ::write(m_eventPipes[3], &b, 1) < 0 )
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Write failed while waking up cleaner!"));
#endif
    }
    else if ( m_ctx == Srv_AttachingContext ) {
        m_attachWait.wakeOne();
    }
    else {
        Q_ASSERT(0);
    }
}

/******************************************************************************
 * Client state callbacks
 *****************************************************************************/

bool SocketServerPrivate::srv_validateAfterHandshake ( SocketClientPrivate* client,
                                                       const IOSender::Handle& h )
{
    // If client is a proxy mng client:
    //    skip all checks!
    if ( client == m_prxMngClient.getImpl() )
        return true;

    // Lock
    QMutexLocker locker( &m_eventMutex );
    if ( m_clientsUuids.contains(h) )
        return false;

    m_clientsUuids[h] = h;
    return true;
}

void SocketServerPrivate::onStateChanged (
    SocketClientPrivate* client,
    const IOSender::Handle& h,
    IOSender::State oldState,
    IOSender::State newState )
{
    // If client is a proxy mng client
    if ( client == m_prxMngClient.getImpl() ) {
        // Lock
        QMutexLocker locker( &m_eventMutex );

        if ( newState == IOSender::Connected ) {
            // Generate new current connection uuid for whole server
            // and its clients which work in server context
            m_currConnectionUuid = Uuid::createUuid().toString();
        }

        // Init server state
        m_state = newState;

        // Unlock
        locker.unlock();

        // Forward proxy mng client state as server state
        emit m_impl->onServerStateChanged( newState );

        // If proxy mng client disconnected:
        //    cleanup all clients
        if ( newState == IOSender::Disconnected )
            cleanAllClients();
    }
    // Real server clients
    else {
        // Successfully connected. Add to main list
        if ( newState == IOSender::Connected ) {

            // Lock
            QMutexLocker locker( &m_eventMutex );

            // Must be in preappend list
            if ( m_preAppendClients.contains(client) ) {
                // Move client from pre append list to main client list
                m_sockClients[h] = m_preAppendClients.take(client);
            }
            else {
                // Client must exist in list!!!
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Client '%s' does not exist in list!"),
                            qPrintable(h));
                Q_ASSERT(0);
                return;
            }

            // Unlock
            locker.unlock();

            // Emit server specific signals
            emit m_impl->onClientStateChanged( h, newState );
            emit m_impl->onClientStateChanged( m_impl, h, newState );
            SmartPtr<IOPackage> p;
            if ( client->srv_getAttachingAdditionalPkg(p) )
                emit m_impl->onClientAttached ( h, p );
            else
                emit m_impl->onClientConnected ( h );
        }
        // Disconnected
        else if ( newState == IOSender::Disconnected ) {

            // If we were connected: emit server specific signals
            if ( oldState == IOSender::Connected ) {
                emit m_impl->onClientDisconnected ( h );
                emit m_impl->onClientStateChanged( h, newState );
                emit m_impl->onClientStateChanged( m_impl, h, newState );
            }

            // Lock
            QMutexLocker locker( &m_eventMutex );

            SmartPtr<SocketClientPrivate> smartClient;
            // Check if we were never connected
            if ( m_preAppendClients.contains(client) ) {
                Q_ASSERT(oldState == IOSender::Disconnected);
                smartClient = m_preAppendClients.take(client);
            }
            // We were connected, and now should do disconnect job
            else if ( m_sockClients.contains(h) ) {
                smartClient = m_sockClients.take(h);
            }
            else {
                // Client must exist in list!!!
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Client '%s' does not exist in list!"),
                            qPrintable(h));
                Q_ASSERT(0);
                return;
            }

            // Add to stopped list
            m_stoppedSockClients.append( smartClient );

            // Remove client validation uuid
            m_clientsUuids.remove( h );

            // Unlock
            locker.unlock();

            // Stopped clients must be cleaned from server thread
            wakeupClientsCleaner();
        }
        else {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Unknown client state %d"), newState);
            Q_ASSERT(0);
            return;
        }
    }
}
/*****************************************************************************/
