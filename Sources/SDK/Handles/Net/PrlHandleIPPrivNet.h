/*
 * PrlHandleIPPrivNet.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */

#ifndef __PARALLELS_HANDLE_PRIVATE_NETWORK_H__
#define __PARALLELS_HANDLE_PRIVATE_NETWORK_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/NetworkConfig/CPrivateNetwork.h>


/**
 * Handle object represents IP private network
 */
class PrlHandleIPPrivNet: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleIPPrivNet();

	/**
	 * Class initialize constructor.
	 */
	PrlHandleIPPrivNet(QString sPrivateNetwork);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	/**
	 * Create new private network handle
	 */
	static PRL_RESULT Create(PRL_HANDLE_PTR phPrivNet);

	/**
	 * Returns private network name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sName, PRL_UINT32_PTR pnNameBufLength);

	/**
	 * Sets private network addresses list
	 * @param [in] new addresses list
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetNetAddresses(const QStringList &lstNetAddresses);

	/**
	 * Returns private network addresses list
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to allocate strings list object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetNetAddresses(PRL_HANDLE_PTR phNetAddresses);

	/**
	 * Sets private network name
	 * @param [in] new name
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetName(PRL_CONST_STR sName);

	/**
	 * Returns is it global private network or not
	 * @param [out] pointer to boolean for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsGlobal(PRL_BOOL_PTR pbGlobal);

	/**
	 * Sets the operating mode of the private network
	 * @param [in] mode - global or not
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetGlobal(PRL_BOOL bGlobal);

private:

	CPrivateNetwork m_PrivateNetwork;
};


typedef PrlHandleSmartPtr<PrlHandleIPPrivNet> PrlHandleIPPrivNetPtr;


#endif	// __PARALLELS_HANDLE_PRIVATE_NETWORK_H__
