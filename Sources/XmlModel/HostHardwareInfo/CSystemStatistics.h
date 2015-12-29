/*
 * CSystemStatistics.h: Definition of the system statistics classes
 * set This classes set implements system resources usage statistics
 * container.
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


#ifndef __CSystemStatistics_H__
#define __CSystemStatistics_H__

#include <QList>
#include <QDomDocument>

#include "ParallelsDomModel.h"
#include "ParallelsNamespace.h"
#include <prlcommon/PrlDataSerializer/CPrlDataSerializer.h>
#include <SDK/Include/PrlIOStructs.h>
#include "XmlModel/VmDataStatistic/CVmDataStatistic.h"
#include "XmlModel/GuestOsInformation/CVmGuestOsInformation.h"

/** Base statistics element class */
class CBaseStatisticsElement
{
public:
	/** Virtual destructor to correct child classes instances destruction */
	virtual ~CBaseStatisticsElement() {}
	/** Get XML representation of the class. */
	QDomElement getXml( QDomDocument* parent_doc );
	/** Read XML and setup internal model according to it. */
	int readXml( QDomElement* parent_element );

	int getItemId() const { return -1; }
	void setItemId(int ) {}

private:
	/** Create class element */
	QDomElement createClassElement(QDomDocument* parent_doc);

private:
	/** Overloadable method that returns corresponding class element name */
	virtual QString getConcreteClassElementName() = 0;
	/** Append class-specific properties */
	virtual void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element) = 0;
	/** Parse class-specific properties */
	virtual int parseClassProperties(QDomElement* parent_element) = 0;
	/** Clean up class-specific properties */
	virtual void cleanupClassProperties() = 0;
};

/**
 * Class implements CPU system statistics element
 */
class CCpuStatistics : public CBaseStatisticsElement, public CPrlDataSerializer
{
public:
	/** Class default constructor */
	CCpuStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CCpuStatistics(QDomElement *parent_element);
	/** Compare class objects operator */
	bool operator==(const CCpuStatistics &_obj);
	/** Sets processor usage percents */
	inline void setPercentsUsage(quint32 _value) {m_uiPercentsUsage = _value;}
	/** Returns processor usage percents */
	inline quint32 getPercentsUsage() const {return (m_uiPercentsUsage);}
	/** Sets processor working total time in secs */
	inline void setTotalTime(quint64 _value) {m_uiTotalTime = _value;}
	/** Returns processor working total time in secs */
	inline quint64 getTotalTime() const {return (m_uiTotalTime);}
	/** Sets processor working user space time in secs */
	inline void setUserTime(quint64 _value) {m_uiUserTime = _value;}
	/** Returns processor working user space time in secs */
	inline quint64 getUserTime() const {return (m_uiUserTime);}
	/** Sets processor working system space time in secs */
	inline void setSystemTime(quint64 _value) {m_uiSystemTime = _value;}
	/** Returns processor working system space time in secs */
	inline quint64 getSystemTime() const {return (m_uiSystemTime);}

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** Percents usage */
	quint32 m_uiPercentsUsage;
	/** Total processor working time in secs */
	quint64 m_uiTotalTime;
	/** Processor user space time */
	quint64 m_uiUserTime;
	/** Processor system space time */
	quint64 m_uiSystemTime;

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
	/** Append class-specific properties */
	void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);
	/** Clean up class-specific properties */
	void cleanupClassProperties();
};


/**
 * Class implements disk system statistics element
 */
class CDiskStatistics : public CBaseStatisticsElement, public CPrlDataSerializer
{
public:
	/** Partitions statistics list */
	QList<class CDiskPartStatistics *> m_lstPartitionsStatistics;

public:
	/** Class default constructor */
	CDiskStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CDiskStatistics(QDomElement *parent_element);
	/** Virtual destructor for correct child class instances destruction */
	virtual ~CDiskStatistics();
	/** Compare class objects operator */
	bool operator==(const CDiskStatistics &_obj);
	/** Sets disk device system name */
	inline void setDeviceSystemName(const QString &_value) {m_sDeviceSystemName = _value;}
	/** Sets disk device system name */
	inline QString getDeviceSystemName() const {return (m_sDeviceSystemName);}
	/** Sets usage disk space in bytes */
	inline void setUsageDiskSpace(quint64 _value) {m_uiUsageDiskSpace = _value;}
	/** Returns usage disk space in bytes */
	inline quint64 getUsageDiskSpace() const {return (m_uiUsageDiskSpace);}
	/** Sets free disk space in bytes */
	inline void setFreeDiskSpace(quint64 _value) {m_uiFreeDiskSpace = _value;}
	/** Returns free disk space in bytes */
	inline quint64 getFreeDiskSpace() const {return (m_uiFreeDiskSpace);}

	void setReadBytesTotal(quint64 _value) {m_uiReadBytesTotal = _value;}
	quint64 getReadBytesTotal() const {return m_uiReadBytesTotal;}
	void setWriteBytesTotal(quint64 _value) {m_uiWriteBytesTotal = _value;}
	quint64 getWriteBytesTotal() const {return m_uiWriteBytesTotal;}

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** System device name */
	QString m_sDeviceSystemName;
	/** Usage disk space size */
	quint64 m_uiUsageDiskSpace;
	/** Free disk space size */
	quint64 m_uiFreeDiskSpace;
	/** Total count of read bytes */
	quint64 m_uiReadBytesTotal;
	/** Total count of write bytes */
	quint64 m_uiWriteBytesTotal;

private:
	/** Returns class element name */
	virtual QString getConcreteClassElementName();
	/** Append class-specific properties */
	void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);
	/** Append partitions statistics */
	void appendPartitionsProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse partitions statistics */
	int parsePartitionsProperties(QDomElement* parent_element);

protected:
	/** Clean up class-specific properties */
	void cleanupClassProperties();
};

/**
 * Class implements disk partition statistics element
 */
class CDiskPartStatistics : public CDiskStatistics
{
public:
	/** Class default constructor */
	CDiskPartStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CDiskPartStatistics(QDomElement *parent_element);
	/** Compare class objects operator */
	bool operator==(const CDiskPartStatistics &_obj);
	/** Compare class objects invertion operator */
	bool operator!=(const CDiskPartStatistics &_obj);

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
};


/**
 * Class implements base element for memory statistics objects (real and virtual)
 */
class CBaseMemStatistics : public CBaseStatisticsElement, public CPrlDataSerializer
{
public:
	/** Class default constructor */
	CBaseMemStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CBaseMemStatistics(QDomElement *parent_element);
	/** Compare class objects operator */
	bool operator==(const CBaseMemStatistics &_obj);
	/** Sets total memory size in bytes */
	inline void setTotalSize(quint64 _value) {m_uiTotalSize = _value;}
	/** Returns total memory size in bytes */
	inline quint64 getTotalSize() const {return (m_uiTotalSize);}
	/** Sets usage memory size in bytes */
	inline void setUsageSize(quint64 _value) {m_uiUsageSize = _value;}
	/** Returns usage memory size in bytes */
	inline quint64 getUsageSize() const {return (m_uiUsageSize);}
	/** Sets free memory size in bytes */
	inline void setFreeSize(quint64 _value) {m_uiFreeSize = _value;}
	/** Returns free memory size in bytes */
	inline quint64 getFreeSize() const {return (m_uiFreeSize);}
	/** Sets real memory size in bytes */
	inline void setRealSize(quint64 _value) {m_uiRealSize = _value;}
	/** Returns real memory size in bytes */
	inline quint64 getRealSize() const {return (m_uiRealSize);}
	/** Clean up class-specific properties */
	void cleanupClassProperties();

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** Total memory size */
	quint64 m_uiTotalSize;
	/** Usage memory size */
	quint64 m_uiUsageSize;
	/** Free memory size */
	quint64 m_uiFreeSize;
	/** Real memory size */
	quint64 m_uiRealSize;

private:
	/** Append class-specific properties */
	void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);
};


/**
 * Class implements RAM statistics element
 */
class CMemoryStatistics : public CBaseMemStatistics
{
public:
	/** Class default constructor */
	CMemoryStatistics() {}
	/** Class constructor. Initializes class data by specified XML element */
	CMemoryStatistics(QDomElement *parent_element);

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
};

/**
 * Class implements swap statistics element
 */
class CSwapStatistics : public CBaseMemStatistics
{
public:
	/** Class default constructor */
	CSwapStatistics() {}
	/** Class constructor. Initializes class data by specified */
	CSwapStatistics(QDomElement *parent_element);

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
};

/**
 * Class implements system uptime statistics element
 */
class CUptimeStatistics : public CBaseStatisticsElement, public CPrlDataSerializer
{
public:
	/** Class default constructor */
	CUptimeStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CUptimeStatistics(QDomElement *parent_element);
	/** Compare class objects operator */
	bool operator==(const CUptimeStatistics &_obj);
	/** Sets OS uptime in secs */
	inline void setOsUptime(quint64 _value) {m_uiOsUptime = _value;}
	/** Returns OS uptime in secs */
	inline quint64 getOsUptime() const {return (m_uiOsUptime);}
	/** Sets dispatcher uptime in secs */
	inline void setDispatcherUptime(quint64 _value) {m_uiDispatcherUptime = _value;}
	/** Returns dispatcher uptime in secs */
	inline quint64 getDispatcherUptime() const {return (m_uiDispatcherUptime);}
	/** Clean up class-specific properties */
	void cleanupClassProperties();

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** OS uptime in secs */
	quint64 m_uiOsUptime;
	/** Dispatcher uptime in secs */
	quint64 m_uiDispatcherUptime;

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
	/** Append class-specific properties */
	void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);
};


/**
 * Class implements process info statistics element
 */
class CProcInfoStatistics : public CBaseStatisticsElement, public CPrlDataSerializer
{
public:
	/** Class default constructor */
	CProcInfoStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CProcInfoStatistics(QDomElement *parent_element);
	/** Compare class objects operator */
	bool operator==(const CProcInfoStatistics &_obj);
	/** Sets process command name */
	inline void setCommandName(const QString &_value) {m_sCommandName = _value;}
	/** Returns process command name */
	inline QString getCommandName() const {return (m_sCommandName);}
	/** Sets process id */
	inline void setProcId(quint32 _value) {m_uiProcId = _value;}
	/** Returns process id */
	inline quint32 getProcId() const {return (m_uiProcId);}
	/** Sets process owner user */
	inline void setOwnerUser(const QString &_value) {m_sOwnerUser = _value;}
	/** Returns process owner user */
	inline QString getOwnerUser() const {return (m_sOwnerUser);}
	/** Sets process total memory in bytes */
	inline void setTotalMemUsage(quint64 _value) {m_uiTotalMemUsage = _value;}
	/** Returns process total memory usage in bytes */
	inline quint64 getTotalMemUsage() const {return (m_uiTotalMemUsage);}
	/** Sets process real memory usage in bytes */
	inline void setRealMemUsage(quint64 _value) {m_uiRealMemUsage = _value;}
	/** Returns process real memory usage in bytes */
	inline quint64 getRealMemUsage() const {return (m_uiRealMemUsage);}
	/** Sets process virtual memory usage in bytes */
	inline void setVirtualMemUsage(quint64 _value) {m_uiVirtualMemUsage = _value;}
	/** Returns process virtual memory usage in bytes */
	inline quint64 getVirtualMemUsage() const {return (m_uiVirtualMemUsage);}
	/** Sets process start time in secs (num of seconds since January 1, 1601 (UTC)) */
	inline void setStartTime(quint64 _value) {m_uiStartTime = _value;}
	/** Returns process start time in secs (num of seconds since January 1, 1601 (UTC)) */
	inline quint64 getStartTime() const {return (m_uiStartTime);}
	/** Sets process total time in secs */
	inline void setTotalTime(quint64 _value) {m_uiTotalTime = _value;}
	/** Returns process total time in secs */
	inline quint64 getTotalTime() const {return (m_uiTotalTime);}
	/** Sets process user space time in secs */
	inline void setUserTime(quint64 _value) {m_uiUserTime = _value;}
	/** Returns process user space time in secs */
	inline quint64 getUserTime() const {return (m_uiUserTime);}
	/** Sets process system space time in secs */
	inline void setSystemTime(quint64 _value) {m_uiSystemTime = _value;}
	/** Returns process system space time in secs */
	inline quint64 getSystemTime() const {return (m_uiSystemTime);}
	/** Sets process state */
	inline void setState(PRL_PROCESS_STATE_TYPE _value) {m_uiState = _value;}
	/** Returns process state */
	inline PRL_PROCESS_STATE_TYPE getState() const {return (m_uiState);}
	/** Sets processor usage percents */
	inline void setPercentsUsage(quint32 _value) {m_uiPercentsUsage = _value;}
	/** Returns processor usage percents */
	inline quint32 getPercentsUsage() const {return (m_uiPercentsUsage);}

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** Process command name */
	QString m_sCommandName;
	/** Process id */
	quint32 m_uiProcId;
	/** Process owner user */
	QString m_sOwnerUser;
	/** Process total mem usage size in bytes */
	quint64 m_uiTotalMemUsage;
	/** Process real mem usage size in bytes */
	quint64 m_uiRealMemUsage;
	/** Process virtual mem usage size in bytes */
	quint64 m_uiVirtualMemUsage;
	/** Process start time in secs */
	quint64 m_uiStartTime;
	/** Process total time in secs */
	quint64 m_uiTotalTime;
	/** Process user time in secs */
	quint64 m_uiUserTime;
	/** Process system time in secs */
	quint64 m_uiSystemTime;
	/** Process state */
	PRL_PROCESS_STATE_TYPE m_uiState;
	/** Percents usage */
	quint32 m_uiPercentsUsage;

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
	/** Append class-specific properties */
	void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);
	/** Clean up class-specific properties */
	void cleanupClassProperties();
	/** Checks whether specified process state value correct */
	bool isCorrectProcessState(PRL_PROCESS_STATE_TYPE _state);
};


/**
 * Class implements network interface system statistics element
 */
class CNetIfaceStatistics : public CBaseStatisticsElement, public CPrlDataSerializer
{
public:
	/** Class default constructor */
	CNetIfaceStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CNetIfaceStatistics(QDomElement *parent_element);
	/** Compare class objects operator */
	bool operator==(const CNetIfaceStatistics &_obj);
	/** Sets net iface device system name */
	inline void setIfaceSystemName(const QString &_value) {m_sIfaceSystemName = _value;}
	/** Returns net iface device system name */
	inline QString getIfaceSystemName() const {return (m_sIfaceSystemName);}
	/** Sets input net iface transmitted data in bytes */
	inline void setInDataSize(quint64 _value) {m_uiInDataSize = _value;}
	/** Returns input net iface transmitted data in bytes */
	inline quint64 getInDataSize() const {return (m_uiInDataSize);}
	/** Sets output net iface transmitted data in bytes */
	inline void setOutDataSize(quint64 _value) {m_uiOutDataSize = _value;}
	/** Returns output net iface transmitted data in bytes */
	inline quint64 getOutDataSize() const {return (m_uiOutDataSize);}
	/** Sets in packages count */
	inline void setInPkgsCount(quint64 _value) {m_uiInPkgsCount = _value;}
	/** Returns in packages count */
	inline quint64 getInPkgsCount() const {return (m_uiInPkgsCount);}
	/** Sets out packages count */
	inline void setOutPkgsCount(quint64 _value) {m_uiOutPkgsCount = _value;}
	/** Returns out packages count */
	inline quint64 getOutPkgsCount() const {return (m_uiOutPkgsCount);}

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** System device name */
	QString m_sIfaceSystemName;
	/** Input transmitted data size in bytes */
	quint64 m_uiInDataSize;
	/** Output transmitted data size in bytes */
	quint64 m_uiOutDataSize;
	/** In packages count */
	quint64 m_uiInPkgsCount;
	/** Out packages count */
	quint64 m_uiOutPkgsCount;

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
	/** Append class-specific properties */
	void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);
	/** Clean up class-specific properties */
	void cleanupClassProperties();
};


/**
 * Class implements user session statistics element
 */
class CUserStatistics : public CBaseStatisticsElement, public CPrlDataSerializer
{
public:
	/** Class default constructor */
	CUserStatistics() {cleanupClassProperties();}
	/** Class constructor. Initializes class data by specified XML element */
	CUserStatistics(QDomElement *parent_element);
	/** Compare class objects operator */
	bool operator==(const CUserStatistics &_obj);
	/** Sets session user name */
	inline void setUserName(const QString &_value) {m_sUserName = _value;}
	/** Returns session user name */
	inline QString getUserName() const {return (m_sUserName);}
	/** Sets session service name */
	inline void setServiceName(const QString &_value) {m_sServiceName = _value;}
	/** Returns session service name */
	inline QString getServiceName() const {return (m_sServiceName);}
	/** Sets session hostname */
	inline void setHostName(const QString &_value) {m_sHostName = _value;}
	/** Returns session hostname */
	inline QString getHostName() const {return (m_sHostName);}
	/** Sets session duration time */
	inline void setSessionTime(quint64 _value) {m_uiSessionTime = _value;}
	/** Returns session duration time */
	inline quint64 getSessionTime() const {return (m_uiSessionTime);}

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** Session user name */
	QString m_sUserName;
	/** Session service name */
	QString m_sServiceName;
	/** Session hostname */
	QString m_sHostName;
	/** Session duration time in secs */
	quint64 m_uiSessionTime;

private:
	/** Returns class element name */
	QString getConcreteClassElementName();
	/** Append class-specific properties */
	void appendClassProperties(QDomDocument* parent_doc, QDomElement* parent_element);
	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);
	/** Clean up class-specific properties */
	void cleanupClassProperties();
};

class CNetClassStatistics
{
public:
	CNetClassStatistics();
	PRL_STAT_NET_TRAFFIC *getStat() {return &m_Stat;}
public:
	PRL_STAT_NET_TRAFFIC m_Stat;
};

/**
 * This class implements common system statistics container.
 */
class CSystemStatistics: public CPrlDataSerializer
{
public:
	/** List of CPUs statistics */
	QList<CCpuStatistics *> m_lstCpusStatistics;

	/** List of disks statistics */
	QList<CDiskStatistics *> m_lstDisksStatistics;

	/** List of processes statistics */
	QList<CProcInfoStatistics *> m_lstProcessesStatistics;

	/** List of network interfaces statistics */
	QList<CNetIfaceStatistics *> m_lstNetIfacesStatistics;

	/** List of user sessions statistics */
	QList<CUserStatistics *> m_lstUsersStatistics;

	CVmDataStatistic	m_VmDataStatistic;

	CVmGuestOsInformation	m_GuestOsInformation;

	/** Class initialization return code. @see PRL_RESULT */
	PRL_RESULT m_uiRcInit;

	/** Parse error string */
	QString m_szErrMsg;

	/** Parse error line */
	int m_iErrLine;

	/** Parse error column */
	int m_iErrCol;

	/** Parse specific return code @see PVE::ConfigurationParser */
	int m_iParseRc;

public:
	/** Returns pointer to RAM statistics object */
	inline CMemoryStatistics *getMemoryStatistics() {return (m_pMemoryStatistics);}
	/** Returns pointer to swap statistics object */
	inline CSwapStatistics *getSwapStatistics() {return (m_pSwapStatistics);}
	/** Returns pointer to uptime statistics object */
	inline CUptimeStatistics *getUptimeStatistics() {return (m_pUptimeStatistics);}
	PRL_STAT_NET_TRAFFIC *getNetClassStatistics() {return (m_NetClassStatistics.getStat());}

public:
	/** Default class constructor */
	CSystemStatistics();

	/** Copy constructor */
	CSystemStatistics(const CSystemStatistics &_obj);

	/** Initializing constructor */
	CSystemStatistics(const QString &source_string);

	/** Class destructor */
	~CSystemStatistics();

	/** Get XML representation of the class */
	QDomDocument getXml(bool include_xml_node = XML_DOC_INCLUDE_XML_NODE);

	/** Read XML and setup internal model according to it */
	int readXml(QDomDocument* source_doc);

	/** Convert object representation to string */
	QString toString();

	/** Read object representation from string */
	int fromString(const QString &source_string);

public:
	/**
	 * Overridden method that serializes statistics element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes statistics element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** Append class-specific properties */
	void appendClassProperties(QDomElement* parent_element);

	/** Parse class-specific properties */
	int parseClassProperties(QDomElement* parent_element);

	/** Attach standard ?xml header to the document */
	void attachXmlHeader(QDomDocument* parent_doc);

	/** Check source doc */
	int checkSourceDoc(QDomDocument* parent_doc);

	/** Append CPUs statistics */
	void appendCpusStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse CPUs statistics */
	int parseCpusStatistics(QDomElement* parent_element);

	/** Append disks statistics */
	void appendDisksStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse disks statistics */
	int parseDisksStatistics(QDomElement* parent_element);

	/** Append RAM statistics */
	void appendMemoryStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse RAM statistics */
	int parseMemoryStatistics(QDomElement* parent_element);

	/** Append swap statistics */
	void appendSwapStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse swap statistics */
	int parseSwapStatistics(QDomElement* parent_element);

	/** Append uptime statistics */
	void appendUptimeStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse uptime statistics */
	int parseUptimeStatistics(QDomElement* parent_element);

	/** Append processes statistics */
	void appendProcInfoStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse processes statistics */
	int parseProcInfoStatistics(QDomElement* parent_element);

	/** Append network interfaces statistics */
	void appendNetIfacesStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse network interfaces statistics */
	int parseNetIfacesStatistics(QDomElement* parent_element);

	/** Append user sessions statistics */
	void appendUsersStatistics(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse user sessions statistics */
	int parseUsersStatistics(QDomElement* parent_element);

	/** Append VM data statistic */
	void appendVmDataStatistic(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse VM data statistic */
	int parseVmDataStatistic(QDomElement* parent_element);

	/** Append guest OS info */
	void appendGuestOsInformation(QDomDocument* parent_doc, QDomElement* parent_element);

	/** Parse guest OS info */
	int parseGuestOsInformation(QDomElement* parent_element);

protected:
	/** Create class document */
	QDomDocument createClassElement();

	/** Clean up class-specific properties */
	void cleanupClassProperties();

private:
	/** Pointer to memory statistics object */
	CMemoryStatistics *m_pMemoryStatistics;
	/** Pointer to swap statistics object */
	CSwapStatistics *m_pSwapStatistics;
	/** Pointer to uptime statistics object */
	CUptimeStatistics *m_pUptimeStatistics;

	CNetClassStatistics m_NetClassStatistics;
};

#endif // __CSystemStatistics_H__
