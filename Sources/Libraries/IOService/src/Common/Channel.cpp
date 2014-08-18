/*
 * Channel.cpp
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


#include <QFile>
#include <QMetaType>
#include <QBuffer>

#include "Channel.h"
#include "IORoutingTableHelper.h"
#include "Libraries/Logging/Logging.h"
#include "Libraries/IOService/src/Common/ConvRoutines.h"

#include "SDK/Include/PrlCommandsFlags.h"
#include "SDK/Include/PrlTypes.h"

// By adding this interface we enable allocations tracing in the module
#include "Interfaces/Debug.h"

using namespace IOService;

/*****************************************************************************
 * Channel
 *****************************************************************************/
Channel::Channel (
		const QString& host,
		quint32 port,
		PRL_SECURITY_LEVEL securityLevel,
		bool useUnixSockets,
		IOSender::Type type ) :
	m_ioClient( new IOClient(IORoutingTableHelper::GetClientRoutingTable(securityLevel),
				type, host, port, useUnixSockets ) ),
	m_ioClientOwner(true),
	m_rwLock( QReadWriteLock::Recursive ),
	m_isValid(false),
	m_state(Disabled)
{
}

Channel::Channel ( IOClientInterface *ioClient ):
	m_ioClient(ioClient),
	m_ioClientOwner(false),
	m_rwLock(QReadWriteLock::Recursive),
	m_isValid(false),
	m_state(Disabled)
{
}

Channel::~Channel ()
{
	if (m_ioClientOwner)
	{
		disable();
		delete m_ioClient;
	}
}

bool Channel::isValid () const
{
	return m_isValid;
}

void Channel::setValid ( bool v )
{
	m_isValid = v;
}

Channel::State Channel::getState () const
{
	m_rwLock.lockForRead();
        Channel::State st = m_state;
        m_rwLock.unlock();
	return st;
}

void Channel::setState ( Channel::State state )
{
	m_rwLock.lockForWrite();
	m_state = state;
	m_rwLock.unlock();
}

void Channel::enable ()
{
	// Connect to server. Async connect
	getIOClient().connectClient();
}

void Channel::disable ()
{
	getIOClient().disconnectClient();
}

IOSendJob::Result Channel::waitForSend ( const IOSendJob::Handle& h,
		quint32 timeout ) const
{
	return m_ioClient->waitForSend( h, timeout );
}

IOSendJob::Result Channel::getSendResult ( const IOSendJob::Handle& h ) const
{
	return m_ioClient->getSendResult( h );
}

IOClientInterface& Channel::getIOClient ()
{
	return *m_ioClient;
}

const IOClientInterface& Channel::getIOClient () const
{
	return *m_ioClient;
}

IOSendJob::Handle Channel::sendPackage ( IOPackage::Type pkgType,
		const void* data, quint32 size )
{
	LOG_MESSAGE(DBG_DEBUG, "Package : type=%d, dataSize=%d",
			pkgType, size);

	if ( ! isValid() ) {
		LOG_MESSAGE(DBG_WARNING, "Channel is invalid!");
		return IOSendJob::InvalidHandle;
	}

	SmartPtr<IOPackage> package =
		IOPackage::createInstance( pkgType, IOPackage::RawEncoding,
				data, size );
	return m_ioClient->sendPackage( package );
}

IOSendJob::Handle Channel::sendPackage ( IOPackage::Type pkgType,
		const SmartPtr<char>& buff,
		quint32 size )
{
	LOG_MESSAGE(DBG_DEBUG, "Package : type=%d, dataSize=%d",
			pkgType, size);

	if ( ! isValid() ) {
		LOG_MESSAGE(DBG_WARNING, "Channel is invalid!");
		return IOSendJob::InvalidHandle;
	}

	SmartPtr<IOPackage> package =
		IOPackage::createInstance( pkgType, IOPackage::RawEncoding,
				buff, size );
	return m_ioClient->sendPackage( package );
}

IOSendJob::Handle Channel::sendPackage ( const SmartPtr<IOPackage> package )
{
	if ( ! isValid() ) {
		LOG_MESSAGE(DBG_WARNING, "Channel is invalid!");
		return IOSendJob::InvalidHandle;
	}

	if ( ! package.isValid() ) {
		LOG_MESSAGE(DBG_FATAL, "Invalid package");
		return IOSendJob::InvalidHandle;
	}

	return m_ioClient->sendPackage( package );
}

/*****************************************************************************/

