/*
 * PrlHandleBackupResult.h
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


#ifndef __PARALLELS_HANDLE_BACKUP_RESULT_H__
#define __PARALLELS_HANDLE_BACKUP_RESULT_H__

#include "PrlHandleBase.h"

/**
 * Handle object represents info about created backup
 */
class PrlHandleBackupResult : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 */
	PrlHandleBackupResult(const QString& sBackupUuid)
		: PrlHandleBase(PHT_BACKUP_RESULT), m_sBackupUuid(sBackupUuid){};

	/**
	 * Class destructor.
	 */
	~PrlHandleBackupResult(){};

	/**
	 * Returns uuid of new backup
	 */
	PRL_RESULT GetBackupUuid(PRL_STR sBackupUuidBuf, PRL_UINT32_PTR pnBackupUuidBufLength)
	{
		return CopyStringValue(m_sBackupUuid, sBackupUuidBuf, pnBackupUuidBufLength);
	}

private:
	QString m_sBackupUuid;
};

#endif // __PARALLELS_HANDLE_BACKUP_RESULT_H__
