/*
 * IOConnection.h
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


#ifndef IOCONNECTION_H
#define IOCONNECTION_H

#include <QReadWriteLock>
#include <QDateTime>

#include "Libraries/Std/SmartPtr.h"
#include "IOProtocol.h"
#include "IOSendJob.h"
#include "IORoutingTable.h"

namespace IOService {

/**
 * Common connection class, which is used for accumulating
 * common members for client/server connections.
 */
class IOConnection
{
public:
	/**
	 * Constructs remote client or server connection.
	 *
	 * @param routingTable routing table
	 * @param type client sender type
	 * @param host of remote server
	 * @param port of remote server
	 * @param proxySessionHandle proxy session uuid
	 */
	inline IOConnection ( const IORoutingTable& routingTable,
						  IOSender::Type type,
						  IOSender::ConnectionMode connMode,
						  const QString& host,
						  quint32 port,
						  const IOSender::Handle& proxySessionHandle
							  = IOSender::InvalidHandle ) :
		m_jobManager( new IOJobManager ),
		m_senderType( type ),
		m_senderConnMode( connMode ),
		m_proxySessionHandle( proxySessionHandle ),
		m_remotePort( port ),
		m_remoteHost( host ),
		m_routingTable( routingTable )
	{}

	/**
	 * Returns routing table.
	 */
	const IORoutingTable& getRoutingTable () const
	{
		return m_routingTable;
	}

	/**
	 * Returns routing table.
	 */
	IORoutingTable& getRoutingTable ()
	{
		return m_routingTable;
	}

	/**
	 * Returns connection sender type
	 */
	inline IOSender::Type senderType () const
	{
		return m_senderType;
	}

	/**
	 * Returns true if connection is via proxy,
	 * in case of direct connection false will be returned.
	 */
	inline bool isConnectionViaProxy () const
	{
		return (m_senderConnMode != IOSender::DirectConnectionMode);
	}

	/**
	 * Returns connection mode.
	 */
	inline IOSender::ConnectionMode connectionMode () const
	{
		return m_senderConnMode;
	}

	/**
	 * Returns remote host name.
	 *
	 * @see #remotePortNumber
	 */
	inline const QString& remoteHostName () const
	{
		return m_remoteHost;
	}

	/**
	 * Returns remote port number.
	 *
	 * @see #remoteHostName
	 */
	inline quint32 remotePortNumber () const
	{
		return m_remotePort;
	}

	/**
	 * Returns proxy session handle.
	 */
	inline const IOSender::Handle& proxySessionHandle () const
	{
		return m_proxySessionHandle;
	}

	/**
	 * Returns job manager.
	 * By default #IOConnection creates job manager.
	 * @see #setJobManager
	 */
	inline SmartPtr<IOJobManager> getJobManager () const
	{
		return m_jobManager;
	}

	/**
	 * Returns current sender handle, which is created
	 * after successfull connection. If we are not connected,
	 * sender will be equal to IOSender::InvalidHandle.
	 */
	virtual IOSender::Handle senderHandle () const = 0;

protected:
	// Virtual destructor to avoid compiler warnings
	virtual ~IOConnection () {}

private:
	// Disable copy constructor and alignment operator
	Q_DISABLE_COPY(IOConnection)

private:
	SmartPtr<IOJobManager> m_jobManager;
	IOSender::Type m_senderType;
	IOSender::ConnectionMode m_senderConnMode;
	IOSender::Handle m_proxySessionHandle;
	quint32 m_remotePort;
	QString m_remoteHost;
	IORoutingTable m_routingTable;
};

/**
 * Class, which is used for holding user Certificate/private key
 */

class IOCertificate
{
	friend struct IOCredentials;

public:
	IOCertificate()
	{

	}

	IOCertificate(const QByteArray& certificate)
		:m_certificate(certificate)
	{

	}

	const QByteArray& toByteArray() const
	{
		return m_certificate;
	}

	void fromByteArray(const QByteArray& certificate)
	{
		m_certificate = certificate;
	}

	QString getRole() const;

	bool isValid() const;

	bool isExpired(const QDateTime& = QDateTime()) const;

	QString errorString() const;

private:

	bool isNull() const;

	void setErrorString(const QString& errorString) const;

	mutable QString m_errorString;

	QByteArray m_certificate;
};

struct IOCredentials
{
	IOCertificate certificate;

	QByteArray privKey;
	QList<IOCertificate> certificatesChain;

	bool isSigned(const QByteArray& CACertificate = QByteArray()) const;
	bool isNull() const;
	bool isValid() const;

	static IOCredentials fromPrivateKeyAndCertificatesChain(const char *privateKey, const char *certificatesChain);
};

} //namespace IOService

#endif //IOCONNECTION_H
