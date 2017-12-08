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

#include "PrlHandleBackup.h"

PrlHandleBackup::PrlHandleBackup(): PrlHandleBase(PHT_BACKUP)
{
}

PrlHandleBackup::PrlHandleBackup(const PrlHandleVmPtr& vm_, const QString& activity_):
	PrlHandleBase(PHT_BACKUP), m_vm(vm_)
{
	m_activity.fromString(activity_);
}

PRL_RESULT PrlHandleBackup::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_activity.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString PrlHandleBackup::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_activity.toString();
}

PRL_RESULT PrlHandleBackup::getUuid(PRL_STR buffer_, PRL_UINT32_PTR cbBuffer_) const
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_activity.getUuid(), buffer_, cbBuffer_);
}

PRL_BOOL PrlHandleBackup::handleState(PRL_STATES_CALLBACK_TYPE, PRL_INT32, PRL_VOID_PTR)
{
	return PRL_TRUE;
}

PRL_RESULT PrlHandleBackup::getDisk(int index_, PRL_HANDLE& dst_) const
{
	QMutexLocker g(&m_HandleMutex);
	BackupSnapshot* x = m_activity.getSnapshot();
	if (NULL == x)
		return PRL_ERR_INVALID_HANDLE;

	if (x->m_lstSnapshotComponents.size() <= index_)
		return PRL_ERR_INVALID_ARG;

	SnapshotComponent* y = x->m_lstSnapshotComponents.at(index_);
	if (NULL == y)
		return PRL_ERR_INVALID_HANDLE;

	Uuid u(x->getUuid());
	QString s = y->getState();
	g.unlock();

	PRL_HANDLE d = PRL_INVALID_HANDLE;
	PRL_RESULT e = PrlDisk_OpenDisk_Local(&d, qPrintable(s),
			PRL_DISK_READ | PRL_DISK_WRITE, NULL);
	if (PRL_FAILED(e))
		return e;
	dst_ = d;
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleBackup::getDisksCount(PRL_UINT32& dst_) const
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	BackupSnapshot* x = m_activity.getSnapshot();
	if (NULL == x)
		return PRL_ERR_INVALID_HANDLE;

	dst_ = x->m_lstSnapshotComponents.size();
	return PRL_ERR_SUCCESS;
}

