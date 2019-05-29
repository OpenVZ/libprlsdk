/*
 * PrlCheckServerHelper.cpp
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


#include "PrlCheckServerHelper.h"
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/IOService/IOCommunication/IORoutingTableHelper.h>

PrlCheckServerHelper::PrlCheckServerHelper ( const QString& host,
                                             PRL_UINT16 port,
                                             PRL_EVENT_HANDLER_PTR handler,
                                             PRL_VOID_PTR userData,
                                             PRL_UINT32 timeout,
                                             const PrlHandleLocalJobPtr& job )
:
m_sHost( host ),
m_nPort( port == 0 ? PrlGetDefaultListenPort() : port ),
m_nTimeout( timeout ),
m_job( job ),
m_pClient( 0 ),
m_handler( handler ),
m_userData( userData )
{
	bool bConnected = QObject::connect(m_job.getHandle(),
							SIGNAL(jobWasCanceled()),
							SLOT(onJobWasCanceled()),
							Qt::DirectConnection);
	PRL_ASSERT(bConnected);
	start();
}

PrlCheckServerHelper::~PrlCheckServerHelper()
{
	wait();
	QMutexLocker _lock( &m_mutex );
	delete m_pClient;
	m_pClient = 0;
	_lock.unlock();
}

void PrlCheckServerHelper::onJobWasCanceled()
{
	QMutexLocker _lock( &m_mutex );
	if ( m_pClient )
		m_pClient->disconnectClient();
}

void PrlCheckServerHelper::concreteRun()
{
	if ( ! m_sHost.isEmpty() )
	{
		IOSender::State state = IOSender::Disconnected;
		QMutexLocker _lock( &m_mutex );
		if ( ! m_job->wasCanceled() )
		{
			m_pClient = new IOClient( IORoutingTableHelper::GetClientRoutingTable(PSL_HIGH_SECURITY),
									IOSender::Client, m_sHost, m_nPort );
			m_pClient->connectClient( m_nTimeout );
			_lock.unlock();
			state = m_pClient->waitForConnection();
		}
		_lock.relock();
		if ( ! m_job->wasCanceled() )
		{
			if ( IOSender::Connected != state )
				WRITE_TRACE(DBG_FATAL, "m_pClient->error()=%u", m_pClient->error());
			if ( IOSender::Connected == state )
				m_job->SetReturnCode( PRL_ERR_SUCCESS );
			else if ( IOSender::ConnectionTimeout == m_pClient->error() )
				m_job->SetReturnCode( PRL_ERR_TIMEOUT );
			else
				m_job->SetReturnCode( PRL_ERR_CANT_CONNECT_TO_DISPATCHER );
		}
	}
	else
		m_job->SetReturnCode( PRL_ERR_INVALID_ARG );
	if ( m_handler )
	{
		m_job->AddRef();
		m_handler( m_job->GetHandle(), m_userData );
	}
}
