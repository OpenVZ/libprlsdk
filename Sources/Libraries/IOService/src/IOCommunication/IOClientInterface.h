/*
 * IOClientInterface.h
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


#ifndef IOCLIENTINTERFACE_H
#define IOCLIENTINTERFACE_H

#include <QObject>
#include "IOSendJobInterface.h"

namespace IOService {

class IOClientInterface : public QObject,
                          public IOSendJobInterface
{
    Q_OBJECT
public:
    /** Returns current client state */
    virtual IOSender::State state () const = 0;

    /** Returns client error code */
    virtual IOSender::Error error () const = 0;

    /**
     * Connects to server. Works in asynchronous manner.
     * @see waitForConnection
     */
    virtual bool connectClient ( quint32 msecsTimeout = 0 ) = 0;

    /**
     * Waits for msecs while state becomes #Connected or changes to other.
     * @note: If msecs == 0 (default), then the wait will never timeout.
     * @return State after waiting.
     */
    virtual IOSender::State waitForConnection ( quint32 msecs = 0 ) const = 0;

    /**
     * Disconnects client.
     * @note: is a synchronous method.
     */
    virtual void disconnectClient () = 0;

    /**
     * Stores received server protocol version.
     * You can get valid protocol version if client is connected to server,
     * or if client has been disconnected.
     */
    virtual bool serverProtocolVersion (
                                 IOCommunication::ProtocolVersion& ) const = 0;

	/**
     * Returns connection security mode
     */
	virtual IOSender::SecurityMode securityMode() const = 0;

    /**
     * Sends package to the server.
     * @note: This method is thread-safe.
     *
     * @return job handle. If client is not started, handle will be invalid
     *                     (#IOSendJob::Handle::isValid)
     */
    virtual IOSendJob::Handle sendPackage ( const SmartPtr<IOPackage>& ) = 0;

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
                                      IOPackage::Null ) = 0;

signals:
    /**
     * Emits when client has been received a package from server.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onPackageReceived ( const SmartPtr<IOPackage> );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onPackageReceived ( IOClientInterface*, const SmartPtr<IOPackage> );

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
    void onResponsePackageReceived ( IOSendJob::Handle,
                                     const SmartPtr<IOPackage> );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onResponsePackageReceived ( IOClientInterface*,
                                     IOSendJob::Handle,
                                     const SmartPtr<IOPackage> );

    /**
     * Signal for receiving detached client state.
     *
     * @see #sendDetachedClient
     */
    void onDetachedClientReceived ( const SmartPtr<IOPackage>,
                                    const IOCommunication::DetachedClient );

    /**
     * Same as above, but 'this' is a first argument.
     * Is used to avoid using of 'QObject::sender()' in multithreaded slots.
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onDetachedClientReceived ( IOClientInterface*,
                                    const SmartPtr<IOPackage>,
                                    const IOCommunication::DetachedClient );

    /**
     * Emits when client has been changed its state
     * @note: Signal emission works in _not_ a main thread.
     *        So be careful if connection to this signal is a DirectConnection.
     */
    void onStateChanged ( IOSender::State );

    /**
     * Emits when client is going to start sending,
     * i.e. writing to native socket.
     * @note: Signal emission works in a write thread.
     */
    void onBeforeSend ( IOClientInterface*,
                        const SmartPtr<IOPackage> );

    /**
     * Emits when package has been sent,
     * i.e. has been written to native socket.
     * @note: Signal emission works in a write thread.
     */
    void onAfterSend ( IOClientInterface*,
                       IOSendJob::Result,
                       const SmartPtr<IOPackage> );
};

} //namespace IOService

#endif //IOCLIENTINTERFACE_H
