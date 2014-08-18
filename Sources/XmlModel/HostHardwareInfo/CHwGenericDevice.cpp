/*
 * CHwGenericDevice.cpp: Implementation of the CHwGenericDevice
 * class. This class implements a generic device detected on
 * Parallels Dispatcher's host.
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


#include "CHwGenericDevice.h"

/**
 * @brief Standard class constructor.
 * @param parent
 * @return
 */
CHwGenericDevice::CHwGenericDevice(PRL_DEVICE_TYPE dev_type)
{
	cleanupClassProperties();

	m_uiDeviceType = dev_type;

} // CHwGenericDevice::CHwGenericDevice()


/**
 * @brief Standard class constructor.
 * @param parent
 * @return
 */
CHwGenericDevice::CHwGenericDevice(PRL_DEVICE_TYPE dev_type, QString dev_name, QString dev_id)
{
	cleanupClassProperties();

	m_uiDeviceType = dev_type;
	m_strDeviceId  = dev_id;
	m_strDeviceName = dev_name;

} // CHwGenericDevice::CHwGenericDevice()


/**
 * @brief Copy constructor.
 * @param other
 * @param parent
 * @return
 */
CHwGenericDevice::CHwGenericDevice(CHwGenericDevice *other)
{
	QDomDocument xmlDummyParentDoc;
	QDomElement xmlDummyParentElement = xmlDummyParentDoc.createElement( XML_HOST_INFO_EL_ROOT );
	xmlDummyParentDoc.appendChild( xmlDummyParentElement );

	QDomElement source_element = other->getXml( &xmlDummyParentDoc );
	readXml( &source_element );

} // CHwGenericDevice::CHwGenericDevice()


/**
 * @brief Class destructor.
 * @return
 */
CHwGenericDevice::~CHwGenericDevice()
{

} // CHwGenericDevice::~CHwGenericDevice()


/**
 * @brief Set device name.
 * @param value
 * @return
 */
void CHwGenericDevice::setDeviceName(QString value)
{
	m_strDeviceName = value;

} // CHwGenericDevice::setDeviceName()


/**
 * @brief Get device name.
 */
QString CHwGenericDevice::getDeviceName() const
{
	return m_strDeviceName;

} // CHwGenericDevice::getDeviceName()


// Set device id
void CHwGenericDevice::setDeviceId(QString value)
{
	m_strDeviceId = value;

} // CHwGenericDevice::setDeviceId()


// Get device id
QString CHwGenericDevice::getDeviceId() const
{
	return m_strDeviceId;

} // CHwGenericDevice::getDeviceId()


// Get device type
PRL_DEVICE_TYPE CHwGenericDevice::getDeviceType() const
{
	return m_uiDeviceType;

} // CHwGenericDevice::getDeviceType()

QStringList CHwGenericDevice::getVmUuids() const
{
	return m_lstVmUuid;
}

void CHwGenericDevice::setVmUuids(QStringList value)
{
	m_lstVmUuid = value;
}

PRL_GENERIC_DEVICE_STATE CHwGenericDevice::getDeviceState() const
{
	return m_ctDeviceState;
}

void CHwGenericDevice::setDeviceState(PRL_GENERIC_DEVICE_STATE value)
{
	m_ctDeviceState = value;
}

/**
 * @brief Get XML representation of the VM security settings.
 * @param parent_doc
 * @return
 */
QDomElement CHwGenericDevice::getXml(QDomDocument* parent_doc, bool no_save_option) const
{
	Q_UNUSED(no_save_option);

	// create basic class element
	QDomElement xmlClassElement = createClassElement( parent_doc );

	// append class-specific properties
	appendClassProperties( parent_doc, &xmlClassElement );

	return xmlClassElement;

} // CHwGenericDevice::getXml()


/**
 * @brief Read XML and setup internal model according to it.
 * @param parent_element
 * @return
 */
int CHwGenericDevice::readXml(QDomElement* parent_element, QString /*ext_tag_name*/, bool /*unite_with_loaded*/)
{
	int i_rc;

	if( !parent_element )
		return HostHwInfoParser::BadDeviceData;

	// cleanup class properties
	cleanupClassProperties();

	// parse class-specific properties
	i_rc = parseClassProperties( parent_element );
	if( !IS_OPERATION_SUCCEEDED( i_rc ) )
		return i_rc;

	return HostHwInfoParser::RcSuccess;

} // CHwGenericDevice::readXml()


/**
 * @brief Create class element.
 * @param parent_doc
 * @return
 */
QDomElement CHwGenericDevice::createClassElement(QDomDocument* parent_doc) const
{
	QString dom_element_name;
	switch( getDeviceType() )
	{
	case PDE_FLOPPY_DISK:
		dom_element_name = XML_HOST_INFO_EL_FLOPPY_DISK;
		break;
	case PDE_OPTICAL_DISK:
		dom_element_name = XML_HOST_INFO_EL_CDROM;
		break;
	case PDE_HARD_DISK:
		dom_element_name = XML_HOST_INFO_EL_HARDDISK;
		break;
	case PDE_GENERIC_NETWORK_ADAPTER:
		dom_element_name = XML_HOST_INFO_EL_NETADAPTER;
		break;
	case PDE_SERIAL_PORT:
		dom_element_name = XML_HOST_INFO_EL_SERIAL_PORT;
		break;
	case PDE_PARALLEL_PORT:
		dom_element_name = XML_HOST_INFO_EL_PARALLEL_PORT;
		break;
	case PDE_SOUND_DEVICE:
		dom_element_name = XML_HOST_INFO_EL_SOUND_OUTPUT;
		break;
	case PDE_MIXER_DEVICE:
		dom_element_name = XML_HOST_INFO_EL_SOUND_MIXER;
		break;
	case PDE_USB_DEVICE:
		dom_element_name = XML_HOST_INFO_EL_USBDEVICE;
		break;
	case PDE_PRINTER:
		dom_element_name = XML_HOST_INFO_EL_PRINTER;
		break;
	case PDE_GENERIC_PCI_DEVICE:
		dom_element_name = XML_HOST_INFO_EL_GENERIC_PCI_DEVICE;
		break;
	case PDE_GENERIC_SCSI_DEVICE:
		dom_element_name = XML_HOST_INFO_EL_GENERIC_SCSI_DEVICE;
		break;
	default:
		dom_element_name = XML_HOST_INFO_EL_GENERIC_DEVICE;
	}

	QDomElement xmlClassElement =
		parent_doc->createElement( dom_element_name );

	return xmlClassElement;

} // CHwGenericDevice::createClassElement()


/**
 * @brief Append class-specific properties.
 * @param parent_doc
 * @param parent_element
 * @return
 */
void CHwGenericDevice::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element) const
{
	// append class-specific attributes

	// append class-specific nodes

	// device name node
	QDomElement xmlNode_Name = parent_doc->createElement( XML_VM_ND_NAME );
	//FIXME: temporary solution to prevent non correct symbols presence at XML document
	QDomText xmlNode_NameValue = parent_doc->createTextNode( UTF8_2QSTR(m_strDeviceName.toUtf8().constData()) );
	xmlNode_Name.appendChild( xmlNode_NameValue );
	parent_element->appendChild( xmlNode_Name );

	// device id node
	QDomElement xmlNode_Uuid = parent_doc->createElement( XML_VM_ND_UUID );
	//FIXME: temporary solution to prevent non correct symbols presence at XML document
	QDomText xmlNode_UuidValue = parent_doc->createTextNode( UTF8_2QSTR(m_strDeviceId.toUtf8().constData()) );
	xmlNode_Uuid.appendChild( xmlNode_UuidValue );
	parent_element->appendChild( xmlNode_Uuid );

	if (getDeviceType() == PDE_USB_DEVICE)
	{
		foreach( QString strUuid, m_lstVmUuid )
		{
			QDomElement xmlNode_VmUuid = parent_doc->createElement( "VmUuid" );
			QDomText xmlNode_VmUuidValue = parent_doc->createTextNode( strUuid );
			xmlNode_VmUuid.appendChild( xmlNode_VmUuidValue );
			parent_element->appendChild( xmlNode_VmUuid );
		}
	}

	if (getDeviceType() == PDE_USB_DEVICE
		|| getDeviceType() == PDE_GENERIC_PCI_DEVICE)
	{
		QDomElement xmlNode_DeviceState = parent_doc->createElement( "DeviceState" );
		QDomText xmlNode_DeviceStateValue = parent_doc->createTextNode( QString::number((unsigned int )m_ctDeviceState) );
		xmlNode_DeviceState.appendChild( xmlNode_DeviceStateValue );
		parent_element->appendChild( xmlNode_DeviceState );
	}

} // CHwGenericDevice::appendClassProperties()


/**
 * @brief Parse class-specific properties.
 * @param parent_element
 * @return
 */
int CHwGenericDevice::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	QString parent_tag_name = parent_element->tagName();
	if (parent_tag_name == XML_HOST_INFO_EL_FLOPPY_DISK)
	{
		m_uiDeviceType = PDE_FLOPPY_DISK;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_CDROM)
	{
		m_uiDeviceType = PDE_OPTICAL_DISK;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_HARDDISK)
	{
		m_uiDeviceType = PDE_HARD_DISK;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_NETADAPTER)
	{
		m_uiDeviceType = PDE_GENERIC_NETWORK_ADAPTER;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_SERIAL_PORT)
	{
		m_uiDeviceType = PDE_SERIAL_PORT;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_PARALLEL_PORT)
	{
		m_uiDeviceType = PDE_PARALLEL_PORT;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_SOUND_OUTPUT)
	{
		m_uiDeviceType = PDE_SOUND_DEVICE;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_SOUND_MIXER)
	{
		m_uiDeviceType = PDE_MIXER_DEVICE;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_USBDEVICE)
	{
		m_uiDeviceType = PDE_USB_DEVICE;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_PRINTER)
	{
		m_uiDeviceType = PDE_PRINTER;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_GENERIC_PCI_DEVICE)
	{
		m_uiDeviceType = PDE_GENERIC_PCI_DEVICE;
	}
	else if (parent_tag_name == XML_HOST_INFO_EL_GENERIC_SCSI_DEVICE)
	{
		m_uiDeviceType = PDE_GENERIC_SCSI_DEVICE;
	}

	// device name attribute
	if( !getParamNodeValue( parent_element, XML_VM_ND_NAME , strParamValue ) )
		return HostHwInfoParser::BadDeviceName;
	m_strDeviceName = strParamValue;

	// device id attribute
	if( !getParamNodeValue( parent_element, XML_VM_ND_UUID , strParamValue ) )
		return HostHwInfoParser::BadDeviceId;
	m_strDeviceId = strParamValue;

	getParamNodeValues( parent_element, "VmUuid" , m_lstVmUuid );

	if( getParamNodeValue( parent_element, "DeviceState" , strParamValue ) )
	{
		m_ctDeviceState = (PRL_GENERIC_DEVICE_STATE )strParamValue.toUInt();
	}

	return HostHwInfoParser::RcSuccess;

} // CHwGenericDevice::parseClassProperties()


/**
 * @brief Get parameter node value.
 * @param parent_element
 * @param param_name
 * @param param_value
 * @return
 */
bool CHwGenericDevice::getParamNodeValue(
	QDomElement* parent_element, const QString& param_name, QString & param_value,
	PVE::ParamFieldDataType, const QString&)
{
	QDomNodeList xmlNodeList = parent_element->elementsByTagName( param_name );
	if( xmlNodeList.isEmpty() )
		return false;

	QDomElement node = xmlNodeList.item( 0 ).toElement();
	if( node.isNull() )
		return false;

	param_value = node.text();

	return true;

} // CHwGenericDevice::getParamNodeValue()

/**
* @brief Get parameter node values.
* @param parent_element
* @param param_name
* @param param_values
* @return
*/
bool CHwGenericDevice::getParamNodeValues(
	QDomElement* parent_element, QString param_name, QStringList & param_values)
{
	QDomNodeList xmlNodeList = parent_element->elementsByTagName( param_name );
	if( xmlNodeList.isEmpty() )
		return false;

	for( int i = 0 ; i < xmlNodeList.size() ; i++ )
	{
		QDomElement node = xmlNodeList.item( i ).toElement();
		if( node.isNull() )
			continue;

		param_values += node.text();
	}

	return !param_values.isEmpty();

} // CHwGenericDevice::getParamNodeValue()

/**
 * @brief Clean up class-specific properties.
 * @return
 */
void CHwGenericDevice::cleanupClassProperties()
{
	//m_uiDeviceType = PVE::GenericDevice;
	m_strDeviceName = "";
	m_strDeviceId = "";
	m_lstVmUuid.clear();
	m_ctDeviceState = PGS_CONNECTED_TO_HOST;

} // CHwGenericDevice::cleanupClassProperties()
