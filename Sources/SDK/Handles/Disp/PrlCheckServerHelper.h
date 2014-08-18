/*
 * PrlCheckServerHelper.h
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


#ifndef PRLCHECKSERVERHELPER_H
#define PRLCHECKSERVERHELPER_H

#include "PrlHandleLocalJob.h"
#include "Libraries/IOService/src/IOCommunication/IOClient.h"

using namespace IOService;

/**
 * Class which implements check server operations
 */
class PrlCheckServerHelper : public PrlThread
{
Q_OBJECT
public:
	PrlCheckServerHelper ( const QString& host,
						   PRL_UINT16 port,
					 PRL_EVENT_HANDLER_PTR handler,
					 PRL_VOID_PTR userData,
					 PRL_UINT32 timeout,
					 const PrlHandleLocalJobPtr& job );
	~PrlCheckServerHelper();

private slots:
	void onJobWasCanceled ();

private:
	void concreteRun();

private:
	QString m_sHost;
	PRL_UINT16 m_nPort;
	PRL_UINT32 m_nTimeout;
	PrlHandleLocalJobPtr m_job;
	IOClient *m_pClient;
	PRL_EVENT_HANDLER_PTR m_handler;
	PRL_VOID_PTR m_userData;
	QMutex m_mutex;
};

#endif //PRLCHECKSERVERHELPER_H

