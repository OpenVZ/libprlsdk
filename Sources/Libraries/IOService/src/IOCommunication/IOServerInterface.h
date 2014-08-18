/*
 * IOServerInterface.h
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


#ifndef IOSERVERINTERFACE_H
#define IOSERVERINTERFACE_H

#include "IOSendJobInterface.h"

namespace IOService {

class IOServerInterface;

class IOServerInterface_Signals : public QObject
{
    Q_OBJECT
signals:

//
// Server side signals
//

    /**
     * Emits when server has been changed its state
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onServerStateChanged ( IOSender::State );

//
// Client side signals
//

    /**
     * Emits when new client has been connected to server.
     * If signal catcher use DirectConnection, it can send first package to
     * client.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onClientConnected ( IOSender::Handle );

    /**
     * Emits when new client has been attached to server.
     * If signal catcher use DirectConnection, it can send first package to
     * client.
     * This signal is analog of #onClientConnected signal, except that
     * #onClientAttached is called on attaching.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onClientAttached ( IOSender::Handle,
                            const SmartPtr<IOPackage> additionalPkg );

    /**
     * Emits when client has been closed connection and is going
     * to be freed.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onClientDisconnected ( IOSender::Handle );

    /**
     * Emits when server has been received a package from client, which
     * is described by client handle.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onPackageReceived ( IOSender::Handle,
                             const SmartPtr<IOPackage> );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     */
    void onPackageReceived ( IOServerInterface*, IOSender::Handle,
                             const SmartPtr<IOPackage> );


    /**
     * If user is interested in some response and has stored some
     * IOSendJob::Handle to receive response, this slot will be
     * called with job handle and response package.
     * If somebody is waiting on 'waitForResponse' call and has been connected
     * to this signal with 'Qt::DirectConnection' param, it is guaranteed, that
     * connected slot will be called firstly.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onResponsePackageReceived ( IOSender::Handle,
                                     IOSendJob::Handle,
                                     const SmartPtr<IOPackage> );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onResponsePackageReceived ( IOServerInterface*,
                                     IOSender::Handle,
                                     IOSendJob::Handle,
                                     const SmartPtr<IOPackage> );

    /**
     * Signal for receiving detached client state from client.
     *
     * @see #sendDetachedClient
     * @see #attachClient
     */
    void onDetachedClientReceived ( IOSender::Handle,
                                    const SmartPtr<IOPackage>,
                                    const IOCommunication::DetachedClient );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onDetachedClientReceived ( IOServerInterface*,
                                    IOSender::Handle,
                                    const SmartPtr<IOPackage>,
                                    const IOCommunication::DetachedClient );

    /**
     * Signal which emits from client thread while this client
     * was detached by #detachClient call.
     *
     * @see #sendDetachedClient
     * @see #detachClient
     * @see #attachClient
     */
    void onDetachClient ( IOSender::Handle,
                          const IOCommunication::DetachedClient );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onDetachClient ( IOServerInterface*,
                          IOSender::Handle,
                          const IOCommunication::DetachedClient );

    /**
     * Emits when client has been changed its state
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onClientStateChanged ( IOSender::Handle, IOSender::State );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onClientStateChanged ( IOServerInterface*,
                                IOSender::Handle,
                                IOSender::State );

    /**
     * Emits when client is going to start sending,
     * i.e. writing to native socket.
     * @note: Signal emission works in a write thread.
     */
    void onBeforeSend ( IOServerInterface*,
                        IOSender::Handle,
                        const SmartPtr<IOPackage> );

    /**
     * Emits when package has been sent,
     * i.e. has been written to native socket.
     * @note: Signal emission works in a write thread.
     */
    void onAfterSend ( IOServerInterface*,
                       IOSender::Handle,
                       IOSendJob::Result,
                       const SmartPtr<IOPackage> );

};

/**
 * Client side part of server interface.
 * Contains all client methods and signals.
 * I.e. such methods/signals which represent
 * client on peer side.
 */
class IOServerInterface_ClientSide
{
public:
    /**
     * Returns client sender type
     */
    virtual IOSender::Type clientSenderType (
                                          const IOSender::Handle& ) const = 0;

	/**
     * Returns client sender security mode
     */
	virtual IOSender::SecurityMode clientSecurityMode(const IOSender::Handle& ) const = 0;

    /**
     * Returns client state. If client does not exist Disconnected state
     * is returned.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual IOSender::State clientState ( const IOSender::Handle& ) const = 0;

    /**
     * Returns client host name by client handle,
     * if handle is wrong return string will be empty.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual QString clientHostName ( const IOSender::Handle& ) const = 0;

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
                                 IOCommunication::ProtocolVersion& ) const = 0;

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
     *
     * @return true if detaching starts, false if detaching failed.
     *
     * @see #attachClient
     * @see #sendDetachedClient
     */
    virtual bool detachClient ( const IOSender::Handle& cliHandle,
                                int specificArg,
                                const SmartPtr<IOPackage>& additionalPkg =
                                    IOPackage::Null ) = 0;

    /**
     * Disconnects client. Returns true if it exists, and was connected,
     * false otherwise.
     */
    virtual bool disconnectClient ( const IOSender::Handle& ) = 0;

    /**
     * Sends package to the client which is described by handle.
     * @note: This method is thread-safe.
     *
     * @return job handle. If it is not started, handle will be invalid
     *                     (#IOSendJob::Handle::isValid)
     */
    virtual IOSendJob::Handle sendPackage ( const IOSender::Handle&,
                                            const SmartPtr<IOPackage>& ) = 0;

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
                                      IOPackage::Null ) = 0;
protected:
    /** Destructor */
    virtual ~IOServerInterface_ClientSide () {}
};

/**
 * Server side part of server interface.
 * Contains all server methods and signals.
 */
class IOServerInterface_ServerSide
{
public:
    /** Returns current server state */
    virtual IOSender::State state () const = 0;

    /**
     * Starts server thread and listens for incomming clients.
     * Returns server state.
     * @note if server is used in proxy mode and returned state is
     *       #IOSender::Disconnected, you should use #waitForProxyConnected
     *       to be sure, server is inited successfully.
     */
    virtual IOSender::State listen () = 0;

    /**
     * Return server binding info (protocol, address, port).
     */
    virtual bool serverBindingInfo ( IPvBindingInfo& ) const = 0;

    /**
     * Disconnects server.
     * @note: is a synchronous method.
     */
    virtual void disconnectServer () = 0;

    /**
     * Counts running clients.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual quint32 countClients () const = 0;

    /**
     * Returns handles of running clients.
     * @note Client can be disconnected or new can be connected at any time.
     *       Use #onClientConnected or #onClientDisconnected signals for
     *       assurance.
     */
    virtual QList<IOSender::Handle> getClientsHandles () const = 0;

    /**
     * Attach detached client.
     * Detached client can be attached only once.
     *
     * @return true of false.
     *
     * @see attachClient
     * @see sendDetachedClient
     */
    virtual bool attachClient ( IOCommunication::DetachedClient ) = 0;

    /**
     * Just creates socket pair and returns one socket handle which can
     * be copied or duplicated by native calls.
     * You can create #IOClient object with this socket handle
     * (see appropriate constructor)
     *
     * @return socket handle, if error occured, returned object will be
     *         invalid, i.e. isValid call will return false.
     */
    virtual IOCommunication::SocketHandle createDetachedClientSocket () = 0;

	/**
     * Set credentials for server ( Authorization for new cliens will be
     * available throw credentials)
     */
	virtual void setCredentials(const IOCredentials& credentials) = 0;

protected:
    /** Destructor */
    virtual ~IOServerInterface_ServerSide () {}
};


/**
 * Client side server interface, which unites client side subsidiary interfaces.
 */
class IOServerInterface_Client : public IOServerInterface_Signals,
                                 public IOServerInterface_ClientSide,
                                 public IOSendJobInterface
{
};

/**
 * Full server interface, which unites all subsidiary interfaces.
 */
class IOServerInterface : public IOServerInterface_Client,
                          public IOServerInterface_ServerSide
{
};


} //namespace IOService

#endif //IOSERVERINTERFACE_H
