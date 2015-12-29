/*
 * CHostHardwareInfo.cpp: Implementation of the CHostHardwareInfo
 * class. This class implements hardware list detected on the
 * Dispatcher's host.
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
#include "CHostHardwareInfo.h"
#include <prlcommon/Logging/Logging.h>

// By adding this interface we enable allocations tracing in the module
#include "Interfaces/Debug.h"

// Standard class constructor
CHostHardwareInfo::CHostHardwareInfo()
 : m_lstFloppyDisks(*((QList<CHwGenericDevice* >* )&m_lstFloppyDisksBase[0]->m_lstFloppyDisk)),
	m_lstOpticalDisks(*((QList<CHwGenericDevice* >* )&m_lstCdROMs[0]->m_lstCdROM)),
	m_lstHardDisks(m_lstHardDisksBase[0]->m_lstHardDisk),
	m_lstSerialPorts(*((QList<CHwGenericDevice* >* )&m_lstSerialPortsBase[0]->m_lstSerialPort)),
	m_lstParallelPorts(*((QList<CHwGenericDevice* >* )&m_lstParallelPortsBase[0]->m_lstParallelPort)),
	m_lstNetworkAdapters(m_lstNetworkAdaptersBase[0]->m_lstNetworkAdapter),
	m_lstSoundOutputDevices(*((QList<CHwGenericDevice* >* )&m_lstSoundDevices[0]->m_lstOutputDevices[0]->m_lstOutputDevice)),
	m_lstSoundMixerDevices(*((QList<CHwGenericDevice* >* )&m_lstSoundDevices[0]->m_lstMixerDevices[0]->m_lstMixerDevice)),
	m_lstUsbDevices(*((QList<CHwUsbDevice* >* )&m_lstUsbDevicesBase[0]->m_lstUsbDevice)),
	m_lstPrinters(*((QList<CHwPrinter* >* )&m_lstPrintersBase[0]->m_lstPrinter)),
	m_lstGenericPciDevices(m_lstGenericPciDevicesBase[0]->m_lstGenericPciDevice),
	m_lstGenericScsiDevices(*((QList<CHwGenericDevice* >* )&m_lstGenericScsiDevicesBase[0]->m_lstGenericScsiDevice))
{
//	m_strSchemaVersion = XML_VM_CONFIG_SCHEMA_VERSION;

	m_szErrMsg = "";
	m_iErrLine = m_iErrCol = -1;
	m_uiRcInit = PRL_ERR_SUCCESS;
	m_iParseRc = HostHwInfoParser::RcSuccess;

//	m_pOsVersion = NULL;
//	m_pCpu = NULL;
//	m_pMemorySettings = NULL;
	cleanupClassProperties();

//	m_pOsVersion = new CHwOsVersion();
//	m_pCpu = new CHwCpu();
//	m_pMemorySettings = new CHwMemorySettings();

} // CHostHardwareInfo::CHostHardwareInfo()


// Copy constructor
CHostHardwareInfo::CHostHardwareInfo(CHostHardwareInfo *other)
 : CHostHardwareInfoBase((CHostHardwareInfoBase* )other),
	m_lstFloppyDisks(*((QList<CHwGenericDevice* >* )&m_lstFloppyDisksBase[0]->m_lstFloppyDisk)),
	m_lstOpticalDisks(*((QList<CHwGenericDevice* >* )&m_lstCdROMs[0]->m_lstCdROM)),
	m_lstHardDisks(m_lstHardDisksBase[0]->m_lstHardDisk),
	m_lstSerialPorts(*((QList<CHwGenericDevice* >* )&m_lstSerialPortsBase[0]->m_lstSerialPort)),
	m_lstParallelPorts(*((QList<CHwGenericDevice* >* )&m_lstParallelPortsBase[0]->m_lstParallelPort)),
	m_lstNetworkAdapters(m_lstNetworkAdaptersBase[0]->m_lstNetworkAdapter),
	m_lstSoundOutputDevices(*((QList<CHwGenericDevice* >* )&m_lstSoundDevices[0]->m_lstOutputDevices[0]->m_lstOutputDevice)),
	m_lstSoundMixerDevices(*((QList<CHwGenericDevice* >* )&m_lstSoundDevices[0]->m_lstMixerDevices[0]->m_lstMixerDevice)),
	m_lstUsbDevices(*((QList<CHwUsbDevice* >* )&m_lstUsbDevicesBase[0]->m_lstUsbDevice)),
	m_lstPrinters(*((QList<CHwPrinter* >* )&m_lstPrintersBase[0]->m_lstPrinter)),
	m_lstGenericPciDevices(m_lstGenericPciDevicesBase[0]->m_lstGenericPciDevice),
	m_lstGenericScsiDevices(*((QList<CHwGenericDevice* >* )&m_lstGenericScsiDevicesBase[0]->m_lstGenericScsiDevice))
{
//	m_strSchemaVersion = XML_VM_CONFIG_SCHEMA_VERSION;

	m_szErrMsg = "";
	m_iErrLine = m_iErrCol = -1;
	m_uiRcInit = PRL_ERR_SUCCESS;
	m_iParseRc = HostHwInfoParser::RcSuccess;

//	m_pOsVersion = NULL;
//	m_pCpu = NULL;
//	m_pMemorySettings = NULL;

//	QDomDocument source_doc = other->getXml( XML_DOC_OMIT_XML_NODE );
//	if( !IS_OPERATION_SUCCEEDED( this->readXml( &source_doc ) ) )
//		m_uiRcInit = PRL_ERR_PARSE_HOST_HW_INFO;

} // CHostHardwareInfo::CHostHardwareInfo()


// Initializing constructor
CHostHardwareInfo::CHostHardwareInfo(QString source_string)
 : m_lstFloppyDisks(*((QList<CHwGenericDevice* >* )&m_lstFloppyDisksBase[0]->m_lstFloppyDisk)),
	m_lstOpticalDisks(*((QList<CHwGenericDevice* >* )&m_lstCdROMs[0]->m_lstCdROM)),
	m_lstHardDisks(m_lstHardDisksBase[0]->m_lstHardDisk),
	m_lstSerialPorts(*((QList<CHwGenericDevice* >* )&m_lstSerialPortsBase[0]->m_lstSerialPort)),
	m_lstParallelPorts(*((QList<CHwGenericDevice* >* )&m_lstParallelPortsBase[0]->m_lstParallelPort)),
	m_lstNetworkAdapters(m_lstNetworkAdaptersBase[0]->m_lstNetworkAdapter),
	m_lstSoundOutputDevices(*((QList<CHwGenericDevice* >* )&m_lstSoundDevices[0]->m_lstOutputDevices[0]->m_lstOutputDevice)),
	m_lstSoundMixerDevices(*((QList<CHwGenericDevice* >* )&m_lstSoundDevices[0]->m_lstMixerDevices[0]->m_lstMixerDevice)),
	m_lstUsbDevices(*((QList<CHwUsbDevice* >* )&m_lstUsbDevicesBase[0]->m_lstUsbDevice)),
	m_lstPrinters(*((QList<CHwPrinter* >* )&m_lstPrintersBase[0]->m_lstPrinter)),
	m_lstGenericPciDevices(m_lstGenericPciDevicesBase[0]->m_lstGenericPciDevice),
	m_lstGenericScsiDevices(*((QList<CHwGenericDevice* >* )&m_lstGenericScsiDevicesBase[0]->m_lstGenericScsiDevice))
{
//	m_strSchemaVersion = XML_VM_CONFIG_SCHEMA_VERSION;

	m_szErrMsg = "";
	m_iErrLine = m_iErrCol = -1;
	m_uiRcInit = PRL_ERR_SUCCESS;
	m_iParseRc = HostHwInfoParser::RcSuccess;

//	m_pOsVersion = NULL;
//	m_pCpu = NULL;
//	m_pMemorySettings = NULL;

//	if( !IS_OPERATION_SUCCEEDED( this->fromString( source_string ) ) )
//		m_uiRcInit = PRL_ERR_PARSE_HOST_HW_INFO;

	fromString(source_string);
} // CHostHardwareInfo::CHostHardwareInfo()


// Class destructor
CHostHardwareInfo::~CHostHardwareInfo()
{
} // CHostHardwareInfo::~CHostHardwareInfo()

void CHostHardwareInfo::cleanupClassProperties()
{
	m_lstFloppyDisksBase[0]->ClearLists();
	m_lstFloppyDisksBase[0]->InitLists();
	m_lstFloppyDisksBase[0]->setDefaults();

	m_lstCdROMs[0]->ClearLists();
	m_lstCdROMs[0]->InitLists();
	m_lstCdROMs[0]->setDefaults();

	m_lstHardDisksBase[0]->ClearLists();
	m_lstHardDisksBase[0]->InitLists();
	m_lstHardDisksBase[0]->setDefaults();

	m_lstSerialPortsBase[0]->ClearLists();
	m_lstSerialPortsBase[0]->InitLists();
	m_lstSerialPortsBase[0]->setDefaults();

	m_lstParallelPortsBase[0]->ClearLists();
	m_lstParallelPortsBase[0]->InitLists();
	m_lstParallelPortsBase[0]->setDefaults();

	m_lstNetworkAdaptersBase[0]->ClearLists();
	m_lstNetworkAdaptersBase[0]->InitLists();
	m_lstNetworkAdaptersBase[0]->setDefaults();

	m_lstSoundDevices[0]->m_lstOutputDevices[0]->ClearLists();
	m_lstSoundDevices[0]->m_lstOutputDevices[0]->InitLists();
	m_lstSoundDevices[0]->m_lstOutputDevices[0]->setDefaults();
	m_lstSoundDevices[0]->setDefaults();

	m_lstSoundDevices[0]->m_lstMixerDevices[0]->ClearLists();
	m_lstSoundDevices[0]->m_lstMixerDevices[0]->InitLists();
	m_lstSoundDevices[0]->m_lstMixerDevices[0]->setDefaults();
	m_lstSoundDevices[0]->setDefaults();

	m_lstUsbDevicesBase[0]->ClearLists();
	m_lstUsbDevicesBase[0]->InitLists();
	m_lstUsbDevicesBase[0]->setDefaults();

	m_lstPrintersBase[0]->ClearLists();
	m_lstPrintersBase[0]->InitLists();
	m_lstPrintersBase[0]->setDefaults();

	m_lstGenericPciDevicesBase[0]->ClearLists();
	m_lstGenericPciDevicesBase[0]->InitLists();
	m_lstGenericPciDevicesBase[0]->setDefaults();

	m_lstGenericScsiDevicesBase[0]->ClearLists();
	m_lstGenericScsiDevicesBase[0]->InitLists();
	m_lstGenericScsiDevicesBase[0]->setDefaults();

	setDefaults();
}

// Add floppy disk to the list
void CHostHardwareInfo::addFloppyDisk(CHwGenericDevice* device)
{
	m_lstFloppyDisks.append( device );

} // CHostHardwareInfo::addFloppyDisk()


// Add optical disk to the list
void CHostHardwareInfo::addOpticalDisk(CHwGenericDevice* device)
{
	m_lstOpticalDisks.append( device );

} // CHostHardwareInfo::addOpticalDisk()


// Add hard disk to the list
void CHostHardwareInfo::addHardDisk(CHwHardDisk* device)
{
	m_lstHardDisks.append( device );

} // CHostHardwareInfo::addHardDisk()


// Add serial port to the list
void CHostHardwareInfo::addSerialPort(CHwGenericDevice* device)
{
	m_lstSerialPorts.append( device );

} // CHostHardwareInfo::addSerialPort()


// Add parallel port to the list
void CHostHardwareInfo::addParallelPort(CHwGenericDevice* device)
{
	m_lstParallelPorts.append( device );

} // CHostHardwareInfo::addParallelPort()


// Add network adapter to the list
void CHostHardwareInfo::addNetworkAdapter(CHwNetAdapter* device)
{
	m_lstNetworkAdapters.append( device );

} // CHostHardwareInfo::addNetworkAdapter()


// Add sound output device to the list
void CHostHardwareInfo::addSoundOutputDevice(CHwGenericDevice* device)
{
	m_lstSoundOutputDevices.append( device );

} // CHostHardwareInfo::addSoundOutputDevice()


// Add sound mixer device to the list
void CHostHardwareInfo::addSoundMixerDevice(CHwGenericDevice* device)
{
	m_lstSoundMixerDevices.append( device );

} // CHostHardwareInfo::addSoundMixerDevice()


// Add USB controller to the list
void CHostHardwareInfo::addUsbDevice(CHwUsbDevice* device)
{
	m_lstUsbDevices.append( device );

} // CHostHardwareInfo::addUsbDevice()


// Add printer to the list
void CHostHardwareInfo::addPrinter(CHwPrinter* device)
{
	m_lstPrinters.append( device );

} // CHostHardwareInfo::addPrinter()


// Add generic PCI device to the list
void CHostHardwareInfo::addGenericPciDevice(CHwGenericPciDevice* device)
{
	m_lstGenericPciDevices.append( device );
}

// Add generic PCI device to the list
void CHostHardwareInfo::addGenericScsiDevice(CHwGenericDevice* device)
{
	m_lstGenericScsiDevices.append( device );
}
