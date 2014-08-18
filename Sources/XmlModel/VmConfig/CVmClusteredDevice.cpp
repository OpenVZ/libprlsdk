/*
 * CVmClusteredDevice.cpp: Implementation of the CVmClusteredDevice
 * class. This class implements a single clustered device.
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
#include "CVmClusteredDevice.h"


CVmClusteredDevice::CVmClusteredDevice()
{
	setDeviceType(PDE_CLUSTERED_DEVICE);
	setInterfaceType();
	setStackIndex();
	setPassthrough();
	setSubType();
}

CVmClusteredDevice::CVmClusteredDevice( const CVmClusteredDevice& rhs )
: CVmDevice( rhs )
{
	setInterfaceType( rhs.getInterfaceType() );
	setStackIndex( rhs.getStackIndex() );
	setPassthrough( rhs.getPassthrough() );
	setSubType( rhs.getSubType() );
}

CVmClusteredDevice::~CVmClusteredDevice()
{
}

CVmClusteredDevice& CVmClusteredDevice::operator=( const CVmClusteredDevice& rhs )
{
	if ( this != &rhs )
	{
		CVmDevice::operator=( rhs );

		setInterfaceType( rhs.getInterfaceType() );
		setStackIndex( rhs.getStackIndex() );
		setPassthrough( rhs.getPassthrough() );
		setSubType( rhs.getSubType() );
	}
	return *this;
}

bool CVmClusteredDevice::operator==( const CVmClusteredDevice& cVmDevice )
{
	if ( CVmDevice::operator==( cVmDevice ) )
	{
		if(
			(getInterfaceType() == cVmDevice.getInterfaceType()) &&
			(getStackIndex() == cVmDevice.getStackIndex()) &&
			(getPassthrough() == cVmDevice.getPassthrough() &&
			getSubType() == cVmDevice.getSubType())
			)
			return true;
	}
	return false;
}

void CVmClusteredDevice::setInterfaceType(unsigned int value)
{
	bool bValueChanged = (m_uiInterfaceType != value);
	m_uiInterfaceType = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalInterfaceTypeChanged(value, getObjectId());
	}
}

void CVmClusteredDevice::slotChangeInterfaceType(unsigned int value)
{
	setInterfaceType(value);
}

PRL_MASS_STORAGE_INTERFACE_TYPE CVmClusteredDevice::getInterfaceType() const
{
	return (PRL_MASS_STORAGE_INTERFACE_TYPE )m_uiInterfaceType;
}

void CVmClusteredDevice::setStackIndex(unsigned int value)
{
	bool bValueChanged = (m_uiStackIndex != value);
	m_uiStackIndex = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalStackIndexChanged(value, getObjectId());
	}
}

void CVmClusteredDevice::slotChangeStackIndex(unsigned int value)
{
	setStackIndex(value);
}

unsigned int CVmClusteredDevice::getStackIndex() const
{
	return m_uiStackIndex;
}

void CVmClusteredDevice::setPassthrough(unsigned int value)
{
	bool bValueChanged = (m_uiPassthrough != value);
	m_uiPassthrough = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalPassthroughChanged(value, getObjectId());
	}
}

void CVmClusteredDevice::slotChangePassthrough(unsigned int value)
{
	setPassthrough(value);
}

PVE::DevicePassthroughMode CVmClusteredDevice::getPassthrough() const
{
	return (PVE::DevicePassthroughMode )m_uiPassthrough;
}

void CVmClusteredDevice::setSubType(unsigned int value)
{
	bool bValueChanged = (m_uiSubType != value);
	m_uiSubType = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalSubTypeChanged(value, getObjectId());
	}
}

void CVmClusteredDevice::slotChangeSubType(unsigned int value)
{
	setSubType(value);
}

PRL_CLUSTERED_DEVICE_SUBTYPE CVmClusteredDevice::getSubType() const
{
	return (PRL_CLUSTERED_DEVICE_SUBTYPE )m_uiSubType;
}
