/*
 * PrlHandleServerNet.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#ifndef __PARALLELS_HANDLE_SERVER_NET_H__
#define __PARALLELS_HANDLE_SERVER_NET_H__

#include "PrlHandleOfflineService.h"
#include "PrlHandleServer.h"


/**
 * Base handle object - all objects representation on the client
 * side should follow it's interface by the means of inheritance.
 */
class PrlHandleServerNet : public PrlHandleServer
{
public:

	/**
	 * Perform get list information of all virtual networks.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetVirtualNetworkList(PRL_UINT32 nFlags);

	/**
	 * Perform add virtual network.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr AddVirtualNetwork(PRL_HANDLE hVirtNet, PRL_UINT32 nFlags);

	/**
	 * Perform update virtual network.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr UpdateVirtualNetwork(PRL_HANDLE hVirtNet, PRL_UINT32 nFlags);

	/**
	 * Perform delete virtual network.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr DeleteVirtualNetwork(PRL_HANDLE hVirtNet, PRL_UINT32 nFlags);

	/**
	 * Performs network service start operation to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr NetServiceStart();

	/**
	 * Performs network service stop operation to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr NetServiceStop();

	/**
	 * Performs network service restart operation to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr NetServiceRestart();

	/**
	 * Performs network service restore defaults operation to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr NetServiceRestoreDefaults();

	/**
	 * Perform getting Net service status.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetNetServiceStatus();

	PrlHandleJobPtr UpdateOfflineService(PrlHandleOfflineServicePtr pOffmgmtService, PRL_UINT32 nFlags);

	PrlHandleJobPtr DeleteOfflineService(PrlHandleOfflineServicePtr pOffmgmtService, PRL_UINT32 nFlags);

	PrlHandleJobPtr GetOfflineServicesList(PRL_UINT32 nFlags);

	PrlHandleJobPtr UpdateNetworkClassesList(PrlHandleHandlesListPtr pList, PRL_UINT32 nFlags);

	PrlHandleJobPtr GetNetworkClassesList(PRL_UINT32 nFlags);

	PrlHandleJobPtr UpdateNetworkShapingConfig(PRL_HANDLE hConfig, PRL_UINT32 nFlags);

	PrlHandleJobPtr GetNetworkShapingConfig(PRL_UINT32 nFlags);

	/**
	 * Perform add of private network.
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr AddIPPrivateNetwork(PRL_HANDLE hPrivNet, PRL_UINT32 nFlags);

	/**
	 * Perform removal of private network.
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr RemoveIPPrivateNetwork(PRL_HANDLE hPrivNet, PRL_UINT32 nFlags);

	/**
	 * Perform update of private network.
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr UpdateIPPrivateNetwork(PRL_HANDLE hPrivNet, PRL_UINT32 nFlags);

	/**
	 * Perform get list information of all private networks.
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetIPPrivateNetworksList(PRL_UINT32 nFlags);

};

#endif // __PARALLELS_HANDLE_SERVER_NET_H__
