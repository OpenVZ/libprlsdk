/*
 * CVmMassStorageDevice.cpp: Implementation of the
 * CVmMassStorageDevice class. This class implements a generic-type
 * mass storage device.
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


#include "ParallelsDomModel.h"
#include "CVmMassStorageDevice.h"


CVmMassStorageDevice::CVmMassStorageDevice()
{
	setDeviceType( PDE_MASSSTORAGE_DEVICE );
}

CVmMassStorageDevice::CVmMassStorageDevice( const CVmMassStorageDevice& rhs )
 : CVmStorageDevice( rhs )
{
}

CVmMassStorageDevice::~CVmMassStorageDevice()
{
}

CVmMassStorageDevice& CVmMassStorageDevice::operator=( const CVmMassStorageDevice& rhs )
{
	if ( this != &rhs )
	{
		CVmStorageDevice::operator=( rhs );
	}
	return *this;
}

bool CVmMassStorageDevice::operator==( const CVmMassStorageDevice& cVmMassStorageDevice )
{
	return CVmStorageDevice::operator==(cVmMassStorageDevice);
}
