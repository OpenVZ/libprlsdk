/*
 * CVmStorageDevice.h: Definition of the CVmStorageDevice class.
 * This class implements a generic-type media storage device.
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


#ifndef CVMSTORAGEDEVICE_H
#define CVMSTORAGEDEVICE_H

#include "CVmClusteredDevice.h"

/**
 * @brief This class implements a generic-type media storage device.
 * @author SergeyM
 */
class CVmStorageDevice : public CVmClusteredDevice
{
	Q_OBJECT
public:
	// Standard class constructor
	CVmStorageDevice();

	CVmStorageDevice( const CVmStorageDevice& rhs );

	// Class destructor
	virtual ~CVmStorageDevice();

	CVmStorageDevice& operator=( const CVmStorageDevice& rhs );

	virtual bool operator==( const CVmStorageDevice& cVmStorageDevice );

	void setTargetDeviceName(QString value = QString());
	QString getTargetDeviceName() const;

protected:
	QString m_qsTargetDeviceName;

signals:
	void signalTargetDeviceNameChanged(QString value, QString object_id = QString());

private slots:
	void slotChangeTargetDeviceName(QString value);
};


#endif
