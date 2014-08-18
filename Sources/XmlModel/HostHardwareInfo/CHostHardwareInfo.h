/*
 * CHostHardwareInfo.h: Definition of the CHostHardwareInfo class.
 * This class implements hardware list detected on the Dispatcher's
 * host.
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


#ifndef __CHostHardwareInfo_H__
#define __CHostHardwareInfo_H__

#include "CHwOsVersion.h"
#include "CHwCpu.h"
#include "CHwMemorySettings.h"
#include "CHwGenericDevice.h"
#include "CHwHardDisk.h"
#include "CHwNetAdapter.h"
#include "CHwUsbDevice.h"

#include "CHostHardwareInfoBase.h"


/**
 * @brief This class implements hardware list detected on the Dispatcher's host.
 * @author SergeyM
 */
class CHostHardwareInfo : public CHostHardwareInfoBase
{
//	Q_OBJECT

public:
	// List of floppy disks
	QList<CHwGenericDevice*>& m_lstFloppyDisks;

	// List of cd/dvd-roms
	QList<CHwGenericDevice*>& m_lstOpticalDisks;

	// List of hard disks
	QList<CHwHardDisk*>& m_lstHardDisks;

	// List of serial ports
	QList<CHwGenericDevice*>& m_lstSerialPorts;

	// List of parallel ports
	QList<CHwGenericDevice*>& m_lstParallelPorts;

	// List of network adapters
	QList<CHwNetAdapter*>& m_lstNetworkAdapters;

	// List of sound devices
	QList<CHwGenericDevice*>& m_lstSoundOutputDevices;

	// List of sound devices
	QList<CHwGenericDevice*>& m_lstSoundMixerDevices;

	// List of USB devices found on host
	QList<CHwUsbDevice*>& m_lstUsbDevices;

	// List of printers controllers
	QList<CHwPrinter*>& m_lstPrinters;

	// List of generic PCI devices
	QList<CHwGenericPciDevice*>& m_lstGenericPciDevices;

	// List of generic SCSI devices
	QList<CHwGenericDevice*>& m_lstGenericScsiDevices;

	// Class initialization RC
	// see PRL_RESULT
	PRL_RESULT m_uiRcInit;

	// Parse error string
	QString m_szErrMsg;

	// Parse error line
	int m_iErrLine;

	// Parse error column
	int m_iErrCol;

	// Parse specific rc
	// see PVE::ConfigurationParser
	int m_iParseRc;

	virtual void cleanupClassProperties();

public:
	// Standard class constructor
	CHostHardwareInfo();

	// Copy constructor
	CHostHardwareInfo(CHostHardwareInfo *other);

	// Initializing constructor
	CHostHardwareInfo(QString source_string);

	// Class destructor
	~CHostHardwareInfo();

	// Add floppy disk to the list
	void addFloppyDisk(CHwGenericDevice* device);

	// Add optical disk to the list
	void addOpticalDisk(CHwGenericDevice* device);

	// Add hard disk to the list
	void addHardDisk(CHwHardDisk* device);

	// Add serial port to the list
	void addSerialPort(CHwGenericDevice* device);

	// Add parallel port to the list
	void addParallelPort(CHwGenericDevice* device);

	// Add network adapter to the list
	void addNetworkAdapter(CHwNetAdapter* device);

	// Add sound output device to the list
	void addSoundOutputDevice(CHwGenericDevice* device);

	// Add sound mixer device to the list
	void addSoundMixerDevice(CHwGenericDevice* device);

	// Add USB controller to the list
	void addUsbDevice(CHwUsbDevice* device);

	// Add printer to the list
	void addPrinter(CHwPrinter* device);

	// Add generic PCI device to the list
	void addGenericPciDevice(CHwGenericPciDevice* device);

	// Add generic SCSI device to the list
	void addGenericScsiDevice(CHwGenericDevice* device);

};

#endif // __CHostHardwareInfo_H__
