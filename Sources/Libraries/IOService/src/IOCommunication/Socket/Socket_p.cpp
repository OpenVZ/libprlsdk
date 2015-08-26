/*
 * Socket_p.cpp
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


#include "Socket_p.h"

#include "Libraries/Logging/Logging.h"
#include "Libraries/Std/Etrace.h"

using namespace IOService;

/*****************************************************************************/

#ifdef _WIN_ // Windows

volatile int WSAInitHelper::s_inited = 0;
QMutex WSAInitHelper::s_initMutex;

bool WSAInitHelper::InitWSA ()
{
    // Lock
    QMutexLocker lock( &s_initMutex );

    if ( s_inited == 0 ) {
        // Initialize Winsock.
        WSADATA wsaData;
        int iResult = ::WSAStartup(MAKEWORD(2,2), &wsaData);
        if ( iResult != NO_ERROR ) {
            return false;
        }
    }

    // Increment
    ++s_inited;

    return true;
}

void WSAInitHelper::DeinitWSA ()
{
    // Lock
    QMutexLocker lock( &s_initMutex );

    if ( s_inited == 0 ) {
        Q_ASSERT(0);
        return;
    }

    if ( s_inited == 1 ) {
        // Cleanup Winsock
        ::WSACleanup();
    }

    // Decrement
    --s_inited;
}

namespace {
    class WSAInit
    {
    public:
        WSAInit()
        {
            WSAInitHelper::InitWSA();
        }
        ~WSAInit()
        {
            WSAInitHelper::DeinitWSA();
        }
    };
    WSAInit g_wsaInit;
}

#endif

/*****************************************************************************/

int IOService::native_error ()
{
#ifdef _WIN_
    return ::WSAGetLastError();
#else
    return errno;
#endif
}

const char* IOService::native_strerror ( int err, char* buff, size_t size )
{
    const size_t ErrnoBuffSize = 24;
    char errnoBuff[ErrnoBuffSize];

    if ( size < 10 )
        return 0;

    buff[0] = 0;

#ifndef _WIN_ // Unix
#if defined(_LIN_) // Linux
    char* errStr = ::strerror_r( err, buff, size );
#endif
    if ( errno == EINVAL || errStr == 0 )
        return errStr;
    size_t strerrLen = ::strlen( errStr );
#else // Windows
    char* errStr = buff;
    size_t strerrLen = ::FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM |
                                         FORMAT_MESSAGE_IGNORE_INSERTS |
                                         FORMAT_MESSAGE_MAX_WIDTH_MASK,
                                         0, //source
                                         err,
                                         0, //language
                                         buff,
                                         (DWORD)size,
                                         0 //args
                                         );
#endif
    // Truncate
    strerrLen = qMin(strerrLen, size);

    // Skip trailing 0
    size -= 1;

    if ( errStr != buff ) {
        ::memcpy( buff, errStr, strerrLen );
        errStr = buff;
    }
#ifdef _WIN_
    size_t errnoLen = ::_snprintf( errnoBuff, ErrnoBuffSize,
                                  " [errno: %d]", err );
#else
    size_t errnoLen = ::snprintf( errnoBuff, ErrnoBuffSize,
                                  " [errno: %d]", err );
#endif
    // Fill with trailing errno only if we have free chars
    if ( size >= errnoLen ) {
        char* ptr = 0;
        if ( strerrLen + errnoLen <= size )
            ptr = errStr + strerrLen;
        else
            ptr = errStr + (size - errnoLen);

        ::memcpy( ptr, errnoBuff, errnoLen );
        ptr[ errnoLen ] = 0;
    }
    else
        errStr[ size ] = 0;

    return errStr;
}

const char* IOService::native_strerror ( char* buff, size_t size )
{
    return native_strerror( native_error(), buff, size );
}

bool IOService::socketpair ( int socks[2] )
{
#ifdef _WIN_
    if ( socks == 0 ) {
        ::WSASetLastError(WSAEINVAL);
        return false;
    }

    socks[0] = (int)INVALID_SOCKET;
    socks[1] = (int)INVALID_SOCKET;

    int listener = ::socket( AF_INET, SOCK_STREAM, 0 );
    if ( listener == INVALID_SOCKET)
        return false;

    // Addr struct
    sockaddr_in addr;
    int addrlen = sizeof(addr);

    // Setup for anonymous connect
    ::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    int err = ::bind( listener, reinterpret_cast<struct sockaddr*>(&addr),
                      sizeof(addr) );
    if ( err == SOCKET_ERROR ) {
        err = WSAGetLastError();
        ::closesocket(listener);
        ::WSASetLastError(err);
        return false;
    }
    err = ::getsockname( listener, reinterpret_cast<struct sockaddr*>(&addr),
                         &addrlen );
    if ( err == SOCKET_ERROR ) {
        err = WSAGetLastError();
        ::closesocket(listener);
        ::WSASetLastError(err);
        return false;
    }

    do {
        // Listen
        if ( ::listen(listener, 1) == SOCKET_ERROR )
            break;

        // Create socket
        socks[0] = ::socket( AF_INET, SOCK_STREAM, 0 );
        if ( socks[0]  == INVALID_SOCKET )
            break;

        // Connect
        if ( ::connect(socks[0], reinterpret_cast<struct sockaddr*>(&addr),
                       sizeof(addr)) == SOCKET_ERROR )
            break;

        // Accept
        socks[1] = ::accept( listener, NULL, NULL );
        if ( socks[1] == INVALID_SOCKET )
            break;

        // Close listener
        ::closesocket(listener);

        return true;

    } while ( 0 );

    err = ::WSAGetLastError();
    ::closesocket(listener);
    ::closesocket(socks[0]);
    ::closesocket(socks[1]);
    ::WSASetLastError(err);

    return false;

#else
    int res = ::socketpair( AF_UNIX, SOCK_STREAM, 0, socks );
    if ( res < 0 ) {
        return false;
    }
    return true;
#endif
}

bool IOService::setCloseOnExec ( int sock )
{
#ifdef _WIN_ // Windows
    if ( ! ::SetHandleInformation((HANDLE)sock, HANDLE_FLAG_INHERIT, 0) )
        return false;
#else // Unix
    int flags = ::fcntl( sock, F_GETFD, 0 );
    if ( flags < 0 ) {
        return false;
    }
    else if ( flags & FD_CLOEXEC ) {
        // Great. Already contains
        return true;
    }
    else if ( ::fcntl(sock, F_SETFD, flags | FD_CLOEXEC) < 0 ) {
        return false;
    }
#endif
    // Great. Successfully set
    return true;
}

bool IOService::setSocketOptions ( int sock )
{
    // Unix:    ensure that socket is closed on exec*()
    // Windows: prevent from inheritance
    if ( ! IOService::setCloseOnExec(sock) )
        return false;

    // Increase send buffer size
    int newSndBufSz = SSLMaxDataLength * 2;
    socklen_t optLen = sizeof(newSndBufSz);
    if ( ::setsockopt(sock, SOL_SOCKET, SO_SNDBUF,
                      reinterpret_cast<char*>(&newSndBufSz),
                      optLen) < 0 )
        return false;

    // Increase receive buffer size
    newSndBufSz = SSLMaxDataLength * 2;
    optLen = sizeof(newSndBufSz);
    if ( ::setsockopt(sock, SOL_SOCKET, SO_RCVBUF,
                      reinterpret_cast<char*>(&newSndBufSz),
                      optLen) < 0 )
        return false;

    int i = 1;
    if ( ::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                      reinterpret_cast<char*>(&i),
                      sizeof(i)) < 0 ){
        WRITE_TRACE(DBG_FATAL, "TCP_NODELAY setting failed");
    }

#ifndef _WIN_
    // Set non-blocking mode
    int flags = ::fcntl( sock, F_GETFL, 0 );
    if ( flags < 0 ) {
        return false;
    }
    else if ( flags & O_NONBLOCK ) {
        // Great. Already contains
        return true;
    }
    else if ( ::fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0 ) {
        return false;
    }
#endif

    return true;
}

bool IOService::getFullNameInfo ( const struct sockaddr *sa, socklen_t salen,
                                  QString& hostNameStr,
                                  quint16& portNumber,
                                  QString& errStr )
{
    Q_ASSERT(sa);

    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];
    char hostName[ NI_MAXHOST ] = {0};
    char servName[ NI_MAXSERV ] = {0};
    int res = 0;

#ifndef _WIN_ // Unix
    // If sockets are local: do nothing
    if ( sa->sa_family == AF_UNIX || sa->sa_family == AF_LOCAL ) {
        hostNameStr = "localhost.AF_LOCAL";
        portNumber = 0;
        return true;
    }
#endif

    //
    // Retreive numeric host name and numeric port
    //
    res = ::getnameinfo( sa, salen,
                         hostName, sizeof(hostName),
                         servName, sizeof(servName),
                         NI_NUMERICHOST | NI_NUMERICSERV );
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

        errStr = "::getnameinfo(NI_NUMERICHOST|NI_NUMERICSERV) failed "
                 "(native error: %1)";
        errStr = errStr.arg( errP );
        return false;
    }

    bool ok = false;
    hostNameStr = hostName;
    portNumber = QString(servName).toInt(&ok);
    return ok;
}

bool IOService::getHostInfo ( int sock,
                              QString& hostNameStr,
                              quint16& portNumber,
                              QString& errStr )
{
    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    sockaddr_storage addr;
    ::memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);

    int res = ::getsockname( sock, reinterpret_cast<sockaddr*>(&addr), &len );
    if ( res < 0 ) {
        int err = native_error();
        errStr = "::getsockname failed (native error: %1)";
        errStr = errStr.arg( native_strerror(err, errBuff, ErrBuffSize) );
        return false;
    }

    return getFullNameInfo( reinterpret_cast<sockaddr*>(&addr), len,
                            hostNameStr, portNumber, errStr );
}

bool IOService::getPeerInfo ( int sock,
                              QString& hostNameStr,
                              quint16& portNumber,
                              QString& errStr )
{
    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    sockaddr_storage addr;
    ::memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);

    int res = ::getpeername( sock, reinterpret_cast<sockaddr*>(&addr), &len );
    if ( res < 0 ) {
        int err = native_error();
        errStr = "::getpeername failed (native error: %1)";
        errStr = errStr.arg( native_strerror(err, errBuff, ErrBuffSize) );
        return false;
    }

    return getFullNameInfo( reinterpret_cast<sockaddr*>(&addr), len,
                            hostNameStr, portNumber, errStr );
}

#ifndef _WIN_ //Unix

SmartPtr<fd_set> IOService::allocFDSet ( int fdNum )
{
    fd_set* ptr = reinterpret_cast<fd_set*>(::malloc(FDNUM2SZ(fdNum)));
    return SmartPtr<fd_set>(ptr, free);
}

int IOService::getMaxFDNumber ()
{
    int fdNum = FD_SETSIZE;
    rlimit limit;
    if ( ::getrlimit(RLIMIT_NOFILE, &limit) == 0 ) {
        fdNum = limit.rlim_max;
        if ( (quint32)fdNum < FD_SETSIZE )
            fdNum = FD_SETSIZE;
        else if ( (quint32)fdNum > MAX_FDNUM )
            fdNum = MAX_FDNUM;
    }
    return fdNum;
}
#endif

QList<addrinfo*> IOService::orderAddrInfo ( addrinfo* addrInfo,
                                            OrderPreference orderPref )
{
    QList<addrinfo*> resPrefered;
    QList<addrinfo*> resOther;
    if ( addrInfo == 0 ) {
        Q_ASSERT(0);
        return resOther;
    }
    // Sort
    for ( addrinfo* ai = addrInfo; ai != 0; ai = ai->ai_next ) {
        if ( orderPref == OP_PreferIPv6 && ai->ai_family == AF_INET6 )
            resPrefered.append(ai);
        else if ( orderPref == OP_PreferIPv4 && ai->ai_family == AF_INET )
            resPrefered.append(ai);
        else
            resOther.append(ai);
    }
    return resPrefered + resOther;
}

/*****************************************************************************/

DetachedClientState::DetachedClientState ()
{
    ::memset( &header, 0, sizeof(header) );
#ifndef _WIN_ // Unix
    header.clientSocket = -1;
#endif
}

/*****************************************************************************/

SocketWriteThread::SocketWriteThread (
    SmartPtr<IOJobManager> jobManager,
    IOSender::Type senderType,
    SocketClientContext ctx,
    IOSender::Statistics& stat,
    SocketWriteListenerInterface* wrListener ) :

    m_jobManager(jobManager),
    m_senderType(senderType),
    m_ctx(ctx),
    m_wrListener(wrListener),
    m_state(IOSender::Disconnected),
    m_peerProtoVersion(IOService::IOProtocolVersion),
#ifdef _WIN_ // Windows
    m_writeEventHandle(WSA_INVALID_EVENT),
#else // Unix
    m_maxFDNum(IOService::getMaxFDNumber()),
    m_rdSet(IOService::allocFDSet(m_maxFDNum)),
    m_wrSet(IOService::allocFDSet(m_maxFDNum)),
#endif
    m_threadState(ThreadIsStopped),
    m_stopReason(IOSendJob::ConnClosedByUser),
    m_sockHandle(-1),
    m_inPause(false),
    m_isDetaching(false),
    m_ssl(0),
    m_sslSSLBio(0),
    m_sslNetworkBio(0),
    m_stat(stat)
{
    Q_ASSERT(m_wrListener);

    // Client context
    if ( m_ctx == Cli_ClientContext )
        INIT_IO_LOG(QString("IO client ctx [write thr] (sender %2): ").
                    arg(m_senderType));
    // Server context
    else if ( m_ctx == Cli_ServerContext )
        INIT_IO_LOG(QString("IO server ctx [write thr] (sender %2): ").
                    arg(m_senderType));
    // Proxy management context
    else if ( m_ctx == Cli_ProxyMngContext )
        INIT_IO_LOG(QString("IO proxy management ctx [write thr] (sender %2): ").
                    arg(m_senderType));
    else
        Q_ASSERT(false);


    // Set stack size for write thread: 100K should be enough
    QThread::setStackSize( 1024 * 100 );

#ifdef _WIN_
    // Zero overlapped structure
    ::memset( &m_sockOverlappedWrite, 0, sizeof(m_sockOverlappedWrite) );
#else
    // Invalidate pipes
    m_eventPipes[0] = -1;
    m_eventPipes[1] = -1;
#endif
}

SocketWriteThread::~SocketWriteThread ()
{
    stopWriteThread();
}

SmartPtr<IOJobManager::JobPool> SocketWriteThread::getJobPool () const
{
    QMutexLocker locker( &m_jobMutex );
    return m_jobPool;
}

bool SocketWriteThread::startWriteThread ( int sock,
                                           const Uuid& currConnUuid,
                                           const Uuid& peerConnUuid,
                                           const IOCommunication::ProtocolVersion& protoVer,
                                           const IORoutingTable& routingTable
#ifndef _WIN_ // Unix
                                           , int eventPipes[2]
#endif
                                           )
{
    Q_ASSERT(sock != -1 );
    Q_ASSERT(! currConnUuid.isNull());
    Q_ASSERT(! peerConnUuid.isNull());
    Q_ASSERT(! routingTable.isNull());
#ifndef _WIN_ // Unix
    Q_ASSERT(eventPipes[0] != -1);
    Q_ASSERT(eventPipes[1] != -1);
#endif

    // Main start/stop lock
    QMutexLocker mainLocker( &m_startStopMutex );

    // Lock
    QMutexLocker locker( &m_jobMutex );

    // 'starting' state is invisible for us,
    // because other starting thread must lock start/stop mutex
    // and wait for correct result
    Q_ASSERT(m_threadState != ThreadIsStarting);

    // Already started, return success
    if ( m_threadState == ThreadIsStarted )
        return true;
    // Stopping in progress, wait for result
    else if ( m_threadState == ThreadIsStopping ) {
        m_threadStateWait.wait( &m_jobMutex );
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
        // Set socket
        m_sockHandle = sock;
        // Set curr conn uuid
        m_currConnUuid = currConnUuid;
        // Set peer conn uuid
        m_peerConnUuid = peerConnUuid;
        // Set peer proto ver
        m_peerProtoVersion = protoVer;
        // Set route table
        m_routingTable = routingTable;

#ifndef _WIN_ // Unix
        m_eventPipes[0] = eventPipes[0];
        m_eventPipes[1] = eventPipes[1];
#endif

        // Mark as starting
        m_threadState = ThreadIsStarting;

        // Clear stop reason
        m_stopReason = IOSendJob::ConnClosedByUser;
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

    m_threadStateWait.wait( &m_jobMutex );
    // Check if failure on start
    if ( m_threadState != ThreadIsStarted ) {
        // Unlock. Stopping thread will use locks
        locker.unlock();
        // Wait for Qt thread
        QThread::wait();
        // Lock
        locker.relock();
        Q_ASSERT(m_threadState == ThreadIsStopped);
        return false;
    }

    // Great!
    return true;
}

IOSendJob::Result SocketWriteThread::stopWriteThread ()
{
    // If call from this thread: just mark as finalized
    if ( QThread::currentThread() == this ) {
        // Lock
        QMutexLocker locker( &m_jobMutex );
        __finalizeThread();
        return m_stopReason;
    }

    // Main start/stop lock
    QMutexLocker mainLocker( &m_startStopMutex );

    // Lock
    QMutexLocker locker( &m_jobMutex );

    // 'starting' state is invisible for us,
    // because starting thread must lock start/stop mutex
    // and wait for correct result
    Q_ASSERT(m_threadState != ThreadIsStarting);

    // Already stopped
    if ( m_threadState == ThreadIsStopped ) {
        // Wait for Qt thread
        QThread::wait();
        return m_stopReason;
    }
    // Stopping in progress, wait for result
    else if ( m_threadState == ThreadIsStopping ) {
        m_threadStateWait.wait( &m_jobMutex );
        Q_ASSERT(m_threadState == ThreadIsStopped);

        // Wait for Qt thread
        QThread::wait();
        return m_stopReason;
    }

    //
    // Finalize writing!
    //
    __finalizeThread();

    m_threadStateWait.wait( &m_jobMutex );
    Q_ASSERT(m_threadState == ThreadIsStopped);

    // Wait for Qt thread
    QThread::wait();
    return m_stopReason;
}

void SocketWriteThread::__finalizeThread ()
{
    // Mark as stopping
    m_threadState = ThreadIsStopping;

    // Signal events
#ifdef _WIN_
    ::WSASetEvent( m_writeEventHandle );
#else
    char b = 0;
    if ( ::write( m_eventPipes[1], &b, 1 ) < 0 )
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Write failed while thread finalization!"));
#endif
    // Wake writing thread
    m_wait.wakeOne();
}

void SocketWriteThread::setUuidsToPkgHeaderAndRegisterJob (
    IOPackage::PODHeader& pkgHeader,
    SmartPtr<IOSendJob>& sendJob ) const
{
    Uuid senderUuid = Uuid::toUuid(pkgHeader.senderUuid);

    // If sender uuid is empty, fill it by connection uuid
    if ( senderUuid.isNull() ) {
        m_currConnUuid.dump(pkgHeader.senderUuid);
    }
    else {
        LOG_MESSAGE(DBG_INFO, IO_LOG("Package sender uuid is not empty! "
                                     "Use it instead of connection uuid"));
    }

    Uuid pkgUuid = Uuid::toUuid(pkgHeader.uuid);

    // If package uuid is empty, generate new uuid
    if ( pkgUuid.isNull() ) {
        Uuid::createUuid( pkgHeader.uuid );
    }
    else {
        LOG_MESSAGE(DBG_INFO, IO_LOG("Package uuid is not empty! "
                                     "Do not generate!"));
    }

    // Calculate CRC16
    pkgHeader.crc16 = IOPackage::headerChecksumCRC16( pkgHeader );

    // Register package uuid
    sendJob->registerPackageUuid( pkgHeader.uuid );
}

IOSendJob::Result SocketWriteThread::write (
    int sock,
#ifdef _WIN_ // Windows
    volatile ThreadState& threadState,
    WSAOVERLAPPED* overlapped,
    WSAEVENT writeEvent,
#else // Unix
    int rdEventPipe,
#endif
    const void* outBuff, quint32 sizeToWrite,
    quint32 msecsTimeout, int* unixfd )
{
    Q_ASSERT(outBuff);
    Q_ASSERT(sizeToWrite != 0);

    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    const char* buff = reinterpret_cast<const char*>(outBuff);
    quint32 size = sizeToWrite;

#ifdef _WIN_
    // Windows

    Q_UNUSED(unixfd);

    Q_ASSERT(writeEvent != WSA_INVALID_EVENT);
    Q_ASSERT(overlapped != 0);

    // Timeout
    IOService::TimeMark startMark = 0;
    DWORD dwTimeout = INFINITE;

    // Init time mark
    if ( msecsTimeout != 0 )
        IOService::timeMark(startMark);

    WSABUF wsaBuff;
    do {
        ::memset( &wsaBuff, 0, sizeof(wsaBuff) );
        wsaBuff.len = size;
        wsaBuff.buf = const_cast<char*>(buff);

        ::memset( overlapped, 0, sizeof(*overlapped) );
        overlapped->hEvent = writeEvent;

        DWORD written = 0;
        DWORD flags = 0;

        // Send
        int res = ::WSASend( sock, &wsaBuff, 1, &written,
                             0, overlapped, NULL );

        // Socket error
        if ( res == SOCKET_ERROR ) {
            int err = ::WSAGetLastError();

            if ( err != WSA_IO_PENDING ) {
                bool knownErr = (err == WSAECONNABORTED ||
                                 err == WSAECONNRESET);

                if ( knownErr )
                    WRITE_TRACE(DBG_INFO,
                                IO_LOG("Write to socket failed because of "
                                       "known err (native error: %s)"),
                                native_strerror(err, errBuff, ErrBuffSize));
                else
                    WRITE_TRACE(DBG_FATAL,
                                IO_LOG("Write to socket failed (native "
                                       "error: %s)"),
                                native_strerror(err, errBuff, ErrBuffSize));

                if ( err == WSAEFAULT )
                    return IOSendJob::InvalidPackage;

                // Try to detect that socket has been
                // correctly shutdown on the other side
                else if ( err == WSAECONNABORTED || err == WSAECONNRESET )
                    return IOSendJob::ConnClosedByPeer;

                return IOSendJob::Fail;
            }
        }

        // Check if stop in progress.
        // The thing is, that ::WSASend always resets event, so,
        // if writing was stopped (i.e. event signaled) before ::WSASend call,
        // we will infinitely wait in ::WSAWaitForMultipleEvents.
        // So we should check this variable manually.
        if ( threadState == ThreadIsStopping ) {
            LOG_MESSAGE(DBG_INFO, IO_LOG("Write stop in progress"));
            return IOSendJob::ConnClosedByUser;
        }

        // Calc timeout if specified
        if ( msecsTimeout != 0 ) {
            IOService::TimeMark endMark = 0;
            IOService::timeMark(endMark);
            quint32 elapsed = IOService::msecsDiffTimeMark(startMark, endMark);
            qint32 msecsToWait = msecsTimeout - elapsed;

            if ( msecsToWait <= 0 ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Wait for write failed: timeout expired!"));
                return IOSendJob::Fail;
            }
            dwTimeout = msecsToWait;
        }

        // Check, that send has been completed immediately
        if ( res == 0 )
            goto send_is_done;

        // Wait for some time
        DWORD dwWait = ::WSAWaitForMultipleEvents( 1,
                                                   &writeEvent,
                                                   TRUE,
                                                   dwTimeout,
                                                   FALSE ); // not alertable

        // Completed
        if ( dwWait == WSA_WAIT_EVENT_0 ) {
            // Write is completed or stop in progress
            if ( threadState == ThreadIsStopping ) {
                LOG_MESSAGE(DBG_INFO, IO_LOG("Write stop in progress"));
                return IOSendJob::ConnClosedByUser;
            }
        }
        // Still pending
        else {
            if ( dwWait == WSA_WAIT_TIMEOUT ) {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Wait for write failed: timeout expired!"));
                return IOSendJob::Timeout;
            }
            else {
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Wait for write failed (native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));
                return IOSendJob::Fail;
            }
        }

        if ( ! ::WSAGetOverlappedResult(sock, overlapped,
                                        &written, FALSE, &flags) ) {
            int err = ::WSAGetLastError();
            bool knownErr = (err == WSAECONNABORTED ||
                             err == WSAECONNRESET);

            if ( knownErr )
                WRITE_TRACE(DBG_INFO,
                            IO_LOG("Overlapped result: write to socket failed "
                                   "because of known err (native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));
            else
                WRITE_TRACE(DBG_FATAL,
                            IO_LOG("Overlapped result: write to socket failed "
                                   "(native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));

            if ( err == WSAEFAULT )
                return IOSendJob::InvalidPackage;

            // Try to detect that socket has been
            // correctly shutdown on the other side
            else if ( err == WSAECONNABORTED || err == WSAECONNRESET )
                return IOSendJob::ConnClosedByPeer;

            return IOSendJob::Fail;
        }

    send_is_done:
        if ( written == 0 ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Connection was closed unexpectedly"));
            return IOSendJob::Fail;
        }

        size -= written;
        buff += written;

    } while ( size > 0 );

#else
    // Unix

    Q_ASSERT(rdEventPipe != -1);

    IOService::TimeMark startMark = 0;

    // Get time mark if timeout is used
    if ( msecsTimeout != 0 )
        IOService::timeMark(startMark);

    do {
        // Create params for select
        int maxfd = (sock > rdEventPipe ? sock : rdEventPipe) + 1;
        if ( maxfd > m_maxFDNum ) {
            WRITE_TRACE(DBG_FATAL,
                        IO_LOG("Out of resources! Descriptor "
                               "'%d' exceeds max possible '%d'."),
                        maxfd, m_maxFDNum);
            return IOSendJob::Fail;
        }

        // We can't use FD_ZERO here, because
        // of not original fd_set size
        ::memset(m_wrSet.getImpl(), 0, FDNUM2SZ(m_maxFDNum));
        ::memset(m_rdSet.getImpl(), 0, FDNUM2SZ(m_maxFDNum));
        FD_SET(sock, m_wrSet.getImpl());
        FD_SET(rdEventPipe, m_rdSet.getImpl());
        timeval timeout = {0, 0};
        timeval* timeo = 0;

        // Create timeout
        if ( msecsTimeout != 0 ) {
            IOService::TimeMark endMark = 0;
            IOService::timeMark(endMark);
            quint32 elapsed = IOService::msecsDiffTimeMark(startMark, endMark);
            qint32 msecsToWait = msecsTimeout - elapsed;

            if ( msecsToWait <= 0 ) {
                WRITE_TRACE(DBG_FATAL, IO_LOG("Wait for write failed: timeout expired!"));
                return IOSendJob::Fail;
            }

            // Set timeout
            timeout.tv_sec = msecsToWait / 1000;
            timeout.tv_usec = (msecsToWait % 1000) * 1000;
            timeo = &timeout;
        }

        // Do select
        int res = ::select(maxfd, m_rdSet.getImpl(), m_wrSet.getImpl(), 0, timeo);
        if ( res == 0 ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Wait for write failed: timeout expired!"));
            return IOSendJob::Timeout;
        }
        else if ( res < 0 && errno == EINTR ) {
            LOG_MESSAGE(DBG_INFO, IO_LOG("Select has been interrupted"));
            continue;
        }
        else if ( res < 0 ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Select failed (native error: %s)"),
                        native_strerror(errBuff, ErrBuffSize));
            return IOSendJob::Fail;
        }

        // Check stop in progress
        if ( FD_ISSET(rdEventPipe, m_rdSet.getImpl()) ) {
            WRITE_TRACE(DBG_INFO, IO_LOG("Stop in progress for write thread"));
            return IOSendJob::ConnClosedByUser;
        }

        // Send msg
        struct iovec iov;
        struct msghdr msg;
        struct {
            struct cmsghdr head;
            int fd;
        } cmsg;

        ::memset( &msg, 0, sizeof(msg) );
        ::memset( &cmsg, 0, sizeof(cmsg) );
        ::memset( &iov, 0, sizeof(iov) );

        iov.iov_base = const_cast<char*>(buff);
        iov.iov_len = size;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        if ( unixfd != 0 && *unixfd >= 0 ) {
            cmsg.head.cmsg_level = SOL_SOCKET;
            cmsg.head.cmsg_type = SCM_RIGHTS;
            cmsg.head.cmsg_len = CMSG_LEN(sizeof(int));
            *(int *)CMSG_DATA(&cmsg.head) = *unixfd;
            msg.msg_control = (caddr_t)&cmsg;
            msg.msg_controllen = sizeof(cmsg);
            *unixfd = -1;
        }
        ssize_t written = ::sendmsg( sock, &msg, 0 );

        if ( written == 0 ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Connection was closed unexpectedly"));
            return IOSendJob::Fail;
        }
        else if ( written < 0 && errno == EINTR ) {
            LOG_MESSAGE(DBG_INFO, IO_LOG("Write has been interrupted"));
            continue;
        }
        else if ( written < 0 ) {
            int err = errno;
            bool knownErr = (err == EPIPE ||    // other side has been closed
                             err == ECONNRESET);// connection reset

            if ( knownErr )
                WRITE_TRACE(DBG_INFO,
                            IO_LOG("Write to socket failed because of known "
                                   "err (native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));
            else
                WRITE_TRACE(DBG_FATAL, IO_LOG("Write to socket failed (native error: %s)"),
                            native_strerror(errBuff, ErrBuffSize));

            if ( err == EFAULT )
                return IOSendJob::InvalidPackage;

            // Try to detect that socket has been
            // correctly shut down on the other side
            else if ( err == EPIPE || err == ECONNRESET )
                return IOSendJob::ConnClosedByPeer;

            return IOSendJob::Fail;
        }

        size -= written;
        buff += written;

    } while ( size > 0 );
#endif

    // Append written bytes to statistics
    AtomicAdd64(&m_stat.sentBytes, sizeToWrite);

    return IOSendJob::Success;
}

IOSendJob::Result SocketWriteThread::plainWrite (
    int sock,
    const void* buff, quint32 size,
    quint32 msecsTimeout, int* unixfd )
{
#define MARK_TIMEOUT \
    if ( msecsTimeout != 0 ) { \
        IOService::timeMark(startMark); \
    }

#define CALCULATE_TIMEOUT \
    if ( msecsTimeout != 0 ) { \
        IOService::TimeMark endMark = 0; \
        IOService::timeMark(endMark); \
        quint32 elapsed = IOService::msecsDiffTimeMark(startMark, endMark); \
        if ( msecsTimeout <= elapsed ) { \
            WRITE_TRACE(DBG_WARNING, IO_LOG("Write timeout expired!")); \
            return IOSendJob::Fail;                                     \
        } \
        msecsTimeout -= elapsed; \
        IOService::timeMark(startMark); \
     }

    const char* outBuff = reinterpret_cast<const char*>(buff);

    Q_ASSERT(outBuff);
    Q_ASSERT(size != 0);

    // Create SSL header of plain data
    SSLv3Header header;
    header.type = 0xff;
    header.sslVersion = 3;

    quint32 iters = size / SSLMaxDataLength;
    quint32 mod = size % SSLMaxDataLength;
    IOService::TimeMark startMark = 0;

    // Default size for iters
    header.sslDataLength = htons(SSLMaxDataLength);

    MARK_TIMEOUT

    for ( quint32 i = 0; i < iters; ++i ) {

        // Write header
        IOSendJob::Result res = write( sock,
#ifdef _WIN_ // Windows
                                       m_threadState,
                                       &m_sockOverlappedWrite,
                                       m_writeEventHandle,
#else // Unix
                                       m_eventPipes[0],
#endif
                                       &header, sizeof(header),
                                       msecsTimeout, unixfd );
        if ( res != IOSendJob::Success )
            return res;

        CALCULATE_TIMEOUT

        // Write body
        res = write( sock,
#ifdef _WIN_ // Windows
                     m_threadState,
                     &m_sockOverlappedWrite,
                     m_writeEventHandle,
#else // Unix
                     m_eventPipes[0],
#endif
                     outBuff, SSLMaxDataLength,
                     msecsTimeout, unixfd );
        if ( res != IOSendJob::Success )
            return res;

        CALCULATE_TIMEOUT

        outBuff += SSLMaxDataLength;
    }

    // Write remains if we have
    if ( mod ) {
        // Write header
        header.sslDataLength = htons(mod);
        IOSendJob::Result res = write( sock,
#ifdef _WIN_ // Windows
                                       m_threadState,
                                       &m_sockOverlappedWrite,
                                       m_writeEventHandle,
#else // Unix
                                       m_eventPipes[0],
#endif
                                       &header, sizeof(header),
                                       msecsTimeout, unixfd );
        if ( res != IOSendJob::Success )
            return res;

        CALCULATE_TIMEOUT

        // Write body
        res = write( sock,
#ifdef _WIN_ // Windows
                     m_threadState,
                     &m_sockOverlappedWrite,
                     m_writeEventHandle,
#else // Unix
                     m_eventPipes[0],
#endif
                     outBuff, mod,
                     msecsTimeout, unixfd );
        if ( res != IOSendJob::Success )
            return res;
    }

    return IOSendJob::Success;
}

IOSendJob::Result SocketWriteThread::sslWrite (
    int sock,
#ifdef _WIN_ // Windows
    volatile ThreadState& threadState,
    WSAOVERLAPPED* overlapped,
    WSAEVENT writeEvent,
#else // Unix
    int rdEventPipe,
#endif
	const void* outBuff, quint32 size,
    quint32 msecsTimeout, int* unixfd )
{
    Q_ASSERT(outBuff);
    Q_ASSERT(size != 0);

    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    // Check that SSL is turned on
    Q_ASSERT(m_ssl && m_sslSSLBio && m_sslNetworkBio);

    const char* buff = reinterpret_cast<const char*>(outBuff);

    // Write to engine
    qint32 offsetToWrite = 0;
    qint32 shouldBeWritten = size;

    do {
        // Lock
        QMutexLocker locker( &m_jobMutex );

        qint32 toWrite = BIO_get_write_guarantee(m_sslSSLBio);
        toWrite = qMin(toWrite, (shouldBeWritten - offsetToWrite));

        qint32 written = BIO_write( m_sslSSLBio, buff + offsetToWrite, toWrite );

        // Check for WANT_WRITE
        if ( written == -1 ) {
            unsigned long err = ERR_get_error();
            if ( ! BIO_should_retry(m_sslSSLBio) ) {
                ERR_error_string_n( err, errBuff, ErrBuffSize );
                WRITE_TRACE(DBG_FATAL, IO_LOG("Error in SSL write: 'BIO_write(toWrite=%d)'"
                                   " failed, but 'BIO_should_retry' returned "
                                   "false (SSL error: %s)"), toWrite, errBuff);
                return IOSendJob::Fail;
            }
            // Ok. Want write
            if ( BIO_should_write(m_sslSSLBio) )
                toWrite = 0;
            else {
                ERR_error_string_n( err, errBuff, ErrBuffSize );
                WRITE_TRACE(DBG_FATAL, IO_LOG("Error in SSL write: "
                                   "'BIO_should_read' = %s ? (SSL error: %s)"),
                            (BIO_should_read(m_sslSSLBio) ? "true" : "false"),
                            errBuff);
                return IOSendJob::Fail;
            }
        }
        else
            toWrite = written;

        // Unlock
        locker.unlock();

        // Update offset
        offsetToWrite += toWrite;

        // Write from SSL
        IOSendJob::Result res = sslWriteFromNetworkBio( sock,
#ifdef _WIN_ // Windows
                                                        threadState,
                                                        overlapped,
                                                        writeEvent,
#else // Unix
                                                        rdEventPipe,
#endif
                                                        msecsTimeout,
                                                        unixfd );
        if ( res != IOSendJob::Success )
            return res;

    } while ( (shouldBeWritten - offsetToWrite) > 0 );

    return IOSendJob::Success;
}

IOSendJob::Result SocketWriteThread::sslWriteFromNetworkBio (
    int sock,
#ifdef _WIN_ // Windows
    volatile ThreadState& threadState,
    WSAOVERLAPPED* overlapped,
    WSAEVENT writeEvent,
#else // Unix
    int rdEventPipe,
#endif
    quint32 msecsTimeout,
    int* unixfd )
{
    // NOTE: we do not need any locks here.
    //       we work only with buffer and not with SSL engine.

    // Check that SSL is turned on
    Q_ASSERT(m_ssl && m_sslSSLBio && m_sslNetworkBio);

    // Read to static buffer and write to socket
    do {
        qint32 pending = BIO_pending(m_sslNetworkBio);
        Q_ASSERT( pending > 0 );

        char* buffIn = 0;
        qint32 read = BIO_nread(m_sslNetworkBio, &buffIn, pending);
        Q_ASSERT( buffIn );

        IOSendJob::Result res = write( sock,
#ifdef _WIN_ // Windows
                                       threadState,
                                       overlapped,
                                       writeEvent,
#else // Unix
                                       rdEventPipe,
#endif
                                       buffIn, read, msecsTimeout, unixfd );

        if ( res != IOSendJob::Success )
            return res;

    } while ( BIO_pending(m_sslNetworkBio) > 0 );

    return IOSendJob::Success;
}

SmartPtr<IOSendJob> SocketWriteThread::sendPackage (
    const SmartPtr<IOPackage>& p,
    bool urgentSend )
{
    // Check if package is invalid
    if ( ! p.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Error: package is invalid!"));
        return SmartPtr<IOSendJob>();
    }

    // Lock
    QMutexLocker locker( &m_jobMutex );

    // Check if write thread is ready
    if ( m_threadState != ThreadIsStarted ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Error: write thread is not started!"));
        return SmartPtr<IOSendJob>();
    }

    // Check that it is not in detaching state
    if ( m_isDetaching && ! urgentSend ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Error: write thread is detaching! "
                                      "It  can be only stopped!"));
        return SmartPtr<IOSendJob>();
    }

    Q_ASSERT(m_jobPool.isValid());

    // Init job
    IOJobManager::Job* job = 0;
    bool initRes = m_jobManager->initActiveJob( m_jobPool, p->header,
                                                p, job, urgentSend );
    // If success, wake up writing thread
    if ( initRes ) {
        Q_ASSERT(job);
        // Init header uuids
        setUuidsToPkgHeaderAndRegisterJob( job->pkgHeader, job->sendJob );
        // Wake writing thread
        m_wait.wakeOne();
    }
    // Can't init, queue is full
    else {
        if ( job == 0 ) {
            WRITE_TRACE(DBG_FATAL, IO_LOG("Error: can't allocate new job!"));
            return SmartPtr<IOSendJob>();
        }

        job->sendJob->wakeSendWaitings( IOSendJob::SendQueueIsFull );
        job->sendJob->wakeResponseWaitings( IOSendJob::SendQueueIsFull,
                                            IOSender::Handle(),
                                            SmartPtr<IOPackage>() );
    }

    return job->sendJob;
}

void SocketWriteThread::run ()
{
    // Backtrace will show us what context is used
    if ( m_ctx == Cli_ServerContext )
        doServerCtxJob();
    else if ( m_ctx == Cli_ClientContext )
        doClientCtxJob();
    else if ( m_ctx == Cli_ProxyMngContext )
        // Write thread is not started for proxy mng context
        Q_ASSERT(false);
}

// Backtrace will show us what context is used
void SocketWriteThread::doServerCtxJob ()
{
    doJob();
}

// Backtrace will show us what context is used
void SocketWriteThread::doClientCtxJob ()
{
    doJob();
}

void SocketWriteThread::doJob ()
{
    // Check that SSL is turned on
    Q_ASSERT(m_ssl && m_sslSSLBio && m_sslNetworkBio);

    // Allocate job pool
    SmartPtr<IOJobManager::JobPool> jobPool = m_jobManager->initJobPool();

    // Default write res
    IOSendJob::Result writeRes = IOSendJob::Success;

    // Statistics
    IOSender::Statistics stat;
    ::memset( &stat, 0, sizeof(stat) );

    // Heart beat flags
    IOService::TimeMark lastHeartBeatMark = 0;
    bool heartBeatSupport =  IOPROTOCOL_HEART_BEAT_SUPPORT(m_peerProtoVersion);

#ifdef _WIN_ // Windows
    const size_t ErrBuffSize = 256;
    char errBuff[ ErrBuffSize ];

    m_writeEventHandle = ::WSACreateEvent();
    if ( m_writeEventHandle == WSA_INVALID_EVENT ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't create write event (native error: %s)"),
                    native_strerror(errBuff, ErrBuffSize) );
        goto cleanup_and_disconnect;
    }
#else // Unix
    // Check allocations
    if ( ! m_rdSet.isValid() || ! m_wrSet.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
        goto cleanup_and_disconnect;
    }
#endif

    // Check job pool
    if ( ! jobPool.isValid() ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't allocate memory!"));
        goto cleanup_and_disconnect;
    }

    // Lock
    m_jobMutex.lock();

    Q_ASSERT(m_threadState == ThreadIsStarting);
    Q_ASSERT(m_sockHandle != -1);
    Q_ASSERT(! m_currConnUuid.isNull());
#ifdef _WIN_ // Windows
    Q_ASSERT(m_writeEventHandle != WSA_INVALID_EVENT);
#else // Unix
    Q_ASSERT(m_eventPipes[0] != -1);
    Q_ASSERT(m_eventPipes[1] != -1);
#endif

    // Drop pause and detach flags
    m_isDetaching = false;
    m_inPause = false;
    m_pausePkg = SmartPtr<IOPackage>();

    // Init job pool
    m_jobPool = jobPool;

    // Set connected state
    m_state = IOSender::Connected;

    // Mark as started
    m_threadState = ThreadIsStarted;
    m_threadStateWait.wakeOne();

    // Unlock
    m_jobMutex.unlock();

    // Last heart beat mark
    if ( heartBeatSupport )
        IOService::timeMark(lastHeartBeatMark);

    while ( 1 ) {
        // Lock job mutex
        QMutexLocker locker( &m_jobMutex );

        // Stop in progress
        if ( m_threadState == ThreadIsStopping )
           goto cleanup_and_disconnect;

        // Get pending
        qint32 pending = BIO_pending(m_sslNetworkBio);

        // Next job
        IOJobManager::Job* jobPtr = 0;

        // Calc heart beat if it is supported
        unsigned long heartBeatTimeout = ULONG_MAX;
        if ( heartBeatSupport ) {
            IOService::TimeMark nowMark = 0;
            IOService::timeMark(nowMark);
            quint32 elapsed =
                IOService::msecsDiffTimeMark(lastHeartBeatMark, nowMark);
            // Send heart beat now
            if ( elapsed >= IOCommunication::IOHeartBeatSendTimeout )
                jobPtr = m_jobManager->getHeartBeatJob( m_jobPool );
            // Calc heart beat timeout
            else {
                heartBeatTimeout = IOCommunication::IOHeartBeatSendTimeout - elapsed;
                jobPtr = m_jobManager->getNextActiveJob( m_jobPool );
            }
        }
        // Get next job
        else
            jobPtr = m_jobManager->getNextActiveJob( m_jobPool );

        if ( jobPtr == 0 && pending <= 0 ) {
            bool waitRes = m_wait.wait( &m_jobMutex, heartBeatTimeout );

            // Stop in progress
            if ( m_threadState == ThreadIsStopping )
                goto cleanup_and_disconnect;

            // Timeout? We should send heart beat
            if ( ! waitRes )
                jobPtr = m_jobManager->getHeartBeatJob( m_jobPool );
        }

        // Or get next job again if 0
        if ( jobPtr == 0 )
            jobPtr = m_jobManager->getNextActiveJob( m_jobPool );

        // Unlock
        locker.unlock();

        // We are unlocked here because we do not work with
        // SSL engine, only with buffer.

        // Check pending
        pending = BIO_pending(m_sslNetworkBio);
        LOG_MESSAGE(DBG_INFO, IO_LOG("SSL pending to write %d"), pending);

        if ( pending ) {
            // Write from SSL
            sslWriteFromNetworkBio(
                m_sockHandle,
#ifdef _WIN_ // Windows
                m_threadState,
                &m_sockOverlappedWrite,
                m_writeEventHandle
#else // Unix
                m_eventPipes[0]
#endif
                );

            // If nothing to write for user
            if ( jobPtr == 0 )
                continue;
        }

        // Relock
        locker.relock();


        // Do not write user packages if handshake
        if ( SSL_in_init(m_ssl) )
            continue;

        // Double check
        if ( jobPtr == 0 )
            continue;

        // Unlock
        locker.unlock();

        if ( jobPtr == m_jobManager->getHeartBeatJob(m_jobPool) )
            LOG_MESSAGE(DBG_DEBUG, IO_LOG("Heart beat has been sent!"));

        // Increment package reference, to be sure it can't
        // be freed in job destruction
        SmartPtr<IOPackage> p = jobPtr->pkg;
        Q_ASSERT( p.isValid() );

        // Before write call
        {
            CALLBACK_MARK;
            if ( p->callback.beforeSendCall ) {
                p->callback.beforeSendCall( false, p->callback.sendContext,
                                            m_currConnUuid,
                                            m_peerConnUuid,
                                            IOSendJob::Success, p );
                WARN_IF_CALLBACK_TOOK_MUCH_TIME;
            }
            m_wrListener->onBeforeWrite( this, p );
            WARN_IF_CALLBACK_TOOK_MUCH_TIME;
        }

        // Find route name
        IORoutingTable::RouteName routeName =
            m_routingTable.findRoute( p->header.type );

        // Unix fd (works only on Unix)
        int unixfd = -1;

#ifndef _WIN_
        if ( p->header.type == IOCommunicationMngPackage::AttachClient ) {
            unixfd = IOCommunication::getUnixfdFromAttachClientPackage( p );
            if ( unixfd == -1 ) {
                LOG_MESSAGE(DBG_WARNING, IO_LOG("Wrong unix descriptor!"));
            }
        }
#endif

        // Drop again to success
        writeRes = IOSendJob::Success;

        if ( routeName == IORoutingTable::SSLRoute )
            // Write secured header
            writeRes = sslWrite( m_sockHandle,
#ifdef _WIN_ // Windows
                                 m_threadState,
                                 &m_sockOverlappedWrite,
                                 m_writeEventHandle,
#else // Unix
                                 m_eventPipes[0],
#endif
                                 &jobPtr->pkgHeader,
                                 sizeof(IOPackage::PODHeader),
                                 0, &unixfd );
        else
            // Write plain header
            writeRes = plainWrite( m_sockHandle, &jobPtr->pkgHeader,
                                   sizeof(IOPackage::PODHeader),
                                   0, &unixfd );

        // Error
        if ( writeRes != IOSendJob::Success ) {

            // NOTE: for now 'writeRes' is not used for detecting the reason
            //       of write thread stop! will be implemented in future ...
            IOSendJob::Result wrRes = writeRes;
            if ( wrRes == IOSendJob::ConnClosedByPeer ||
                 wrRes == IOSendJob::ConnClosedByUser )
                wrRes = IOSendJob::Fail;

            // After write call
            {
                CALLBACK_MARK;
                if ( p->callback.afterSendCall ) {
                    p->callback.afterSendCall( true, p->callback.sendContext,
                                               m_currConnUuid,
                                               m_peerConnUuid,
                                               wrRes, p );
                    WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                }
                m_wrListener->onAfterWrite( this, wrRes, p );
                WARN_IF_CALLBACK_TOOK_MUCH_TIME;
            }

            jobPtr->sendJob->wakeSendWaitings( wrRes );
            jobPtr->sendJob->wakeResponseWaitings( wrRes,
                                                   IOSender::Handle(),
                                                   SmartPtr<IOPackage>() );
            m_jobManager->putActiveJob( m_jobPool, jobPtr );
            jobPtr = 0;
            goto cleanup_and_disconnect;
        }

		quint64 sent_sz = sizeof(IOPackage::PODHeader);

        // Send buffers if exist
        if ( p->header.buffersNumber ) {

            const IOPackage::PODData* pkgData = IODATAMEMBER(p);

            if ( routeName == IORoutingTable::SSLRoute )
                // Write secured buffers data
                writeRes = sslWrite( m_sockHandle,
#ifdef _WIN_ // Windows
                                     m_threadState,
                                     &m_sockOverlappedWrite,
                                     m_writeEventHandle,
#else // Unix
                                     m_eventPipes[0],
#endif
                                     pkgData, IODATASIZE(p) );
            else
                // Write plain buffers data
                writeRes = plainWrite( m_sockHandle, pkgData, IODATASIZE(p) );

            // Error
            if ( writeRes != IOSendJob::Success ) {

                // NOTE: for now 'writeRes' is not used for detecting the reason
                //       of write thread stop! will be implemented in future ...
                IOSendJob::Result wrRes = writeRes;
                if ( wrRes == IOSendJob::ConnClosedByPeer ||
                     wrRes == IOSendJob::ConnClosedByUser )
                    wrRes = IOSendJob::Fail;

                // After write call
                {
                    CALLBACK_MARK;
                    if ( p->callback.afterSendCall ) {
                        p->callback.afterSendCall( true, p->callback.sendContext,
                                                   m_currConnUuid,
                                                   m_peerConnUuid,
                                                   wrRes, p );
                        WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                    }
                    m_wrListener->onAfterWrite( this, wrRes, p );
                    WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                }

                jobPtr->sendJob->wakeSendWaitings( wrRes );
                jobPtr->sendJob->wakeResponseWaitings( wrRes,
                                                       IOSender::Handle(),
                                                       SmartPtr<IOPackage>() );
                m_jobManager->putActiveJob( m_jobPool, jobPtr );
                jobPtr = 0;
                goto cleanup_and_disconnect;
            }

			sent_sz += IODATASIZE(p);

            // Write buffers
            for ( quint32 i = 0; i < p->header.buffersNumber; ++i ) {

                LOG_MESSAGE(DBG_INFO, IO_LOG("Writing buffer #%d with size %d"),
                            i, pkgData[i].bufferSize);

                if ( pkgData[i].bufferSize == 0 ) {
                    LOG_MESSAGE(DBG_INFO, IO_LOG("Buffer #%d size is zero!"));
                    continue;
                }

                if ( routeName == IORoutingTable::SSLRoute )
                    // Write secured buffers
                    writeRes = sslWrite( m_sockHandle,
#ifdef _WIN_ // Windows
                                         m_threadState,
                                         &m_sockOverlappedWrite,
                                         m_writeEventHandle,
#else // Unix
                                         m_eventPipes[0],
#endif
                                         p->buffers[i].getImpl(),
                                         pkgData[i].bufferSize );
                else
                    // Write plain buffers
                    writeRes = plainWrite( m_sockHandle,
                                           p->buffers[i].getImpl(),
                                           pkgData[i].bufferSize );

                // Error
                if ( writeRes != IOSendJob::Success ) {

                    // NOTE: for now 'writeRes' is not used for detecting the reason
                    //       of write thread stop! will be implemented in future ...
                    IOSendJob::Result wrRes = writeRes;
                    if ( wrRes == IOSendJob::ConnClosedByPeer ||
                         wrRes == IOSendJob::ConnClosedByUser )
                        wrRes = IOSendJob::Fail;

                    // After write call
                    {
                        CALLBACK_MARK;
                        if ( p->callback.afterSendCall ) {
                            p->callback.afterSendCall( true, p->callback.sendContext,
                                                       m_currConnUuid,
                                                       m_peerConnUuid,
                                                       wrRes, p );
                            WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                        }
                        m_wrListener->onAfterWrite( this, wrRes, p );
                        WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                    }

                    jobPtr->sendJob->wakeSendWaitings( wrRes );
                    jobPtr->sendJob->wakeResponseWaitings( wrRes,
                                                           IOSender::Handle(),
                                                           SmartPtr<IOPackage>() );
                    m_jobManager->putActiveJob( m_jobPool, jobPtr );
                    jobPtr = 0;
                    goto cleanup_and_disconnect;
                }

				sent_sz += pkgData[i].bufferSize;
            }
        }

        // Increment statistics value
        AtomicInc64(&m_stat.sentPackages);

		ETRACE_LOG(m_sockHandle & 0xFF, ETRACE_CP_IOSERVICE,
			((quint64)p->header.type << 32) |
			(((quint32)sent_sz & 0x3FFFF) << 14) |
			(((quint16)p->header.buffersNumber & 0xF) << 10) |
			(((quint16)m_senderType & 0xF) << 6) |
			((routeName == IORoutingTable::SSLRoute ?
				 ETRACE_IOS_EVENT_SEND_PKG_SSL : ETRACE_IOS_EVENT_SEND_PKG) & 0x3F));

        // After write call
        {
            CALLBACK_MARK;
            if ( p->callback.afterSendCall ) {
                p->callback.afterSendCall( true, p->callback.sendContext,
                                           m_currConnUuid,
                                           m_peerConnUuid,
                                           IOSendJob::Success, p );
                WARN_IF_CALLBACK_TOOK_MUCH_TIME;
            }
            m_wrListener->onAfterWrite( this, IOSendJob::Success, p );
            WARN_IF_CALLBACK_TOOK_MUCH_TIME;
        }

        // Wake with success
        jobPtr->sendJob->wakeSendWaitings( IOSendJob::Success );

        // Heart beat has been sent! Mark this timestamp
        if ( jobPtr == m_jobManager->getHeartBeatJob(m_jobPool) )
            IOService::timeMark(lastHeartBeatMark);

        // Mark as not active just after all access to this job.
        m_jobManager->putActiveJob( m_jobPool, jobPtr );
        jobPtr = 0;

        // NOTE: Job ptr can't be used any more.
        //       Package 'p' can be used, because it is created
        //       by another SmartPtr instance.

        // Check if want to send and pause
        if ( m_inPause ) {
            // Lock
            QMutexLocker locker( &m_jobMutex );
            // Only `continue writing` and `stop write` calls
            // can wake us, e.g. read thread can wake up write
            // thread in case of SSL reading, but flags will be
            // the same.
            while ( m_threadState == ThreadIsStarted &&
                    m_inPause && m_pausePkg.isValid() && m_pausePkg == p ) {
                m_wait.wait( &m_jobMutex );
            }

            // Stop in progress
            if ( m_threadState == ThreadIsStopping )
                goto cleanup_and_disconnect;
        }
    }

 cleanup_and_disconnect:

    //
    // Finalize write thread
    //

    //
    // Save state that we are disconnected
    //
    IOSender::State oldState = m_state;
    // Default stop reason for now
    IOSendJob::Result stopReason = IOSendJob::Fail;
    {
        // Lock
        QMutexLocker locker( &m_jobMutex );

        // Set disconneted state
        m_state = IOSender::Disconnected;
    }

    // If we were successfully started:
    //   check write result,
    //   wake up all active jobs
    if ( oldState == IOSender::Connected ) {

        // If writing thread is stopped from outside, last writing result
        // can be 'Success', so we must mark it as closed by user
        // NOTE: for now 'writeRes' is not used for detecting the reason
        //       of write thread stop! will be implemented in future ...
        if ( writeRes == IOSendJob::Success )
            writeRes = IOSendJob::ConnClosedByUser;

        // Mark all busy jobs with fail result of sending operation
        QList<IOJobManager::Job*> sendJobs =
            m_jobManager->getBusySendJobs( m_jobPool );
        foreach ( IOJobManager::Job* jobPtr, sendJobs ) {
            SmartPtr<IOSendJob>& job = jobPtr->sendJob;
            // Wake send waitings
            if ( ! job->sendWaitingsWereWaken() ) {
                SmartPtr<IOPackage>& p = jobPtr->pkg;
                Q_ASSERT(p.isValid());

                //
                // All active jobs must be waken by common result
                // (smb has gone wrong, or connection was closed),
                // not by result of last write, e.g. invalid package result.
                // Note: we do not wake up response waiters.
                //       this should be done by read thread.
                //
                // Here we call only package callbacks, we do not call
                // onBeforeWrite or onAfterWrite.
                {
                    CALLBACK_MARK;
                    if ( p->callback.beforeSendCall ) {
                        p->callback.beforeSendCall( false, p->callback.sendContext,
                                                    m_currConnUuid,
                                                    m_peerConnUuid,
                                                    IOSendJob::Fail, p );
                        WARN_IF_CALLBACK_TOOK_MUCH_TIME;
                    }
                }

                job->wakeSendWaitings( IOSendJob::Fail );
            }
        }
    }

    // Lock
    m_jobMutex.lock();

#ifdef _WIN_ // Windows

    // Close event
    if ( m_writeEventHandle != WSA_INVALID_EVENT ) {
        ::WSACloseEvent(m_writeEventHandle);
        m_writeEventHandle = WSA_INVALID_EVENT;
    }
#else // Unix

    // Just invalidate, not close
    m_eventPipes[0] = -1;
    m_eventPipes[1] = -1;
#endif

    // Clean
    m_sockHandle = -1;
    m_currConnUuid = Uuid();
    m_peerConnUuid = Uuid();
    m_peerProtoVersion = IOService::IOProtocolVersion;
    m_routingTable = IORoutingTable();
    m_ssl = 0;
    m_sslSSLBio = 0;
    m_sslNetworkBio = 0;

    // Mark as stopped!
    m_threadState = ThreadIsStopped;
    m_stopReason = stopReason;
    m_threadStateWait.wakeOne();

    // Unlock
    m_jobMutex.unlock();

    // If we were in connected state and write has finished with any error:
    //    we should emit signal
    if ( oldState == IOSender::Connected &&
         writeRes != IOSendJob::ConnClosedByPeer &&
         writeRes != IOSendJob::ConnClosedByUser )
        m_wrListener->onWriteIsFinishedWithErrors( this );
}

bool SocketWriteThread::sendDetachRequestAndPauseWriting ()
{
    // Send and pause writing. Thread will be in detaching state.
    return sendAndPauseWriting( IOPackage::createInstance(
                         IOCommunicationMngPackage::DetachClientRequest, 0),
                                true );
}

bool SocketWriteThread::sendDetachResponseAndPauseWriting ()
{
    // Send and pause writing. Thread will wait for continue writing call.
    return sendAndPauseWriting( IOPackage::createInstance(
                         IOCommunicationMngPackage::DetachClientResponse, 0),
                                false );
}

bool SocketWriteThread::sendAndPauseWriting ( const SmartPtr<IOPackage>& p,
                                              bool isDetaching )
{
    Q_ASSERT( p.isValid() );

    // Lock
    QMutexLocker locker( &m_jobMutex );

    if ( m_inPause ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Writing thread is paused!"));
        return false;
    }
    m_isDetaching = isDetaching;
    m_inPause = true;
    m_pausePkg = p;

    // Unlock
    locker.unlock();

    // Urgent send
    SmartPtr<IOSendJob> job = sendPackage( m_pausePkg, true );

    // Check send result
    if ( job.isValid() ) {
        bool urgentlyWaked = false;
        job->waitForSend(UINT_MAX, urgentlyWaked);
        Q_ASSERT(urgentlyWaked == false);
        IOSendJob::Result sendRes = job->getSendResult();
        if ( sendRes == IOSendJob::Success )
            return true;
    }

    // Lock
    locker.relock();

    // Drop all pause flags in case of error
    m_isDetaching = false;
    m_inPause = false;
    m_pausePkg = SmartPtr<IOPackage>();
    return false;
}

void SocketWriteThread::continueWriting ()
{
    // Lock
    QMutexLocker locker( &m_jobMutex );
    if ( m_isDetaching ) {
        WRITE_TRACE(DBG_FATAL,
                    IO_LOG("Can't continue writing from detaching state! "
                           "Writing thread can be only stopped!"));
        return;
    }
    // Drop all pause flags and wake up
    m_inPause = false;
    m_pausePkg = SmartPtr<IOPackage>();
    m_wait.wakeOne();
}

bool SocketWriteThread::initSSL ( SSL* ssl, BIO* sslBio, BIO* networkBio )
{
    // Lock
    QMutexLocker locker( &m_jobMutex );
    // Must be started and paused
    if ( m_threadState == ThreadIsStarted && ! m_inPause ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't init SSL. Write thread is started but "
                           "not paused!"));
        return false;
    }
    // Must be stopped or started
    else if ( m_threadState != ThreadIsStarted &&
              m_threadState != ThreadIsStopped ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't init SSL. Thread state is %d!"),
                    m_threadState);
        return false;
    }

    m_ssl = ssl;
    m_sslSSLBio = sslBio;
    m_sslNetworkBio = networkBio;
    return true;
}

bool SocketWriteThread::deinitSSL ()
{
    // Lock
    QMutexLocker locker( &m_jobMutex );
    // Must be started and paused
    if ( m_threadState == ThreadIsStarted && ! m_inPause ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't deinit SSL. Write thread is started but "
                           "not paused!"));
        return false;
    }
    // Must be stopped or started
    else if ( m_threadState != ThreadIsStarted &&
              m_threadState != ThreadIsStopped ) {
        WRITE_TRACE(DBG_FATAL, IO_LOG("Can't deinit SSL. Thread state is %d!"),
                    m_threadState);
        return false;
    }

    m_ssl = 0;
    m_sslSSLBio = 0;
    m_sslNetworkBio = 0;
    return true;
}

QMutex* SocketWriteThread::getSSLMutex ()
{
    return &m_jobMutex;
}

void SocketWriteThread::wakeSSLWriter ()
{
    m_wait.wakeOne();
}

/*****************************************************************************/
