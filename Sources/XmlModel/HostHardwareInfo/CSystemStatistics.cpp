/*
 * CSystemStatistics.cpp: Implementation of the system statistics
 * classes set. This classes set implements system resources usage
 * statistics container.
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
#include "CSystemStatistics.h"

#include <prlcommon/PrlDataSerializer/CPrlOpaqueTypeDataSerializer.h>
#include <prlcommon/PrlDataSerializer/CPrlStringDataSerializer.h>

// By adding this interface we enable allocations tracing in the module
#include "Interfaces/Debug.h"


namespace {
/**
 * Auxiliary method that extracts parameter value from specified element
 * @param pointer to processing XML element
 * @param parameter name
 * @param out variable for storing parameter value
 * @return sign whether
 */
bool getParamNodeValue(QDomElement* parent_element, const QString &param_name, QString &param_value)
{
	QDomElement node = parent_element->firstChildElement(param_name);
	if( node.isNull() )
		return false;

	param_value = node.text();
	return true;
}

}

/************************************Base statisitcs element implementation*********************/

QDomElement CBaseStatisticsElement::getXml( QDomDocument* parent_doc )
{
	QDomElement xmlClassElement = createClassElement( parent_doc );
	appendClassProperties( parent_doc, &xmlClassElement );
	return xmlClassElement;
}

int CBaseStatisticsElement::readXml( QDomElement* parent_element )
{
	int i_rc;

	if( !parent_element )
		return SystemStatisticsParser::BadStatisticsElementData;

	cleanupClassProperties();

	i_rc = parseClassProperties( parent_element );
	if( !IS_OPERATION_SUCCEEDED( i_rc ) )
		return i_rc;

	return SystemStatisticsParser::RcSuccess;
}

QDomElement CBaseStatisticsElement::createClassElement(QDomDocument* parent_doc)
{
	QDomElement xmlClassElement = parent_doc->createElement( getConcreteClassElementName() );
	return xmlClassElement;
}

/************************************CPU statisitcs element implementation*********************/

CCpuStatistics::CCpuStatistics(QDomElement *parent_element)
{
	readXml(parent_element);
}

bool CCpuStatistics::operator==(const CCpuStatistics &_obj)
{
	return (m_uiPercentsUsage == _obj.m_uiPercentsUsage &&
					m_uiTotalTime == _obj.m_uiTotalTime &&
					m_uiUserTime == _obj.m_uiUserTime &&
					m_uiSystemTime == _obj.m_uiSystemTime);
}

void CCpuStatistics::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlNode_PercentsUsage = parent_doc->createElement( XML_STATISTICS_CPU_PERCENTS_USAGE );
	QDomText xmlNode_PercentsUsageValue = parent_doc->createTextNode( QString::number(m_uiPercentsUsage) );
	xmlNode_PercentsUsage.appendChild( xmlNode_PercentsUsageValue );
	parent_element->appendChild( xmlNode_PercentsUsage );

	QDomElement xmlNode_TotalTime = parent_doc->createElement( XML_STATISTICS_CPU_TOTAL_TIME );
	QDomText xmlNode_TotalTimeValue = parent_doc->createTextNode( QString::number(m_uiTotalTime) );
	xmlNode_TotalTime.appendChild( xmlNode_TotalTimeValue );
	parent_element->appendChild( xmlNode_TotalTime );

	QDomElement xmlNode_SystemTime = parent_doc->createElement( XML_STATISTICS_CPU_SYSTEM_TIME );
	QDomText xmlNode_SystemTimeValue = parent_doc->createTextNode( QString::number(m_uiSystemTime) );
	xmlNode_SystemTime.appendChild( xmlNode_SystemTimeValue );
	parent_element->appendChild( xmlNode_SystemTime );

	QDomElement xmlNode_UserTime = parent_doc->createElement( XML_STATISTICS_CPU_USER_TIME );
	QDomText xmlNode_UserTimeValue = parent_doc->createTextNode( QString::number(m_uiUserTime) );
	xmlNode_UserTime.appendChild( xmlNode_UserTimeValue );
	parent_element->appendChild( xmlNode_UserTime );
}

int CCpuStatistics::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_CPU_PERCENTS_USAGE, strParamValue ) )
		return SystemStatisticsParser::BadCpuPercentsUsage;
	m_uiPercentsUsage = strParamValue.toUInt();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_CPU_TOTAL_TIME, strParamValue ) )
		return SystemStatisticsParser::BadCpuTotalTime;
	m_uiTotalTime = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_CPU_USER_TIME, strParamValue ) )
		return SystemStatisticsParser::BadCpuUserTime;
	m_uiUserTime = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_CPU_SYSTEM_TIME, strParamValue ) )
		return SystemStatisticsParser::BadCpuSystemTime;
	m_uiSystemTime = strParamValue.toULongLong();

	return SystemStatisticsParser::RcSuccess;
}

void CCpuStatistics::cleanupClassProperties()
{
	m_uiPercentsUsage = m_uiTotalTime = m_uiUserTime = m_uiSystemTime = 0;
}

QString CCpuStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_CPU);
}

void CCpuStatistics::Serialize(QDataStream &_stream)
{
	PrlOpaqueSerializer(m_uiPercentsUsage).Serialize(_stream);
	PrlOpaqueSerializer(m_uiTotalTime).Serialize(_stream);
	PrlOpaqueSerializer(m_uiUserTime).Serialize(_stream);
	PrlOpaqueSerializer(m_uiSystemTime).Serialize(_stream);
}

void CCpuStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	PrlOpaqueSerializer(m_uiPercentsUsage).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiTotalTime).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiUserTime).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiSystemTime).Deserialize(_stream);
}

/************************************Disk statisitcs element implementation*********************/

CDiskStatistics::CDiskStatistics(QDomElement *parent_element)
{
	readXml(parent_element);
}

CDiskStatistics::~CDiskStatistics()
{
	cleanupClassProperties();
}

bool CDiskStatistics::operator==(const CDiskStatistics &_obj)
{
	if (m_sDeviceSystemName != _obj.m_sDeviceSystemName ||
					m_uiUsageDiskSpace != _obj.m_uiUsageDiskSpace ||
					m_uiFreeDiskSpace != _obj.m_uiFreeDiskSpace)
		return (false);

	if (m_lstPartitionsStatistics.size() != _obj.m_lstPartitionsStatistics.size())
		return (false);

	for(int i = 0; i < m_lstPartitionsStatistics.size(); ++i)
	{
		CDiskPartStatistics *pPartition1 = m_lstPartitionsStatistics.value(i);
		CDiskPartStatistics *pPartition2 = _obj.m_lstPartitionsStatistics.value(i);
		if (*pPartition1 != *pPartition2)
			return (false);
	}

	return (true);
}

void CDiskStatistics::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlNode_DeviceSystemName = parent_doc->createElement( XML_STATISTICS_DISK_SYSTEM_NAME );
	QDomText xmlNode_DeviceSystemNameValue = parent_doc->createTextNode( m_sDeviceSystemName );
	xmlNode_DeviceSystemName.appendChild( xmlNode_DeviceSystemNameValue );
	parent_element->appendChild( xmlNode_DeviceSystemName );

	QDomElement xmlNode_UsageDiskSpace = parent_doc->createElement( XML_STATISTICS_DISK_USAGE_DISK_SPACE );
	QDomText xmlNode_UsageDiskSpaceValue = parent_doc->createTextNode( QString::number(m_uiUsageDiskSpace) );
	xmlNode_UsageDiskSpace.appendChild( xmlNode_UsageDiskSpaceValue );
	parent_element->appendChild( xmlNode_UsageDiskSpace );

	QDomElement xmlNode_FreeDiskSpace = parent_doc->createElement( XML_STATISTICS_DISK_FREE_DISK_SPACE );
	QDomText xmlNode_FreeDiskSpaceValue = parent_doc->createTextNode( QString::number(m_uiFreeDiskSpace) );
	xmlNode_FreeDiskSpace.appendChild( xmlNode_FreeDiskSpaceValue );
	parent_element->appendChild( xmlNode_FreeDiskSpace );

	QDomElement xmlStatGroup = parent_doc->createElement( XML_STATISTICS_EL_DISK_PARTITIONS_GROUP );
	parent_element->appendChild( xmlStatGroup );

	foreach(CDiskPartStatistics *pDiskPartStatistics, m_lstPartitionsStatistics)
		xmlStatGroup.appendChild( pDiskPartStatistics->getXml( parent_doc ) );
}

int CDiskStatistics::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_DISK_SYSTEM_NAME, strParamValue ) )
		return SystemStatisticsParser::BadDiskSystemName;
	m_sDeviceSystemName = strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_DISK_USAGE_DISK_SPACE, strParamValue ) )
		return SystemStatisticsParser::BadDiskUsageDiskSpace;
	m_uiUsageDiskSpace = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_DISK_FREE_DISK_SPACE, strParamValue ) )
		return SystemStatisticsParser::BadDiskFreeDiskSpace;
	m_uiFreeDiskSpace = strParamValue.toULongLong();

	QDomElement xmlPartitionsStatGroupElement = parent_element->firstChildElement( XML_STATISTICS_EL_DISK_PARTITIONS_GROUP );
	if (xmlPartitionsStatGroupElement.isNull())
		return (SystemStatisticsParser::AbsentPartitionsStatGroupElement);

	QDomElement xmlDiskPartStatElement = xmlPartitionsStatGroupElement.firstChildElement( XML_STATISTICS_EL_DISK_PARTITION );
	for (; !xmlDiskPartStatElement.isNull();
				xmlDiskPartStatElement = xmlDiskPartStatElement.nextSiblingElement( XML_STATISTICS_EL_DISK_PARTITION ))
		m_lstPartitionsStatistics.append(new CDiskPartStatistics(&xmlDiskPartStatElement));

	return SystemStatisticsParser::RcSuccess;
}

void CDiskStatistics::cleanupClassProperties()
{
	m_sDeviceSystemName = "";
	m_uiUsageDiskSpace = m_uiFreeDiskSpace = 0;

	while( !m_lstPartitionsStatistics.isEmpty() )
	{
		CDiskPartStatistics *pDiskPartStatItem = m_lstPartitionsStatistics.takeFirst();
		m_lstPartitionsStatistics.removeAll( pDiskPartStatItem );
 		delete pDiskPartStatItem;
	}
}

QString CDiskStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_DISK);
}

void CDiskStatistics::Serialize(QDataStream &_stream)
{
	CPrlStringDataSerializer(m_sDeviceSystemName).Serialize(_stream);
	PrlOpaqueSerializer(m_uiUsageDiskSpace).Serialize(_stream);
	PrlOpaqueSerializer(m_uiFreeDiskSpace).Serialize(_stream);
	int nPartitionsListSize = m_lstPartitionsStatistics.size();
	PrlOpaqueSerializer(nPartitionsListSize).Serialize(_stream);
	foreach(CDiskPartStatistics *pPartition, m_lstPartitionsStatistics)
		pPartition->Serialize(_stream);
}

void CDiskStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	CPrlStringDataSerializer(m_sDeviceSystemName).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiUsageDiskSpace).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiFreeDiskSpace).Deserialize(_stream);
	int nPartitionsListSize = 0;
	PrlOpaqueSerializer(nPartitionsListSize).Deserialize(_stream);
	for(int i = 0; i < nPartitionsListSize; ++i)
	{
		CDiskPartStatistics *pPartition = new CDiskPartStatistics;
		pPartition->Deserialize(_stream);
		m_lstPartitionsStatistics.append(pPartition);
	}
}

/************************************Disk partition statisitcs element implementation*********************/

CDiskPartStatistics::CDiskPartStatistics(QDomElement *parent_element)
: CDiskStatistics(parent_element)
{}

bool CDiskPartStatistics::operator==(const CDiskPartStatistics &_obj)
{
	return CDiskStatistics::operator==(_obj);
}

bool CDiskPartStatistics::operator!=(const CDiskPartStatistics &_obj)
{
	return !operator==(_obj);
}

QString CDiskPartStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_DISK_PARTITION);
}

/************************************Memory base statisitcs element implementation*********************/

CBaseMemStatistics::CBaseMemStatistics(QDomElement *parent_element)
{
	readXml(parent_element);
}

bool CBaseMemStatistics::operator==(const CBaseMemStatistics &_obj)
{
	return (m_uiTotalSize == _obj.m_uiTotalSize &&
			m_uiUsageSize == _obj.m_uiUsageSize &&
			m_uiFreeSize == _obj.m_uiFreeSize &&
			m_uiRealSize == _obj.m_uiRealSize);
}

void CBaseMemStatistics::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlNode_TotalSize = parent_doc->createElement( XML_STATISTICS_MEM_TOTAL_SIZE );
	QDomText xmlNode_TotalSizeValue = parent_doc->createTextNode( QString::number(m_uiTotalSize) );
	xmlNode_TotalSize.appendChild( xmlNode_TotalSizeValue );
	parent_element->appendChild( xmlNode_TotalSize );

	QDomElement xmlNode_UsageSize = parent_doc->createElement( XML_STATISTICS_MEM_USAGE_SIZE );
	QDomText xmlNode_UsageSizeValue = parent_doc->createTextNode( QString::number(m_uiUsageSize) );
	xmlNode_UsageSize.appendChild( xmlNode_UsageSizeValue );
	parent_element->appendChild( xmlNode_UsageSize );

	QDomElement xmlNode_FreeSize = parent_doc->createElement( XML_STATISTICS_MEM_FREE_SIZE );
	QDomText xmlNode_FreeSizeValue = parent_doc->createTextNode( QString::number(m_uiFreeSize) );
	xmlNode_FreeSize.appendChild( xmlNode_FreeSizeValue );
	parent_element->appendChild( xmlNode_FreeSize );

	QDomElement xmlNode_RealSize = parent_doc->createElement( XML_STATISTICS_MEM_REAL_SIZE );
	QDomText xmlNode_RealSizeValue = parent_doc->createTextNode( QString::number(m_uiRealSize) );
	xmlNode_RealSize.appendChild( xmlNode_RealSizeValue );
	parent_element->appendChild( xmlNode_RealSize );
}

int CBaseMemStatistics::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_MEM_TOTAL_SIZE, strParamValue ) )
		return SystemStatisticsParser::BadMemTotalSize;
	m_uiTotalSize = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_MEM_USAGE_SIZE, strParamValue ) )
		return SystemStatisticsParser::BadMemUsageSize;
	m_uiUsageSize = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_MEM_FREE_SIZE, strParamValue ) )
		return SystemStatisticsParser::BadMemFreeSize;
	m_uiFreeSize = strParamValue.toULongLong();

	if( getParamNodeValue( parent_element, XML_STATISTICS_MEM_REAL_SIZE, strParamValue ) )
		m_uiRealSize = strParamValue.toULongLong();

	return SystemStatisticsParser::RcSuccess;
}

void CBaseMemStatistics::cleanupClassProperties()
{
	m_uiTotalSize = m_uiUsageSize = m_uiFreeSize = m_uiRealSize = 0;
}

void CBaseMemStatistics::Serialize(QDataStream &_stream)
{
	PrlOpaqueSerializer(m_uiTotalSize).Serialize(_stream);
	PrlOpaqueSerializer(m_uiUsageSize).Serialize(_stream);
	PrlOpaqueSerializer(m_uiFreeSize).Serialize(_stream);
	PrlOpaqueSerializer(m_uiRealSize).Serialize(_stream);
}

void CBaseMemStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	PrlOpaqueSerializer(m_uiTotalSize).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiUsageSize).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiFreeSize).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiRealSize).Deserialize(_stream);
}

/************************************RAM statisitcs element implementation*********************/

CMemoryStatistics::CMemoryStatistics(QDomElement *parent_element)
: CBaseMemStatistics(parent_element)
{}

QString CMemoryStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_MEMORY);
}

/************************************Swap statisitcs element implementation*********************/

CSwapStatistics::CSwapStatistics(QDomElement *parent_element)
: CBaseMemStatistics(parent_element)
{}

QString CSwapStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_SWAP);
}

/************************************Network Classfull Statistics  implementation*****************/
CNetClassStatistics::CNetClassStatistics()
{
	memset(&m_Stat, 0, sizeof(m_Stat));
}

/************************************Uptime statisitcs element implementation*********************/

CUptimeStatistics::CUptimeStatistics(QDomElement *parent_element)
{
	readXml(parent_element);
}

bool CUptimeStatistics::operator==(const CUptimeStatistics &_obj)
{
	return ( m_uiOsUptime == _obj.m_uiOsUptime &&
					m_uiDispatcherUptime == _obj.m_uiDispatcherUptime);
}

void CUptimeStatistics::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlNode_OsUptime = parent_doc->createElement( XML_STATISTICS_UPTIME_OS_UPTIME );
	QDomText xmlNode_OsUptimeValue = parent_doc->createTextNode( QString::number(m_uiOsUptime) );
	xmlNode_OsUptime.appendChild( xmlNode_OsUptimeValue );
	parent_element->appendChild( xmlNode_OsUptime );

	QDomElement xmlNode_DispatcherUptime = parent_doc->createElement( XML_STATISTICS_UPTIME_DISPATCHER_UPTIME );
	QDomText xmlNode_DispatcherUptimeValue = parent_doc->createTextNode( QString::number(m_uiDispatcherUptime) );
	xmlNode_DispatcherUptime.appendChild( xmlNode_DispatcherUptimeValue );
	parent_element->appendChild( xmlNode_DispatcherUptime );
}

int CUptimeStatistics::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_UPTIME_OS_UPTIME, strParamValue ) )
		return SystemStatisticsParser::BadUptimeOsUptime;
	m_uiOsUptime = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_UPTIME_DISPATCHER_UPTIME, strParamValue ) )
		return SystemStatisticsParser::BadUptimeDispatcherUptime;
	m_uiDispatcherUptime = strParamValue.toULongLong();

	return SystemStatisticsParser::RcSuccess;
}

void CUptimeStatistics::cleanupClassProperties()
{
	m_uiOsUptime = m_uiDispatcherUptime = 0;
}

QString CUptimeStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_UPTIME);
}

void CUptimeStatistics::Serialize(QDataStream &_stream)
{
	PrlOpaqueSerializer(m_uiOsUptime).Serialize(_stream);
	PrlOpaqueSerializer(m_uiDispatcherUptime).Serialize(_stream);
}

void CUptimeStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	PrlOpaqueSerializer(m_uiOsUptime).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiDispatcherUptime).Deserialize(_stream);
}

/************************************Process statisitcs element implementation*********************/

CProcInfoStatistics::CProcInfoStatistics(QDomElement *parent_element)
{
	readXml(parent_element);
}

bool CProcInfoStatistics::operator==(const CProcInfoStatistics &_obj)
{
	return (m_sCommandName == _obj.m_sCommandName &&
					m_uiProcId == _obj.m_uiProcId &&
					m_sOwnerUser == _obj.m_sOwnerUser &&
					m_uiTotalMemUsage == _obj.m_uiTotalMemUsage &&
					m_uiRealMemUsage == _obj.m_uiRealMemUsage &&
					m_uiVirtualMemUsage == _obj.m_uiVirtualMemUsage &&
					m_uiStartTime == _obj.m_uiStartTime &&
					m_uiTotalTime == _obj.m_uiTotalTime &&
					m_uiUserTime == _obj.m_uiUserTime &&
					m_uiSystemTime == _obj.m_uiSystemTime &&
					m_uiState == _obj.m_uiState &&
					m_uiPercentsUsage == _obj.m_uiPercentsUsage);
}

void CProcInfoStatistics::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlNode_PercentsUsage = parent_doc->createElement( XML_STATISTICS_CPU_PERCENTS_USAGE );
	QDomText xmlNode_PercentsUsageValue = parent_doc->createTextNode( QString::number(m_uiPercentsUsage) );
	xmlNode_PercentsUsage.appendChild( xmlNode_PercentsUsageValue );
	parent_element->appendChild( xmlNode_PercentsUsage );

	QDomElement xmlNode_CommandName = parent_doc->createElement( XML_STATISTICS_PROCESS_COMMAND_NAME );
	QDomText xmlNode_CommandNameValue = parent_doc->createTextNode( m_sCommandName );
	xmlNode_CommandName.appendChild( xmlNode_CommandNameValue );
	parent_element->appendChild( xmlNode_CommandName );

	QDomElement xmlNode_ProcId = parent_doc->createElement( XML_STATISTICS_PROCESS_ID );
	QDomText xmlNode_ProcIdValue = parent_doc->createTextNode( QString::number(m_uiProcId) );
	xmlNode_ProcId.appendChild( xmlNode_ProcIdValue );
	parent_element->appendChild( xmlNode_ProcId );

	QDomElement xmlNode_OwnerUser = parent_doc->createElement( XML_STATISTICS_PROCESS_OWNER_USER );
	QDomText xmlNode_OwnerUserValue = parent_doc->createTextNode( m_sOwnerUser );
	xmlNode_OwnerUser.appendChild( xmlNode_OwnerUserValue );
	parent_element->appendChild( xmlNode_OwnerUser );

	QDomElement xmlNode_TotalMemUsage = parent_doc->createElement( XML_STATISTICS_PROCESS_TOTAL_MEM_USAGE );
	QDomText xmlNode_TotalMemUsageValue = parent_doc->createTextNode( QString::number(m_uiTotalMemUsage) );
	xmlNode_TotalMemUsage.appendChild( xmlNode_TotalMemUsageValue );
	parent_element->appendChild( xmlNode_TotalMemUsage );

	QDomElement xmlNode_RealMemUsage = parent_doc->createElement( XML_STATISTICS_PROCESS_REAL_MEM_USAGE );
	QDomText xmlNode_RealMemUsageValue = parent_doc->createTextNode( QString::number(m_uiRealMemUsage) );
	xmlNode_RealMemUsage.appendChild( xmlNode_RealMemUsageValue );
	parent_element->appendChild( xmlNode_RealMemUsage );

	QDomElement xmlNode_VirtualMemUsage = parent_doc->createElement( XML_STATISTICS_PROCESS_VIRTUAL_MEM_USAGE );
	QDomText xmlNode_VirtualMemUsageValue = parent_doc->createTextNode( QString::number(m_uiVirtualMemUsage) );
	xmlNode_VirtualMemUsage.appendChild( xmlNode_VirtualMemUsageValue );
	parent_element->appendChild( xmlNode_VirtualMemUsage );

	QDomElement xmlNode_StartTime = parent_doc->createElement( XML_STATISTICS_PROCESS_START_TIME );
	QDomText xmlNode_StartTimeValue = parent_doc->createTextNode( QString::number(m_uiStartTime) );
	xmlNode_StartTime.appendChild( xmlNode_StartTimeValue );
	parent_element->appendChild( xmlNode_StartTime );

	QDomElement xmlNode_TotalTime = parent_doc->createElement( XML_STATISTICS_PROCESS_TOTAL_TIME );
	QDomText xmlNode_TotalTimeValue = parent_doc->createTextNode( QString::number(m_uiTotalTime) );
	xmlNode_TotalTime.appendChild( xmlNode_TotalTimeValue );
	parent_element->appendChild( xmlNode_TotalTime );

	QDomElement xmlNode_UserTime = parent_doc->createElement( XML_STATISTICS_PROCESS_USER_TIME );
	QDomText xmlNode_UserTimeValue = parent_doc->createTextNode( QString::number(m_uiUserTime) );
	xmlNode_UserTime.appendChild( xmlNode_UserTimeValue );
	parent_element->appendChild( xmlNode_UserTime );

	QDomElement xmlNode_SystemTime = parent_doc->createElement( XML_STATISTICS_PROCESS_SYSTEM_TIME );
	QDomText xmlNode_SystemTimeValue = parent_doc->createTextNode( QString::number(m_uiSystemTime) );
	xmlNode_SystemTime.appendChild( xmlNode_SystemTimeValue );
	parent_element->appendChild( xmlNode_SystemTime );

	QDomElement xmlNode_State = parent_doc->createElement( XML_STATISTICS_PROCESS_STATE );
	QDomText xmlNode_StateValue = parent_doc->createTextNode( QString::number(m_uiState) );
	xmlNode_State.appendChild( xmlNode_StateValue );
	parent_element->appendChild( xmlNode_State );
}

int CProcInfoStatistics::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_CPU_PERCENTS_USAGE, strParamValue ) )
		return SystemStatisticsParser::BadCpuPercentsUsage;
	m_uiPercentsUsage = strParamValue.toUInt();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_COMMAND_NAME, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoCommandName;
	m_sCommandName = strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_ID, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoProcId;
	m_uiProcId = strParamValue.toUInt();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_OWNER_USER, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoOwnerUser;
	m_sOwnerUser = strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_TOTAL_MEM_USAGE, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoTotalMemUsage;
	m_uiTotalMemUsage = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_REAL_MEM_USAGE, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoRealMemUsage;
	m_uiRealMemUsage = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_VIRTUAL_MEM_USAGE, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoVirtualMemUsage;
	m_uiVirtualMemUsage = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_START_TIME, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoStartTime;
	m_uiStartTime = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_TOTAL_TIME, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoTotalTime;
	m_uiTotalTime = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_USER_TIME, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoUserTime;
	m_uiUserTime = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_SYSTEM_TIME, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoSystemTime;
	m_uiSystemTime = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_PROCESS_STATE, strParamValue ) )
		return SystemStatisticsParser::BadProcInfoState;
	m_uiState = (PRL_PROCESS_STATE_TYPE)strParamValue.toUInt();
	if (!isCorrectProcessState(m_uiState))
	{
		m_uiState = PPS_PROC_IDLE;
		return (SystemStatisticsParser::BadProcInfoState);
	}

	return SystemStatisticsParser::RcSuccess;
}

void CProcInfoStatistics::cleanupClassProperties()
{
	m_sCommandName = m_sOwnerUser = "";
	m_uiProcId = 0;
	m_uiState = PPS_PROC_IDLE;
	m_uiTotalMemUsage = m_uiRealMemUsage = m_uiVirtualMemUsage = m_uiStartTime = m_uiTotalTime = 0;
	m_uiUserTime = m_uiSystemTime = 0;
	m_uiPercentsUsage = 0;
}

QString CProcInfoStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_PROCESS);
}

bool CProcInfoStatistics::isCorrectProcessState(PRL_PROCESS_STATE_TYPE _state)
{
	switch (_state)
	{
		case PPS_PROC_SLEEP:
		case PPS_PROC_RUN:
		case PPS_PROC_STOP:
		case PPS_PROC_ZOMBIE:
		case PPS_PROC_IDLE:
			return (true);
	}
	return (false);
}

void CProcInfoStatistics::Serialize(QDataStream &_stream)
{
	CPrlStringDataSerializer(m_sCommandName).Serialize(_stream);
	PrlOpaqueSerializer(m_uiProcId).Serialize(_stream);
	CPrlStringDataSerializer(m_sOwnerUser).Serialize(_stream);
	PrlOpaqueSerializer(m_uiTotalMemUsage).Serialize(_stream);
	PrlOpaqueSerializer(m_uiRealMemUsage).Serialize(_stream);
	PrlOpaqueSerializer(m_uiVirtualMemUsage).Serialize(_stream);
	PrlOpaqueSerializer(m_uiStartTime).Serialize(_stream);
	PrlOpaqueSerializer(m_uiTotalTime).Serialize(_stream);
	PrlOpaqueSerializer(m_uiUserTime).Serialize(_stream);
	PrlOpaqueSerializer(m_uiSystemTime).Serialize(_stream);
	PrlOpaqueSerializer( (quint32 &)PrlOpaqueTypeConverter(m_uiState)  ).Serialize(_stream);
	PrlOpaqueSerializer(m_uiPercentsUsage).Serialize(_stream);
}

void CProcInfoStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	CPrlStringDataSerializer(m_sCommandName).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiProcId).Deserialize(_stream);
	CPrlStringDataSerializer(m_sOwnerUser).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiTotalMemUsage).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiRealMemUsage).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiVirtualMemUsage).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiStartTime).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiTotalTime).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiUserTime).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiSystemTime).Deserialize(_stream);
	PrlOpaqueSerializer( (quint32 &)PrlOpaqueTypeConverter(m_uiState) ).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiPercentsUsage).Deserialize(_stream);
}

/************************************Network interface statisitcs element implementation*********************/

CNetIfaceStatistics::CNetIfaceStatistics(QDomElement *parent_element)
{
	readXml(parent_element);
}

bool CNetIfaceStatistics::operator==(const CNetIfaceStatistics &_obj)
{
	return (m_sIfaceSystemName == _obj.m_sIfaceSystemName &&
					m_uiInDataSize == _obj.m_uiInDataSize &&
					m_uiOutDataSize == _obj.m_uiOutDataSize &&
					m_uiInPkgsCount == _obj.m_uiInPkgsCount &&
					m_uiOutPkgsCount == _obj.m_uiOutPkgsCount);
}

void CNetIfaceStatistics::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlNode_IfaceSystemName = parent_doc->createElement( XML_STATISTICS_NET_IFACE_SYSTEM_NAME );
	QDomText xmlNode_IfaceSystemNameValue = parent_doc->createTextNode( m_sIfaceSystemName );
	xmlNode_IfaceSystemName.appendChild( xmlNode_IfaceSystemNameValue );
	parent_element->appendChild( xmlNode_IfaceSystemName );

	QDomElement xmlNode_InDataSize = parent_doc->createElement( XML_STATISTICS_NET_IFACE_IN_SIZE );
	QDomText xmlNode_InDataSizeValue = parent_doc->createTextNode( QString::number(m_uiInDataSize) );
	xmlNode_InDataSize.appendChild( xmlNode_InDataSizeValue );
	parent_element->appendChild( xmlNode_InDataSize );

	QDomElement xmlNode_OutDataSize = parent_doc->createElement( XML_STATISTICS_NET_IFACE_OUT_SIZE );
	QDomText xmlNode_OutDataSizeValue = parent_doc->createTextNode( QString::number(m_uiOutDataSize) );
	xmlNode_OutDataSize.appendChild( xmlNode_OutDataSizeValue );
	parent_element->appendChild( xmlNode_OutDataSize );

	QDomElement xmlNode_InPkgsCount = parent_doc->createElement( XML_STATISTICS_NET_IFACE_IN_PKGS_COUNT );
	QDomText xmlNode_InPkgsCountValue = parent_doc->createTextNode( QString::number(m_uiInPkgsCount) );
	xmlNode_InPkgsCount.appendChild( xmlNode_InPkgsCountValue );
	parent_element->appendChild( xmlNode_InPkgsCount );

	QDomElement xmlNode_OutPkgsCount = parent_doc->createElement( XML_STATISTICS_NET_IFACE_OUT_PKGS_COUNT );
	QDomText xmlNode_OutPkgsCountValue = parent_doc->createTextNode( QString::number(m_uiOutPkgsCount) );
	xmlNode_OutPkgsCount.appendChild( xmlNode_OutPkgsCountValue );
	parent_element->appendChild( xmlNode_OutPkgsCount );
}

int CNetIfaceStatistics::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_NET_IFACE_SYSTEM_NAME, strParamValue ) )
		return SystemStatisticsParser::BadNetIfaceSystemName;
	m_sIfaceSystemName = strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_NET_IFACE_IN_SIZE, strParamValue ) )
		return SystemStatisticsParser::BadNetIfaceInDataSize;
	m_uiInDataSize = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_NET_IFACE_OUT_SIZE, strParamValue ) )
		return SystemStatisticsParser::BadNetIfaceOutDataSize;
	m_uiOutDataSize = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_NET_IFACE_IN_PKGS_COUNT, strParamValue ) )
		return SystemStatisticsParser::BadNetIfaceInPkgsCount;
	m_uiInPkgsCount = strParamValue.toULongLong();

	if( !getParamNodeValue( parent_element, XML_STATISTICS_NET_IFACE_OUT_PKGS_COUNT, strParamValue ) )
		return SystemStatisticsParser::BadNetIfaceOutPkgsCount;
	m_uiOutPkgsCount = strParamValue.toULongLong();

	return SystemStatisticsParser::RcSuccess;
}

void CNetIfaceStatistics::cleanupClassProperties()
{
	m_sIfaceSystemName = "";
	m_uiInDataSize = m_uiOutDataSize = m_uiInPkgsCount = m_uiOutPkgsCount = 0;
}

QString CNetIfaceStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_NET_IFACE);
}

void CNetIfaceStatistics::Serialize(QDataStream &_stream)
{
	CPrlStringDataSerializer(m_sIfaceSystemName).Serialize(_stream);
	PrlOpaqueSerializer(m_uiInDataSize).Serialize(_stream);
	PrlOpaqueSerializer(m_uiOutDataSize).Serialize(_stream);
	PrlOpaqueSerializer(m_uiInPkgsCount).Serialize(_stream);
	PrlOpaqueSerializer(m_uiOutPkgsCount).Serialize(_stream);
}

void CNetIfaceStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	CPrlStringDataSerializer(m_sIfaceSystemName).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiInDataSize).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiOutDataSize).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiInPkgsCount).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiOutPkgsCount).Deserialize(_stream);
}

/************************************Users sessions statisitcs element implementation*********************/

CUserStatistics::CUserStatistics(QDomElement *parent_element)
{
	readXml(parent_element);
}

bool CUserStatistics::operator==(const CUserStatistics &_obj)
{
	return (m_sUserName == _obj.m_sUserName &&
					m_sServiceName == _obj.m_sServiceName &&
					m_sHostName == _obj.m_sHostName &&
					m_uiSessionTime == _obj.m_uiSessionTime);
}

void CUserStatistics::appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlNode_UserName = parent_doc->createElement( XML_STATISTICS_SESSION_USER_NAME );
	QDomText xmlNode_UserNameValue = parent_doc->createTextNode( m_sUserName );
	xmlNode_UserName.appendChild( xmlNode_UserNameValue );
	parent_element->appendChild( xmlNode_UserName );

	QDomElement xmlNode_ServiceName = parent_doc->createElement( XML_STATISTICS_SESSION_SERVICE_NAME );
	QDomText xmlNode_ServiceNameValue = parent_doc->createTextNode( m_sServiceName );
	xmlNode_ServiceName.appendChild( xmlNode_ServiceNameValue );
	parent_element->appendChild( xmlNode_ServiceName );

	QDomElement xmlNode_HostName = parent_doc->createElement( XML_STATISTICS_SESSION_HOST_NAME );
	QDomText xmlNode_HostNameValue = parent_doc->createTextNode( m_sHostName );
	xmlNode_HostName.appendChild( xmlNode_HostNameValue );
	parent_element->appendChild( xmlNode_HostName );

	QDomElement xmlNode_SessionTime = parent_doc->createElement( XML_STATISTICS_SESSION_DURATION_TIME );
	QDomText xmlNode_SessionTimeValue = parent_doc->createTextNode( QString::number(m_uiSessionTime) );
	xmlNode_SessionTime.appendChild( xmlNode_SessionTimeValue );
	parent_element->appendChild( xmlNode_SessionTime );
}

int CUserStatistics::parseClassProperties(QDomElement* parent_element)
{
	QString strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_SESSION_USER_NAME, strParamValue ) )
		return SystemStatisticsParser::BadSessionUserName;
	m_sUserName = strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_SESSION_SERVICE_NAME, strParamValue ) )
		return SystemStatisticsParser::BadSessionServiceName;
	m_sServiceName = strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_SESSION_HOST_NAME, strParamValue ) )
		return SystemStatisticsParser::BadSessionHostName;
	m_sHostName = strParamValue;

	if( !getParamNodeValue( parent_element, XML_STATISTICS_SESSION_DURATION_TIME, strParamValue ) )
		return SystemStatisticsParser::BadSessionDurationTime;
	m_uiSessionTime = strParamValue.toULongLong();

	return SystemStatisticsParser::RcSuccess;
}

void CUserStatistics::cleanupClassProperties()
{
	m_sUserName = m_sServiceName = m_sHostName = "";
	m_uiSessionTime = 0;
}

QString CUserStatistics::getConcreteClassElementName()
{
	return (XML_STATISTICS_EL_USER_SESSION);
}

void CUserStatistics::Serialize(QDataStream &_stream)
{
	CPrlStringDataSerializer(m_sUserName).Serialize(_stream);
	CPrlStringDataSerializer(m_sServiceName).Serialize(_stream);
	CPrlStringDataSerializer(m_sHostName).Serialize(_stream);
	PrlOpaqueSerializer(m_uiSessionTime).Serialize(_stream);
}

void CUserStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	CPrlStringDataSerializer(m_sUserName).Deserialize(_stream);
	CPrlStringDataSerializer(m_sServiceName).Deserialize(_stream);
	CPrlStringDataSerializer(m_sHostName).Deserialize(_stream);
	PrlOpaqueSerializer(m_uiSessionTime).Deserialize(_stream);
}

/************************************System statisitcs implementation**************************/

CSystemStatistics::CSystemStatistics()
: m_pMemoryStatistics(new CMemoryStatistics), m_pSwapStatistics(new CSwapStatistics),
	m_pUptimeStatistics(new CUptimeStatistics)
{}

CSystemStatistics::CSystemStatistics(const CSystemStatistics &_obj)
:	CPrlDataSerializer(),
	m_pMemoryStatistics(new CMemoryStatistics(*_obj.m_pMemoryStatistics)),
	m_pSwapStatistics(new CSwapStatistics(*_obj.m_pSwapStatistics)),
	m_pUptimeStatistics(new CUptimeStatistics(*_obj.m_pUptimeStatistics))
{
	foreach(CCpuStatistics *pCpuStat, _obj.m_lstCpusStatistics)
		m_lstCpusStatistics.append(new CCpuStatistics(*pCpuStat));
	foreach(CDiskStatistics *pDiskStat, _obj.m_lstDisksStatistics)
		m_lstDisksStatistics.append(new CDiskStatistics(*pDiskStat));
	foreach(CProcInfoStatistics *pProcInfoStat, _obj.m_lstProcessesStatistics)
		m_lstProcessesStatistics.append(new CProcInfoStatistics(*pProcInfoStat));
	foreach(CNetIfaceStatistics *pNetIfaceStat, _obj.m_lstNetIfacesStatistics)
		m_lstNetIfacesStatistics.append(new CNetIfaceStatistics(*pNetIfaceStat));
	foreach(CUserStatistics *pUserStat, _obj.m_lstUsersStatistics)
		m_lstUsersStatistics.append(new CUserStatistics(*pUserStat));
}

CSystemStatistics::CSystemStatistics(const QString &source_string)
: m_pMemoryStatistics(new CMemoryStatistics), m_pSwapStatistics(new CSwapStatistics),
	m_pUptimeStatistics(new CUptimeStatistics)
{
	if( !IS_OPERATION_SUCCEEDED( this->fromString( source_string ) ) )
		m_uiRcInit = PRL_ERR_PARSE_STATISTICS;
}

CSystemStatistics::~CSystemStatistics()
{
	cleanupClassProperties();
	delete m_pMemoryStatistics;
	m_pMemoryStatistics = NULL;
	delete m_pSwapStatistics;
	m_pSwapStatistics = NULL;
	delete m_pUptimeStatistics;
	m_pUptimeStatistics = NULL;
}

void CSystemStatistics::appendClassProperties(QDomElement* parent_element)
{
	parent_element->setAttribute( XML_VM_ATTR_SCHEMA_VERSION, XML_VM_CONFIG_SCHEMA_VERSION );
}

int CSystemStatistics::parseClassProperties(QDomElement *)
{
	return SystemStatisticsParser::RcSuccess;
}

QDomDocument CSystemStatistics::getXml(bool include_xml_node)
{
	// create document and root element
	QDomDocument xmlSystemStatisticsDocument = createClassElement();
	QDomElement xmlSystemStatisticsRoot = xmlSystemStatisticsDocument.firstChild().toElement();

	// attach standard ?xml header
	if( include_xml_node )
		attachXmlHeader( &xmlSystemStatisticsDocument );

	// Append class-specific properties
	appendClassProperties( &xmlSystemStatisticsRoot );

	// Append CPUs statistics
	appendCpusStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append disks statistics
	appendDisksStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append RAM statistics
	appendMemoryStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append swap statistics
	appendSwapStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append uptime statistics
	appendUptimeStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append processes statistics
	appendProcInfoStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append net ifaces statistics
	appendNetIfacesStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append users statistics
	appendUsersStatistics( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append VM data statistic
	appendVmDataStatistic( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	// Append guest OS info
	appendGuestOsInformation( &xmlSystemStatisticsDocument, &xmlSystemStatisticsRoot );

	return xmlSystemStatisticsDocument;
}

#define CHECK_RETURN_CODE(i_rc)\
	if( !IS_OPERATION_SUCCEEDED( i_rc ) )\
	{\
		m_iParseRc = i_rc;\
		m_uiRcInit = PRL_ERR_PARSE_STATISTICS;\
		return i_rc;\
	}

int CSystemStatistics::readXml(QDomDocument* source_doc)
{
	int i_rc;
	m_uiRcInit = PRL_ERR_SUCCESS;

	// check source document
	i_rc = checkSourceDoc( source_doc );
	CHECK_RETURN_CODE(i_rc)

	QDomElement xmlStatisticsRoot = source_doc->documentElement();
	cleanupClassProperties();

	// parse class-specific properties
	i_rc = parseClassProperties( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse CPUs statistics
	i_rc = parseCpusStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse disk statistics
	i_rc = parseDisksStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse RAM statistics
	i_rc = parseMemoryStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse swap statistics
	i_rc = parseSwapStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse uptime statistics
	i_rc = parseUptimeStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse processes statistics
	i_rc = parseProcInfoStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse net ifaces statistics
	i_rc = parseNetIfacesStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse user sessions statistics
	i_rc = parseUsersStatistics( &xmlStatisticsRoot );
	CHECK_RETURN_CODE(i_rc)

	// parse VM data statistic
	parseVmDataStatistic( &xmlStatisticsRoot );

	// parse guest OS info
	parseGuestOsInformation( &xmlStatisticsRoot );

	m_iParseRc = SystemStatisticsParser::RcSuccess;
	return m_iParseRc;
}

void CSystemStatistics::appendCpusStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlStatGroup = parent_doc->createElement( XML_STATISTICS_EL_CPUS_GROUP );
	parent_element->appendChild( xmlStatGroup );

	foreach(CCpuStatistics *pCpuStatistics, m_lstCpusStatistics)
		xmlStatGroup.appendChild( pCpuStatistics->getXml( parent_doc ) );
}

void CSystemStatistics::appendDisksStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlStatGroup = parent_doc->createElement( XML_STATISTICS_EL_DISKS_GROUP );
	parent_element->appendChild( xmlStatGroup );

	foreach(CDiskStatistics *pDiskStatistics, m_lstDisksStatistics)
		xmlStatGroup.appendChild( pDiskStatistics->getXml( parent_doc ) );
}

void CSystemStatistics::appendMemoryStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	parent_element->appendChild( m_pMemoryStatistics->getXml(parent_doc) );
}

void CSystemStatistics::appendSwapStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	parent_element->appendChild( m_pSwapStatistics->getXml(parent_doc) );
}

void CSystemStatistics::appendUptimeStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	parent_element->appendChild( m_pUptimeStatistics->getXml(parent_doc) );
}

void CSystemStatistics::appendProcInfoStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlStatGroup = parent_doc->createElement( XML_STATISTICS_EL_PROCESSES_GROUP );
	parent_element->appendChild( xmlStatGroup );

	foreach(CProcInfoStatistics *pProcInfoStatistics, m_lstProcessesStatistics)
		xmlStatGroup.appendChild( pProcInfoStatistics->getXml( parent_doc ) );
}

void CSystemStatistics::appendNetIfacesStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlStatGroup = parent_doc->createElement( XML_STATISTICS_EL_NET_IFACES_GROUP );
	parent_element->appendChild( xmlStatGroup );

	foreach(CNetIfaceStatistics *pNetIfaceStatistics, m_lstNetIfacesStatistics)
		xmlStatGroup.appendChild( pNetIfaceStatistics->getXml( parent_doc ) );
}

void CSystemStatistics::appendUsersStatistics(QDomDocument* parent_doc, QDomElement* parent_element)
{
	QDomElement xmlStatGroup = parent_doc->createElement( XML_STATISTICS_EL_USER_SESSIONS_GROUP );
	parent_element->appendChild( xmlStatGroup );

	foreach(CUserStatistics *pUserStatistics, m_lstUsersStatistics)
		xmlStatGroup.appendChild( pUserStatistics->getXml( parent_doc ) );
}

void CSystemStatistics::appendVmDataStatistic(QDomDocument* parent_doc, QDomElement* parent_element)
{
	parent_element->appendChild(m_VmDataStatistic.getXml(parent_doc));
}

void CSystemStatistics::appendGuestOsInformation(QDomDocument* parent_doc, QDomElement* parent_element)
{
	parent_element->appendChild(m_GuestOsInformation.getXml(parent_doc));
}

int CSystemStatistics::parseCpusStatistics(QDomElement* parent_element)
{
	QDomElement xmlCpusStatGroupElement = parent_element->firstChildElement( XML_STATISTICS_EL_CPUS_GROUP );
	if (xmlCpusStatGroupElement.isNull())
		return (SystemStatisticsParser::AbsentCpusStatGroupElement);

	QDomElement xmlCpuStatElement = xmlCpusStatGroupElement.firstChildElement( XML_STATISTICS_EL_CPU );
	for (; !xmlCpuStatElement.isNull(); xmlCpuStatElement = xmlCpuStatElement.nextSiblingElement( XML_STATISTICS_EL_CPU ))
		m_lstCpusStatistics.append(new CCpuStatistics(&xmlCpuStatElement));

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseDisksStatistics(QDomElement* parent_element)
{
	QDomElement xmlDisksStatGroupElement = parent_element->firstChildElement( XML_STATISTICS_EL_DISKS_GROUP );
	if (xmlDisksStatGroupElement.isNull())
		return (SystemStatisticsParser::AbsentDisksStatGroupElement);

	QDomElement xmlDiskStatElement = xmlDisksStatGroupElement.firstChildElement( XML_STATISTICS_EL_DISK );
	for (; !xmlDiskStatElement.isNull(); xmlDiskStatElement = xmlDiskStatElement.nextSiblingElement( XML_STATISTICS_EL_DISK ))
		m_lstDisksStatistics.append(new CDiskStatistics(&xmlDiskStatElement));

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseMemoryStatistics(QDomElement* parent_element)
{
	QDomElement xmlMemoryStatElement = parent_element->firstChildElement( XML_STATISTICS_EL_MEMORY );
	if (xmlMemoryStatElement.isNull())
		return (SystemStatisticsParser::AbsentMemoryStatElement);

	m_pMemoryStatistics->readXml(&xmlMemoryStatElement);

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseSwapStatistics(QDomElement* parent_element)
{
	QDomElement xmlSwapStatElement = parent_element->firstChildElement( XML_STATISTICS_EL_SWAP );
	if (xmlSwapStatElement.isNull())
		return (SystemStatisticsParser::AbsentSwapStatElement);

	m_pSwapStatistics->readXml(&xmlSwapStatElement);

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseUptimeStatistics(QDomElement* parent_element)
{
	QDomElement xmlUptimeStatElement = parent_element->firstChildElement( XML_STATISTICS_EL_UPTIME );
	if (xmlUptimeStatElement.isNull())
		return (SystemStatisticsParser::AbsentUptimeStatElement);

	m_pUptimeStatistics->readXml(&xmlUptimeStatElement);

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseProcInfoStatistics(QDomElement* parent_element)
{
	QDomElement xmlProcInfoStatGroupElement = parent_element->firstChildElement( XML_STATISTICS_EL_PROCESSES_GROUP );
	if (xmlProcInfoStatGroupElement.isNull())
		return (SystemStatisticsParser::AbsentProcInfoStatGroupElement);

	QDomElement xmlProcInfoStatElement = xmlProcInfoStatGroupElement.firstChildElement( XML_STATISTICS_EL_PROCESS );
	for (; !xmlProcInfoStatElement.isNull();
					xmlProcInfoStatElement = xmlProcInfoStatElement.nextSiblingElement( XML_STATISTICS_EL_PROCESS ))
		m_lstProcessesStatistics.append(new CProcInfoStatistics(&xmlProcInfoStatElement));

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseNetIfacesStatistics(QDomElement* parent_element)
{
	QDomElement xmlNetIfacesStatGroupElement = parent_element->firstChildElement( XML_STATISTICS_EL_NET_IFACES_GROUP );
	if (xmlNetIfacesStatGroupElement.isNull())
		return (SystemStatisticsParser::AbsentNetIfacesStatGroupElement);

	QDomElement xmlNetIfaceStatElement = xmlNetIfacesStatGroupElement.firstChildElement( XML_STATISTICS_EL_NET_IFACE );
	for (; !xmlNetIfaceStatElement.isNull();
					xmlNetIfaceStatElement = xmlNetIfaceStatElement.nextSiblingElement( XML_STATISTICS_EL_NET_IFACE ))
		m_lstNetIfacesStatistics.append(new CNetIfaceStatistics(&xmlNetIfaceStatElement));

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseUsersStatistics(QDomElement* parent_element)
{
	QDomElement xmlUsersStatGroupElement = parent_element->firstChildElement( XML_STATISTICS_EL_USER_SESSIONS_GROUP );
	if (xmlUsersStatGroupElement.isNull())
		return (SystemStatisticsParser::AbsentUsersStatGroupElement);

	QDomElement xmlUserStatElement = xmlUsersStatGroupElement.firstChildElement( XML_STATISTICS_EL_USER_SESSION );
	for (; !xmlUserStatElement.isNull();
					xmlUserStatElement = xmlUserStatElement.nextSiblingElement( XML_STATISTICS_EL_USER_SESSION ))
		m_lstUsersStatistics.append(new CUserStatistics(&xmlUserStatElement));

	return SystemStatisticsParser::RcSuccess;
}

int CSystemStatistics::parseVmDataStatistic(QDomElement* parent_element)
{
	QDomElement elem = parent_element->firstChildElement("ParallelsVmDataStatistic");
	return m_VmDataStatistic.readXml(&elem);
}

int CSystemStatistics::parseGuestOsInformation(QDomElement* parent_element)
{
	QDomElement elem = parent_element->firstChildElement("GuestOsInformation");
	return m_GuestOsInformation.readXml(&elem);
}

int CSystemStatistics::checkSourceDoc(QDomDocument* parent_doc)
{
	// get root element of the XML doc
	QDomElement xmlSystemStatisticsRoot = parent_doc->documentElement();
	if( !( xmlSystemStatisticsRoot.tagName().compare( XML_STATISTICS_EL_ROOT ) == 0 ) )
		return SystemStatisticsParser::BadRoot;

	// get doc schema version
	QDomAttr xmlSchemaVersion = xmlSystemStatisticsRoot.attributeNode( XML_VM_ATTR_SCHEMA_VERSION );
	if( xmlSchemaVersion.isNull() )
		return SystemStatisticsParser::NoSchemaVersion;

	// check if we have a compatible schema version
	if( !( xmlSchemaVersion.value().compare( XML_VM_CONFIG_SCHEMA_VERSION ) == 0 ) )
		return SystemStatisticsParser::IncompatibleSchema;

	return SystemStatisticsParser::RcSuccess;
}

QDomDocument CSystemStatistics::createClassElement()
{
	QDomDocument xmlSystemStatisticsDocument;
	QDomElement xmlSystemStatisticsRoot;

	// create root element
	xmlSystemStatisticsRoot = xmlSystemStatisticsDocument.createElement( XML_STATISTICS_EL_ROOT );
	xmlSystemStatisticsDocument.appendChild( xmlSystemStatisticsRoot );

	return xmlSystemStatisticsDocument;
}

void CSystemStatistics::attachXmlHeader(QDomDocument* xml_doc)
{
	QDomNode xmlXmlVersionNode;

	xmlXmlVersionNode = xml_doc->createProcessingInstruction(
		XML_DOC_VERSION_TAG, XML_DOC_VERSION_DATA );
	xml_doc->insertBefore( xmlXmlVersionNode, xml_doc->firstChild() );
}

void CSystemStatistics::cleanupClassProperties()
{
	m_szErrMsg = "";
	m_iErrLine = m_iErrCol = -1;
	m_uiRcInit = PRL_ERR_SUCCESS;
	m_iParseRc = SystemStatisticsParser::RcSuccess;

	while( !m_lstCpusStatistics.isEmpty() )
	{
		CCpuStatistics *pCpuStatItem = m_lstCpusStatistics.takeFirst();
		m_lstCpusStatistics.removeAll( pCpuStatItem );
		if( pCpuStatItem )
 			delete pCpuStatItem;
	}

	while( !m_lstDisksStatistics.isEmpty() )
	{
		CDiskStatistics *pDiskStatItem = m_lstDisksStatistics.takeFirst();
		m_lstDisksStatistics.removeAll( pDiskStatItem );
		if( pDiskStatItem )
 			delete pDiskStatItem;
	}

	while( !m_lstProcessesStatistics.isEmpty() )
	{
		CProcInfoStatistics *pProcInfoStatItem = m_lstProcessesStatistics.takeFirst();
		m_lstProcessesStatistics.removeAll( pProcInfoStatItem );
		if( pProcInfoStatItem )
 			delete pProcInfoStatItem;
	}

	while( !m_lstNetIfacesStatistics.isEmpty() )
	{
		CNetIfaceStatistics *pNetIfaceStatItem = m_lstNetIfacesStatistics.takeFirst();
		m_lstNetIfacesStatistics.removeAll( pNetIfaceStatItem );
		if( pNetIfaceStatItem )
 			delete pNetIfaceStatItem;
	}

	while( !m_lstUsersStatistics.isEmpty() )
	{
		CUserStatistics *pUserStatItem = m_lstUsersStatistics.takeFirst();
		m_lstUsersStatistics.removeAll( pUserStatItem );
		if( pUserStatItem )
 			delete pUserStatItem;
	}

	m_pMemoryStatistics->cleanupClassProperties();
	m_pSwapStatistics->cleanupClassProperties();
	m_pUptimeStatistics->cleanupClassProperties();
}

QString CSystemStatistics::toString()
{
	QString strOutput;
	QTextStream out( &strOutput );
	this->getXml( XML_DOC_INCLUDE_XML_NODE ).save( out, 3 );
	out.flush();
	return strOutput;

}

int CSystemStatistics::fromString(const QString &source_string)
{
	cleanupClassProperties();

	QDomDocument xmlSourceDoc;

	bool parse_rc = xmlSourceDoc.setContent( source_string, true, &m_szErrMsg, &m_iErrLine, &m_iErrCol );
	if( !parse_rc )
	{
		m_uiRcInit = PRL_ERR_PARSE_STATISTICS;
		m_iParseRc = SystemStatisticsParser::BadSourceData;
		return m_iParseRc;
	}

	int read_rc = this->readXml( &xmlSourceDoc );
	if( !IS_OPERATION_SUCCEEDED( read_rc ) )
	{
		m_uiRcInit = PRL_ERR_PARSE_STATISTICS;
		return read_rc;
	}

	return SystemStatisticsParser::RcSuccess;
}

#define SERIALIZE_LIST_SIZE(list)\
	{\
		int nListSize = list.size();\
		PrlOpaqueSerializer(nListSize).Serialize(_stream);\
	}

void CSystemStatistics::Serialize(QDataStream &_stream)
{
	SERIALIZE_LIST_SIZE(m_lstCpusStatistics)
	foreach(CCpuStatistics *pCpuStat, m_lstCpusStatistics)
		pCpuStat->Serialize(_stream);
	SERIALIZE_LIST_SIZE(m_lstDisksStatistics)
	foreach(CDiskStatistics *pDiskStat, m_lstDisksStatistics)
		pDiskStat->Serialize(_stream);
	SERIALIZE_LIST_SIZE(m_lstProcessesStatistics)
	foreach(CProcInfoStatistics *pProcInfoStat, m_lstProcessesStatistics)
		pProcInfoStat->Serialize(_stream);
	SERIALIZE_LIST_SIZE(m_lstNetIfacesStatistics)
	foreach(CNetIfaceStatistics *pNetIfaceStat, m_lstNetIfacesStatistics)
		pNetIfaceStat->Serialize(_stream);
	SERIALIZE_LIST_SIZE(m_lstUsersStatistics)
	foreach(CUserStatistics *pUserStat, m_lstUsersStatistics)
		pUserStat->Serialize(_stream);
	m_pMemoryStatistics->Serialize(_stream);
	m_pSwapStatistics->Serialize(_stream);
	m_pUptimeStatistics->Serialize(_stream);
}

#define DESERIALIZE_LIST(list, element_type)\
	{\
		int nListSize = 0;\
		PrlOpaqueSerializer(nListSize).Deserialize(_stream);\
		for (int i = 0; i < nListSize; ++i)\
		{\
			element_type *_element = new element_type;\
			_element->Deserialize(_stream);\
			list.append(_element);\
		}\
	}

void CSystemStatistics::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	DESERIALIZE_LIST(m_lstCpusStatistics, CCpuStatistics)
	DESERIALIZE_LIST(m_lstDisksStatistics, CDiskStatistics)
	DESERIALIZE_LIST(m_lstProcessesStatistics, CProcInfoStatistics)
	DESERIALIZE_LIST(m_lstNetIfacesStatistics, CNetIfaceStatistics)
	DESERIALIZE_LIST(m_lstUsersStatistics, CUserStatistics)
	m_pMemoryStatistics->Deserialize(_stream);
	m_pSwapStatistics->Deserialize(_stream);
	m_pUptimeStatistics->Deserialize(_stream);
}
