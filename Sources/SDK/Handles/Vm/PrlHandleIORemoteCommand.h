/*
 * PrlHandleIORemoteCommand.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#ifndef __VIRTUOZZO_HANDLE_IO_REMOTE_COMMAND_H__
#define __VIRTUOZZO_HANDLE_IO_REMOTE_COMMAND_H__

#include <QBuffer>
#include <QByteArray>
#include <QDataStream>

#include "PrlHandleRemoteCommand.h"

class PrlHandleIORemoteCommand : public PrlHandleRemoteCommand
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pServer Pointer to the server object where action takes place
	 * @param p package to wrap inside event
	 */
	PrlHandleIORemoteCommand(const PrlHandleServerPtr&,
		const SmartPtr<IOService::IOPackage> p)
	{
		QBuffer		buffer;
		PRL_UINT32	uTargetType		= 0;
		PRL_UINT32	uTargetIndex	= 0;
		PRL_UINT16	uCommandCode	= 0;

                QByteArray data =
                  QByteArray::fromRawData( p->buffers[0].getImpl(),
                                           p->data[0].bufferSize );
                buffer.setBuffer( &data );
		buffer.open( QIODevice::ReadOnly );

		QDataStream in( &buffer );
		in.setVersion(QDataStream::Qt_4_0);

		// Deserialize from stream
		in >> uTargetType >> uTargetIndex >> uCommandCode;

		m_uTargetType	= (PRL_DEVICE_TYPE)uTargetType;
		m_uTargetIndex	= uTargetIndex;
		m_uCommandCode	= uCommandCode;
		m_ioBuffer.data	= (PRL_CHAR_PTR)p->buffers[0].getImpl() + buffer.pos();
		m_ioBuffer.size	= p->data[0].bufferSize - buffer.pos();
	}

private:
	SmartPtr<IOService::IOPackage>	m_spPackage;
};

#endif	// __VIRTUOZZO_HANDLE_IO_REMOTE_COMMAND_H__
