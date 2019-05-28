/*
 * PrlHandleServerDeprecated.h
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


#ifndef __PARALLELS_HANDLE_SERVER_DEPRECATED_H__
#define __PARALLELS_HANDLE_SERVER_DEPRECATED_H__

#include "PrlHandleServer.h"


/**
 * Base handle object - all objects representation on the client
 * side should follow it's interface by the means of inheritance.
 */
class PrlHandleServerDeprecated : public PrlHandleServer
{
public:

	/**
	* Perform start convert hdds request to server.
	*
	* @return pointer to the class that abstracts the job process
	*/
	PrlHandleJobPtr StartConvertHdd(PRL_HANDLE hStringsList);

	/**
	 * Perform get server runtime info command to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr SmcGetRuntimeInfo();

	/**
	 * Perform stand by all guests with generic PCI devices.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr PrepareForHibernate(PRL_UINT32 nFlags);

	/**
	* Performs to wake up generic PCI devices after host resume.
	*
	* @return pointer to the class that abstracts the job process
	*/
	PrlHandleJobPtr AfterHostResume(PRL_UINT32 nFlags);

	/**
	 * Adds Network Adapter.
	 * @param handle of PHT_DISP_NET_ADPTER type
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr AddNetAdapter(PRL_HANDLE hNetAdapter);

	/**
	 * Deletes Network Adapter.
	 * @param Index of Network Adapter
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr DeleteNetAdapter(PRL_UINT32 nIndex);

	/**
	 * Updates Network Adapter.
	 * @param handle of PHT_DISP_NET_ADPTER type
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr UpdateNetAdapter(PRL_HANDLE hNetAdapter);

	PrlHandleJobPtr RestartNetworkShaping(PRL_UINT32 nFlags);

	PrlHandleJobPtr DspCmdStorageSetValue(PRL_CONST_STR sKey, PRL_CONST_STR sValue, PRL_UINT32 nFlags);

	/**
	* send to server client statistics
	* @param sStatistics - statistics as string
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdSendClientStatistics( PRL_CONST_STR sStatistics, PRL_UINT32 nFlags );

	/**
	* updates usb assoc list on server
	* @param PRL_HANDLE hStringsList - usb assoc list
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdUpdateUsbAssocList( PRL_HANDLE hList, PRL_UINT32 nListVersion, PRL_UINT32 nFlags );

	/**
	* Update VM device information on server side
	* @param VM device configuration
	*/
	PrlHandleJobPtr DspCmdUpdateDeviceInfo(PRL_CONST_STR sVmDeviceConfiguration);

};

#endif // __PARALLELS_HANDLE_SERVER_DEPRECATED_H__
