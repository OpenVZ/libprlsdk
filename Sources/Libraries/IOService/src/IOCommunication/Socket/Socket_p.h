/*
 * Socket_p.h
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


#ifndef SOCKETP_H
#define SOCKETP_H

#ifdef _WIN_
 #include <WinSock2.h>
 #include <WS2tcpip.h>
 #include <Wspiapi.h>	// for getnameinfo() on Windows 2000
#else
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <sys/socket.h>
 #include <sys/resource.h>
 #include <sys/un.h>
 #include <signal.h>
 #include <unistd.h>
 #include <arpa/inet.h>
 #include <errno.h>
 #include <fcntl.h>
 #include <netdb.h>
 #include <dirent.h>
 #include <netinet/tcp.h>
#endif

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>

#include "../IOConnection.h"
#include "SocketListeners_p.h"
#include "SslHelper.h"

#include "Libraries/Logging/Logging.h"

// Define flags and types for Windows
#ifdef _WIN_
  typedef int socklen_t;

  #ifndef AI_ADDRCONFIG
    #define AI_ADDRCONFIG 0x0400
  #endif
  #ifndef IPV6_V6ONLY
    #define IPV6_V6ONLY 27
  #endif
  #ifndef SHUT_RDWR
    #define SHUT_RDWR SD_BOTH
  #endif
  #ifndef SHUT_WR
    #define SHUT_WR SD_SEND
  #endif
  #ifndef SHUT_RD
    #define SHUT_RD SD_RECV
  #endif
#endif

// Just prefix to all IO logging strings
#define DEFINE_IO_LOG QString m_ioStrBgn;
#define IO_LOG(str) "%s" str, qPrintable(m_ioStrBgn)
#define INIT_IO_LOG(str) do { m_ioStrBgn = (str); } while( 0 )

#define CALLBACK_MARK \
    IOService::TimeMark callbackStartMark__; \
    IOService::timeMark(callbackStartMark__);

#define WARN_IF_CALLBACK_TOOK_MUCH_TIME \
    IOService::TimeMark callbackEndMark__; \
    IOService::timeMark(callbackEndMark__); \
    quint32 callbackElapsed__ = \
        IOService::msecsDiffTimeMark(callbackStartMark__, callbackEndMark__); \
    callbackStartMark__ = callbackEndMark__;                            \
    if ( callbackElapsed__ >= IOService::IOCommunication::IOCallbackPossibleTime ) \
        WRITE_TRACE(DBG_FATAL,                                \
            IO_LOG("WARNING: callback took too much time: "             \
                   "about %d msecs. This is absolutely incorrect! "     \
                   "Callback must be rewritten!"),                      \
                    callbackElapsed__);                                 \

namespace IOService {

// Error description
struct ErrDesc
{
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    int error;
    int codeLine;
};

// Thread states flag
enum ThreadState { ThreadIsStopped = 0,
                   ThreadIsStopping,
                   ThreadIsStarting,
                   ThreadIsStarted };

// Socket client contexts
enum SocketClientContext { Cli_ClientContext = 0,
                           Cli_ServerContext,
                           Cli_ProxyMngContext };

// Socket server contexts
enum SocketServerContext { Srv_AttachingContext = 0,
                           Srv_ListeningContext,
                           Srv_ProxyMngContext };

int native_error ();
const char* native_strerror ( int err, char* buff, size_t size );
const char* native_strerror ( char* buff, size_t size );

/** Platform independent socketpair call */
bool socketpair ( int sv[2] );

/**
 * Prevent sockets from inheritance in child process
 * Unix:   set FD_CLOEXEC flag on socket
 * Windows: set no inheritance flag on handle
 */
bool setCloseOnExec ( int );

/**
 * Setup default socket options
 */
bool setSocketOptions ( int );

/**
 * Returns numeric host name and numeric port number of the sockaddr
 */
bool getFullNameInfo ( const struct sockaddr *sa, socklen_t salen,
                       QString& hostNameStr,
                       quint16& portNumber,
                       QString& errStr );

/**
 * Returns numeric host name and numeric port number
 * to which the socket sockfd is bound.
 */
bool getHostInfo ( int sockfd, QString& hostName,
                   quint16& portNumber, QString& err );

/**
 * Returns numeric host name and numeric port number
 * of the peer connected to socket sockfd.
 */
bool getPeerInfo ( int sockfd, QString& hostName,
                   quint16& portNumber, QString& err );

#ifndef _WIN_ //Unix

#define MAX_FDNUM 65536
#define FDNUM2SZ(fdNum) ( ((fdNum) + 7) / 8 )

/** Returns max FD size according to RLIMIT_NOFILE resource */
int getMaxFDNumber ();

/** Returns allocated fd_set according fd num */
SmartPtr<fd_set> allocFDSet ( int fdNum );

#else //Windows

class WSAInitHelper
{
public:
    static bool InitWSA ();
    static void DeinitWSA ();
private:
    static volatile int s_inited;
    static QMutex  s_initMutex;
};

#endif

enum OrderPreference {
    OP_PreferIPv4 = 0,
    OP_PreferIPv6
};
/** Order addrinfo with specified preference */
QList<addrinfo*> orderAddrInfo ( addrinfo* addrInfo,
                                 OrderPreference orderPref );

/** Detached client state */
struct DetachedClientState
{
#ifdef _WIN_
    typedef WSAPROTOCOL_INFO SocketState;
#else
    typedef int SocketState;
#endif

#include <Interfaces/packed.h>
    struct PODHeader
    {
        IOCommunication::ProtocolVersion clientProtocolVersion;
        IOCommunication::HandshakeHeader clientHandshakeHeader;
        SocketState clientSocket;
        quint32 routingTableBuffSize;
        quint32 sslSessionSize;
        quint32 readAheadSize;

    } PACKED header;
#include <Interfaces/unpacked.h>

    struct Data
    {
        SmartPtr<char> routingTableBuff;
        SmartPtr<char> sslSession;
        SmartPtr<IOPackage> additionalPkg;
        SmartPtr<char> readAhead;

    } data;

    /** Constructor */
    DetachedClientState ();
};

#include <Interfaces/packed.h>
struct SSLv3Header
{
    quint8 type;
    quint16 sslVersion;
    quint16 sslDataLength;

} PACKED;
#include <Interfaces/unpacked.h>

// Write thread implementation
class SocketWriteThread : protected QThread
{
    Q_OBJECT
public:
    SocketWriteThread ( SmartPtr<IOJobManager> jobManager,
                        IOSender::Type senderType,
                        SocketClientContext ctx,
                        IOSender::Statistics& stat,
                        SocketWriteListenerInterface* );
    ~SocketWriteThread ();

    SmartPtr<IOSendJob> sendPackage ( const SmartPtr<IOPackage>& p,
                                      bool urgentSend );

    bool startWriteThread ( int sockHandle,
                            const Uuid& currConnUuid,
                            const Uuid& peerConnUuid,
                            const IOCommunication::ProtocolVersion& protoVer,
                            const IORoutingTable& routingTable
#ifndef _WIN_ // Unix
                            , int eventPipes[2]
#endif
                            );

    IOSendJob::Result stopWriteThread ();

    bool sendDetachRequestAndPauseWriting ();
    bool sendDetachResponseAndPauseWriting ();
    void continueWriting ();

    SmartPtr<IOJobManager::JobPool> getJobPool () const;

    IOSendJob::Result write ( int sock,
#ifdef _WIN_ // Windows
                              volatile ThreadState& threadState,
                              WSAOVERLAPPED* overlapped,
                              WSAEVENT writeEvent,
#else // Unix
                              int rdEventPipe,
#endif
                              const void* outBuff, quint32 size,
                              quint32 timeoutMsecs, int* unixfd = 0 );

    IOSendJob::Result sslWrite ( int sock,
#ifdef _WIN_ // Windows
                                 volatile ThreadState& threadState,
                                 WSAOVERLAPPED* overlapped,
                                 WSAEVENT writeEvent,
#else // Unix
                                 int rdEventPipe,
#endif
                                 const void*, quint32,
                                 quint32 timeoutMsecs = 0,  int* unixfd = 0 );

    //
    // SSL subsidiary methods
    //

    // Reinit inner structures. Must be called when write thread is paused
    bool initSSL ( SSL*, BIO*, BIO* );
    bool deinitSSL ();
    QMutex* getSSLMutex ();
    void wakeSSLWriter ();

private:
    IOSendJob::Result plainWrite ( int sock, const void*, quint32,
                                   quint32 timeoutMsecs = 0, int* unixfd = 0 );

    void run ();
    // Backtrace will show us what context is used
    void doServerCtxJob ();
    // Backtrace will show us what context is used
    void doClientCtxJob ();
    void doJob ();

    // Mark thread as finalized.
    // NOTE: not thread-safe
    void __finalizeThread ();

    void setUuidsToPkgHeaderAndRegisterJob ( IOPackage::PODHeader&,
                                             SmartPtr<IOSendJob>& ) const;
    IOSendJob::Result sslWriteFromNetworkBio ( int sock,
#ifdef _WIN_ // Windows
                                               volatile ThreadState& threadState,
                                               WSAOVERLAPPED* overlapped,
                                               WSAEVENT writeEvent,
#else // Unix
                                               int rdEventPipe,
#endif
                                               quint32 msecsTimeout = 0,
                                               int* unixfd = 0 );
    bool sendAndPauseWriting ( const SmartPtr<IOPackage>&, bool isDetaching );

private:
    DEFINE_IO_LOG

    SmartPtr<IOJobManager> m_jobManager;
    IOSender::Type m_senderType;

    SocketClientContext m_ctx;
    SocketWriteListenerInterface* m_wrListener;
    volatile IOSender::State m_state;
    Uuid m_currConnUuid;
    Uuid m_peerConnUuid;
    IOCommunication::ProtocolVersion m_peerProtoVersion;
    IORoutingTable m_routingTable;
    SmartPtr<IOJobManager::JobPool> m_jobPool;
#ifdef _WIN_
    WSAOVERLAPPED m_sockOverlappedWrite;
    WSAEVENT m_writeEventHandle;
#else
    int m_eventPipes[2];
    int m_maxFDNum;
    SmartPtr<fd_set> m_rdSet;
    SmartPtr<fd_set> m_wrSet;
#endif
    volatile ThreadState m_threadState;
    IOSendJob::Result m_stopReason;
    int m_sockHandle;
    QWaitCondition m_wait;
    QWaitCondition m_threadStateWait;
    mutable QMutex m_jobMutex;
    QMutex m_startStopMutex;

    // Pause and detach members
    SmartPtr<IOPackage> m_pausePkg;
    bool m_inPause;
    bool m_isDetaching;

    // SSL members
    SSL* m_ssl;
    BIO* m_sslSSLBio;
    BIO* m_sslNetworkBio;

    // Connection statistics member
    IOSender::Statistics& m_stat;
};

} //namespace IOService

#endif //SOCKETP_H
