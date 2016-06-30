/*
 * PrlHandleVirtualDiskEncryption.cpp
 *
 * Copyright (C) 1999-2016 Parallels IP Holdings GmbH
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


#include "PrlHandleVirtualDiskEncryption.h"

PrlHandleVirtualDiskEncryption::PrlHandleVirtualDiskEncryption()
: PrlHandleBase(PHT_VIRTUAL_DISK_ENCRYPTION)
{
}

PrlHandleVirtualDiskEncryption::PrlHandleVirtualDiskEncryption(const QString& sVirtualDiskEncryption)
: PrlHandleBase(PHT_VIRTUAL_DISK_ENCRYPTION)
{
	m_encryption.fromString(sVirtualDiskEncryption);
}

PRL_RESULT PrlHandleVirtualDiskEncryption::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_encryption.fromString(QString::fromUtf8(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleVirtualDiskEncryption::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_encryption.toString();
}


PRL_RESULT PrlHandleVirtualDiskEncryption::getKeyId(PRL_STR sKeyId, PRL_UINT32_PTR pnKeyIdBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_encryption.getKeyId(), sKeyId, pnKeyIdBufLength);
}

PRL_RESULT PrlHandleVirtualDiskEncryption::setKeyId(PRL_CONST_STR sKeyId)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_encryption.setKeyId(UTF8_2QSTR(sKeyId));
	return PRL_ERR_SUCCESS;
}
