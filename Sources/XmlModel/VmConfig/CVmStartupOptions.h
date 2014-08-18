/*
 * CVmStartupOptions.h: Definition of the CVmStartupOptions class.
 * This class implements a collection of VM startup parameters.
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


#ifndef CVMSTARTUPOPTIONS_H
#define CVMSTARTUPOPTIONS_H


#include "CVmStartupOptionsBase.h"


class CVmDevice;

/**
 * @brief This class implements a collection of VM startup parameters.
 * @author SergeyM
 */
class CVmStartupOptions : public CVmStartupOptionsBase
{

public:

    // A subsidiary class for boot device representation
	class CVmBootDevice : public CVmBootDeviceBase
    {
    public:
		CVmBootDevice()
		: CVmBootDeviceBase()
		{
		}
		CVmBootDevice(const CVmBootDevice& bd)
		: CVmBootDeviceBase(bd)
		{
		}
		virtual ~CVmBootDevice()
		{
		}
		CVmBootDevice( PRL_DEVICE_TYPE devType, uint devIdx, uint numb, bool inUse ) :
			CVmBootDeviceBase(devType, devIdx, numb, inUse)
		{
		}
		// compare operator
		bool operator==( const CVmBootDevice& cVmBootDevice)
		{
			return CVmBootDeviceBase::operator ==(cVmBootDevice);
		}

		// ***DO NOT INSERT IN THIS CLASS ANY PROPERTIES***
		// ***USE BASE CLASS FOR THIS PURPOSE***
    };

	// Standard class constructor
	CVmStartupOptions();
	// Copy constructor
	CVmStartupOptions(CVmStartupOptions *other);
	// Class destructor
	virtual ~CVmStartupOptions();
	// compare operator
	virtual bool operator==( const CVmStartupOptions& cVmStartupOptions );

    // Get boot device list
    QList<CVmBootDevice*> getBootDeviceList () const;
    // Set boot device list
    void setBootDeviceList( QList<CVmBootDevice*> list );

	// Boot device list
    QList<CVmBootDevice* >& m_lstBootDeviceList;

	virtual void cleanupClassProperties();
};

#endif
