/*
 * PrlHandleServerStat.h
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


#ifndef __VIRTUOZZO_HANDLE_SERVER_STAT_H__
#define __VIRTUOZZO_HANDLE_SERVER_STAT_H__

#include "PrlHandleServer.h"


/**
 * Base handle object - all objects representation on the client
 * side should follow it's interface by the means of inheritance.
 */
class PrlHandleServerStat : public PrlHandleServer
{
public:

	/**
	 * Perform get server host system resources usage statistics.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetStatistics();

	/**
	 * Perform send subscription to host statistics request to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr SubscribeToHostStatistics();

	/**
	 * Perform send unsubscription to host statistics request to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr UnsubscribeFromHostStatistics();

	/**
	 * Perform send perfomance statistics subscription request to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
     */
	PrlHandleJobPtr SubscribePerfStats(PRL_CONST_STR filter)
	{
		return DspCmdSubscribePerfStats(filter) ;
	}

	/**
	 * Perform send unsubscription to host statistics request to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr UnsubscribePerfStats()
    {
    	return DspCmdUnsubscribePerfStats() ;
	}

	/**
     * Perform send perfomance statistics subscription request to the remote server.
     *
     * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetPerfStats(PRL_CONST_STR filter)
	{
		return DspCmdGetPerfStats(filter) ;
	}

	PrlHandleJobPtr DspCmdSubscribePerfStats(PRL_CONST_STR filter, PRL_CONST_STR sVmUuid = NULL) ;

	PrlHandleJobPtr DspCmdUnsubscribePerfStats(PRL_CONST_STR sVmUuid = NULL) ;

	PrlHandleJobPtr DspCmdGetPerfStats(PRL_CONST_STR filter, PRL_CONST_STR sVmUuid = NULL) ;

};

#endif // __VIRTUOZZO_HANDLE_SERVER_STAT_H__
