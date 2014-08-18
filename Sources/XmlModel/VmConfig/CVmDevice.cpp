/*
 * CVmDevice.cpp: Implementation of the CVmDevice class. This class
 * implements the basic VM device. All the devices, which can be
 * included into the VM hardware list, inherit from this class.
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


#include <QTextStream>

#include "ParallelsDomModel.h"
#include "CVmHardware.h"
#include <QDir>
#include <QFileInfo>
#include "Libraries/Std/PrlAssert.h"


CVmDevice::CVmDevice()
{
	static bool first_time = true;
	if (first_time)
	{
		qRegisterMetaType<PRL_DEVICE_TYPE >("PRL_DEVICE_TYPE");
		first_time = false;
	}

	setDeviceType( PDE_GENERIC_DEVICE );
	Init();
}

CVmDevice::CVmDevice( const CVmDevice& rhs )
: CBaseNodeSignals()
{
	setDeviceType( rhs.getDeviceType() );
	setIndex( rhs.getIndex() );
	setEnabled( rhs.getEnabled() );
	setConnected( rhs.getConnected() );
	setEmulatedType( rhs.getEmulatedType() );
	setSystemName( rhs.getSystemName() );
	setUserFriendlyName( rhs.getUserFriendlyName() );
	setRemote( rhs.isRemote() );
	setDescription( rhs.getDescription() );
}

void CVmDevice::Init()
{
	setEmulatedType( PVE::RealGenericDevice );
	setIndex();
	setEnabled();
	setConnected();
	setRemote();
}

CVmDevice::~CVmDevice()
{
}

CVmDevice& CVmDevice::operator=( const CVmDevice& rhs )
{
	if ( this != &rhs )
	{
		setDeviceType( rhs.getDeviceType() );
		setIndex( rhs.getIndex() );
		setEnabled( rhs.getEnabled() );
		setConnected( rhs.getConnected() );
		setEmulatedType( rhs.getEmulatedType() );
		setSystemName( rhs.getSystemName() );
		setUserFriendlyName( rhs.getUserFriendlyName() );
		setRemote( rhs.isRemote() );
		setDescription( rhs.getDescription() );
	}

	return *this;
}

bool CVmDevice::operator==( const CVmDevice& cVmDevice )
{
	return (
		(getDeviceType() == cVmDevice.getDeviceType()) &&
		(getEnabled() == cVmDevice.getEnabled()) &&
		(getConnected() == cVmDevice.getConnected()) &&
		(getEmulatedType() == cVmDevice.getEmulatedType()) &&
		(getSystemName() == cVmDevice.getSystemName()) &&
		(getUserFriendlyName() == cVmDevice.getUserFriendlyName()) &&
		(isRemote() == cVmDevice.isRemote()) &&
		(getIndex() == cVmDevice.getIndex())
		);
}

QString CVmDevice::getObjectId() const
{
	return QString("%1 %2").arg((unsigned int )getDeviceType()).arg(getIndex());
}

void CVmDevice::setDeviceType(PRL_DEVICE_TYPE value)
{
	bool bValueChanged = (m_uiDeviceType != value);
	m_uiDeviceType = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalDeviceTypeChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeDeviceType(PRL_DEVICE_TYPE value)
{
	setDeviceType(value);
}

PRL_DEVICE_TYPE CVmDevice::getDeviceType() const
{
	return m_uiDeviceType;
}

void CVmDevice::setEnabled(unsigned int value)
{
	bool bValueChanged = (m_uiEnabled != value);
	m_uiEnabled = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalEnabledChanged(value, getObjectId());
		emit signalEnabledChanged(value == PVE::DeviceEnabled, getObjectId());
	}
}

void CVmDevice::slotChangeEnabled(unsigned int value)
{
	setEnabled(value);
}

void CVmDevice::slotChangeEnabled(bool value)
{
	setEnabled(value ? PVE::DeviceEnabled : PVE::DeviceDisabled);
}

PVE::DeviceEnabledState CVmDevice::getEnabled() const
{
	return (PVE::DeviceEnabledState )m_uiEnabled;
}

void CVmDevice::setConnected(unsigned int value)
{
	bool bValueChanged = (m_uiConnected != value);
	m_uiConnected = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalConnectedChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeConnected(unsigned int value)
{
	setConnected(value);
}

PVE::DeviceConnectedState CVmDevice::getConnected() const
{
	return (PVE::DeviceConnectedState )m_uiConnected;
}

void CVmDevice::setEmulatedType(unsigned int value)
{
	bool bValueChanged = (m_uiEmulatedType != value);
	m_uiEmulatedType = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalEmulatedTypeChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeEmulatedType(unsigned int value)
{
	setEmulatedType(value);
}

unsigned int CVmDevice::getEmulatedType() const
{
	return m_uiEmulatedType;
}

void CVmDevice::setIndex(unsigned int value)
{
	bool bValueChanged = (m_uiIndex != value);
	m_uiIndex = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalIndexChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeIndex(unsigned int value)
{
	setIndex(value);
}

unsigned int CVmDevice::getIndex() const
{
	return m_uiIndex;
}

void CVmDevice::setSystemName(QString value)
{
	bool bValueChanged = (m_qsSystemName != value);
	m_qsSystemName = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalSystemNameChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeSystemName(QString value)
{
	setSystemName(value);
}

QString CVmDevice::getSystemName() const
{
	return m_qsSystemName;
}

void CVmDevice::setUserFriendlyName(QString value)
{
	bool bValueChanged = (m_qsUserFriendlyName != value);
	m_qsUserFriendlyName = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalUserFriendlyNameChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeUserFriendlyName(QString value)
{
	setUserFriendlyName(value);
}

QString CVmDevice::getUserFriendlyName() const
{
	return m_qsUserFriendlyName;
}

void CVmDevice::setRemote(bool value)
{
	bool bValueChanged = (m_bRemote != value);
	m_bRemote = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalRemoteChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeRemote(bool value)
{
	setRemote(value);
}

bool CVmDevice::isRemote() const
{
	return m_bRemote;
}

void CVmDevice::setDescription(QString value)
{
	bool bValueChanged = (m_qsDeviceDescription != value);
	m_qsDeviceDescription = value;
	if (isSignalsEnabled() && bValueChanged)
	{
		emit signalDescriptionChanged(value, getObjectId());
	}
}

void CVmDevice::slotChangeDescription(QString value)
{
	setDescription(value);
}

QString CVmDevice::getDescription() const
{
	return m_qsDeviceDescription;
}

//////////////////////////////////////////////////////////////////////////////

void CVmDevice::setRelativeSystemName( const QString & strVmDirectory, bool bShadowVm)
{
	if (getSystemName().isEmpty())
		return;

	PRL_DEVICE_TYPE type = getDeviceType();
	switch(type)
	{
		case PDE_FLOPPY_DISK:
		case PDE_HARD_DISK:
		case PDE_OPTICAL_DISK:
		case PDE_PARALLEL_PORT:
		case PDE_SERIAL_PORT:
		{
			// #127869
			bool bIsRealOrBootcampHdd =
				(type == PDE_HARD_DISK)
				&& ( getEmulatedType() == PDT_USE_REAL_HDD || getEmulatedType() == PDT_USE_BOOTCAMP);
			if (
					( bIsRealOrBootcampHdd
					|| getEmulatedType() == PDT_USE_IMAGE_FILE
					|| getEmulatedType() == PDT_USE_OUTPUT_FILE
					)
					&& ! isRemote()
				)
			{
				if(/*getSystemName().contains(strVmDirectory) &&*/
							QDir(strVmDirectory).exists() &&
							QFileInfo(getSystemName()).isAbsolute() &&
							( bIsRealOrBootcampHdd	|| QFileInfo(getUserFriendlyName()).isAbsolute()
							)
					)
				{
					QString strImageDir = QFileInfo(getSystemName()).dir().absolutePath();
					Qt::CaseSensitivity caseSens = Qt::CaseSensitive;
#ifndef _LIN_
					caseSens = Qt::CaseInsensitive;
#endif
					QString sRelativePath = getSystemName();

					if (strImageDir.startsWith(strVmDirectory,caseSens))
					{
						sRelativePath.remove(strVmDirectory);
						while (sRelativePath.startsWith('/') || sRelativePath.startsWith('\\'))
							sRelativePath.remove(0, 1);
					}
					else if (bShadowVm)
					{
						// NOTE: VM bundle is sub directory of native VM
						//       and it lays in native VM bundle
						QString qsShadowVmDir = QFileInfo(strVmDirectory).absolutePath();
						if (strImageDir.startsWith(qsShadowVmDir,caseSens))
						{
							sRelativePath.remove(qsShadowVmDir);
							while (sRelativePath.startsWith('/') || sRelativePath.startsWith('\\'))
								sRelativePath.remove(0, 1);
							sRelativePath = "../" + sRelativePath;
						}
					}

					setSystemName( sRelativePath );
					if( !bIsRealOrBootcampHdd )
						setUserFriendlyName(getSystemName());
				}
			}
			break;
		}
		default:
			break;
	}
}

QString CVmDevice::RevertToInitialSystemName(const QString & strVmDirectory)
{
	if (getSystemName().isEmpty() || getUserFriendlyName().isEmpty())
		return "";

	PRL_DEVICE_TYPE type = getDeviceType();
	switch(type)
	{
		case PDE_FLOPPY_DISK:
		case PDE_HARD_DISK:
		case PDE_OPTICAL_DISK:
		case PDE_PARALLEL_PORT:
		case PDE_SERIAL_PORT:
		{
			// #127869
			bool bIsRealOrBootcampHdd =
					(type == PDE_HARD_DISK)
					&& ( getEmulatedType() == PDT_USE_REAL_HDD || getEmulatedType() == PDT_USE_BOOTCAMP);

			if (
					( bIsRealOrBootcampHdd
					|| getEmulatedType() == PDT_USE_IMAGE_FILE
					|| getEmulatedType() == PDT_USE_OUTPUT_FILE
					)
					&& ! isRemote()
				)
			{
				if( !strVmDirectory.isEmpty()
					&& !QFileInfo(getSystemName()).isAbsolute()
					&& ( bIsRealOrBootcampHdd || !QFileInfo(getUserFriendlyName()).isAbsolute() )
					)
				{
					QString strTemp;
					if (getSystemName().startsWith("../"))
						strTemp = QFileInfo(strVmDirectory).absolutePath()
							+ "/" + getSystemName().remove("../");
					else
						strTemp = strVmDirectory + "/" + getSystemName();

					setSystemName(strTemp);
					if(!bIsRealOrBootcampHdd)
						setUserFriendlyName(getSystemName());
				}
			}
			break;
		}
		default:
			break;
	}
	return getSystemName();
}

int CVmDevice::readXml(QDomElement *parent_element, QString /*ext_tag_name*/, bool /*unite_with_loaded*/)
{
	if (parent_element->tagName() == XML_VM_CONFIG_EL_FLOPPY_DISK)
		m_uiDeviceType = PDE_FLOPPY_DISK;
	else if (parent_element->tagName() == XML_VM_CONFIG_EL_HARD_DISK)
		m_uiDeviceType = PDE_HARD_DISK;
	else if (parent_element->tagName() == XML_VM_CONFIG_EL_OPTICAL_DISK)
		m_uiDeviceType = PDE_OPTICAL_DISK;
	else if (parent_element->tagName() == XML_VM_CONFIG_EL_PARALLEL_PORT
				|| parent_element->tagName() == "Printer")
		m_uiDeviceType = PDE_PARALLEL_PORT;
	else if (parent_element->tagName() == XML_VM_CONFIG_EL_SERIAL_PORT)
		m_uiDeviceType = PDE_SERIAL_PORT;
	else if (parent_element->tagName() == XML_VM_CONFIG_EL_USB_DEVICE)
		m_uiDeviceType = PDE_USB_DEVICE;
	else if (parent_element->tagName() == XML_VM_CONFIG_EL_SOUND_DEVICE)
		m_uiDeviceType = PDE_SOUND_DEVICE;
	else if (parent_element->tagName() == XML_VM_CONFIG_EL_GEN_NET_ADAPTER)
		m_uiDeviceType = PDE_GENERIC_NETWORK_ADAPTER;
	else if (parent_element->tagName() == "GenericPciDevice")
		m_uiDeviceType = PDE_GENERIC_PCI_DEVICE;
	else if (parent_element->tagName() == "GenericScsiDevice")
		m_uiDeviceType = PDE_GENERIC_SCSI_DEVICE;
	else if (parent_element->tagName() == "PciVideoAdapter")
		m_uiDeviceType = PDE_PCI_VIDEO_ADAPTER;
	else if (parent_element->tagName() == "GenericDevice")
		m_uiDeviceType = PDE_GENERIC_DEVICE;
	else
		return PRL_ERR_PARSE_VM_CONFIG;

	return (PRL_ERR_SUCCESS);
}

CVmDevice * CVmDevice::getDeviceInstanceFromString( const QString & strDevice )
{
	CVmDevice cDevice;
	cDevice.fromString( strDevice );
	if ( PRL_FAILED( cDevice.m_iParseRc ) )
		return NULL;


	CVmDevice * pDevToReturn = NULL;
	switch( cDevice.getDeviceType() )
	{
		case PDE_FLOPPY_DISK:
			pDevToReturn = new CVmFloppyDisk( );
			break;

		case PDE_HARD_DISK:
			pDevToReturn = new CVmHardDisk( );
			break;

		case PDE_OPTICAL_DISK:
			pDevToReturn = new CVmOpticalDisk( );
			break;

		case PDE_PARALLEL_PORT:
			pDevToReturn = new CVmParallelPort( );
			break;

		case PDE_SERIAL_PORT:
			pDevToReturn = new CVmSerialPort( );
			break;

		case PDE_USB_DEVICE:
			pDevToReturn = new CVmUsbDevice( );
			break;

		case PDE_SOUND_DEVICE:
		case PDE_MIXER_DEVICE:
			pDevToReturn = new CVmSoundDevice( );
			break;

		case PDE_GENERIC_NETWORK_ADAPTER:
			pDevToReturn = new CVmGenericNetworkAdapter( );
			break;

		case PDE_GENERIC_DEVICE:
			pDevToReturn = new CVmGenericDevice( );
			break;

		case PDE_GENERIC_PCI_DEVICE:
			pDevToReturn = new CVmGenericPciDevice( );
			break;

		case PDE_PCI_VIDEO_ADAPTER:
			pDevToReturn = new CVmPciVideoAdapter( );
			break;
		case PDE_GENERIC_SCSI_DEVICE:
			pDevToReturn = new CVmGenericScsiDevice( );
			break;
		case PDE_PRINTER:
		case PDE_CLUSTERED_DEVICE:
		case PDE_STORAGE_DEVICE:
		case PDE_VIRTUAL_SNAPSHOT_DEVICE:
		default:
			PRL_ASSERT( pDevToReturn );
			// Unknown Device Type
			break;
	}

	if ( pDevToReturn )
	{
		pDevToReturn->fromString( strDevice );
		if ( PRL_FAILED( pDevToReturn->m_iParseRc ) )
			return NULL;
	}

	return pDevToReturn;
}

