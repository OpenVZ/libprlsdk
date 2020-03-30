/*
 * PrlHandleVirtualDiskEncryption.h
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


#ifndef __VIRTUOZZO_HANDLE_VIRTUAL_DISK_ENCRYPTION_H__
#define __VIRTUOZZO_HANDLE_VIRTUAL_DISK_ENCRYPTION_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/VmConfig/CVmHddEncryption.h>


class PrlHandleVirtualDiskEncryption: public PrlHandleBase
{
public:

	PrlHandleVirtualDiskEncryption();
	explicit PrlHandleVirtualDiskEncryption(const QString& sVirtualDiskEncryption);

	PRL_RESULT fromString(PRL_CONST_STR sXml);
	QString toString();

	/**
	 * Return encryption key identifier
	 *
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 *                           on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * (in this case the buffer will contain only a part of the key identifier)
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT getKeyId(PRL_STR sKeyId, PRL_UINT32_PTR pnKeyIdBufLength);

	/**
	 * Set encryption key identifier
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT setKeyId(PRL_CONST_STR sKeyId);

private:

	CVmHddEncryption m_encryption;
};


typedef PrlHandleSmartPtr<PrlHandleVirtualDiskEncryption> PrlHandleVirtualDiskEncryptionPtr;


#endif	// __VIRTUOZZO_HANDLE_VIRTUAL_DISK_ENCRYPTION_H__
