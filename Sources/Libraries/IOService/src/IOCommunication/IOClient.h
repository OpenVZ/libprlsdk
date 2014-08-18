/*
 * IOClient.h
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


#ifndef IOCLIENT_H
#define IOCLIENT_H

#include "IOProtocol.h"
#include "IOConnection.h"
#include "IOClientInterface.h"

#include "Socket/SocketListeners_p.h"

namespace IOService {

struct IOCredentials;

class IOClient : public IOClientInterface,
                 public IOConnection,
                 private ReceiveSendClientListenerInterface,
                 private StateClientListenerInterface
{
    Q_OBJECT
public:
    /**
     * Constructs client by detached socket handle.
     *
     * @param routingTable client routing table
     * @param type client sender type
     * @param name of connection
     * @see #connectClient
     */
    IOClient ( const IORoutingTable& routingTable,
               IOSender::Type type,
			   IOCommunication::SocketHandle,
			   const IOCredentials& credentials = IOCredentials());


    /**
     * Constructs remote TCP/IP client.
     *
     * @param routingTable routing table
     * @param type client sender type
     * @param host of remote server
     * @param port of remote server
     * @param useUnixSockets sign whether UNIX socket connection
     *        type should be used (Note: is not supported under Win)
     * @see #connectClient
	 */
	IOClient ( const IORoutingTable& routingTable,
			   IOSender::Type type,
			   const QString& host,
			   quint32 port,
			   bool useUnixSockets = false,
			   const IOCredentials& credentials = IOCredentials()
			);

    /**
     * Constructs remote TCP/IP client via proxy.
     *
     * @param routingTable routing table
     * @param type client sender type
     * @param proxyPeerHandle handle of registered server host,
     *        to which we want to communicate. this handle
     *        must be authorized, registered as server on proxy
     *        and received from trustworthy source.
     * @param proxyHost host of proxy
     * @param proxyPort port of proxy
     * @see #connectClient
     */
    IOClient ( const IORoutingTable& routingTable,
               IOSender::Type type,
               const IOSender::Handle& proxyPeerHandle,
               const QString& proxyHost,
			   quint32 proxyPort,
			   const IOCredentials& credentials = IOCredentials()
                );

    /** Destructor */
    virtual ~IOClient ();

    /** Returns current client state */
    virtual IOSender::State state () const;

    /** Returns client error code */
    virtual IOSender::Error error () const;

    /**
     * Returns current sender handle, which is created
     * after successfull connection. If we are not connected,
     * sender will be equal to IOSender::InvalidHandle.
     */
    virtual IOSender::Handle senderHandle () const;

    /**
     * Connects to server. Works in asynchronous manner.
     * @see waitForConnection
     */
    virtual bool connectClient ( quint32 msecsTimeout = 0 );

    /**
     * Waits for msecs while state becomes #Connected or changes to other.
     * @note: If msecs == 0 (default), then the wait will never timeout.
     * @return State after waiting.
     */
    virtual IOSender::State waitForConnection ( quint32 msecs = 0 ) const;

    /**
     * Disconnects client.
     * @note: is a synchronous method.
     */
    virtual void disconnectClient ();

    /**
     * Stores received server protocol version.
     * You can get valid protocol version if client is connected to server,
     * or if client has been disconnected.
     */
    virtual bool serverProtocolVersion (
                                 IOCommunication::ProtocolVersion& ) const;

	/**
     * Returns connection security mode
     */
	virtual IOSender::SecurityMode securityMode() const;

    /**
     * Sends package to the server.
     * @note: This method is thread-safe.
     *
     * @return job handle. If client is not started, handle will be invalid
     *                     (#IOSendJob::Handle::isValid)
     */
    virtual IOSendJob::Handle sendPackage ( const SmartPtr<IOPackage>& );

    /**
     * Sends detached client.
     * You can specify request package to send detached client as response
     * on this request package.
     *
     * @note: This method is thread-safe.
     *
     * @return job handle. If it is not started, handle will be invalid
     *                     (#IOSendJob::Handle::isValid)
     *
     * @see #onDetachedClientReceived
     */
    virtual IOSendJob::Handle sendDetachedClient (
                                  const IOCommunication::DetachedClient&,
                                  const SmartPtr<IOPackage>& request =
                                      IOPackage::Null );

	/**
	 * Set limit error logging.
	 * This method is useful, for example, at delayed connection
	 * when client polling many times the moment appearing service.
	 */
	void setLimitErrorLogging(bool bLimitErrorLogging);

private:
    // Disable copy constructor and alignment operator
    Q_DISABLE_COPY(IOClient)

    /** Just common init routine */
    void init ();

    //
    // Callbacks
    //
    virtual void onBeforeSend ( SocketClientPrivate*,
                                const IOSender::Handle&,
                                const SmartPtr<IOPackage>& );

    virtual void onAfterSend ( SocketClientPrivate*,
                               const IOSender::Handle&,
                               IOSendJob::Result,
                               const SmartPtr<IOPackage>& );

    virtual void onDetachedClientReceived (
                                   SocketClientPrivate*,
                                   const IOSender::Handle&,
                                   const SmartPtr<IOPackage>&,
                                   const IOCommunication::DetachedClient& );

    virtual void onPackageReceived ( SocketClientPrivate*,
                                     const IOSender::Handle&,
                                     const SmartPtr<IOPackage>& );

    virtual void onResponsePackageReceived ( SocketClientPrivate*,
                                             const IOSender::Handle&,
                                             const IOSendJob::Handle&,
                                             const SmartPtr<IOPackage>& );

    virtual void onStateChanged ( SocketClientPrivate*,
                                  const IOSender::Handle&,
                                  IOSender::State oldState,
                                  IOSender::State newState );

private:
    friend class SocketClientPrivate;
    class SocketClientPrivate* m_sockImpl;
};

} //namespace IOService

#endif //IOCLIENT_H
