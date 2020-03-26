/*
 * PrlHandleRemoteCommand.h
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


#ifndef __VIRTUOZZO_HANDLE_REMOTE_COMMAND_H__
#define __VIRTUOZZO_HANDLE_REMOTE_COMMAND_H__

#include "PrlHandleEvent.h"
//#include "PrlHandleServer.h"

typedef PrlHandleSmartPtr<class PrlHandleRemoteCommand>	PrlHandleRemoteCommandPtr;

class PrlHandleRemoteCommand : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 *
	 * @param pServer Pointer to the server object where action takes place
	 * @param p package to wrap inside event
	 */
	PrlHandleRemoteCommand(PRL_DEVICE_TYPE	uTargetType		= PDE_GENERIC_DEVICE,
							PRL_UINT32		uTargetIndex	= 0,
							PRL_UINT16		uCommandCode	= 0,
							PRL_VOID_PTR	pDataBuffer		= NULL,
							PRL_UINT32		uDataBufferSize	= 0)
		: PrlHandleBase(PHT_REMOTEDEV_CMD),
		m_uTargetType(uTargetType),
		m_uTargetIndex(uTargetIndex),
		m_uCommandCode(uCommandCode)
	{
		m_ioBuffer.data = pDataBuffer;
		m_ioBuffer.size = uDataBufferSize;
	}

	virtual ~PrlHandleRemoteCommand()
	{
	}

	virtual PRL_DEVICE_TYPE GetTargetType() const
	{
		return m_uTargetType;
	}

	virtual PRL_UINT32 GetTargetIndex() const
	{
		return m_uTargetIndex;
	}

	virtual PRL_UINT16 GetCommandCode() const
	{
		return m_uCommandCode;
	}

	virtual PRL_RESULT GetDataBuffer(PRL_IO_GENERIC_BUFFER_PTR pBuffer) const
	{
		PRL_RESULT	prlResult = PRL_ERR_INVALID_ARG;

		if (pBuffer)
		{
			*pBuffer	= m_ioBuffer;
			prlResult	= PRL_ERR_SUCCESS;
		}

		return prlResult;
	}

protected:
	PRL_DEVICE_TYPE			m_uTargetType;
	PRL_UINT32				m_uTargetIndex;
	PRL_UINT16				m_uCommandCode;
	PRL_IO_GENERIC_BUFFER	m_ioBuffer;
};

#endif	// __VIRTUOZZO_HANDLE_IO_REMOTE_COMMAND_H__
