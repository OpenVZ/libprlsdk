/* Copyright (c) 2017 Virtuozzo International GmbH.  All rights reserved.
 *
 * This file is part of OpenVZ. OpenVZ is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland.
 */
#ifndef __PARALLELS_HANDLE_BACKUP_H__
#define __PARALLELS_HANDLE_BACKUP_H__

#include <prlxmlmodel/BackupActivity/BackupActivity.h>
#include "PrlHandleBase.h"


/**
 * Handle object represents a backup activity.
 */
class PrlHandleBackup: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	PrlHandleBackup();

	/**
	 * Class initialize constructor.
	 */
	PrlHandleBackup(const PrlHandleVmPtr& vm_, const QString& activity_);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

	const PrlHandleVmPtr& getVm() const
	{
		return m_vm;
	}

	PRL_RESULT getUuid(PRL_STR buffer_, PRL_UINT32_PTR cbBuffer_) const;

	PRL_RESULT getDisk(int index_, PRL_HANDLE& dst_) const;

	PRL_RESULT getDisksCount(PRL_UINT32& dst_) const;

private:
	static PRL_BOOL handleState(PRL_STATES_CALLBACK_TYPE, PRL_INT32, PRL_VOID_PTR);

	PrlHandleVmPtr m_vm;
	BackupActivity m_activity;
};


typedef PrlHandleSmartPtr<PrlHandleBackup> PrlHandleBackupPtr;


#endif	// __PARALLELS_HANDLE_BACKUP_H__
