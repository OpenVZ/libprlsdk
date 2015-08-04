/*
 * IOServer.h
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


#ifndef IOSERVER_H
#define IOSERVER_H

#include "IOProtocol.h"
#include "IOConnection.h"
#include "IOServerInterface.h"

#include "Socket/SocketListeners_p.h"

namespace IOService {

class IOServer : public IOServerInterface,
                 public IOConnection,
                 private ReceiveSendClientListenerInterface
{
    Q_OBJECT
public:
    /** Common server constants */
    enum ServerConsts {
        ClientConnectionTimeout = 5*60000, /**< Client's handshaking must take
                                                this timeout msecs at the most */
    };

    /**
     * Constructs TCP/IP server.
     * You can only attach clients to this server.
     * Server does not really start listen to new connections
     * after #listen call.
     *
     * @param routingTable routing table
     * @param type server sender type
     * @param host of server
     * @param port of server
     * @see #listen
     */
    IOServer ( const IORoutingTable& routingTable,
			   IOSender::Type type,
			   const IOCredentials& credentials = IOCredentials());

    /**
     * Constructs remote TCP/IP server.
     *
     * @param routingTable routing table
     * @param type server sender type
     * @param host of server
     * @param port of server
     * @param useUnixSockets sign whether UNIX socket connection
     *        type should be used (Note: is not supported under Win)
     * @see #listen
     */
    IOServer ( const IORoutingTable& routingTable,
               IOSender::Type type,
               const QString& host,
			   quint32 port,
			   bool useUnixSockets = false,
			   const IOCredentials& credentials = IOCredentials());

    /** Destructor */
    virtual ~IOServer ();

    /** Returns current server state */
    virtual IOSender::State state () const;

    /**
     * Returns current sender handle, which is created
     * after successfull connection. If we are not connected,
     * sender will be equal to IOSender::InvalidHandle.
     */
    virtual IOSender::Handle senderHandle () const;

    /**
     * Starts server thread and listens for incomming clients.
     * Returns server state.
     */
    virtual IOSender::State listen ();

    /**
     * Get server binding info (protocol, address, port).
     */
    virtual bool serverBindingInfo ( IPvBindingInfo& ) const;

    /**
     * Disconnects server.
     * @note: is a synchronous method.
     */
    virtual void disconnectServer ();

    /**
     * Counts running clients.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual quint32 countClients () const;

    /**
     * Returns handles of running clients.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual QList<IOSender::Handle> getClientsHandles () const;

    /**
     * Returns client sender type
     */
    virtual IOSender::Type clientSenderType ( const IOSender::Handle& ) const;

	/**
     * Returns client sender security mode
     */
	virtual IOSender::SecurityMode clientSecurityMode(const IOSender::Handle& ) const;

    /**
     * Returns client state. If client does not exist Disconnected state
     * is returned.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual IOSender::State clientState ( const IOSender::Handle& ) const;

    /**
     * Returns client host name by client handle,
     * if handle is wrong return string will be empty.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual QString clientHostName ( const IOSender::Handle& ) const;

    /**
     * If client is connected to server, argument will be filled by
     * server protocol, and true will be returns.
     * If client is not connected, only false will be returned.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual bool clientProtocolVersion (
                                 const IOSender::Handle&,
                                 IOCommunication::ProtocolVersion& ) const;

    /**
     * Requests client detaching in asynchronous manner.
     * Use #onDetachClient signal to catch detached state.
     * Detached client must be attached only once to another server.
     * You can send detached client by calling #sendDetachedClient.
     * After send detached client becomes invalid, i.e. you can not
     * send or attach detached clients several times.
     *
     * @param cliHandle client handle, which should be detached
     * @param specificArg this is platform dependent param:
     *                    for Windows we should specify target process Id
     *                       for which we want to detach handle;
     *                    for Unix it does not matter, pass 0.
     * @param additionalPkg just an additional package, which can be
     *                      send to other side with this detached client.
     * @param detachBothSides true indicates that both sides of the connection
     *                        must be detached, while false indicates that only
     *                        the server side must be detached (default)
     *
     * @return true if detaching starts, false if detaching failed.
     *
     * @see #attachClient
     * @see #sendDetachedClient
     */
    virtual bool detachClient ( const IOSender::Handle& cliHandle,
                                int specificArg,
                                const SmartPtr<IOPackage>& additionalPkg =
                                    IOPackage::Null,
                                bool detachBothSides = false );

    /**
     * Attach detached client.
     * Detached client can be attached only once.
     *
     * @return true of false.
     *
     * @see attachClient
     * @see sendDetachedClient
     */
    virtual bool attachClient ( IOCommunication::DetachedClient );

    /**
     * Just creates socket pair and returns one socket handle which can
     * be copied or duplicated by native calls.
     * You can create #IOClient object with this socket handle
     * (see appropriate constructor)
     *
     * @return socket handle, if error occured, returned object will be
     *         invalid, i.e. isValid call will return false.
     */
    virtual IOCommunication::SocketHandle createDetachedClientSocket ();

	/**
     * Set credentials for server ( Authorization for new cliens will be
     * available throw credentials)
     */
	virtual void setCredentials(const IOCredentials& credentials);

    /**
     * Disconnects client. Returns true if it exists, and was connected,
     * false otherwise.
     */
    virtual bool disconnectClient ( const IOSender::Handle& );

    /**
     * Sends package to the client which is described by handle.
     * @note: This method is thread-safe.
     *
     * @return job handle. If it is not started, handle will be invalid
     *                     (#IOSendJob::Handle::isValid)
     */
    virtual IOSendJob::Handle sendPackage ( const IOSender::Handle&,
                                            const SmartPtr<IOPackage>& );

    /**
     * Sends detached client to another client.
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
                                  const IOSender::Handle&,
                                  const IOCommunication::DetachedClient&,
                                  const SmartPtr<IOPackage>& request =
                                      IOPackage::Null );

private:
    /** Just common init routine */
    void init ();

    /** Prevent copying */
    IOServer ( const IOServer& );
    /** Prevent copying */
    IOServer& operator= ( const IOServer& );

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

    virtual void srv_onDetachClient ( SocketClientPrivate*,
                                      const IOSender::Handle&,
                                      const IOCommunication::DetachedClient& );

private:
    friend class SocketClientPrivate;
    friend class SocketServerPrivate;
    class SocketServerPrivate* m_sockImpl;
};

} //namespace IOService

#endif //IOSERVER_H
