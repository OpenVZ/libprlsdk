/*
 * CVmMassStorageDevice.h: Definition of the CVmMassStorageDevice
 * class. This class implements a generic-type mass storage device.
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


#ifndef CVMMASSSTORAGEDEVICE_H
#define CVMMASSSTORAGEDEVICE_H

#include "CVmStorageDevice.h"
#include "Interfaces/ParallelsNamespace.h"


/**
 * @brief This class implements a generic-type mass storage device
 * @author SergeyM
 */
class CVmMassStorageDevice : public CVmStorageDevice
{
public:

	// Standard class constructor
	CVmMassStorageDevice();

	CVmMassStorageDevice(const CVmMassStorageDevice& rhs );

	// Class destructor
	virtual ~CVmMassStorageDevice();

	CVmMassStorageDevice& operator=( const CVmMassStorageDevice& rhs );

	virtual bool operator==( const CVmMassStorageDevice& cVmMassStorageDevice );

};


#endif
