/*
 * CVmDevice.h: Definition of the CVmDevice class. This class
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


#ifndef CVMDEVICE_H
#define CVMDEVICE_H


#include "XmlModel/ParallelsObjects/CBaseNode.h"


/**
 * @brief This class implements the basic VM device. All the devices, which can
 * be included into the VM hardware list, inherit from this class.
 * @author SergeyM
 */
class CVmDevice : public CBaseNodeSignals
{
	Q_OBJECT
public:
	// Standard class constructor
	CVmDevice();

	CVmDevice( const CVmDevice& rhs );

	// Class destructor
	virtual ~CVmDevice();

	CVmDevice& operator=( const CVmDevice& rhs );

	virtual bool operator==( const CVmDevice& cVmDevice );

	QString getObjectId() const;

	// Set device type
	void setDeviceType(PRL_DEVICE_TYPE value);
	// Get device type
	virtual PRL_DEVICE_TYPE getDeviceType() const;

	// Set Enabled flag value
	void setEnabled(unsigned int value = PVE::DeviceDisabled);
	// Get Enabled flag value
	PVE::DeviceEnabledState getEnabled() const;
	virtual QString getEnabled_id() const { return ""; }

	// Set Connected flag value
	void setConnected(unsigned int value = PVE::DeviceConnected);
	// Get Connected flag value
	PVE::DeviceConnectedState getConnected() const;

	// Set EmulatedType value
	void setEmulatedType(unsigned int value);
	// Get EmulatedType value
	unsigned int getEmulatedType() const;
	virtual QString getEmulatedType_id() const { return ""; }

	// Set device index
	void setIndex(unsigned int value = 0);
	// Note: devices are identified by pair [DeviceType;DeviceIndex]
	// In principle, VMDevice doesn't have index, but for consistency
	// this method is defined and always returns 0. Actual indexes
	// are defined in the lower level of hierarchy
	unsigned int getIndex() const;
	virtual QString getIndex_id() const { return ""; }

	void setSystemName(QString value = QString());
	QString getSystemName() const;
	virtual QString getSystemName_id() const { return ""; }

	void setUserFriendlyName(QString value = QString());
	QString getUserFriendlyName() const;

	void setRemote(bool value = false);
	bool isRemote() const;
	virtual QString getRemote_id() const { return ""; }

	void setDescription(QString value = QString());
	QString getDescription() const;

	void setRelativeSystemName( const QString & strVmDirectory, bool bShadowVm = false );
	QString RevertToInitialSystemName(const QString & strVmDirectory );

	static CVmDevice * getDeviceInstanceFromString( const QString & strDevice );

	virtual int readXml(QDomElement*,
						QString ext_tag_name = QString(),
						bool unite_with_loaded = false);

protected:

	void Init();

	PRL_DEVICE_TYPE				m_uiDeviceType;
	unsigned int				m_uiEmulatedType;
	unsigned int				m_uiIndex;
	unsigned int				m_uiEnabled;
	unsigned int				m_uiConnected;
	QString						m_qsSystemName;
	QString						m_qsUserFriendlyName;
	bool						m_bRemote;
	QString						m_qsDeviceDescription;

signals:

	void signalDeviceTypeChanged(PRL_DEVICE_TYPE value, QString object_id = QString());
	void signalEnabledChanged(unsigned int value, QString object_id = QString());
	void signalEnabledChanged(bool value, QString object_id = QString());
	void signalConnectedChanged(unsigned int value, QString object_id = QString());
	void signalEmulatedTypeChanged(unsigned int value, QString object_id = QString());
	void signalIndexChanged(unsigned int value, QString object_id = QString());
	void signalSystemNameChanged(QString value, QString object_id = QString());
	void signalUserFriendlyNameChanged(QString value, QString object_id = QString());
	void signalRemoteChanged(bool value, QString object_id = QString());
	void signalDescriptionChanged(QString value, QString object_id = QString());

private slots:

	void slotChangeDeviceType(PRL_DEVICE_TYPE value);
	void slotChangeEnabled(unsigned int value);
	void slotChangeEnabled(bool value);
	void slotChangeConnected(unsigned int value);
	void slotChangeEmulatedType(unsigned int value);
	void slotChangeIndex(unsigned int value);
	void slotChangeSystemName(QString value);
	void slotChangeUserFriendlyName(QString value);
	void slotChangeRemote(bool value);
	void slotChangeDescription(QString value);

};

#ifndef DM_795237
#define DM_795237
Q_DECLARE_METATYPE(PVE::DeviceEnabledState)
#endif
#ifndef DM_21711685
#define DM_21711685
Q_DECLARE_METATYPE(PVE::DeviceConnectedState)
#endif

Q_DECLARE_METATYPE(PVE::FloppyEmulatedType)
Q_DECLARE_METATYPE(PVE::CdromEmulatedType)
Q_DECLARE_METATYPE(PVE::HardDiskEmulatedType)
Q_DECLARE_METATYPE(PVE::GenericDeviceEmulatedType)
Q_DECLARE_METATYPE(PVE::ParallelPortEmulatedType)
Q_DECLARE_METATYPE(PVE::SerialPortEmulatedType)
Q_DECLARE_METATYPE(PVE::SoundDeviceEmulatedType)
Q_DECLARE_METATYPE(PVE::UsbDeviceEmulatedType)
#ifndef DM_79465381
#define DM_79465381
Q_DECLARE_METATYPE(PRL_NET_ADAPTER_EMULATED_TYPE)
#endif

#endif
