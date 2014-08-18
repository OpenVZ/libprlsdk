/*
 * Channel.h
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


#ifndef CHANNEL_H
#define CHANNEL_H

#include <QString>
#include <QByteArray>
#include <QEvent>
#include <QReadWriteLock>
#include <QVector>

#include "Interfaces/ParallelsNamespace.h"
#include "Libraries/IOService/src/IOCommunication/IOClient.h"
#include "SDK/Include/PrlIOStructs.h"

namespace IOService {

/**
 * Basic VM desktop interface.
 * IO events should be passed via this interface.
 */
class Channel: public QObject
{
    Q_OBJECT
public:

    /**
     * Desktop state.
     * @see onStateChange
     */
    enum State {
        Disabled               = 1<<0, /**< Desktop has not been enabled */
        Started                = 1<<1, /**< Started. */
        Stopped                = 1<<2, /**< Is stopped. */
        UnknownVMId            = 1<<3, /**< Unknown uuid. */
        ConnectionTimeout      = 1<<4, /**< Connection timeout. */
        AuthenticationFailed   = 1<<5, /**< Authentication failed. */
    };

public:
    /** Destructor */
    virtual ~Channel();

    /**
     * This method starts desktop thread.
     * Call this method after all initial job is done:
     *   desktop object is constructed,
     *   output image or widget is prepared, and so on.
     */
    virtual void enable ();

    /** Stops desktop thread. */
    virtual void disable ();

    /** Checks if desktop is valid */
    bool isValid () const;
    void setValid ( bool v );

    /** Returns current desktop state */
    Channel::State getState () const;

   /**
     * Returns send result.
     * @see IOClient::getSendResult
     */
    IOSendJob::Result getSendResult ( const IOSendJob::Handle& h ) const;

    IOClientInterface& getIOClient ();
    /** Returns inner IO client instance (const variant) */
    const IOClientInterface& getIOClient () const ;

    IOSender::Handle getHandle() const
    {
	return reinterpret_cast<IOClient *>(m_ioClient)->senderHandle();
    }

protected:
    /**
     * Constructs desktop instance with remote or local connection.
     * @param host of the remote server.
     * @param port of the remote server.
     * @param securityLevel level of security
     * @param useUnixSockets sign specifies whether UNIX sockets should be used
     *        for connection (Note: is not supported under Win platform)
     */
    Channel (
		    const QString& host,
		    quint32 port,
		    PRL_SECURITY_LEVEL securityLevel,
		    bool useUnixSockets = false,
		    IOSender::Type type = IOSender::IOClient );

    /**
     * Constructs desktop instance with remote via proxy.
     * @param proxyPeerHandle proxy peer handle
     * @param host of the remote server.
     * @param port of the remote server.
     * @param securityLevel level of security
     */
    Channel (
		    const IOSender::Handle& proxyPeerHandle,
		    const QString& host,
		    quint32 port,
		    PRL_SECURITY_LEVEL securityLevel,
		    IOSender::Type type = IOSender::IOClient );

    /**
     * Constructs desktop instance with ready IOClient
	 * @param ioClient IOClient object
     */
	Channel ( IOClientInterface *ioClient );


    /** Sets desktop state */
    virtual void setState ( Channel::State st);
    QReadWriteLock &getLock() { return m_rwLock; }
    QReadWriteLock &getLock() const { return m_rwLock; }


public slots:
    /**
     * Sends data of specified type and size to server.
     *
     * @return IO send job handle
     */
    IOSendJob::Handle sendPackage ( IOPackage::Type,
                                    const void*, quint32 size );

    /**
     * Sends data of specified type and size to server.
     *
     * @return IO send job handle
     */
    IOSendJob::Handle sendPackage ( IOPackage::Type,
                                    const SmartPtr<char>&, quint32 size );

    /**
     * Sends package directly
     *
     * @return IO send job handle
     */
    IOSendJob::Handle sendPackage ( const SmartPtr<IOPackage> );

    /**
     * Waits for send.
     * @see IOClient::waitForSend
     */
    IOSendJob::Result waitForSend ( const IOSendJob::Handle&,
                                    quint32 timeout = UINT_MAX ) const;

protected:
    IOClientInterface* m_ioClient;
	const bool m_ioClientOwner;

    mutable QReadWriteLock m_rwLock;
    volatile bool m_isValid;
    volatile State m_state;
};

} //namespace
#endif

