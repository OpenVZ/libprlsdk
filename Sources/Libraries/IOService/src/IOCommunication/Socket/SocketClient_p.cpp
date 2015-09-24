/*
 * SocketClient_p.cpp
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


#include "Socket/SocketClient_p.h"
#include "IOClient.h"
#include "IOSSLInterface.h"
#include <Libraries/OpenSSL/OpenSSL.h>
#include <Libraries/Std/Etrace.h>

using namespace IOService;

/*****************************************************************************/
#define IOSRV_ETRACE(m)	ETRACE_LOG(sockHandle & 0xFF, ETRACE_CP_IOSERVICE,\
			(((quint16)m_senderType & 0xF) << 6) | ((m) & 0x3F))

#define CHECK_SRV_CTX(err_op)                                           \
    do {                                                                \
        Q_ASSERT(m_ctx == Cli_ServerContext);                           \
        if ( m_ctx != Cli_ServerContext ) err_op;                       \
    } while (0);

#define CHECK_CLI_CTX(err_op)                                           \
    do {                                                                \
        Q_ASSERT(m_ctx == Cli_ClientContext);                           \
        if ( m_ctx != Cli_ClientContext ) err_op;                       \
    } while (0);

#define CHECK_CLI_OR_SRV_CTX(err_op)                                           \
    do {                                                                       \
        Q_ASSERT(m_ctx == Cli_ServerContext || m_ctx == Cli_ClientContext);    \
        if ( m_ctx != Cli_ServerContext && m_ctx != Cli_ClientContext) err_op; \
    } while (0);

#define CHECK_CURRENT_THR(err_op)                                       \
    do {                                                                \
        Q_ASSERT(QThread::currentThread() == this);                     \
        if ( QThread::currentThread() != this )                         \
            err_op;                                                     \
    } while (0);

#define CHECK_DIRECT_MODE(err_op)                                       \
    do {                                                                \
        Q_ASSERT(m_senderConnMode ==                                    \
                 IOSender::DirectConnectionMode);                       \
        if ( m_senderConnMode !=                                        \
             IOSender::DirectConnectionMode )                           \
            err_op;                                                     \
    } while (0);

#define CALC_TIMEOUT(in_timeout, out_timeout, log_msg, err_op)           \
    if ( (in_timeout) != 0 ) {                                          \
        IOService::TimeMark endMark = 0;                                \
        IOService::timeMark(endMark);                                   \
        quint32 elapsed = IOService::msecsDiffTimeMark(startMark, endMark); \
        out_timeout = (in_timeout) - elapsed;                           \
                                                                        \
        if ( out_timeout <= 0 ) {                                       \
            WRITE_TRACE(DBG_FATAL, IO_LOG(log_msg));                    \
            err_op;                                                     \
        }                                                               \
    }

// Macros for easy heart beat timeout estimation
//   1. If elapsed >= heart beat timeout, we set heartBeatTimeout = -1:
//         we try to read all packages from socket without blocking.
//         Error will be returned if we have nothing to read.
//   2. If elapsed < heart beat timeout, we set heartBeatTimeout to
//         some value and block in read.
#define CALC_HEART_BEAT_TIMEOUT(arg_heartBeatTimeout, arg_heartBeatOff) \
    if ( heartBeatSupport && ! arg_heartBeatOff ) {                     \
        IOService::TimeMark nowMark = 0;                                \
        IOService::timeMark(nowMark);                                   \
        quint32 elapsed =                                               \
            IOService::msecsDiffTimeMark(lastHeartBeatMark, nowMark);   \
        if ( elapsed >= arg_heartBeatTimeout ) {                        \
            heartBeatTimeout = -1;                                      \
        }                                                               \
        else {                                                          \
            heartBeatTimeout = arg_heartBeatTimeout - elapsed;          \
        }                                                               \
    }                                                                   \
    else {                                                              \
        heartBeatTimeout = 0;                                           \
    }

/*****************************************************************************/

SocketClientPrivate::SocketClientPrivate (
    SmartPtr<IOJobManager> jobManager,
    const IORoutingTable& routingTable,
    IOSender::Type senderType,
    IOSender::ConnectionMode senderConnMode,
    const QString& remoteHost,
    quint32 remotePort,
    const IOSender::Handle& proxySessionHandle,

    SocketClientContext ctx,
    ReceiveSendClientListenerInterface* rcvSndListener,
    StateClientListenerInterface* stateListener,
    int sock,
    const Uuid& srv_currConnUuid,
    const IOCommunication::DetachedClient& srv_clientState,
    const IOCredentials& credentials,
    bool useUnixSockets,
    bool securePrxMng) :

    m_jobManager(jobManager),
    m_routingTable(routingTable),
    m_senderType(senderType),
    m_senderConnMode(senderConnMode),
    m_securityMode(IOSender::UnknownMode),
    m_remoteHost(remoteHost),
    m_remotePort(remotePort),
    m_localPort(0),
    m_proxySessionHandle(proxySessionHandle),

    m_ctx(ctx),
    m_rcvSndListener(rcvSndListener),
    m_stateListener(stateListener),
    m_state(IOSender::Disconnected),
    m_error(IOSender::UnknownError),
    m_threadState(ThreadIsStopped),
    m_connTimeout(0),
    m_sockHandle(sock),
    m_isReusableSocket(true),
    m_currConnectionUuid(srv_currConnUuid),
    m_peerSenderType(IOSender::UnknownType),
    m_writeThread(jobManager, senderType, ctx, m_stat, this),
#ifdef _WIN_ // Windows
    m_readEventHandle(WSA_INVALID_EVENT),
#else // Unix
    m_maxFDNum(IOService::getMaxFDNumber()),
    m_rdSet(IOService::allocFDSet(m_maxFDNum)),
    m_wrSet(IOService::allocFDSet(m_maxFDNum)),
#endif
    // SSL members
    m_ssl(0),
    m_sslBio(0),
    m_sslNetworkBio(0),
    m_sslSSLBio(0),
    m_sslHelper(0),
	m_localCredentials(credentials),
    m_securePrxMng(securePrxMng),
    // SSL read states
    m_remainToRead(0),
    m_pendingInSSL(false),
    m_newHeader(false),
    m_encryptedDataToRead(false),
    m_useUnixSockets(useUnixSockets),
    m_lastfd(-1),
	m_bLimitErrorLogging(false)
{
	m_rl.rate = 1;
	m_rl.last = -1;

    Q_ASSERT(m_rcvSndListener && m_stateListener);

    // #448853 Set stack size for read thread: 1024K should be enough
    // #457155      NOTE: 100k stack size occurrs dispatcher crash
    QThread::setStackSize( 1024 * 1024 );

    // Init srv ctx
    m_srvCtx.m_clientState = srv_clientState;
    m_srvCtx.m_isDetaching = false;
    m_srvCtx.m_detachingSpecificArg = -1;

    // Client context
    if ( m_ctx == Cli_ClientContext ) {
	// Only one value can be valid
	Q_ASSERT( m_srvCtx.m_clientState.isValid() ^ Uuid(m_currConnectionUuid).isNull() );
        // Direct connection
        if ( m_senderConnMode == IOSender::DirectConnectionMode ) {
            // Check proxy handle
            Q_ASSERT(m_proxySessionHandle == IOSender::InvalidHandle);

            // Init IO log with client state or handle
            if ( m_srvCtx.m_clientState.isValid() ) {
                INIT_IO_LOG(QString("IO client ctx [read thr] (cli_state %1, sender %2): ").
                            arg(QString().sprintf("%p", m_srvCtx.m_clientState->m_state)).
                            arg(m_senderType));
            }
            else {
                INIT_IO_LOG(QString("IO client ctx [read thr] (sender %1): ").
                            arg(m_senderType));
            }

        }
    }
    // Server context
    else if ( m_ctx == Cli_ServerContext ) {
        // Must be set for srv context
        Q_ASSERT( ! Uuid(m_currConnectionUuid).isNull() );

        // Direct connection
        if ( m_senderConnMode == IOSender::DirectConnectionMode ) {
            // Check proxy params
            Q_ASSERT(m_proxySessionHandle == IOSender::InvalidHandle);

            // Only one value can be valid
            Q_ASSERT( m_srvCtx.m_clientState.isValid() ^ (m_sockHandle != -1) );

            // Init IO log with client state or handle
            if ( m_srvCtx.m_clientState.isValid() ) {
                INIT_IO_LOG(QString("IO server ctx [read thr] (cli_state %1, sender %2): ").
                            arg(QString().sprintf("%p",
                                                  m_srvCtx.m_clientState->m_state)).
                            arg(m_senderType));
            }
            else {
                INIT_IO_LOG(QString("IO server ctx [read thr] (handle %1, sender %2): ").
                            arg(m_sockHandle).
                            arg(m_senderType));
            }
        }
        else
            Q_ASSERT(false);
    }
    else {
        Q_ASSERT(false);
    }

    // Drop reusable flag always for server context and
    // for client context with attached socket
    if ( m_ctx == Cli_ServerContext ||
         (m_ctx == Cli_ClientContext && m_sockHandle != -1) )
        m_isReusableSocket = false;

#ifndef _WIN_
    // Ignore SIGPIPE signal
    struct sigaction act;
    sigemptyset( &act.sa_mask );
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    sigaction( SIGPIPE, &act, NULL );

    // Invalidate pipes
    m_eventPipes[0] = -1;
    m_eventPipes[1] = -1;
#else
    ::memset( &m_overlappedRead, 0, sizeof(m_overlappedRead) );
#endif

    ::memset( &m_peerProtoVersion, 0, sizeof(m_peerProtoVersion) );
    ::memset( &m_proxyPeerProtoVersion, 0, sizeof(m_proxyPeerProtoVersion) );
    ::memset( &m_stat, 0, sizeof(m_stat) );
    if ( srv_clientState.isValid() ) {
        DetachedClientState* state = reinterpret_cast<DetachedClientState*>(
                                             srv_clientState->m_state);
        if ( 0 < state->header.readAheadSize ) {
            m_rawBuffer.append(state->data.readAhead.getImpl(),
                               state->header.readAheadSize);
        }
    }
}

SocketClientPrivate::~SocketClientPrivate ()
{
    stopClient();
}

IOSender::State SocketClientPrivate::state () const
{
    return m_state;
}

IOSender::Error SocketClientPrivate::error () const
{
    return m_error;
}

bool SocketClientPrivate::srv_getAttachingAdditionalPkg (
    SmartPtr<IOPackage>& additionalPkg  ) const
{
    CHECK_CLI_OR_SRV_CTX(return false);
    CHECK_CURRENT_THR(return false);

    QMutexLocker locker( &m_eventMutex );
    if ( m_threadState != ThreadIsStarted &&
         m_state != IOSender::Connected )
        return false;
    else if ( ! m_srvCtx.m_clientState.isValid() )
        return false;

    additionalPkg = m_srvCtx.m_attachingAdditionalPkg;
    return true;
}

QString SocketClientPrivate::currentConnectionUuid () const
{
    QMutexLocker locker( &m_eventMutex );
    if ( QThread::currentThread() != this &&
         m_state != IOSender::Connected )
        return QString();

    return m_currConnectionUuid;
}

QString SocketClientPrivate::peerHostName () const
{
    QMutexLocker locker( &m_eventMutex );

    if ( QThread::currentThread() != this &&
         m_state != IOSender::Connected )
        return QString();

    return m_peerHostName;
}

QString SocketClientPrivate::peerConnectionUuid () const
{
    QMutexLocker locker( &m_eventMutex );
    if ( QThread::currentThread() != this &&
         m_state != IOSender::Connected )
        return QString();

    return m_peerConnectionUuid;
}

IOSender::Type SocketClientPrivate::peerSenderType () const
{
    QMutexLocker locker( &m_eventMutex );
    if ( QThread::currentThread() != this &&
         m_state != IOSender::Connected )
        return IOSender::UnknownType;

    return m_peerSenderType;
}

IOSender::SecurityMode SocketClientPrivate::securityMode () const
{
	QMutexLocker locker( &m_eventMutex );
    if ( QThread::currentThread() != this &&
         m_state != IOSender::Connected )
        return IOSender::UnknownMode;

    return m_securityMode;
}

bool SocketClientPrivate::peerProtocolVersion (
    IOCommunication::ProtocolVersion& ver ) const
{
    // We do not check current thread or connected state,
    // because protocol version can be returned when client
    // has been stopped by error.

    QMutexLocker locker( &m_eventMutex );
    ver = m_peerProtoVersion;
    return true;
}

IOSender::State SocketClientPrivate::waitForConnection (
    quint32 msecs ) const
{
    QMutexLocker locker( &m_eventMutex );
    if ( QThread::currentThread() == this )
        return m_state;
    else if ( m_threadState != ThreadIsStarting &&
              m_threadState != ThreadIsStarted )
        return m_state;
    else if ( m_state == IOSender::Connected )
        return m_state;

    // Wait
    if ( msecs == 0 )
        m_ioStateWait.wait( &m_eventMutex );
    else
        m_ioStateWait.wait( &m_eventMutex, msecs );
    return m_state;
}

IOCommunication::DetachedClient SocketClientPrivate::srv_doDetach ( int sock )
{
    CHECK_CLI_OR_SRV_CTX(IOCommunication::DetachedClient());
    CHECK_CURRENT_THR(IOCommunication::DetachedClient());

    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    // Lock
    QMutexLocker locker( &m_eventMutex );

    if ( m_state != IOSender::Connected ) {
        LOG_MESSAGE(DBG_FATAL, IO_LOG("Can't do detach for "
                                      "not started client"));
        return IOCommunication::DetachedClient();
    }

    if ( ! m_srvCtx.m_isDetaching ) {
        LOG_MESSAGE(DBG_FATAL, IO_LOG("We are not in detaching state! "
                                      "Close connecion"));
        return IOCommunication::DetachedClient();
    }
    m_srvCtx.m_isDetaching = false;

	unsigned char *sslSession;
	unsigned int sslSessionLen;

	sslSession = sslGetSession(&sslSessionLen);

	if (!sslSession) {
		WRITE_TRACE(DBG_FATAL, "Can't get SSL_SESSION at detaching");
		return IOCommunication::DetachedClient();
	}

	// Wrap session into smart pointer
	SmartPtr<char> sslWrapped = SmartPtr<char>((char *)sslSession, free);

    // Routing table buffer
    quint32 rtSize = 0;
    SmartPtr<char> rtBuff = m_peerRoutingTable.toBuffer( rtSize );
    if ( ! rtBuff.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
        return IOCommunication::DetachedClient();
    }

    DetachedClientState* state = new(std::nothrow) DetachedClientState;
    if ( state == 0 ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
        return IOCommunication::DetachedClient();
    }
    IOCommunication::DetachedClient detachedCli(
        new(std::nothrow) IOCommunication::DetachedClientPrivate(state) );
    if ( ! detachedCli.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
        return IOCommunication::DetachedClient();
    }

#ifdef _WIN_
    int res = ::WSADuplicateSocket( sock,
                                    m_srvCtx.m_detachingSpecificArg,
                                    &state->header.clientSocket );
    if ( res != 0 ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Error: duplicate socket failed: "
                           "(native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize));
        return IOCommunication::DetachedClient();
    }
#else

    int newSock = ::dup( sock );
    if ( newSock < 0 ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Error: duplicate socket failed: "
                           "(native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize));
        return IOCommunication::DetachedClient();
    }

    state->header.clientSocket = newSock;
#endif

    state->header.readAheadSize = m_rawBuffer.size();
    if ( 0 < state->header.readAheadSize ) {
        state->data.readAhead = SmartPtr<char>( new(std::nothrow)
                                     char[state->header.readAheadSize],
                                     SmartPtrPolicy::ArrayStorage );
        if ( ! state->data.readAhead.isValid() ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
            return IOCommunication::DetachedClient();
        }
        memcpy(state->data.readAhead.getImpl(), m_rawBuffer.constData(),
               state->header.readAheadSize);
    }

    state->header.clientProtocolVersion = m_peerProtoVersion;
    state->header.clientHandshakeHeader.senderType = m_peerSenderType;
    Uuid::dump( m_peerConnectionUuid,
                state->header.clientHandshakeHeader.connectionUuid );
    state->header.routingTableBuffSize = rtSize;
    state->header.sslSessionSize = sslSessionLen;
    state->data.routingTableBuff = rtBuff;
    state->data.sslSession = sslWrapped;
    state->data.additionalPkg = m_srvCtx.m_detachingAdditionalPkg;

    return detachedCli;
}

bool SocketClientPrivate::srv_detachClient (
    int specificArg,
    const SmartPtr<IOPackage>& additionalPkg,
    bool detachBothSides )
{
    CHECK_SRV_CTX(return false);

    // Lock
    QMutexLocker locker( &m_eventMutex );

    bool sendRes = m_writeThread.sendDetachRequestAndPauseWriting(detachBothSides);
    if ( ! sendRes ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Sending detach request and pausing are failed."));
        return false;
    }

    m_srvCtx.m_isDetaching = true;
    m_srvCtx.m_detachingSpecificArg = specificArg;
    m_srvCtx.m_detachingAdditionalPkg = additionalPkg;

    return true;
}

bool SocketClientPrivate::startClient ( quint32 msecsTimeout )
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
    if ( m_threadState == ThreadIsStarted )
        return true;
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
        // Set timeout
        m_connTimeout = msecsTimeout;
        // Drop error
        m_error = IOSender::UnknownError;

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
        return false;
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
        return false;
    }

    // Great!
    return true;
}

void SocketClientPrivate::stopClient ()
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
    // Finalize reading!
    //
    __finalizeThread();

    m_threadStateWait.wait( &m_eventMutex );
    Q_ASSERT(m_threadState == ThreadIsStopped);

    // Wait for Qt thread
    QThread::wait();
    return;
}

void SocketClientPrivate::__finalizeThread ()
{
    // Mark as stopping
    m_threadState = ThreadIsStopping;

    // Signal events
#ifdef _WIN_
    ::WSASetEvent( m_readEventHandle );
#else
    char b = 0;
    if ( ::write( m_eventPipes[1], &b, 1 ) < 0 )
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Write failed while thread finalization!"));
#endif
}

IOSendJob::Handle SocketClientPrivate::sendPackage (
    const SmartPtr<IOPackage>& p )
{
    LOG_MESSAGE(DBG_DEBUG, IO_LOG("Send package : packageType=%d"),
                p->header.type);

    // Non urgent send
    SmartPtr<IOSendJob> job = m_writeThread.sendPackage( p, false );
    if ( ! job.isValid() ) {
        return IOSendJob::Handle();
    }

    return IOSendJob::Handle( job );
}

IOSendJob::Handle SocketClientPrivate::sendDetachedClient (
    const IOCommunication::DetachedClient& detachedClient,
    const SmartPtr<IOPackage>& request )
{
    if ( ! detachedClient.isValid() ||
         ! detachedClient->attach() ) {
        LOG_MESSAGE(DBG_WARNING, IO_LOG("Client is already attached!"));
        return IOSendJob::Handle();
    }

    SmartPtr<IOPackage> pkg =
        IOCommunication::createAttachClientPackage( detachedClient, request );
    if ( ! pkg.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't create attach client package!"));
        return IOSendJob::Handle();
    }

    return sendPackage( pkg );
}

bool SocketClientPrivate::read ( int sock, char* inBuf, quint32 size,
                                 quint32& wasRead, IOReadMode readMode,
                                 int msecsTimeout, int* unixfd,
                                 bool* timeoutExpired )
{
    Q_ASSERT(inBuf);
    Q_ASSERT(size != 0);

    if ( timeoutExpired )
        *timeoutExpired = false;

    enum {
        ErrBuffSize = 1<<8,
        BUFFER_LIMIT = 1<<18
    };
    char errBuff[ ErrBuffSize ];

    // Init to zero
    wasRead = 0;
#ifndef _WIN_
    IOService::TimeMark startMark = 0;

    // Get time mark if timeout is used
    if ( msecsTimeout > 0 )
        IOService::timeMark(startMark);

    int lastfd = -1;
    std::swap(lastfd, m_lastfd);
    do {
        quint32 s = m_rawBuffer.size();
        if ( size <= s ) {
            break;
        }
        // Create params for select
        int pipe = m_eventPipes[0];
        int maxfd = (sock > pipe ? sock : pipe) + 1;
        if ( maxfd > m_maxFDNum ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Out of resources! Descriptor "
                                          "'%d' exceeds max possible '%d'."),
                        maxfd, m_maxFDNum);
            return false;
        }
        // We can't use FD_ZERO here, because
        // of not original fd_set size
        ::memset(m_rdSet.getImpl(), 0, FDNUM2SZ(m_maxFDNum));
        FD_SET(sock, m_rdSet.getImpl());

        // Add pipe event
        if ( readMode != IOGracefulShutdownRead )
            FD_SET(pipe, m_rdSet.getImpl());

        timeval timeout = {0, 0};
        timeval* timeo = 0;

        // Calc timeout
        if ( msecsTimeout > 0 ) {
            quint32 msecsToWait = 0;
            CALC_TIMEOUT(msecsTimeout, msecsToWait,
                         "Wait for read failed: timeout expired",
                         return false);

            // Set timeout
            timeout.tv_sec = msecsToWait / 1000;
            timeout.tv_usec = (msecsToWait % 1000) * 1000;
            timeo = &timeout;
        }
        // Tests read queue state and returns immediately
        else if ( msecsTimeout < 0 ) {
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
            timeo = &timeout;
        }

        // Do select
        int res = ::select(maxfd, m_rdSet.getImpl(), 0, 0, timeo);
        if ( res == 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Wait for read failed: timeout expired!"));
            if ( timeoutExpired )
                *timeoutExpired = true;
            return false;
        }
        else if ( res < 0 && errno == EINTR ) {
            LOG_MESSAGE(DBG_INFO, IO_LOG("Select has been interrupted"));
            continue;
        }
        else if ( res < 0 ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Select failed (native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize));
            return false;
        }

        // Check stop in progress
        if ( readMode != IOGracefulShutdownRead &&
             FD_ISSET(pipe, m_rdSet.getImpl()) ) {
            WRITE_TRACE(DBG_INFO, IO_LOG("Stop in progress for read thread"));
            return false;
        }

        // Recv msg
        struct iovec iov[1];
        struct msghdr msg;
        struct {
            struct cmsghdr head;
            int fd;
        } cmsg;

        ::memset( &msg, 0, sizeof(msg) );
        ::memset( &cmsg, 0, sizeof(cmsg) );

        m_rawBuffer.reserve(BUFFER_LIMIT);
        iov->iov_base = m_rawBuffer.data() + s;
        iov->iov_len = m_rawBuffer.capacity() - s;
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_name = 0;
        msg.msg_namelen = 0;
        msg.msg_control = (caddr_t)&cmsg;
        msg.msg_controllen = sizeof(cmsg);

        ssize_t readBytes = ::recvmsg( sock, &msg, 0 );

        if ( readBytes == 0 ) {
            WRITE_TRACE(DBG_INFO, IO_LOG("Socket graceful shutdown detected. "
                                         "No worries, everything goes fine."));
            return false;
        }
        else if ( readBytes < 0 && errno == EINTR ) {
            LOG_MESSAGE(DBG_INFO, IO_LOG("Read has been interrupted"));
            continue;
        }
        else if ( readBytes < 0 && (errno == EAGAIN || errno == EWOULDBLOCK) ) {
            LOG_MESSAGE(DBG_INFO, IO_LOG("Read would block"));
            continue;
        }
        else if ( readBytes < 0 ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Read from socket failed "
                               "(native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize));

            return false;
        }
	m_rawBuffer.resize(m_rawBuffer.size() + readBytes);
        if ( msg.msg_controllen == sizeof(cmsg) ) {
            lastfd = *(int *)CMSG_DATA(&cmsg.head);
        }
    } while ( readMode == IOContinuousRead );

    wasRead = qMin((quint32)m_rawBuffer.size(), size);
    ::memcpy(inBuf, m_rawBuffer.constData(), wasRead);
    m_rawBuffer.remove(0, wasRead);
    m_lastfd = lastfd;
    // We want to receive descriptor?
    if ( unixfd != 0 && *unixfd < 0 ) {
        *unixfd = m_lastfd;
    }
#else
    Q_UNUSED(unixfd);

    DWORD flags = 0;
    DWORD remainBytes = size;

    WSABUF buff;
    char* buf = inBuf;

    // Timeout
    IOService::TimeMark startMark = 0;
    DWORD dwTimeout = WSA_INFINITE;

    // Init time mark
    if ( msecsTimeout > 0 )
        IOService::timeMark(startMark);

    do {
        DWORD readBytes = 0;

        ::memset( &buff, 0, sizeof(buff) );
        buff.len = remainBytes;
        buff.buf = buf;

        ::memset( &m_overlappedRead, 0, sizeof(m_overlappedRead) );
        m_overlappedRead.hEvent = m_readEventHandle;

        int res = ::WSARecv( sock, &buff,  1, &readBytes,
                             &flags,  &m_overlappedRead,  NULL );

        if ( res == SOCKET_ERROR ) {
            int err = ::WSAGetLastError();

            if ( err != WSA_IO_PENDING ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Read from socket failed "
                                              "(native error: %s)"),
                            native_strerror(err, errBuff, ErrBuffSize));

                return false;
            }
        }

        // Check if stop in progress.
        // The thing is, that ::WSARecv always resets event, so,
        // if reading was stopped (i.e. event signaled) before ::WSARecv call,
        // we will infinitely wait in ::WSAWaitForMultipleEvents.
        // So we should check this variable manually.
        if ( readMode != IOGracefulShutdownRead &&
             m_threadState == ThreadIsStopping ) {
            LOG_MESSAGE(DBG_INFO, IO_LOG("Client stop in progress"));
            return false;
        }

        // Calc timeout
        if ( msecsTimeout > 0 ) {
            quint32 msecsToWait = 0;
            CALC_TIMEOUT(msecsTimeout, msecsToWait,
                         "Wait for read failed: timeout expired!",
                         return false);
            dwTimeout = msecsToWait;
        }
        // Tests read queue state and returns immediately
        else if ( msecsTimeout < 0 ) {
            dwTimeout = 0;
        }

        // Check, that recv has been completed immediately
        if ( res == 0 )
            goto recv_is_done;

        // Wait for some time
        DWORD dwWait = ::WSAWaitForMultipleEvents( 1,
                                                   &m_readEventHandle,
                                                   TRUE,
                                                   dwTimeout,
                                                   FALSE ); // not alertable

        // Completed
        if ( dwWait == WSA_WAIT_EVENT_0 ) {
            // Write is completed or stop in progress
            if ( readMode != IOGracefulShutdownRead &&
                 m_threadState == ThreadIsStopping ) {
                LOG_MESSAGE(DBG_INFO, IO_LOG("Client stop in progress"));
                return false;
            }
        }
        // Still pending
        else {
            if ( dwWait == WSA_WAIT_TIMEOUT ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Wait for read failed: timeout expired!"));
                if ( timeoutExpired )
                    *timeoutExpired = true;
            }
            else
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Wait for read failed (native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));

            return false;
        }

        if ( ! ::WSAGetOverlappedResult(sock, &m_overlappedRead,
                                        &readBytes, FALSE, &flags) ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Overlapped result: read from socket failed "
                               "(native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize));

            return false;
        }

    recv_is_done:
        // Detect graceful shutdown for stream-oriented socket
        if ( readBytes == 0 ) {
            WRITE_TRACE(DBG_INFO, IO_LOG("Socket graceful shutdown detected. "
                                         "No worries, everything goes fine."));
            return false;
        }

        remainBytes -= readBytes;
        buf += readBytes;
        wasRead += readBytes;

    }  while ( remainBytes > 0 && readMode == IOContinuousRead );
#endif

    // Append read bytes to statistics
    AtomicAdd64(&m_stat.receivedBytes, wasRead);

    return true;
}

bool SocketClientPrivate::cli_sendHandshake (
    int sock,
    const Uuid& currConnUuid,
    quint32 msecsTimeout )
{
    CHECK_CLI_CTX(return false);

    // Send client protocol version
    {
        IOSendJob::Result res = writeData( sock, &IOProtocolVersion, sizeof(IOProtocolVersion), msecsTimeout );
        if ( res != IOSendJob::Success ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: protocol version send "
                               "has been failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
    }

    // Send handshake structure
    {
        IOCommunication::HandshakeHeader handshakeStruct;
        handshakeStruct.senderType = m_senderType;
        currConnUuid.dump( handshakeStruct.connectionUuid );
        IOSendJob::Result res = writeData( sock, &handshakeStruct, sizeof(handshakeStruct), msecsTimeout );
        if ( res != IOSendJob::Success ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: handshake struct send "
                               "has been failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
    }

    // Send routing table
    {
        quint32 rtBuffSz = 0;
        SmartPtr<char> rtBuff = m_routingTable.toBuffer( rtBuffSz );
        if ( ! rtBuff.isValid() ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
        IOSendJob::Result res = writeData( sock, rtBuff.getImpl(), rtBuffSz, msecsTimeout );
        if ( res != IOSendJob::Success ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake error: routing table send "
                               "has been failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
    }
    return true;
}

bool SocketClientPrivate::cli_parseHandshake (
    int sock,
    quint32 msecsTimeout )
{
    bool res;
    IOCommunication::ProtocolVersion protocolVer;
    IORoutingTable routingTable;
    QString servConnectionUuid;
    IOSender::Type senderType = IOSender::UnknownType;

    // Read server protocol version
    {
        res = readData( sock, reinterpret_cast<char*>(&protocolVer),
                        sizeof(protocolVer), msecsTimeout );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake error: protocol version "
                               "read failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
        // Protocol version magic bytes check
        if ( 0 != ::memcmp(protocolVer.whoWeAre,
                           IOProtocolVersion.whoWeAre,
                           sizeof(IOProtocolVersion.whoWeAre)) ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("IO protocol magic bytes differs!"));
            m_error = IOSender::HandshakeError;
            return false;
        }

        // For client context we save protocol version just after we
        // have received it to have possibility to check this version
        // later.
        {
            QMutexLocker locker( &m_eventMutex );
            m_peerProtoVersion = protocolVer;
        }

        // Protocol version check
        if ( protocolVer.majorNumber !=
             IOProtocolVersion.majorNumber ) {

            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("IO protocol major version number differs!"));
            m_error = IOSender::ProtocolVersionError;
            return false;
        }
        if ( protocolVer.minorNumber !=
             IOProtocolVersion.minorNumber ) {

            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("IO protocol minor version number differs!"));
        }
    }

    // Parse handshake header
    {
        IOCommunication::HandshakeHeader header;
        res = readData( sock, reinterpret_cast<char*>(&header), sizeof(header), msecsTimeout );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: handshake header read "
                               "failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }

        // Parse connection uuid
        Uuid connUuid = Uuid::toUuid( header.connectionUuid );
        if ( connUuid.isNull() ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: handshake header parse "
                               "failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
        servConnectionUuid = connUuid.toString();

        // Get sender type
        senderType = header.senderType;
        if ( senderType == IOSender::UnknownType ||
             senderType >= IOSender::EndTypeBound ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: sender type is wrong!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
    }

    // Parse routing table
    {
        IORoutingTable::Header header;
        res = readData( sock, reinterpret_cast<char*>(&header), sizeof(header), msecsTimeout );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table header read "
                               "failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
        quint32 sizeToAllocate = IORoutingTable::validateHeader( header );
        if ( sizeToAllocate == 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table header parse "
                               "failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
        Q_ASSERT(sizeToAllocate >= sizeof(header));

        SmartPtr<char> buff( new(std::nothrow) char[sizeToAllocate],
                             SmartPtrPolicy::ArrayStorage );
        if ( ! buff.isValid() ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
        IORoutingTable::Header* destHeader =
            reinterpret_cast<IORoutingTable::Header*>(buff.getImpl());
        ::memcpy( destHeader, &header, sizeof(header) );
        if ( header.routesNumber > 0 ) {
            Q_ASSERT(sizeToAllocate - sizeof(header) > 0);
            res = readData( sock, reinterpret_cast<char*>(destHeader + 1),
                            sizeToAllocate - sizeof(header), msecsTimeout );
            if ( ! res ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Handshake error: routes read failed!"));
                m_error = IOSender::HandshakeError;
                return false;
            }
        }

        routingTable = IORoutingTable::fromBuffer( buff, sizeToAllocate, res );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table parse failed!"));
            m_error = IOSender::HandshakeError;
            return false;
        }
        if ( routingTable.isNull() ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table was not "
                               "accepted by server!"));
            m_error = IOSender::RoutingTableAcceptError;
            return false;
        }
    }

    Q_ASSERT( ! servConnectionUuid.isEmpty() );
    Q_ASSERT( ! routingTable.isNull() );
    Q_ASSERT( senderType != IOSender::UnknownType );

    // Lock
    QMutexLocker locker( &m_eventMutex );

    m_peerConnectionUuid = servConnectionUuid;
    m_peerSenderType = senderType;
    m_peerRoutingTable = routingTable;

    // Unlock
    locker.unlock();

    return true;
}

bool SocketClientPrivate::srv_parseHandshake (
    int sock,
    quint32 msecsTimeout )
{
    CHECK_SRV_CTX(return false);

    bool res = false;
    bool accepted = false;
    IOCommunication::ProtocolVersion protocolVer;
    IORoutingTable acceptedTable;
    IORoutingTable clientTable;
    QString clientConnectionUuid;
    IOSender::Type senderType = IOSender::UnknownType;

    // Read client protocol version
    {
        res = readData( sock, reinterpret_cast<char*>(&protocolVer),
                        sizeof(protocolVer), msecsTimeout );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: protocol version read "
                               "failed!"));
            return false;
        }

        // Protocol version magic bytes check
        if ( 0 != ::memcmp(protocolVer.whoWeAre,
                           IOProtocolVersion.whoWeAre,
                           sizeof(IOProtocolVersion.whoWeAre)) ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("IO protocol magic bytes differs!"));
            return false;
        }
        // Protocol version check
        if ( protocolVer.majorNumber != IOProtocolVersion.majorNumber ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("IO protocol major version number differs!"));
            return false;
        }
        if ( protocolVer.minorNumber != IOProtocolVersion.minorNumber ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("IO protocol minor version number differs!"));
        }
    }

    // Parse handshake header
    {
        IOCommunication::HandshakeHeader header;
        res = readData( sock, reinterpret_cast<char*>(&header),
                        sizeof(header), msecsTimeout );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: handshake header read "
                               "failed!"));
            return false;
        }

        // Parse connection uuid
        Uuid connUuid = Uuid::toUuid( header.connectionUuid );
        if ( connUuid.isNull() ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: handshake header parse "
                               "failed!"));
            return false;
        }
        clientConnectionUuid = connUuid.toString();

        // Get sender type
        senderType = header.senderType;
        if ( senderType == IOSender::UnknownType ||
             senderType >= IOSender::EndTypeBound ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: sender type is wrong!"));
            return false;
        }
    }

    // Parse routing table
    {
        IORoutingTable::Header header;
        res = readData( sock, reinterpret_cast<char*>(&header),
                        sizeof(header), msecsTimeout );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table header "
                               "read failed!"));
            return false;
        }
        quint32 sizeToAllocate = IORoutingTable::validateHeader( header );
        if ( sizeToAllocate == 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table header "
                               "parse failed!"));
            return false;
        }
        Q_ASSERT(sizeToAllocate >= sizeof(header));

        SmartPtr<char> buff( new(std::nothrow) char[sizeToAllocate],
                             SmartPtrPolicy::ArrayStorage );
        if ( ! buff.isValid() ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
            return false;
        }
        IORoutingTable::Header* destHeader =
            reinterpret_cast<IORoutingTable::Header*>(buff.getImpl());
        ::memcpy( destHeader, &header, sizeof(header) );
        if ( header.routesNumber > 0 ) {
            Q_ASSERT(sizeToAllocate - sizeof(header) > 0);
            res = readData( sock, reinterpret_cast<char*>(destHeader + 1),
                            sizeToAllocate - sizeof(header), msecsTimeout );
            if ( ! res ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Handshake error: routes read failed!"));
                return false;
            }
        }

        clientTable = IORoutingTable::fromBuffer( buff, sizeToAllocate, res );
        if ( ! res ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table parse failed!"));
            return false;
        }
        if ( clientTable.isNull() ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: routing table is wrong!"));
            return false;
        }
        accepted = m_routingTable.accept( clientTable, acceptedTable );
    }

    if ( ! accepted ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Handshake error: can't accept client's table!"));
        return false;
    }

    Q_ASSERT( ! clientConnectionUuid.isEmpty() );
    Q_ASSERT( ! acceptedTable.isNull() );
    Q_ASSERT( senderType != IOSender::UnknownType );

    // Lock
    QMutexLocker locker( &m_eventMutex );

    m_peerProtoVersion = protocolVer;
    m_peerConnectionUuid = clientConnectionUuid;
    m_peerSenderType = senderType;
    m_peerRoutingTable = acceptedTable;

    // Unlock
    locker.unlock();

    return true;
}

bool SocketClientPrivate::srv_sendHandshake (
    int sock,
    const Uuid& currConnUuid,
    quint32 msecsTimeout )
{
    CHECK_SRV_CTX(return false);

    // Send server protocol version
    {
        IOSendJob::Result res = writeData( sock, &IOProtocolVersion, sizeof(IOProtocolVersion), msecsTimeout );
        if ( res != IOSendJob::Success ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: protocol version send "
                               "has been failed!"));
            return false;
        }
    }

    // Send handshake structure
    {
        IOCommunication::HandshakeHeader handshakeStruct;
        handshakeStruct.senderType = m_senderType;
        currConnUuid.dump( handshakeStruct.connectionUuid );
        IOSendJob::Result res = writeData( sock, &handshakeStruct, sizeof(handshakeStruct), msecsTimeout );
        if ( res != IOSendJob::Success ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: handshake struct send "
                               "has been failed!"));
            return false;
        }
    }

    return true;
}

bool SocketClientPrivate::srv_sendRoutingTable (
    int sock,
    quint32 msecsTimeout )
{
    CHECK_SRV_CTX(return false);

    IORoutingTable clientTable;

    // Send accepted routing table
    {
        quint32 rtBuffSz = 0;
        SmartPtr<char> rtBuff = m_peerRoutingTable.toBuffer( rtBuffSz );
        if ( ! rtBuff.isValid() ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
            return false;
        }

        IOSendJob::Result res = writeData( sock, rtBuff.getImpl(), rtBuffSz, msecsTimeout );
        if ( res != IOSendJob::Success ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake error: routing table send "
                               "has been failed!"));
            return false;
        }
    }

    return true;
}

bool SocketClientPrivate::connectToHost ( int& outSockHandle, quint32 connTimeout )
{
    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ] = {0};

    bool connected = false;
    int sockHandle = -1;
    addrinfo* addrInfo = 0;
#ifndef _WIN_
    struct sockaddr_un *pAddr = 0;
#endif
    addrinfo* connAddrInfo = 0;
    QList<addrinfo*> addrInfoList;
    QList<ErrDesc> errList;
    QString host = m_remoteHost;
    QString port = QString("%1").arg(m_remotePort);
    bool isAnyAddr = (host == IOService::AnyAddr);
    bool isLoopbackAddr = (host == IOService::LoopbackAddr);
    // Get time mark if timeout is used
    IOService::TimeMark startMark = 0;
    if ( connTimeout != 0 )
        IOService::timeMark(startMark);

#ifndef _WIN_
    if (!m_useUnixSockets)
    {
#endif
    addrinfo hints;
    ::memset( &hints, 0, sizeof (hints) );
    hints.ai_flags = AI_ADDRCONFIG;
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
        if ( res == EAI_NONAME )
            m_error = IOSender::HostnameResolveError;
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't do getaddrinfo for remote host name "
                           "'%s', (native error: %s)"),
                    qPrintable(host),
                    errP);

        goto cleanup;
    }
#ifndef _WIN_
    }
    else
    {
        addrInfo = (addrinfo *)::malloc(sizeof(addrinfo));
        if ( !addrInfo )
        {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Failed to allocate memory"));
            goto cleanup;
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
            goto cleanup;
        }
        ::memset(pAddr, 0, sockaddr_size );
        pAddr->sun_family = AF_UNIX;

        QByteArray path = host.toUtf8();
        if( path.size() > PATH_MAX - 1 )
        {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Too large unix socket path."));
            goto cleanup;
        }
        ::strncpy(pAddr->sun_path, path.constData(), PATH_MAX - 1);
    }
#endif

    // Order addrinfo with IPv6 priority
    addrInfoList = orderAddrInfo( addrInfo, OP_PreferIPv6 );

    // Get suitable addr info in loop while not connected
    for ( qint32 i = 0; i < addrInfoList.size(); ++i ) {
        addrinfo* ai = addrInfoList[i];

#ifndef _WIN_
        if (!m_useUnixSockets)
#endif
            sockHandle = ::socket( ai->ai_family,
                               ai->ai_socktype,
                               ai->ai_protocol );
#ifndef _WIN_
        else
            sockHandle = ::socket( AF_LOCAL, SOCK_STREAM, 0 );
#endif

        if ( sockHandle < 0 ) {
            // Create error
            ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                ai->ai_protocol, native_error(),
                                __LINE__ };
            errList.append( errDesc );

            // Try next address
            continue;
        }

#ifndef _WIN_
        // Set non-blocking mode
        int flags = 0;
        if ( (flags = ::fcntl(sockHandle, F_GETFL, 0)) < 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Can't get flags for socket "
                               "(native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize) );
            goto cleanup;
        }
        flags |= O_NONBLOCK;

        if ( ::fcntl(sockHandle, F_SETFL, flags) < 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Can't set flags for socket "
                               "(native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize) );
            goto cleanup;
        }

        // Connect with msecs timeout
        int res = 0;
#ifndef _WIN_
        if (!m_useUnixSockets)
#endif
            res = ::connect(sockHandle, ai->ai_addr, ai->ai_addrlen);
#ifndef _WIN_
        else
            res = ::connect(sockHandle, (struct sockaddr *)pAddr, SUN_LEN(pAddr));
#endif
        if ( res < 0 ) {
            if ( errno != EINPROGRESS ) {
                // Create error
                ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                    ai->ai_protocol, errno,
                                    __LINE__ };
                errList.append( errDesc );

                // Close socket before next try
                ::close(sockHandle);
                sockHandle = -1;

                // Try next address
                continue;
            }

            do {
                // Create params for select
                int pipe = m_eventPipes[0];
                int maxfd = (sockHandle > pipe ? sockHandle : pipe) + 1;
                if ( maxfd > m_maxFDNum ) {
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Out of resources! Descriptor "
                                       "'%d' exceeds max possible '%d'."),
                                maxfd, m_maxFDNum);
                    goto cleanup;
                }

                // We can't use FD_ZERO here, because
                // of not original fd_set size
                ::memset(m_wrSet.getImpl(), 0, FDNUM2SZ(m_maxFDNum));
                ::memset(m_rdSet.getImpl(), 0, FDNUM2SZ(m_maxFDNum));
                FD_SET(sockHandle, m_wrSet.getImpl());
                FD_SET(pipe, m_rdSet.getImpl());
                timeval timeout = {0, 0};
                timeval* timeo = 0;

                // Calc timeout
                if ( connTimeout != 0 ) {
                    qint32 msecsToWait = 0;
                    CALC_TIMEOUT(connTimeout, msecsToWait,
                                 "Connection timeout expired!",
                                 goto cleanup);

                    // Set timeout
                    timeout.tv_sec = msecsToWait / 1000;
                    timeout.tv_usec = (msecsToWait % 1000) * 1000;
                    timeo = &timeout;
                }

                // Do select
                int res = ::select( maxfd, m_rdSet.getImpl(),
                                    m_wrSet.getImpl(), 0, timeo );
                if ( res == 0 ) {
                    // Timeout
                    WRITE_TRACE(DBG_FATAL, IO_LOG("Connection timeout "
                                                  "(%d msecs) expired"),
                                connTimeout);
                    m_error = IOSender::ConnectionTimeout;
                    goto cleanup;
                }
                else if ( res < 0 && errno == EINTR ) {
                    LOG_MESSAGE(DBG_INFO,
                                IO_LOG("Conn has been interrupted"));
                    continue;
                }
                else if ( res < 0 ) {
                    WRITE_TRACE(DBG_FATAL, IO_LOG("Can't connect to "
                                                  "socket "
                                                  "(native error: %s)"),
                                native_strerror(errBuff, ErrBuffSize));
                    goto cleanup;
                }

                // Check stop in progress
                if ( FD_ISSET(pipe, m_rdSet.getImpl()) ) {
                    WRITE_TRACE(DBG_INFO, IO_LOG("Stop in progress for "
                                                 "connection"));
                    goto cleanup;
                }

                // Socket selected for write, may be connected?
                int err = 0;
                socklen_t errSz = sizeof(err);
                // Check possible error
                if ( ::getsockopt(sockHandle, SOL_SOCKET, SO_ERROR,
                                  &err, &errSz) < 0 ) {
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Can't get socket options "
                                       "for socket (native error: %s)"),
                                native_strerror(errBuff, ErrBuffSize));
                    goto cleanup;
                }

                // Check the value returned...
                if ( err != 0 ) {
                    // Create error
                    ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                        ai->ai_protocol, err,
                                        __LINE__ };
                    errList.append( errDesc );

                    // Close socket before next try
                    ::close(sockHandle);
                    sockHandle = -1;

                    // Try next address
                    break;
                }

                // Successfully connected
                goto successfully_connected;

            } while (1);
        }
        else {
            // Successfully connected
            goto successfully_connected;
        }
#else
        // Associate read event
        if ( ::WSAEventSelect(sockHandle, m_readEventHandle,
                              FD_CONNECT) ==  SOCKET_ERROR ){
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Can't associate event with FD_CONNECT"
                               " (native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize) );
            goto cleanup;
        }

        int res = ::connect( sockHandle, ai->ai_addr, (int)ai->ai_addrlen );

        int err = ::WSAGetLastError();

        if ( res != SOCKET_ERROR ) {
            // Successfully connected
            goto successfully_connected;
        }
        else if ( res == SOCKET_ERROR && err == WSAEWOULDBLOCK ) {

            // Create timeout
            qint32 msecsToWait = 0;
            CALC_TIMEOUT(connTimeout, msecsToWait,
                         "Connection timeout expired!",
                         goto cleanup);

            // Wait for connection
            res = ::WSAWaitForMultipleEvents(
                                             1,
                                             &m_readEventHandle,
                                             TRUE,
                                             (msecsToWait == 0 ?
                                              WSA_INFINITE : msecsToWait),
                                             FALSE ); // not alertable

            // Client was stopped from another thread or wait failed
            if ( m_threadState == ThreadIsStopping ||
                 res == WSA_WAIT_FAILED ) {

                if ( m_threadState != ThreadIsStopping )
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Can't connect to socket, "
                                       "wait failed (native error: %s)"),
                                native_strerror(errBuff, ErrBuffSize) );
                goto cleanup;
            }

            // Ok. Get result
            WSANETWORKEVENTS wsaEventsRes;
            res = ::WSAEnumNetworkEvents( sockHandle,
                                          m_readEventHandle,
                                          &wsaEventsRes );
            if ( res == SOCKET_ERROR ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Can't connect to socket, "
                                   "WSAEnumNetworkEvents failed "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize) );
                goto cleanup;
            }

            err = wsaEventsRes.iErrorCode[ FD_CONNECT_BIT ];
            if ( err != 0 ) {
                // Create error
                ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                    ai->ai_protocol, err,
                                    __LINE__ };
                errList.append( errDesc );

                // Close socket before next try
                ::closesocket(sockHandle);
				ETRACE_LOG(sockHandle & 0xFF, ETRACE_CP_IOSERVICE,
					(((quint16)m_senderType & 0xF) << 6) |
					(ETRACE_IOS_EVENT_CLI_CLOSE & 0x3F));
                sockHandle = -1;

                // Try next address
                continue;
            }

            // Successfully connected
            goto successfully_connected;
        }
        else {
            // Create error
            ErrDesc errDesc = { ai->ai_family, ai->ai_socktype,
                                ai->ai_protocol, err,
                                __LINE__ };
            errList.append( errDesc );

            // Close socket before next try
            ::closesocket(sockHandle);
			ETRACE_LOG(sockHandle & 0xFF, ETRACE_CP_IOSERVICE,
				(((quint16)m_senderType & 0xF) << 6) |
				(ETRACE_IOS_EVENT_CLI_CLOSE & 0x3F));
            sockHandle = -1;

            // Try next address
            continue;
        }
#endif
        continue;

    successfully_connected:
        // Mark as connected
        connected = true;
        connAddrInfo = ai;
        break;
    }

    // Done.

    // Log out all errors
    if ( ! errList.isEmpty() ) {

		bool bPrintLog = true;

		// Success log header if we are connected
        if ( connected ) {
            Q_ASSERT(connAddrInfo);
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Successfully connected to socket "
                               "ai_family = %d, ai_socktype = %d, "
                               "ai_protocol = %d"),
                        connAddrInfo->ai_family,
                        connAddrInfo->ai_socktype,
                        connAddrInfo->ai_protocol);
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("But there were some troubles:"));
            WRITE_TRACE(DBG_FATAL, IO_LOG("WARNINGS BEGIN"));
        }
        // Error header
        else {

			if (m_bLimitErrorLogging)
				bPrintLog = LogCheckModifyRate(&m_rl);

			if (bPrintLog)
				WRITE_TRACE(DBG_FATAL, IO_LOG("Can't connect to socket:"));
			if (bPrintLog)
				WRITE_TRACE(DBG_FATAL, IO_LOG("ERRORS BEGIN"));
        }

        foreach ( ErrDesc errDesc, errList )
		  if (bPrintLog)
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

        if ( connected )
            WRITE_TRACE(DBG_FATAL, IO_LOG("WARNINGS END"));
        else if (bPrintLog)
            WRITE_TRACE(DBG_FATAL, IO_LOG("ERRORS END"));
    }

 cleanup:
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

    // Close socket if connection failed
    if ( ! connected && sockHandle != -1 ) {
#ifdef _WIN_
        ::closesocket(sockHandle);
#else
        ::close(sockHandle);
#endif
		ETRACE_LOG(sockHandle & 0xFF, ETRACE_CP_IOSERVICE,
			(((quint16)m_senderType & 0xF) << 6) |
			(ETRACE_IOS_EVENT_CLI_CLOSE & 0x3F));
        sockHandle = -1;
    }

    outSockHandle = sockHandle;
    return connected;
}

void SocketClientPrivate::run ()
{
    // Backtrace will show us what context is used
    if ( m_ctx == Cli_ServerContext )
        doServerCtxJob();
    else if ( m_ctx == Cli_ClientContext )
        doClientCtxJob();
}

// Backtrace will show us what context is used
void SocketClientPrivate::doServerCtxJob ()
{
    doJob();
}

// Backtrace will show us what context is used
void SocketClientPrivate::doClientCtxJob ()
{
    doJob();
}

/*
 * Get session, deinit and init. We should not use any locks here,
 * because write thread is in pause
 */
bool SocketClientPrivate::reinitSslSession(void)
{
	unsigned int size;
	unsigned char *asn;
	bool res = false;

	// Get session
	asn = sslGetSession(&size);

	if (!asn) {
		WRITE_TRACE(DBG_FATAL, "SSL get session failed!");
		return false;
	}

	// Deinit
	if (!sslDeinit()) {
		WRITE_TRACE(DBG_FATAL, IO_LOG("SSL deinit failed!"));
		goto exit;
	}

	// Init
	if (!sslInit(m_localCredentials)) {
		WRITE_TRACE(DBG_FATAL, IO_LOG("SSL init failed!"));
		goto exit;
	}

	// Set session
	if (!cli_sslSetSession(asn, size)) {
		WRITE_TRACE(DBG_FATAL, "SSL set session failed!");
		goto exit;
	}

	res = true;

exit:
	free(asn);
	return res;
}

void SocketClientPrivate::doJob ()
{
    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ] = {0};
    QString errStr;

    int sockHandle = -1;
    qint32 msecsToWait = 0;

    // Peer description
    quint16 peerPortNumber = 0;
    QString peerHostName;

    // Local connection description
    quint16 localPortNumber = 0;
    QString localHostName;

    // Get time mark if timeout is used
    IOService::TimeMark startMark = 0;
    if ( m_connTimeout != 0 )
        IOService::timeMark(startMark);

    // Heart beat flags
    IOService::TimeMark lastHeartBeatMark = 0;
    bool heartBeatSupport = false;


    // XXX: Turn off heart beat!
    bool heartBeatOff = true;

    // Zero some flags
    bool cli_doSSLRehandshake = false;
    bool srv_detaching = false;
    bool cli_detach = false;

    // Zero SSL read state
    m_remainToRead = 0;
    m_pendingInSSL = false;
    m_newHeader = false;
    m_encryptedDataToRead = false;

    // Zero statistics
    AtomicWrite64(&m_stat.sentPackages, 0);
    AtomicWrite64(&m_stat.receivedPackages, 0);
    AtomicWrite64(&m_stat.sentBytes, 0);
    AtomicWrite64(&m_stat.receivedBytes, 0);

    // Check if this client ctx can be reused:
    //   sock handle must be droped after first start, but
    //   reusable flag will be set to false, so we can determine
    //   that this is a double start.
    if ( m_ctx != Cli_ServerContext && m_sockHandle == -1 && ! m_isReusableSocket ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't start client with non-reusable socket twice!"));
        goto cleanup_and_disconnect;
    }
    sockHandle = m_sockHandle;
    m_sockHandle = -1;

#ifdef _WIN_
    // Init WSA
    if ( ! WSAInitHelper::InitWSA() ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Winsock 2.2 does not supported (native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize) );
        goto cleanup_and_disconnect;
    }

    m_readEventHandle = ::WSACreateEvent();
    if ( m_readEventHandle == WSA_INVALID_EVENT ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't create read event (native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize) );
        goto cleanup_and_disconnect;
    }
#else
    // Check allocations
    if ( ! m_rdSet.isValid() || ! m_wrSet.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
        goto cleanup_and_disconnect;
    }

    // Create pipes
    if ( ::pipe(m_eventPipes) < 0 ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't create event pipes (native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize) );
        goto cleanup_and_disconnect;
    }
    // Set non-blocking mode for pipes
    for ( int i = 0; i < 2; ++i ) {
        if ( ::fcntl(m_eventPipes[i], F_SETFL, O_NONBLOCK) < 0 ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Can't set O_NONBLOCK for event pipe #%d "
                               "(native error: %s)"),
                        i, native_strerror(errBuff, ErrBuffSize) );
            goto cleanup_and_disconnect;
        }
    }
#endif

    // Mark as started
    m_eventMutex.lock();

    // For server ctx uuid must be already created.
    if ( m_ctx == Cli_ServerContext )
        Q_ASSERT(! Uuid(m_currConnectionUuid).isNull());
    // Use newly generated connection uuid for other ctx.
    else
        m_currConnectionUuid = Uuid::createUuid().toString();

    // We should mark as started just after event pipes/handle creation
    // and before looooong operations to have a possibility to safely
    // stop this thread from any place.
    // For client we must mark as stopped before connection, because its
    // really looong operation.
    m_threadState = ThreadIsStarted;
    m_threadStateWait.wakeOne();

    // Unlock
    m_eventMutex.unlock();

    // Init ssl
	if ( ! sslInit(m_localCredentials) ) {
		WRITE_TRACE(DBG_FATAL, IO_LOG("SSL inition failed!"));
		goto cleanup_and_disconnect;
	}

    // Server context
    if ( m_ctx == Cli_ServerContext ) {
        // Server context, direct mode
        if ( m_senderConnMode == IOSender::DirectConnectionMode ) {
            // Client attaching
            if ( m_srvCtx.m_clientState.isValid() ) {
            	if (!getDetachedSocket(sockHandle, errBuff, ErrBuffSize))
            		goto cleanup_and_disconnect;
            }
            else
                Q_ASSERT(sockHandle != -1);

	    IOSRV_ETRACE(ETRACE_IOS_EVENT_SRV_CONNECT);
        }
        // Server context, proxy mode
        else {
            bool res = false;

            // Trying to connect to proxy
            res = connectToHost( sockHandle, m_connTimeout );
            if ( ! res ) {
                // All logging made from previous call
                goto cleanup_and_disconnect;
            }

	    IOSRV_ETRACE(ETRACE_IOS_EVENT_SRV_CONNECT_PROXY);
        }
    }
    // Client context
    else if ( m_ctx == Cli_ClientContext ) {
        // Client context, direct mode
        if ( m_senderConnMode == IOSender::DirectConnectionMode ) {
		// Client attaching
		if ( m_srvCtx.m_clientState.isValid() ) {
			if (!getDetachedSocket(sockHandle, errBuff, ErrBuffSize))
				goto cleanup_and_disconnect;
		} else {
			// Try to connect if not already connected
			if ( sockHandle == -1 ) {
				bool res = false;
				// Trying to connect
				res = connectToHost( sockHandle, m_connTimeout );
				if ( ! res ) {
					// All logging made from previous call
					goto cleanup_and_disconnect;
				}
			}
			// Sock handle is already inited
			else {
				// Nothing to do
			}

		IOSRV_ETRACE(ETRACE_IOS_EVENT_CLI_CONNECT);
		}
        }
        // Client context, proxy mode
        else {
            bool res = false;

            // Trying to connect to proxy
            res = connectToHost( sockHandle, m_connTimeout );
            if ( ! res ) {
                // All logging made from previous call
                goto cleanup_and_disconnect;
            }

	    IOSRV_ETRACE(ETRACE_IOS_EVENT_CLI_CONNECT_PROXY);
        }
    }
    else
        Q_ASSERT(false);

    // Setup default socket options
    if ( ! IOService::setSocketOptions(sockHandle) ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Setting of default socket options failed "
                           "(native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize));
        goto cleanup_and_disconnect;
    }

    // Init peer host name
    if ( ! IOService::getPeerInfo(sockHandle, peerHostName,
                                  peerPortNumber, errStr) )
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Getting of peer host name failed, error: %s"),
                    qPrintable(errStr));

    // Init local host name
    if ( ! IOService::getHostInfo(sockHandle, localHostName,
                                  localPortNumber, errStr) )
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Getting of local host name failed, error: %s"),
                    qPrintable(errStr));

    // Calc timeout
    CALC_TIMEOUT(m_connTimeout, msecsToWait,
                 "Connection timeout expired!",
                 goto cleanup_and_disconnect);

    //
    // Do client side handshaking
    // before write thread start
    // to avoid any locks
    //

    // Server context
    if ( m_ctx == Cli_ServerContext ) {
        bool handshaked = false;

        // Server context, client was created from detached state
        if ( m_srvCtx.m_clientState.isValid() ) {
            // Only direct mode for attaching state
            CHECK_DIRECT_MODE(goto cleanup_and_disconnect);

            // TODO
            // XXX
            // XXX Actually we should do full rehandshake, because attaching
            // XXX could be done to the server with incompatible version.
            // XXX   ---------------------------
            // XXX   |         |               |   |
            // XXX CLI_1 <---> | SRV_1 <---> SRV_2 |
            // XXX  v2         |   v3         v1   |
            // XXX
            // XXX i.e. after rehandshaking SRV_2 must send its handshake
            // XXX structure to CLI_1, because SRV_1 and SRV_2 version
            // XXX differs.

            // Set our handshaked state
            handshaked = srv_setHandshakeFromState();
            if ( ! handshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake from state is wrong! "
                                   "Connection will be closed!"));
                goto cleanup_and_disconnect;
            }

		IOSRV_ETRACE(ETRACE_IOS_EVENT_SRV_SET_HS);

            // Add SSL session and rehandshake
            handshaked = srv_addSSLSessionFromStateAndRehandshake(
                                           sockHandle,
                                           m_srvCtx.m_attachingAdditionalPkg,
                                           msecsToWait );
            if ( ! handshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("SSL session is wrong!"));
                goto cleanup_and_disconnect;
            }

	    IOSRV_ETRACE(ETRACE_IOS_EVENT_SRV_SET_HS_SSL);
        }
        // Server context, socket handle is passed as argument
        else {

	    //------------------------------------------------------------
	    // Send
	    //------------------------------------------------------------

            // send our handshake
            handshaked = srv_sendHandshake( sockHandle,
                                                    m_currConnectionUuid,
                                                    msecsToWait );
            if ( ! handshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake is wrong! "
                                   "Connection will be closed!"));
                goto cleanup_and_disconnect;
            }

	    //------------------------------------------------------------
	    // Receive and parse
	    //------------------------------------------------------------

            // Parse handshake
            handshaked = srv_parseHandshake( sockHandle, msecsToWait );
            if ( ! handshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake is wrong! "
                                   "Connection will be closed!"));
                goto cleanup_and_disconnect;
            }

	    // ------------------------------------------------------------
            // finally send routing
            // ------------------------------------------------------------
            handshaked = srv_sendRoutingTable( sockHandle, msecsToWait );
            if ( ! handshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake is wrong! "
                                   "Connection will be closed!"));
                goto cleanup_and_disconnect;
            }

	    IOSRV_ETRACE(ETRACE_IOS_EVENT_SRV_SEND_HS);

            // Calc timeout
            CALC_TIMEOUT(m_connTimeout, msecsToWait,
                         "Connection timeout expired!",
                         goto cleanup_and_disconnect);

            // SSL handshake
            handshaked = sslHandshake( sockHandle, msecsToWait );
            if ( ! handshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("SSL handshake failed!"));
                goto cleanup_and_disconnect;
            }

		IOSRV_ETRACE(ETRACE_IOS_EVENT_SRV_SEND_HS_SSL);
        }
    }
    // Client context
    else if ( m_ctx == Cli_ClientContext ) {
        bool handshaked = false;
        if ( m_srvCtx.m_clientState.isValid() ) {
            // Set our handshaked state
            handshaked = srv_setHandshakeFromState();
            if ( ! handshaked ) {
        	    WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake from state is wrong! "
        			    "Connection will be closed!"));
        	    goto cleanup_and_disconnect;
            }
            // Add SSL session and rehandshake
            handshaked = srv_addSSLSessionFromStateAndRehandshake(
                                           sockHandle,
                                           m_srvCtx.m_attachingAdditionalPkg,
                                           msecsToWait );
            if ( ! handshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("SSL session is wrong!"));
                goto cleanup_and_disconnect;
            }
        }
        else {
	//------------------------------------------------------------
	// Send
	//------------------------------------------------------------
        // Send our handshake
        handshaked = cli_sendHandshake( sockHandle,
                                                m_currConnectionUuid,
                                                msecsToWait );
        if ( ! handshaked ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake failed!"));
            goto cleanup_and_disconnect;
        }

	//------------------------------------------------------------
	// Receive and Parse
	//------------------------------------------------------------

        handshaked = cli_parseHandshake( sockHandle, msecsToWait );
        if ( ! handshaked ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake failed!"));
            goto cleanup_and_disconnect;
        }

	IOSRV_ETRACE(ETRACE_IOS_EVENT_CLI_SEND_HS);

        // Calc timeout
        CALC_TIMEOUT(m_connTimeout, msecsToWait,
                     "Connection timeout expired!",
                     goto cleanup_and_disconnect);

        // SSL handshake
        handshaked = sslHandshake( sockHandle, msecsToWait );
        if ( ! handshaked ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("SSL handshake failed!"));
            goto cleanup_and_disconnect;
        }

	IOSRV_ETRACE(ETRACE_IOS_EVENT_CLI_SEND_HS_SSL);
        }
    }
    else
        Q_ASSERT(false);


    // Do some stuff which is used only for non proxy mng context
        //
        // Validate client after handshake for server context
        // Is often used for server context to check connection uuid.
        //
        if ( m_ctx == Cli_ServerContext ) {
            bool validated = m_stateListener->srv_validateAfterHandshake(
                                                   this, m_peerConnectionUuid );
            if ( ! validated ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Validation of client '%s' failed!"),
                            qPrintable(m_peerConnectionUuid));
                goto cleanup_and_disconnect;
            }
        }

        // Start write thread
        {
            bool wrStarted = m_writeThread.startWriteThread( sockHandle,
                                                             m_currConnectionUuid,
                                                             m_peerConnectionUuid,
                                                             m_peerProtoVersion,
                                                             m_peerRoutingTable
#ifndef _WIN_ // Unix
                                                             , m_eventPipes
#endif
                                                             );

            if ( ! wrStarted ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Can't start write thread"));
                goto cleanup_and_disconnect;
            }
        }

    //
    // Check heart beat support.
    //
    if (
          IOPROTOCOL_HEART_BEAT_SUPPORT(m_peerProtoVersion) &&
          IOPROTOCOL_HEART_BEAT_SUPPORT(IOProtocolVersion)
		) {
        IOService::timeMark(lastHeartBeatMark);
        heartBeatSupport = true;
    }
    else
        WRITE_TRACE(DBG_FATAL, IO_LOG("Peer does not have heart beat support!"));

    //
    // Great. We are absolutely connected now.
    // We can't handle errors from here, because after connected state emition
    // we should emit disconnected state, not goto error handler.
    //
    {
        // Lock
        QMutexLocker locker( &m_eventMutex );

        // Set peer host name
        m_peerHostName = peerHostName;

        // Set local connection description
        m_localHost = localHostName;
		m_localPort = localPortNumber;

        // Set connected state
        m_state = IOSender::Connected;
        m_ioStateWait.wakeAll();

        // Unlock
        locker.unlock();

        // Emit connected state
        {
            CALLBACK_MARK;
            m_stateListener->onStateChanged( this,
                                             m_peerConnectionUuid,
                                             IOSender::Disconnected,
                                             IOSender::Connected );
            WARN_IF_CALLBACK_TOOK_MUCH_TIME;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    // Main client/server context job loop
    //
    while ( 1 ) {
        // Do rehandshaking
        if ( cli_doSSLRehandshake ) {
            cli_doSSLRehandshake = false;

            // Send detach response and pause writing
            bool res = m_writeThread.sendDetachResponseAndPauseWriting();
            if ( ! res ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Sending detach response and pausing "
                                   "are failed. Stop connection."));
                goto cleanup_and_disconnect;
            }

            if (cli_detach) {
                IOCommunication::DetachedClient detached = srv_doDetach(sockHandle);
                if (!detached.isValid())
                    WRITE_TRACE(DBG_FATAL,IO_LOG("Can't create detached state! Close connecion"));
                else {
                    CALLBACK_MARK;
                    m_rcvSndListener->srv_onDetachClient(this, m_peerConnectionUuid, detached);
                    WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                }
                goto cleanup_and_disconnect;
            }

			// All logging was done inside
			if (!reinitSslSession())
					goto cleanup_and_disconnect;

            // TODO
            // XXX
            // XXX Actually we should do full rehandshake, because attaching
            // XXX could be done to the server with incompatible version.
            // XXX   ---------------------------
            // XXX   |         |               |   |
            // XXX CLI_1 <---> | SRV_1 <---> SRV_2 |
            // XXX  v2         |   v3         v1   |
            // XXX
            // XXX i.e. after rehandshaking SRV_2 must send its handshake
            // XXX structure to CLI_1, because SRV_1 and SRV_2 version
            // XXX differs.

            // SSL rehandshake
            bool rehandshaked = sslHandshake( sockHandle, m_connTimeout );

            if ( ! rehandshaked ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("SSL rehandshake failed!"));
                // Do not continue writing
                // Anyway writing will be stopped
                goto cleanup_and_disconnect;
            }

			ETRACE_LOG(sockHandle & 0xFF, ETRACE_CP_IOSERVICE,
				(((quint16)m_senderType & 0xF) << 6) |
				(ETRACE_IOS_EVENT_SRV_SEND_REHS_SSL & 0x3F));

            if ( ! SSL_session_reused(m_ssl) ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("SSL session was not reused!"));
                // Do not continue writing
                // Anyway writing will be stopped
                goto cleanup_and_disconnect;
            }

            // Continue writing
            m_writeThread.continueWriting();
        }

        bool success = false;

        // Buffer for header
        char pkgHeaderBuff[ sizeof(IOPackage::PODHeader) ];

        // Unix fd (works only on Unix)
        int unixfd = -1;

        // Heart beat timeout
        int heartBeatTimeout = 0;

        // Timeout expiration flag
        bool timeoutExpired = false;

        // Calc heart beat
        CALC_HEART_BEAT_TIMEOUT(IOCommunication::IOHeartBeatReceiveTimeout,
                                heartBeatOff);

        // Read header package
        success = sslReadAndWait( sockHandle,
                                  pkgHeaderBuff, sizeof(IOPackage::PODHeader),
                                  heartBeatTimeout, &unixfd, &timeoutExpired );

        // Check if error
        if ( ! success ) {
            if ( timeoutExpired ) {
                m_error = IOSender::HeartBeatTimeoutError;
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Error: no heart beat was received for "
                                   "%d msecs. Connection problems?"),
                            IOCommunication::IOHeartBeatReceiveTimeout);
            }
            goto cleanup_and_disconnect;
        }
		quint64 recv_sz = sizeof(IOPackage::PODHeader);

        // Create empty package with received header
        SmartPtr<IOPackage> p;
        {
            // Safe cast
            IOPackage::PODHeader* header =
                reinterpret_cast<IOPackage::PODHeader*>(pkgHeaderBuff);

            // Check header CRC16
            if ( header->crc16 != IOPackage::headerChecksumCRC16(*header) ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Package header CRC16 is wrong! "
                                   "Connection will be closed!"));
                goto cleanup_and_disconnect;
            }

#ifndef _WIN_
            if ( header->type == IOCommunicationMngPackage::AttachClient ) {
                if ( unixfd == -1 ) {
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Attach client package was recieved, "
                                       "but unix file descriptor is invalid!"));
                }
            }
#endif

            p = IOPackage::createInstance( header->type,
                                           header->buffersNumber );
            if ( ! p.isValid() ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
                goto cleanup_and_disconnect;
            }
            ::memcpy( &p->header, header, sizeof(IOPackage::PODHeader) );
        }

        if ( p->header.buffersNumber > 0 ) {

            IOPackage::PODData* pkgData = IODATAMEMBER(p);

            // Calc heart beat
            CALC_HEART_BEAT_TIMEOUT(IOCommunication::IOHeartBeatReceiveTimeout,
                                    heartBeatOff);

            // Read data for buffers
            success = sslReadAndWait( sockHandle,
                                      reinterpret_cast<char*>(pkgData),
                                      IODATASIZE(p), heartBeatTimeout,
                                      0, &timeoutExpired );

             if ( ! success ) {
                 if ( timeoutExpired ) {
                     m_error = IOSender::HeartBeatTimeoutError;
                     WRITE_TRACE(DBG_FATAL,
                                 IO_LOG("Error: no heart beat was received for "
                                        "%d msecs. Connection problems?"),
                                 IOCommunication::IOHeartBeatReceiveTimeout);
                 }
                 goto cleanup_and_disconnect;
             }

			 recv_sz += IODATASIZE(p);

             // Create buffers
             for ( quint32 i = 0; i < p->header.buffersNumber; ++i ) {
                 LOG_MESSAGE(DBG_INFO,
                             IO_LOG("Read buffer #%d with size=%d"), i,
                             pkgData[i].bufferSize);

                 if ( pkgData[i].bufferSize == 0 ) {
                     LOG_MESSAGE(DBG_INFO, IO_LOG("Buffer #%d size is zero!"));
                     continue;
                 }

                 SmartPtr<char> buff = IOPackage::allocPODBuffer( *(&pkgData[i]) );
                 if ( ! buff.isValid() ) {
                     WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
                     goto cleanup_and_disconnect;
                 }

                 // Calc heart beat
                 CALC_HEART_BEAT_TIMEOUT(IOCommunication::IOHeartBeatReceiveTimeout,
                                         heartBeatOff);

                 // Read buffers from socket
                 success = sslReadAndWait( sockHandle, buff.getImpl(),
                                           pkgData[i].bufferSize,
                                           heartBeatTimeout, 0,
                                           &timeoutExpired );

                if ( ! success ) {
                    if ( timeoutExpired ) {
                        m_error = IOSender::HeartBeatTimeoutError;
                        WRITE_TRACE(DBG_FATAL,
                                    IO_LOG("Error: no heart beat was received for "
                                           "%d msecs. Connection problems?"),
                                    IOCommunication::IOHeartBeatReceiveTimeout);
                    }
                    goto cleanup_and_disconnect;
                }

                p->buffers[i] = buff;

				recv_sz += pkgData[i].bufferSize;
            }
        }

        LOG_MESSAGE(DBG_DEBUG,
                    IO_LOG("Package recieved from server : packageType=%d"),
                    p->header.type);

        // Increment statistics value
        AtomicInc64(&m_stat.receivedPackages);

		// Log to Etrace
		ETRACE_LOG(sockHandle & 0xFF, ETRACE_CP_IOSERVICE,
			((quint64)p->header.type << 32) |
			(((quint32)recv_sz  & 0x3FFFF) << 14) |
			(((quint16)p->header.buffersNumber & 0xF) << 10) |
			(((quint16)m_senderType & 0xF) << 6) |
			(ETRACE_IOS_EVENT_RECV_PKG_SSL & 0x3F));

        // Check that is not a management pkg
        if ( p->header.type != IOCommunicationMngPackage::HeartBeat &&
             ( (m_ctx == Cli_ClientContext &&
		p->header.type != IOCommunicationMngPackage::DetachClientRequest &&
		p->header.type != IOCommunicationMngPackage::AttachClient &&
        	p->header.type != IOCommunicationMngPackage::DetachBothSidesRequest) ||
               (m_ctx == Cli_ServerContext && p->header.type !=
                IOCommunicationMngPackage::DetachClientResponse) ) ) {

            // Package callback
            {
                CALLBACK_MARK;
                m_rcvSndListener->onPackageReceived( this,
                                                     m_peerConnectionUuid, p );
                WARN_IF_CALLBACK_TOOK_MUCH_TIME;
            }
        }

        //
        // Check some internal types
        //

        if ( p->header.type == IOCommunicationMngPackage::HeartBeat ) {
            LOG_MESSAGE(DBG_DEBUG, IO_LOG("Heart beat has been received!"));
            if ( ! heartBeatSupport )
                WRITE_TRACE(DBG_WARNING,
                            IO_LOG("Warning: heart beat package has been "
                                   "received, but peer does not support it!"));
            else
                IOService::timeMark(lastHeartBeatMark);
        }
        else if ( p->header.type == IOCommunicationMngPackage::AttachClient ) {
            IOCommunication::DetachedClient detachedClient =
                IOCommunication::parseAttachClientPackage( p, unixfd );
            if ( ! detachedClient.isValid() ) {
                LOG_MESSAGE(DBG_WARNING,
                            IO_LOG("Can't parse attach client package"));
            }
            else {
                DetachedClientState* state =
                    reinterpret_cast<DetachedClientState*>(
                                                     detachedClient->m_state);

                // Detach client callback
                {
                    CALLBACK_MARK;
                    m_rcvSndListener->onDetachedClientReceived(
                                                     this,
                                                     m_peerConnectionUuid,
                                                     state->data.additionalPkg,
                                                     detachedClient );
                    WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                }
            }
        }
        else if ( p->header.type ==
                  IOCommunicationMngPackage::DetachClientRequest ) {
            if ( m_ctx != Cli_ClientContext )
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Error: received client management package! "
                                   "Running in not client context!"));
            else
                cli_doSSLRehandshake = true;
        }
        else if ( p->header.type ==
                  IOCommunicationMngPackage::DetachBothSidesRequest ) {
            if ( m_ctx != Cli_ClientContext )
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Error: received client management package! "
                                   "Running in not client context!"));
            else {
                cli_doSSLRehandshake = true;
                cli_detach = true;
                m_srvCtx.m_isDetaching = true;
            }
        }
        else if ( p->header.type ==
                  IOCommunicationMngPackage::DetachClientResponse ) {
            if ( m_ctx != Cli_ServerContext )
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Received server management package! "
                                   "Running in not server context!"));
            else {
                IOCommunication::DetachedClient detachedClient =
                    srv_doDetach( sockHandle );

                if ( ! detachedClient.isValid() )
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Can't create detached state! "
                                       "Close connecion"));
                else {
                    // Detach client callback
                    CALLBACK_MARK;
                    m_rcvSndListener->srv_onDetachClient( this,
                                                          m_peerConnectionUuid,
                                                          detachedClient );
                    WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                }

                // Client must be stopped
                srv_detaching = true;
            }
        }

        // Check if response and not rehandshake
        if ( ! cli_doSSLRehandshake && p->isResponsePackage() ) {
            SmartPtr<IOSendJob> job = m_jobManager->findJobByResponsePackage(
                                                     m_writeThread.getJobPool(),
                                                     p );

            // Find valid job
            if ( job.isValid() ) {
                // Response package callback
                CALLBACK_MARK;
                m_rcvSndListener->onResponsePackageReceived(
                                                     this,
                                                     m_peerConnectionUuid,
                                                     job, p );
                WARN_IF_CALLBACK_TOOK_MUCH_TIME;

                job->wakeResponseWaitings( IOSendJob::Success,
                                           m_peerConnectionUuid,
                                           p );
            }
        }

        // Stop client if detaching
        if ( m_ctx == Cli_ServerContext && srv_detaching )
            goto cleanup_and_disconnect;
    }

cleanup_and_disconnect:
    //
    // Finalize client thread
    //

    //
    // We are disconnecting now. Save state and set it to disconneted.
    //
    IOSender::State oldState = m_state;
    {
        // Lock
        QMutexLocker locker( &m_eventMutex );

        // Set disconneted state
        m_state = IOSender::Disconnected;
        m_ioStateWait.wakeAll();

        // Mark as stopping
        m_threadState = ThreadIsStopping;

        // Unlock
        locker.unlock();

        // Emit disconnected state
        {
            CALLBACK_MARK;
            m_stateListener->onStateChanged( this,
                                             m_peerConnectionUuid,
                                             oldState,
                                             IOSender::Disconnected );
            WARN_IF_CALLBACK_TOOK_MUCH_TIME;
        }
    }

    // If we were successfully connected:
    //   stop writing and wake up all active jobs,
    //   which are waiting for some response
    //
    if ( oldState == IOSender::Connected ) {

        // Stop write thread
        IOSendJob::Result stopReason = m_writeThread.stopWriteThread();

        // Write thread is stopped, now we are alone
        // Mark all busy (active) jobs with fail result
        // of response wait operation
        QList<IOJobManager::Job*> sendJobs =
            m_jobManager->getBusySendJobs( m_writeThread.getJobPool() );
        foreach ( IOJobManager::Job* jobPtr, sendJobs ) {
            SmartPtr<IOSendJob>& job = jobPtr->sendJob;
            if ( ! job->responseWaitingsWereWaken() ) {
                job->wakeResponseWaitings( stopReason,
                                           IOSender::Handle(),
                                           SmartPtr<IOPackage>() );
            }
            // Mark as not active just after all access to this job.
            m_jobManager->putActiveJob( m_writeThread.getJobPool(), jobPtr );
        }
    }

#ifdef _WIN_ // Windows

    // Close handle
    if ( sockHandle != -1 ) {
        // Do graceful shutdown if not in detaching state
        // and we were successfully connected
        if ( ! srv_detaching && !cli_detach && oldState == IOSender::Connected )
            __gracefulShutdown(sockHandle,
                               IOCommunication::IOGracefulShutdownTimeout);
        ::closesocket(sockHandle);
		ETRACE_LOG(sockHandle & 0xFF, ETRACE_CP_IOSERVICE,
			(((quint16)m_senderType & 0xF) << 6) |
			(ETRACE_IOS_EVENT_CLI_CLOSE & 0x3F));
        sockHandle = -1;
    }

#else // Unix

    // Close handle
    if ( sockHandle != -1 ) {
        // Do graceful shutdown if not in detaching state
        // and we were successfully connected
        if ( ! srv_detaching && !cli_detach && oldState == IOSender::Connected )
            __gracefulShutdown(sockHandle,
                               IOCommunication::IOGracefulShutdownTimeout);
        ::close(sockHandle);
        sockHandle = -1;
        m_lastfd = -1;
        m_rawBuffer.clear();
    }

#endif

    // Remove session for server ctx
    if ( m_ctx == Cli_ServerContext && m_ssl )
        SSL_CTX_remove_session( m_sslHelper->GetServerSSLContext(),
                                SSL_get_session(m_ssl) );

    // SSL shutdown
    if ( m_sslNetworkBio )
        (void)BIO_reset( m_sslNetworkBio );
    if ( m_sslSSLBio )
        (void)BIO_reset( m_sslSSLBio );
    if ( m_ssl )
        SSL_shutdown( m_ssl );

    // Deinit SSL
    sslDeinit();

    // Atomic stop
    m_eventMutex.lock();

#ifdef _WIN_ // Windows

    // Close event
    if ( m_readEventHandle != WSA_INVALID_EVENT ) {
        ::WSACloseEvent(m_readEventHandle);
        m_readEventHandle = WSA_INVALID_EVENT;
    }

    // Deinit WSA
    WSAInitHelper::DeinitWSA();

#else // Unix

    // Close pipes
    if ( m_eventPipes[0] != -1 ) {
        ::close( m_eventPipes[0] );
        m_eventPipes[0] = -1;
    }
    if ( m_eventPipes[1] != -1 ) {
        ::close( m_eventPipes[1] );
        m_eventPipes[1] = -1;
    }

#endif

    // Drop peer members
    // NOTE: do not zero protocol version members.
    //       these structures can be returned after
    //       client has been disconnected.
    m_peerConnectionUuid = QString();
    m_currConnectionUuid = QString();
    m_peerSenderType = IOSender::UnknownType;
    m_peerHostName = QString();
    m_peerRoutingTable = IORoutingTable();

    // Mark as stopped
    m_threadState = ThreadIsStopped;
    m_threadStateWait.wakeOne();

    // Unlock
    m_eventMutex.unlock();

    return;
}

void SocketClientPrivate::__gracefulShutdown ( int sock,
                                               quint32 timeout )
{
    (void)timeout;

    // Shutdown write
    ::shutdown( sock, SHUT_WR );
}

bool SocketClientPrivate::sslInit (const IOCredentials& cert)
{
	if ( ! IOService::initSSLLibrary() )
		return false;

	if (! m_sslHelper){
		m_sslHelper = new(std::nothrow)  SSLHelper();

		if (! m_sslHelper)
		{
			WRITE_TRACE(DBG_FATAL,
			            IO_LOG("Failed to create ssl helper object"));
			return false;
		}

		if (cert.isValid())
			m_securityMode = IOSender::TrustedChannelConnection;
		else
			m_securityMode = IOSender::SelfSignedConnection;

		if ( ! m_sslHelper->InitSSLContext(cert) )
			return false;
	}

    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    if ( m_ctx == Cli_ServerContext )
        m_ssl = SSL_new( m_sslHelper->GetServerSSLContext() );
    else
        m_ssl = SSL_new( m_sslHelper->GetClientSSLContext() );

    if ( ! m_ssl ) {
        ERR_error_string_n( ERR_get_error(), errBuff, ErrBuffSize );
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't create SSL object (SSL error: %s)"),
                    errBuff);
        return false;
    }

    if ( ! BIO_new_bio_pair(&m_sslBio, SSLHelper::SSLBuffSize,
                            &m_sslNetworkBio, SSLHelper::SSLBuffSize) ) {
        ERR_error_string_n( ERR_get_error(), errBuff, ErrBuffSize );
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't create SSL bio pair (SSL error: %s)"),
                    errBuff);
        return false;
    }

    m_sslSSLBio = BIO_new(BIO_f_ssl());
    if ( ! m_sslSSLBio ) {
        ERR_error_string_n( ERR_get_error(), errBuff, ErrBuffSize );
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't create SSL bio (SSL error: %s)"), errBuff);
        return false;
    }

    SSL_set_bio(m_ssl, m_sslBio, m_sslBio);
    BIO_set_ssl(m_sslSSLBio, m_ssl, BIO_NOCLOSE);

    return m_writeThread.initSSL( m_ssl, m_sslSSLBio, m_sslNetworkBio );
}

bool SocketClientPrivate::sslDeinit ()
{
    bool res = m_writeThread.deinitSSL();
    if ( ! res )
        return false;

    if ( m_sslBio ) {
        BIO_free( m_sslBio );
        m_sslBio = 0;
    }
    if ( m_sslNetworkBio ) {
        BIO_free( m_sslNetworkBio );
        m_sslNetworkBio = 0;
    }
    if ( m_sslSSLBio ) {
        BIO_free( m_sslSSLBio );
        m_sslSSLBio = 0;
    }

    if ( m_ssl ) {
        SSL_free( m_ssl );
        m_ssl = 0;
    }

	if ( m_sslHelper)
	{
		m_sslHelper->DeinitSSLContext();
		delete m_sslHelper;
		m_sslHelper = 0;
	}

    // Drop all SSL errors, which can affect on next SSL initialization
    // and next operations: BIO_write, for example.
    while ( ERR_get_error() != 0 ) ;

    IOService::deinitSSLLibrary();

    return true;
}

unsigned char* SocketClientPrivate::sslGetSession(unsigned int *size)
{
	unsigned char *asn;

	if (!OpenSSL::SessionToASN(m_ssl, &asn, size)) {
		WRITE_TRACE(DBG_FATAL, "Error serializing SSL CTX to ASN: %s",
					OpenSSL::GetErrorStr().c_str());
		return 0;
	}

	return asn;
}

bool SocketClientPrivate::cli_sslSetSession(const unsigned char *session,
											unsigned int size )
{
	CHECK_CLI_CTX(return false);

	if (!OpenSSL::ASNToSession(m_ssl, session, size)) {
		WRITE_TRACE(DBG_FATAL, "SSL error: can't create SSL_SESSION from ANSI "
					"(SSL error: %s)", OpenSSL::GetErrorStr().c_str());
		return false;
	}

	return true;
}

bool SocketClientPrivate::sslHandshake ( int sock, quint32 msecsTimeout, bool rehandshake)
{
    Q_ASSERT( m_ssl );
    Q_ASSERT( m_sslNetworkBio );

    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    char buff[ SSLHelper::SSLBuffSize ];
    bool result = true;

    IOService::TimeMark startMark = 0;
    IOService::timeMark(startMark);

    if ( m_ctx == Cli_ServerContext )
        // Set accept state for server
        SSL_set_accept_state(m_ssl);
    else
        // Set connect state for client
        SSL_set_connect_state(m_ssl);

    for (;;) {
        // Calc timeout
        qint32 msecsToWait = 0;
        CALC_TIMEOUT(msecsTimeout, msecsTimeout,
                     "SSL handshake timeout expired!",
                     result = false; break);

		qint32 ret = SSL_do_handshake(m_ssl);

        // If we have smth to write to network -- write!
        qint32 pending  = BIO_pending( m_sslNetworkBio );
        if ( pending > 0 ) {
            char* ptr = 0;
            qint32 toWrite = BIO_nread( m_sslNetworkBio, &ptr, pending );

            Q_ASSERT( toWrite == pending );
            Q_ASSERT( BIO_pending(m_sslNetworkBio) <= 0 );

            // Do write to socket
            IOSendJob::Result done = writeData( sock, ptr, toWrite, msecsToWait );
            if ( done != IOSendJob::Success ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("SSL handshake: write failed, timeout %d"),
                            msecsToWait);
                result = false;
                break;
            }
        }

        // Check if handshake is finished
        if ( ! SSL_in_init(m_ssl) ) {
            result = true;
            m_pendingInSSL = (BIO_pending(m_sslSSLBio) > 0);
            break;
        }

        if ( ret <= 0 ) {
            qint32 err = SSL_get_error(m_ssl, ret);
            if ( err == SSL_ERROR_WANT_READ ) {
                qint32 readRequest = BIO_get_read_request(m_sslNetworkBio);
                Q_ASSERT( readRequest <= SSLHelper::SSLBuffSize );

                // Do read from socket
                quint32 wasRead = 0;
                bool res = read( sock, buff, readRequest, wasRead,
                                 IOSingleRead, msecsToWait );
                if ( ! res ) {
					ERR_error_string_n( ERR_get_error(), errBuff, ErrBuffSize );
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Write handshake to SSL failed "
                                       "(SSL error: %s)"), errBuff);

                    WRITE_TRACE(DBG_FATAL, IO_LOG("SSL handshake: read failed, "
                                       "timeout %d"),
                                msecsToWait );
                    result = false;
                    break;
                }

                // Write to SSL
                qint32 written = BIO_write(m_sslNetworkBio, buff, wasRead );
                if ( written != (qint32)wasRead ) {
                    ERR_error_string_n( ERR_get_error(), errBuff, ErrBuffSize );
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Write handshake to SSL failed "
                                       "(SSL error: %s)"), errBuff);
                    result = false;
                    break;
                }
            }
            else if ( err == SSL_ERROR_WANT_WRITE ) {
                // Write should be already handled
            }
            else {
				ERR_error_string_n( ERR_get_error(), errBuff, ErrBuffSize );
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("SSL handshake: "
                                   "(SSL error: %s)"), errBuff);
                result = false;
                break;
            }
        }
        else {
            result = true;
            m_pendingInSSL = (BIO_pending(m_sslSSLBio) > 0);
            break;
        }
    }

    // Set handshake error
    if ( ! result )
        m_error = IOSender::SSLHandshakeError;
	else
	{
		if (!(result = m_sslHelper->postHandshakeCheck(m_ssl, rehandshake))) {
			m_error = IOSender::SSLHandshakeError;
			result = false;
		}
		else {
			m_securityMode = SSLHelper::isTrustedChannelCipher(SSLHelper::GetConnectionChipher(m_ssl))
				? IOSender::TrustedChannelConnection
				: IOSender::SelfSignedConnection;
		}
	}

    return result;
}


bool SocketClientPrivate::sslHeaderCheck ( const SSLv3Header& header ) const
{
    // Check version
    if ( header.sslVersion != 3 ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("SSL version is wrong!"));
        return false;
    }

    // Data size check, must be <= (2 ^ 14)
    if ( ntohs(header.sslDataLength) > 16384 ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("SSL data size is > 16384!"));
        return false;
    }
    // Data size check, must be > 0
    else if ( header.sslDataLength == 0 ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("SSL data size is 0!"));
        return false;
    }

    return true;
}

bool SocketClientPrivate::sslReadAndWait ( int sock,
                                           char* buffOut, quint32 sizeOut,
                                           int msecsTimeout, int* unixfd,
                                           bool* timeoutExpired )
{
    Q_ASSERT( m_ssl );
    Q_ASSERT( m_sslNetworkBio );

    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    do {
        // Read SSL header
        if ( m_remainToRead == 0 && ! m_pendingInSSL ) {

            quint32 readBytes = 0;
            bool res = read( sock, reinterpret_cast<char*>(&m_header),
                             sizeof(m_header), readBytes, IOContinuousRead,
                             msecsTimeout, unixfd, timeoutExpired );
            if ( ! res  ) {
                LOG_MESSAGE(DBG_INFO, IO_LOG("Read failed"));
                return false;
            }

            m_remainToRead = ntohs(m_header.sslDataLength);
            m_encryptedDataToRead = (m_header.type != 0xff);
            m_newHeader = m_encryptedDataToRead;

            if ( ! sslHeaderCheck(m_header) ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("SSL header is wrong!"));
                return false;
            }
        }

        // SSL data
        if ( m_encryptedDataToRead || m_pendingInSSL ) {
            qint32 pending = BIO_pending(m_sslSSLBio);

            // If SSL engine is empty, read from socket
            // or we have smth to write to SSL
            if ( pending == 0 ) {
                Q_ASSERT( m_remainToRead > 0 );

                // Write header to SSL
                if ( m_newHeader ) {
                    qint32 written = BIO_write( m_sslNetworkBio,
                                                &m_header, sizeof(m_header));
                    if ( written <= 0 ) {
                        ERR_error_string_n( ERR_get_error(), errBuff,
                                            ErrBuffSize );
                        WRITE_TRACE(DBG_FATAL, IO_LOG("Write to SSL failed "
                                           "(SSL error: %s"), errBuff);
                        return false;
                    }
                    else if ( written != (qint32)sizeof(m_header) ) {
                        WRITE_TRACE(DBG_FATAL,
                                    IO_LOG("Write to SSL failed: must be "
                                           "written: %d, but was %d"),
                                    (qint32)sizeof(m_header), written);
                        return false;
                    }
                    m_newHeader = false;
                }

                // Get pointer to buffer
                char* buff = 0;
                qint32 wrGuarantee = BIO_nwrite0(m_sslNetworkBio, &buff);
                Q_ASSERT(wrGuarantee > 0);
                Q_ASSERT(buff);

                quint32 toRead = qMin( m_remainToRead, wrGuarantee );

                // Read from socket
                quint32 readB = 0;
                bool res = read( sock, buff, toRead, readB, IOSingleRead,
                                 msecsTimeout, unixfd, timeoutExpired );
                if ( ! res  ) {
                    LOG_MESSAGE(DBG_INFO, IO_LOG("Read failed"));
                    return false;
                }

                m_remainToRead -= readB;

                // Advance buffer
                toRead = BIO_nwrite(m_sslNetworkBio, &buff, readB);
                Q_ASSERT(toRead > 0);
                Q_ASSERT((quint32)toRead == readB);
                Q_ASSERT(buff);

                // Lock
                QMutexLocker locker( m_writeThread.getSSLMutex() );

                qint32 readBytes = BIO_read( m_sslSSLBio, buffOut, sizeOut );
                if ( readBytes < 0 ) {
                    unsigned long err = ERR_get_error();
                    if ( !BIO_should_retry(m_sslSSLBio) ) {
                        ERR_error_string_n( err, errBuff, ErrBuffSize );
                        WRITE_TRACE(DBG_FATAL,
                                    IO_LOG("Error in SSL (SSL error: %s)"),
                                    errBuff);
                        return false;
                    }

                    // Handshake check
                    if ( SSL_in_init(m_ssl) ) {
                        LOG_MESSAGE(DBG_INFO,
                                    IO_LOG("Handshake received! Want for "
                                           "%s\n"),
                                    (BIO_should_write(m_sslSSLBio) ?
                                     "write" : "read") );
                    }
                    else {
                        LOG_MESSAGE(DBG_INFO,
                                    IO_LOG("Data received! Want for %s\n"),
                                    (BIO_should_write(m_sslSSLBio) ?
                                     "write" : "read") );
                    }

                    // If we have smth to write -- write!
                    if ( BIO_pending(m_sslNetworkBio) > 0 )
                        // Wake up writer
                        m_writeThread.wakeSSLWriter();

                    continue;
                }
                else if ( readBytes == 0 ) {
                    WRITE_TRACE(DBG_FATAL, IO_LOG("SSL startup failed"));
                    return false;
                }

                // Unlock
                locker.unlock();

                buffOut += readBytes;
                sizeOut -= readBytes;

                m_pendingInSSL = (BIO_pending(m_sslSSLBio) > 0);
            }
            // Smth is already decoded, just read from SSL bio
            else {
                // Lock
                QMutexLocker locker( m_writeThread.getSSLMutex() );

                qint32 readBytes = BIO_read(m_sslSSLBio, buffOut, sizeOut);
                if ( readBytes < 0 ) {
                    unsigned long err = ERR_get_error();
                    if ( !BIO_should_retry(m_sslSSLBio) ) {
                        ERR_error_string_n( err, errBuff, ErrBuffSize );
                        WRITE_TRACE(DBG_FATAL,
                                    IO_LOG("Error in SSL (SSL error: %s)"),
                                    errBuff);
                        return false;
                    }

                    // Handshake check
                    if ( SSL_in_init(m_ssl) ) {
                        LOG_MESSAGE(DBG_INFO,
                                    IO_LOG("Handshake received! Want for "
                                           "%s\n"),
                                    (BIO_should_write(m_sslSSLBio) ?
                                     "write" : "read") );
                    }
                    else {
                        LOG_MESSAGE(DBG_INFO,
                                    IO_LOG("Data received! Want for %s\n"),
                                    (BIO_should_write(m_sslSSLBio) ?
                                     "write" : "read") );
                    }

                    // If we have smth to write -- write!
                    if ( BIO_pending(m_sslNetworkBio) > 0 )
                        // Wake up writer
                        m_writeThread.wakeSSLWriter();

                    continue;
                }
                else if ( readBytes == 0 ) {
                    WRITE_TRACE(DBG_FATAL, IO_LOG("SSL startup failed"));
                    return false;
                }

                // Unlock
                locker.unlock();

                buffOut += readBytes;
                sizeOut -= readBytes;

                m_pendingInSSL = (BIO_pending(m_sslSSLBio) > 0);
            }
        }
        // Plain data
        else {
            Q_ASSERT( m_remainToRead > 0 );
            quint32 toRead = qMin( m_remainToRead, (qint32)sizeOut );
            quint32 readBytes = 0;
            bool res = read( sock, buffOut, toRead, readBytes,
                             IOContinuousRead, msecsTimeout, unixfd,
                             timeoutExpired );
            if ( ! res  ) {
                LOG_MESSAGE(DBG_INFO, IO_LOG("Read failed"));
                return false;
            }

            m_remainToRead -= readBytes;
            buffOut += readBytes;
            sizeOut -= readBytes;
        }

    } while ( sizeOut );

    return true;
}

bool SocketClientPrivate::readData ( int sock,
                                     char* buffOut, quint32 sizeOut,
                                     int msecsTimeout, int* unixfd,
                                     bool* timeoutExpired )
{
    quint32 wasRead = 0;
    return read( sock, buffOut, sizeOut, wasRead, IOContinuousRead,
                     msecsTimeout, unixfd, timeoutExpired );
}

IOSendJob::Result SocketClientPrivate::writeData ( int sock,
                                                  const void* data,
                                                  quint32 size,
                                                  int msecsTimeout )
{
        return m_writeThread.write( sock,
#ifdef _WIN_ // Windows
                                   m_threadState,
                                   &m_overlappedRead,
                                   m_readEventHandle,
#else // Unix
                                   m_eventPipes[0],
#endif
                                   data,
                                   size,
                                   msecsTimeout);
}

bool SocketClientPrivate::srv_setHandshakeFromState ()
{
    CHECK_CLI_OR_SRV_CTX(return false);

    Q_ASSERT(m_srvCtx.m_clientState.isValid());

    const DetachedClientState* state =
        reinterpret_cast<const DetachedClientState*>(
                                             m_srvCtx.m_clientState->m_state);

    const DetachedClientState::PODHeader& header = state->header;
    const IOCommunication::ProtocolVersion& protocolVer =
        header.clientProtocolVersion;
    QString clientConnectionUuid;
    IOSender::Type senderType = header.clientHandshakeHeader.senderType;
    bool tableParsed = false;
    IORoutingTable acceptedTable =
        IORoutingTable::fromBuffer( state->data.routingTableBuff,
                                    header.routingTableBuffSize,
                                    tableParsed );

    // Check client protocol version
    {
        // Protocol version magic bytes check
        if ( 0 != ::memcmp(protocolVer.whoWeAre,
                           IOProtocolVersion.whoWeAre,
                           sizeof(IOProtocolVersion.whoWeAre)) ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("IO protocol magic bytes differs!"));
            return false;
        }
        // Protocol version check
        if ( protocolVer.majorNumber != IOProtocolVersion.majorNumber ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("IO protocol major version number differs!"));
            return false;
        }
        if ( protocolVer.minorNumber != IOProtocolVersion.minorNumber ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("IO protocol minor version number differs!"));
        }
    }

    // Check handshake header
    {
        Uuid connUuid =
            Uuid::toUuid( header.clientHandshakeHeader.connectionUuid );
        if ( connUuid.isNull() ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: handshake header parse "
                               "failed!"));
            return false;
        }
        clientConnectionUuid = connUuid.toString();

        // Get sender type
        if ( senderType == IOSender::UnknownType ||
             senderType >= IOSender::EndTypeBound ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Handshake error: sender type is wrong!"));
            return false;
        }
    }

    // Check routing table
    {
        if ( ! tableParsed || acceptedTable.isNull() ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Handshake error: "
                               "can't accept client's table!"));
            return false;
        }
    }

    Q_ASSERT( ! clientConnectionUuid.isEmpty() );
    Q_ASSERT( ! acceptedTable.isNull() );
    Q_ASSERT( senderType != IOSender::UnknownType );

    // Lock
    QMutexLocker locker( &m_eventMutex );

    m_peerProtoVersion = protocolVer;
    m_peerConnectionUuid = clientConnectionUuid;
    m_peerSenderType = senderType;
    m_peerRoutingTable = acceptedTable;

    // Unlock
    locker.unlock();

    return true;
}

bool SocketClientPrivate::srv_addSSLSessionFromStateAndRehandshake (
    int sock,
    SmartPtr<IOPackage>& additionalPkg,
    quint32 msecsTimeout )
{
    CHECK_CLI_OR_SRV_CTX(return false);

    Q_ASSERT(m_srvCtx.m_clientState.isValid());

    DetachedClientState* state =
        reinterpret_cast<DetachedClientState*>(m_srvCtx.m_clientState->m_state);
    additionalPkg = SmartPtr<IOPackage>();

    if (m_ctx == Cli_ServerContext) {
	    const size_t ErrBuffSize = 256;
	    char errBuff[ ErrBuffSize ];
	    SSL_SESSION* sslSession = 0;
	    const unsigned char* pSession = reinterpret_cast<const unsigned char*>(
	                                             state->data.sslSession.getImpl());
	    sslSession = d2i_SSL_SESSION( &sslSession, &pSession,
	                                  state->header.sslSessionSize );
	    if ( sslSession == 0 ) {
	        ERR_error_string_n( ERR_get_error(), errBuff, ErrBuffSize );
	        WRITE_TRACE(DBG_FATAL,
	                    IO_LOG("SSL error: can't create SSL_SESSION from ANSI "
	                           "(SSL error: %s)"), errBuff);
	        return false;
	    }

	    // Add session to server context
	    int res = SSL_CTX_add_session(m_sslHelper->GetServerSSLContext(), sslSession);
	    SSL_SESSION_free(sslSession);

	    if ( res != 1 ) {
	        WRITE_TRACE(DBG_FATAL, IO_LOG("SSL error: can't add SSL_SESSION"));
	        return false;
	    }
    }
    else if (m_ctx == Cli_ClientContext) {
	    if (!cli_sslSetSession(reinterpret_cast<unsigned char*>(state->data.sslSession.get()), state->header.sslSessionSize)) {
		    WRITE_TRACE(DBG_FATAL, IO_LOG("SSL error: can't set client's SSL session"));
		    return false;
	    }
    }

    // Do SSL rehandshake after session addition
    bool ret = sslHandshake( sock, msecsTimeout, true );
    if ( ! ret ) {
        return false;
    }

    if ( ! SSL_session_reused(m_ssl) ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("SSL session was not reused!"));
        // New session was created!
        // It is not correct!
        return false;
    }

    // Save additional package
    additionalPkg = state->data.additionalPkg;

    return true;
}

/******************************************************************************
 * Socket write callbacks
 *****************************************************************************/

void SocketClientPrivate::onBeforeWrite ( SocketWriteThread*,
                                          const SmartPtr<IOPackage>& p )
{
    CALLBACK_MARK;
    m_rcvSndListener->onBeforeSend( this, m_peerConnectionUuid, p );
    WARN_IF_CALLBACK_TOOK_MUCH_TIME;
}

void SocketClientPrivate::onAfterWrite ( SocketWriteThread*,
                                         IOSendJob::Result res,
                                         const SmartPtr<IOPackage>& p )
{
    CALLBACK_MARK;
    m_rcvSndListener->onAfterSend( this, m_peerConnectionUuid, res, p );
    WARN_IF_CALLBACK_TOOK_MUCH_TIME;
}

void SocketClientPrivate::onWriteIsFinishedWithErrors ( SocketWriteThread* )
{
    //
    // Finalize reading!
    //
    QMutexLocker locker( &m_eventMutex );
    __finalizeThread();
}

bool SocketClientPrivate::getDetachedSocket(int& sock, char *errBuff, size_t ErrBuffSize)
{
	int sockHandle;
#ifdef _WIN_
	DetachedClientState* state = reinterpret_cast<DetachedClientState*>(
								  m_srvCtx.m_clientState->m_state);
	sockHandle = ::WSASocket( FROM_PROTOCOL_INFO,
							  FROM_PROTOCOL_INFO,
							  FROM_PROTOCOL_INFO,
							  &state->header.clientSocket, 0,
							  WSA_FLAG_OVERLAPPED );
	if ( sockHandle == INVALID_SOCKET ) {
		WRITE_TRACE(DBG_FATAL,
					IO_LOG("Can't create socket from saved client "
						   "state (native error: %s)"),
					native_strerror(errBuff, ErrBuffSize) );
		return false;
	}
#else
	Q_UNUSED(errBuff)
	Q_UNUSED(ErrBuffSize)
	sockHandle = m_srvCtx.m_clientState->takeSocketHandle();
#endif
	sock = sockHandle;
	return true;
}

/*****************************************************************************/

void SocketClientPrivate::setLimitErrorLogging(bool bLimitErrorLogging)
{
	m_bLimitErrorLogging = bLimitErrorLogging;
}

