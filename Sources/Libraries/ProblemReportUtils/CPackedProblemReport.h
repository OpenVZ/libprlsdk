/*
 * CPackedProblemReport.h: wrapper on problem report archived model
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


#ifndef CPACKED_PROBLEMREPORT_H
#define CPACKED_PROBLEMREPORT_H

#include <QString>

#include <prlcommon/Std/SmartPtr.h>
#include <prlxmlmodel/ProblemReport/CProblemReport.h>
#include "SDK/Include/PrlErrors.h"

class QByteArray;

#define PRL_PACKED_REPORT_SUPPORTED

#define PR_PACKED_REP_MAIN_XML                      "Report.xml"
#define PR_PACKED_REP_HOST_STAT                     "HostStatistics.xml"
#define PR_PACKED_REP_HOST_INFO                     "HostInfo.xml"
#define PR_PACKED_REP_MORE_HOST_INFO                "MoreHostInfo.xml"
#define PR_PACKED_REP_ALL_PROCESSES                 "AllProcesses.txt"
#define PR_PACKED_REP_ALL_PROCESSES_SAMPLES			"AllProcessesSamples.txt"
#define PR_PACKED_REP_ALL_LOADED_DRIVERS						"AllLoadedDrivers.txt"
#define PR_PACKED_REP_CLIENT_PROXY_INFO         "ClientProxyInfo.txt"
#define PR_PACKED_REP_APP_CONFIG					"AppConfig.xml"
#define PR_PACKED_REP_NET_CONFIG					"NetConfig.xml"
#define PR_PACKED_REP_CURRENT_VM					"CurrentVm.xml"
#define PR_PACKED_REP_CURRENT_VM_DOMAIN					"CurrentVmDomain.xml"
#define PR_PACKED_REP_ADVANCED_VM_INFO				"AdvancedVmInfo.xml"
#define PR_PACKED_REP_MONITOR_DATA					"MonitorData.txt"
#define PR_PACKED_REP_PERFORMANCE_COUNTERS			"PerformanceCounters.txt"
#define PR_PACKED_REP_GUEST_OS						"GuestOs.xml"
#define PR_PACKED_REP_VM_DIRECTORY					"VmDirectory.xml"
#define PR_PACKED_REP_CLIENT_INFO					"ClientInfo.xml"
#define PR_PACKED_REP_INSTALLED_SOFTWARE			"InstalledSoftware.txt"
#define PR_PACKED_REP_APP_SWITCH_PACKAGES			"Packages.xml"
#define PR_PACKED_REP_VZ_REPORT						"VzReport.xml"
#define PR_PACKED_REP_FILES_MD5_IN_PROD_BUNDLE		"FilesMd5InProdBundle.txt"
#define PR_PACKED_REP_LAUNCHD_INFO					"LaunchdInfo.txt"

class CPackedProblemReport : public CProblemReport
{

public:
	enum packedReportSide
	{
		VmSide = 0,
		DispSide = 1,
		ClientSide = 2,
		ReportToolSide = 3,
	};

	/**
	* Creates Instance of packed problem report object
	* @param1 in packedReportSide side - where it creates
	* @param2 out CProblemReport ** ppReport = double pointer to base CProblemReport
	* @param3 int QByteArray data - binary data of archive
	* Creates instance from binary data or create new empty tar archive
	*/
	static PRL_RESULT createInstance( CPackedProblemReport::packedReportSide side,
										CPackedProblemReport ** ppReport,
										const QByteArray & data = QByteArray(),
										const QString & strTempDirPath = QString() );

	/**
	* Creates Instance of packed problem report object
	* @param1 in packedReportSide side - where it creates
	* @param2 out CProblemReport ** ppReport = double pointer to base CProblemReport
	* @param3 int const QString & strPathFromLoad - path from load
	* Creates instance from binary data
	*/
	static PRL_RESULT createInstance( CPackedProblemReport::packedReportSide side,
										CPackedProblemReport ** ppReport,
										const QString & strPathFromLoad );


	CPackedProblemReport( const QString & strPathToSave ,
						const QByteArray & data = QByteArray() );

	CPackedProblemReport( const QString & strPathToSave ,
							const QString & strPathToTempDir );


	~CPackedProblemReport();

	//////////////////////////////////////////////////////////////////////////
	// reimplementation from base CProblemReport
	//////////////////////////////////////////////////////////////////////////
	/*
	* note this function eats too much memory and provided for old schema support
	* all new logs must be added by appendSystemLog( const QString & , int ,const QString &);
	*/
	virtual void appendSystemLog( CRepSystemLog * plog );

	virtual void appendSystemLog( const QString & strPathFrom,
									const QString &strCustomName );

	void appendTemplateSystemLog( const QString& strPathFrom,
							const QString& strCustomName,
							int iLogsCount );

	virtual void appendCrashDump( CRepCrashDump * pDump );
	virtual void appendMemoryDump( CRepMemoryDump * pDump );

	virtual void clearSystemLogs();

	virtual void saveMainXml();
	virtual int loadMainXml();

	virtual QString getTempDirPath() const;
	virtual QString getArchivePath() const;

	virtual void setVzReport(CRepVzReport* pVzReport);

	virtual void setAdvancedVmInfo(CRepAdvancedVmInfo* pAdvancedVmInfo);

	virtual void setClientInfo(ClientInfo* pClientInfo);

	virtual void setHostStatistic(QString value = QString());

	virtual void setHostInfo(QString value = QString());

	virtual void setMoreHostInfo(QString value = QString());

	virtual void setAllProcesses(QString value = QString());

	virtual void setAllProcessesSamples(QString value = QString());

	virtual void setLoadedDrivers(QString value = QString());

	virtual void setClientProxyInfo(QString value = QString());

	virtual void setAppConfig(QString value = QString());

	virtual void setNetConfig(QString value = QString());

	virtual void setMonitorData(QString value = QString());

	virtual void setPerformanceCounters(QString value = QString());

	virtual void setVmConfig(QString value = QString());

	virtual void setVmDomain(QString value = QString());

	virtual void setGuestOs(QString value = QString());

	virtual void setVmDirectory(QString value = QString());

	virtual void setInstalledSoftware(QString value = QString());

	virtual void setFilesMd5InProductBundle(QString value = QString());

	virtual void setLaunchdInfo(QString value = QString());

	PRL_RESULT packReport();

	SmartPtr<CProblemReport> convertToProblemReportOldFormat();

	void setCleanupTempDir(bool bValue = true);

	CPackedProblemReport::packedReportSide getPackedReportSide(){return m_Side;}

	void setPackedReportSide( CPackedProblemReport::packedReportSide side){m_Side = side;}

	int fromBaseReport( const QString & strBaseReport );

	QString getVmConfigFromArchive() const;

	QString getVmDomainFromArchive() const;

	bool isValid() const;

	virtual ClientInfo* getClientInfo() const;

	void setQuitFromPack( bool bSet ){ m_bQuitFromPack = bSet; }

	bool isQuitFromPack(){ return m_bQuitFromPack; }

private:
	static QString getPathToSave( packedReportSide side, const QString & strBaseName = QString() );
	QString getTempStorageDirPath( const QString & strBaseDir );

	PRL_RESULT extractArchive( const QString & strPath );

	bool createNewArchive();

	QStringList createReportFilesList();

	void setValue( const QString & strValue,
					const QString & strNameInArchive ) const;

	QString getValue( const QString & strNameInArchive ) const;

	QString getElementPathFromArchive( const QString & strVaule );

	virtual CRepVzReport* getVzReport() const;

	// to private section - it is no supported for this object
	virtual CRepAdvancedVmInfo* getAdvancedVmInfo() const;

	QString getArchivePathFromTopLevelObject( CBaseNode *,const QString& );

private:

	bool				m_bValid;
	bool				m_bCleanupTempDir;
	QString				m_strArchPath;
	QString				m_strTempDirPath;
	packedReportSide	m_Side;
	volatile bool		m_bQuitFromPack;
};

#endif //CPACKED_PROBLEMREPORT_H
