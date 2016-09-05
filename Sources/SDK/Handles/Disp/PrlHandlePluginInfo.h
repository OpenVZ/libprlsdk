
/*
 * PrlHandlePluginInfo.h
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

#ifndef __PARALLELS_HANDLE_PLUGIN_INFO_H__
#define __PARALLELS_HANDLE_PLUGIN_INFO_H__


#include "PrlHandleBase.h"
#include <prlcommon/Messaging/CVmEvent.h>


/**
 * Plugin info handle
 */
class PrlHandlePluginInfo: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandlePluginInfo();

	/**
	 * Class initialize constructor.
	 */
	PrlHandlePluginInfo(QString sPluginInfo);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	/**
	 * Gets plugin vendor
	 */
	PRL_RESULT GetVendor(PRL_STR sVendor, PRL_UINT32_PTR pnBufLength);

	/**
	 * Gets plugin copyright
	 */
	PRL_RESULT GetCopyright(PRL_STR sCopyright, PRL_UINT32_PTR pnBufLength);

	/**
	 * Gets short description
	 */
	PRL_RESULT GetShortDescription(PRL_STR sDescr, PRL_UINT32_PTR pnBufLength);

	/**
	 * Gets long description
	 */
	PRL_RESULT GetLongDescription(PRL_STR sDescr, PRL_UINT32_PTR pnBufLength);

	/**
	 * Gets plugin version
	 */
	PRL_RESULT GetVersion(PRL_STR sVersion, PRL_UINT32_PTR pnBufLength);

	/**
	 * Gets plugin id in GUID format
	 */
	PRL_RESULT GetId(PRL_STR sId, PRL_UINT32_PTR pnBufLength);

private:

	CVmEvent m_VmEvent;
};


typedef PrlHandleSmartPtr<PrlHandlePluginInfo> PrlHandlePluginInfoPtr;


#endif	// __PARALLELS_HANDLE_PLUGIN_INFO_H__
