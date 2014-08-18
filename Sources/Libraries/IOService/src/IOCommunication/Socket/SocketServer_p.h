/*
 * SocketServer_p.h
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


#ifndef SOCKETSERVERP_H
#define SOCKETSERVERP_H

#include <QQueue>

#include "Socket_p.h"
#include "SocketClient_p.h"

namespace IOService {

class IOServer;

class SocketServerPrivate : protected QThread,
                            private ReceiveSendClientListenerInterface,
                            private StateClientListenerInterface
{
public:
	SocketServerPrivate ( IOServer*,
                          SocketServerContext ctx,
						  bool useUnnixSockets,
						  const IOCredentials& credentials);
    virtual ~SocketServerPrivate ();

    IOSender::State state () const;

    bool serverBindingInfo ( IPvBindingInfo& ) const;

    QList< IOSendJob::Handle > sendPackage ( const SmartPtr<IOPackage>& );

    IOSendJob::Handle sendPackage ( const IOSender::Handle&,
                                    const SmartPtr<IOPackage>& );
    IOSendJob::Handle sendDetachedClient (
                                const IOSender::Handle&,
                                const IOCommunication::DetachedClient&,
                                const SmartPtr<IOPackage>& );

    IOSender::State startServer ();
    void stopServer ();

    QString currentConnectionUuid () const;
    quint32 countClients () const;

    QList<IOSender::Handle> getClientsHandles () const;

    IOSender::Type clientSenderType ( const IOSender::Handle& ) const;

    IOSender::SecurityMode clientSecurityMode ( const IOSender::Handle& ) const;

    IOSender::State clientState ( const IOSender::Handle& ) const;

    QString clientHostName ( const IOSender::Handle& ) const;

    bool clientProtocolVersion ( const IOSender::Handle&,
                                 IOCommunication::ProtocolVersion& ) const;
    bool detachClient ( const IOSender::Handle& cliHandle,
                        int specificArg,
                        const SmartPtr<IOPackage>& additionalPkg );

    bool attachClient ( IOCommunication::DetachedClient );

	void setCredentials( const IOCredentials& credentials );

    IOCommunication::SocketHandle createDetachedClientSocket ();

    bool stopClient ( const IOSender::Handle& );

private:
    void run ();

    // Mark thread as finalized.
    // NOTE: not thread-safe
    void __finalizeThread ();

    void sendPackage ( QList<IOSendJob::Handle>&,
                       const IOSender::Handle&,
                       const SmartPtr<IOPackage>& );

    //
    // Client state callbacks
    //
    virtual bool srv_validateAfterHandshake ( SocketClientPrivate* client,
											 const IOSender::Handle& h );

    virtual void onStateChanged ( SocketClientPrivate*,
								 const IOSender::Handle&,
								 IOSender::State oldState,
								 IOSender::State newState );
    bool createAndStartNewSockClient ( int cliHandle,
                                                                         IOCommunication::DetachedClient);

private:
    void cleanAllClients ();
    void cleanStoppedClients ();
    void wakeupClientsCleaner ();

private:
    DEFINE_IO_LOG

    IOServer* m_impl;
    SocketServerContext m_ctx;
    SmartPtr<SocketClientPrivate> m_prxMngClient;
    IOSender::State m_state;
    QString m_currConnectionUuid;
    QMutex m_startStopMutex;
    mutable QMutex m_eventMutex;
    mutable QWaitCondition m_threadStateWait;
    volatile ThreadState m_threadState;
#ifdef _WIN_
    WSAEVENT m_connectEventHandle;
    WSAEVENT m_cleanEventHandle;
#else
    int m_eventPipes[4];
    int m_maxFDNum;
    SmartPtr<fd_set> m_rdSet;
#endif
    QHash<SocketClientPrivate*, SmartPtr<SocketClientPrivate> > m_preAppendClients;
    QHash<IOSender::Handle, SmartPtr<SocketClientPrivate> > m_sockClients;
    QList< SmartPtr<SocketClientPrivate> > m_stoppedSockClients;
    QHash<QString, QString> m_clientsUuids;
	IOCredentials m_localCredentials;
    // Members for attaching detached clients
    QWaitCondition m_attachWait;
    QQueue<IOCommunication::DetachedClient> m_attachQueue;

    IPvBindingInfo m_bindingInfo;

    // Use unix sockets for connection
    bool m_useUnixSockets;
};

} //namespace IOService

#endif //SOCKETSERVERP_H
