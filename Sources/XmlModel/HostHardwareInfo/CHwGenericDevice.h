/*
 * CHwGenericDevice.h: Definition of the CHwGenericDevice class.
 * This class implements a generic device detected on Parallels
 * Dispatcher's host
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


#ifndef CHwGenericDevice_H
#define CHwGenericDevice_H

#include <QDomDocument>
#include <QDomElement>
#include <prlcommon/Interfaces/ParallelsDomModel.h>
#include <prlcommon/Interfaces/ParallelsNamespace.h>

#include "XmlModel/ParallelsObjects/CBaseNode.h"


/**
 * @brief This class implements a generic device detected on
 * Parallels Dispatcher's host
 * @author SergeyM
 */
class CHwGenericDevice : public CBaseNode
{
//	Q_OBJECT

public:

	CHwGenericDevice() {}

	// Standard class constructor
	CHwGenericDevice(PRL_DEVICE_TYPE dev_type);

	// Standard class constructor
	CHwGenericDevice(PRL_DEVICE_TYPE dev_type, QString dev_name, QString dev_id = "");

	// Copy constructor
	CHwGenericDevice(CHwGenericDevice *other);

	// Class destructor
	virtual ~CHwGenericDevice();

	// Set device name
	virtual void setDeviceName(QString value = QString());

	// Get device name
	virtual QString getDeviceName() const;

	// Set device id
	virtual void setDeviceId(QString value = QString());

	// Get device id
	virtual QString getDeviceId() const;

	// Get device type
	virtual PRL_DEVICE_TYPE getDeviceType() const;

	virtual QStringList getVmUuids() const;
	virtual void setVmUuids(QStringList value = QStringList());

	virtual PRL_GENERIC_DEVICE_STATE getDeviceState() const;
	virtual void setDeviceState(PRL_GENERIC_DEVICE_STATE value = PGS_CONNECTED_TO_HOST);

	// Get XML representation of the VM security settings
	virtual QDomElement getXml(QDomDocument* parent_doc, bool no_save_option = false) const;

	// Read XML and setup internal model according to it
	virtual int readXml(QDomElement* parent_element,
						QString ext_tag_name = QString(),
						bool unite_with_loaded = false);

protected:
	// Create class element
	QDomElement createClassElement(QDomDocument* parent_doc) const;

	// Append class-specific properties
	virtual void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element) const;

	// Parse class-specific properties
	virtual int parseClassProperties(QDomElement* parent_element);

	// Get parameter node value
	bool getParamNodeValue(QDomElement* parent_element,
							const QString& param_name,
							QString & param_value,
							PVE::ParamFieldDataType value_type = PVE::Text,
							const QString& attribute_name = XML_VM_ATTR_VALUE);

	bool getParamNodeValues(QDomElement* parent_element, QString param_name, QStringList & param_values);

	// Clean up class-specific properties
	virtual void cleanupClassProperties();

protected:
	QStringList			m_lstVmUuid;

private:
	// Device Name
	QString m_strDeviceName;

	// Device id
	QString m_strDeviceId;

	// Device type
	PRL_DEVICE_TYPE m_uiDeviceType;

	PRL_GENERIC_DEVICE_STATE	m_ctDeviceState;
};

#endif
