/*
 * SocketClient_p.h
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


#ifndef SOCKETCLIENTP_H
#define SOCKETCLIENTP_H

#include "Socket_p.h"

namespace IOService {

class SSLHelper;

/** IO client implementation */
class SocketClientPrivate : protected QThread,
                            protected SocketWriteListenerInterface
{
public:
    SocketClientPrivate (
                          SmartPtr<IOJobManager> jobManager,
                          const IORoutingTable& routingTable,
                          IOSender::Type senderType,
                          IOSender::ConnectionMode senderConnMode,
                          const QString& remoteHost,
                          quint32 remotePort,
                          const IOSender::Handle& proxySessionHandle,

                          SocketClientContext ctx,
                          ReceiveSendClientListenerInterface*,
                          StateClientListenerInterface*,
                          int,
                          const Uuid& srv_currConnUuid,
                          const IOCommunication::DetachedClient& srv_state,
                          const IOCredentials& credentials,
                          bool useUnixSockets,
                          bool securePrxMng = false);
    virtual ~SocketClientPrivate ();

    IOSender::State state () const;
    IOSender::Error error () const;

    bool startClient ( quint32 msecsTimeout );
    void stopClient ();
    IOSender::State waitForConnection ( quint32 msecsTimeout = 0 ) const;

    bool srv_getAttachingAdditionalPkg ( SmartPtr<IOPackage>& ) const;
    QString currentConnectionUuid () const;
    bool peerProtocolVersion ( IOCommunication::ProtocolVersion& ) const;
    bool peerProxyProtocolVersion ( IOCommunication::ProtocolVersion& ) const;
    IOSender::Type peerSenderType () const;
	IOSender::SecurityMode securityMode () const;
    QString peerConnectionUuid () const;
    QString peerHostName () const;

    IOSendJob::Handle sendPackage ( const SmartPtr<IOPackage>& );
    IOSendJob::Handle sendDetachedClient (
                                     const IOCommunication::DetachedClient&,
                                     const SmartPtr<IOPackage>& );

    bool srv_detachClient ( int specificArg,
                            const SmartPtr<IOPackage>& additionalPkg );

	void setLimitErrorLogging(bool bLimitErrorLogging);

private:
    enum IOReadMode {
        IOSingleRead = 0,
        IOContinuousRead,
        IOGracefulShutdownRead
    };

    // Do graceful shutdown:
    //    1. shutdown write
    //    2. read till error: other side is closed or timeout expired
    void __gracefulShutdown ( int sock, quint32 timeout );

    bool read ( int sock, char* inBuf, quint32 size, quint32& wasRead,
                IOReadMode readMode, int msecsTimeout,
                int* unixfd = 0, bool* timeoutExpired = 0);

    bool cli_sendHandshake ( int sock, const Uuid& currConnUuid,
                                     quint32 msecsTimeout );
    bool cli_parseHandshake ( int sock, quint32 msecsTimeout );
    bool srv_parseHandshake ( int sock, quint32 msecsTimeout );
    bool srv_sendHandshake ( int sock, const Uuid& currConnUuid,
                                     quint32 msecsTimeout );
    bool srv_sendRoutingTable (int sock, quint32 msecsTimeout );


    IOCommunication::DetachedClient srv_doDetach ( int sock );

    // Do connecting to host.
    // If connection succeed, true will be returned and sockHandle
    // param will be inited with valid sock.
    bool connectToHost ( int& sockHandle, quint32 msecsTimeout );

    void run ();
    // Backtrace will show us what context is used
    void doServerCtxJob ();
    // Backtrace will show us what context is used
    void doClientCtxJob ();
    // Backtrace will show us what context is used
    void doProxyMngCtxJob ();
    void doJob ();

    // Mark thread as finalized.
    // NOTE: not thread-safe
    void __finalizeThread ();

    // SSL routines
	bool sslInit (const IOCredentials& cert = IOCredentials());
    bool sslDeinit ();

    bool sslHandshake ( int sock, quint32 msecsTimeout, bool rehandshake = false);
    bool sslHeaderCheck ( const SSLv3Header& header ) const;
    bool sslReadAndWait ( int sock, char* buffOut, quint32 sizeOut,
                          int msecsTimeout, int* unixfd = 0,
                          bool* timeoutExpired = 0 );
    bool readData ( int sock, char* buffOut, quint32 sizeOut,
                    int msecsTimeout, int* unixfd = 0,
                    bool* timeoutExpired = 0 );
    IOSendJob::Result writeData ( int sock, const void* data, quint32 size,
                                  int msecsTimeout );

	bool reinitSslSession(void);
	unsigned char* sslGetSession(unsigned int *size);
	bool cli_sslSetSession(const unsigned char* session,
						   unsigned int size);

    bool srv_setHandshakeFromState ();
    bool srv_addSSLSessionFromStateAndRehandshake ( int sock,
                                                    SmartPtr<IOPackage>&,
                                                    quint32 msecsTimeout );

	//
	// SocketWriteListenerInterface
    //
    // Socket write callbacks
    //
    virtual void onBeforeWrite ( SocketWriteThread*,
                                 const SmartPtr<IOPackage>& );
    virtual void onAfterWrite ( SocketWriteThread*,
                                IOSendJob::Result,
                                const SmartPtr<IOPackage>& );
    virtual void onWriteIsFinishedWithErrors ( SocketWriteThread* );


private:
    DEFINE_IO_LOG

    SmartPtr<IOJobManager> m_jobManager;
    const IORoutingTable& m_routingTable;
    IOSender::Type m_senderType;
    IOSender::ConnectionMode m_senderConnMode;
	IOSender::SecurityMode m_securityMode;
    QString m_remoteHost;
    quint32 m_remotePort;
	QString m_localHost;
	quint32 m_localPort;
    IOSender::Handle m_proxySessionHandle;

    SocketClientContext m_ctx;
    ReceiveSendClientListenerInterface* m_rcvSndListener;
    StateClientListenerInterface* m_stateListener;
    QMutex m_startStopMutex;
    mutable QMutex m_eventMutex;
    mutable QWaitCondition m_threadStateWait;
    mutable QWaitCondition m_ioStateWait;
    volatile IOSender::State m_state;
    volatile IOSender::Error m_error;
    volatile ThreadState m_threadState;
    quint32 m_connTimeout;
    int m_sockHandle;
    bool m_isReusableSocket;
    QString m_peerConnectionUuid;
    QString m_currConnectionUuid;
    IOSender::Type m_peerSenderType;
    IOCommunication::ProtocolVersion m_peerProtoVersion;
    IOCommunication::ProtocolVersion m_proxyPeerProtoVersion;
    QString m_peerHostName;
    IORoutingTable m_peerRoutingTable;
    SocketWriteThread m_writeThread;
#ifdef _WIN_
    WSAEVENT m_readEventHandle;
    WSAOVERLAPPED m_overlappedRead;
#else
    int m_eventPipes[2];
    int m_maxFDNum;
    SmartPtr<fd_set> m_rdSet;
    SmartPtr<fd_set> m_wrSet;
#endif

    // Server context
    struct {
        IOCommunication::DetachedClient m_clientState;
        bool m_isDetaching;
        int m_detachingSpecificArg;
        SmartPtr<IOPackage> m_detachingAdditionalPkg;
        SmartPtr<IOPackage> m_attachingAdditionalPkg;

    } m_srvCtx;

    // SSL members
    SSL* m_ssl;
    BIO* m_sslBio;
    BIO* m_sslNetworkBio;
    BIO* m_sslSSLBio;
    SSLHelper* m_sslHelper;
    IOCredentials m_localCredentials;
    bool m_securePrxMng;

    // SSL read states
    qint32 m_remainToRead;
    bool m_pendingInSSL;
    bool m_newHeader;
    SSLv3Header m_header;
    bool m_encryptedDataToRead;

    // Use unix sockets for connection
    bool m_useUnixSockets;
    QByteArray m_rawBuffer;
    qint32 m_lastfd;
    // Statistics structure
    IOSender::Statistics m_stat;

	// Log limit rate structure
	LogRateLimit m_rl;
	bool	m_bLimitErrorLogging;
};

} //namespace IOService

#endif //SOCKETCLIENTP_H
