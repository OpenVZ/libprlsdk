/*
 * CVmStorageDevice.cpp: Implementation of the CVmStorageDevice
 * class. This class implements a generic-type media storage device.
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


#include <prlcommon/Interfaces/ParallelsDomModel.h>
#include "CVmStorageDevice.h"


CVmStorageDevice::CVmStorageDevice()
{
	setDeviceType( PDE_STORAGE_DEVICE );
}

CVmStorageDevice::CVmStorageDevice( const CVmStorageDevice& rhs )
 : CVmClusteredDevice( rhs )
{
	setTargetDeviceName(rhs.getTargetDeviceName());
}

CVmStorageDevice::~CVmStorageDevice()
{
}

CVmStorageDevice& CVmStorageDevice::operator=( const CVmStorageDevice& rhs )
{
	if ( this != &rhs )
	{
		CVmClusteredDevice::operator=( rhs );
		setTargetDeviceName(rhs.getTargetDeviceName());
	}
	return *this;
}

bool CVmStorageDevice::operator==( const CVmStorageDevice& cVmStorageDevice )
{
	return CVmClusteredDevice::operator==( cVmStorageDevice );
}

void CVmStorageDevice::setTargetDeviceName(QString value)
{
	bool bValueChanged = (m_qsTargetDeviceName != value);
	m_qsTargetDeviceName = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalTargetDeviceNameChanged(value, getObjectId());
	}
}

void CVmStorageDevice::slotChangeTargetDeviceName(QString value)
{
	setTargetDeviceName(value);
}

QString CVmStorageDevice::getTargetDeviceName() const
{
	return m_qsTargetDeviceName;
}
