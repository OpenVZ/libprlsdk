/*
 * main.cpp: That is to parse xml files and to fill  proper config
 * classes.
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


#include <stdio.h>

#include <QTextStream>
#include <QFile>
#include <QDateTime>

#include <CVmConfiguration.h>
#include <CVmDirectories.h>
#include <CDispatcherConfig.h>
#include <CProblemReport.h>
#include <CVmEvent.h>
#include <CVmEventParameter.h>
#include <CHostHardwareInfo.h>
#include <CHwFileSystemInfo.h>
#include <CClientPreferences.h>
#include <CUpdaterConfig.h>
#include <CUpdates.h>
#include <CVaConfig.h>

static QTextStream qerr(stderr, QIODevice::WriteOnly);

/**
 * Shows how to use this test.
 */
static void usage()
{
    qerr << "Usage: parsexml -VmConfig <infile.xml>\n";
    qerr << "Usage: parsexml -DispConfig <infile.xml>\n";
    qerr << "Usage: parsexml -ProbRep <infile.xml>\n";
    qerr << "Usage: parsexml -VmDirs <infile.xml>\n";
    qerr << "Usage: parsexml -VmEvent <infile.xml>\n";
    qerr << "Usage: parsexml -HostHwInfo <infile.xml>\n";
    qerr << "Usage: parsexml -HostFsInfo <infile.xml>\n";
    qerr << "Usage: parsexml -ClientPrefs <infile.xml>\n";
    qerr << "Usage: parsexml -UpdConfig <infile.xml>\n";
    qerr << "Usage: parsexml -Updates <infile.xml>\n";
	qerr << "Usage: parsexml -VaConfig <infile.xml>\n";
   exit(1);
}


/**
 * Parses a XML file and fills a CVmConfiguration class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseVmConfig(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CVmConfiguration* pVmCfg = new CVmConfiguration();
	iResult = pVmCfg->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		iResult = pVmCfg->saveToFile(&OutFile);
	}
	delete pVmCfg;

	return iResult;
}

/**
 * Parses a XML file and fills a CDispatcherConfig class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseDispConfig(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CDispatcherConfig* pDispCfg = new CDispatcherConfig();
	iResult = pDispCfg->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		iResult = pDispCfg->saveToFile( qsOut );
	}
	delete pDispCfg;

	return iResult;
}

/**
 * Parses a XML file and fills a CDispatcherConfig class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseClientPreferences(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CClientPreferences* pPrefs = new CClientPreferences();
	iResult = pPrefs->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		iResult = pPrefs->saveToFile(&OutFile);
	}
	delete pPrefs;

	return iResult;
}

/**
 * Parses a XML file and fills a CProblemReport class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseProblemReport(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CProblemReport* pProbRep = new CProblemReport();
	iResult = pProbRep->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		iResult = pProbRep->saveToFile(&OutFile);
	}
	delete pProbRep;

	return iResult;
}

/**
 * Parses a XML file and fills a CVmDirectory class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseVmDirs(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CVmDirectories* pVmDirs = new CVmDirectories();
	iResult = pVmDirs->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		iResult = pVmDirs->saveToFile(&OutFile);
	}
	delete pVmDirs;

	return iResult;
}

/**
 * Parses a XML file and fills a CVmEvent class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseVmEvent(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	QString qsXmlIn;
	if (InFile.open(QIODevice::ReadOnly))
	{
		qsXmlIn = InFile.readAll();
		InFile.close();
	}

	CVmEvent* pEvn = new CVmEvent();
	QString errorMsg = "";
	int errorLine = 0;
	int errorColumn = 0;
	iResult = pEvn->fromString(qsXmlIn, &errorMsg, &errorLine, &errorColumn);

	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsXmlOut = pEvn->toString();
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		if (OutFile.open(QIODevice::WriteOnly))
		{
			OutFile.write(qsXmlOut.toUtf8(), qsXmlOut.size());
			OutFile.close();
		}
	}
	delete pEvn;

	return iResult;
}

/**
 * Parses a XML file and fills a CHwFileSystemInfo class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseHostFileSystemInfo(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	QString qsXmlIn;
	if (InFile.open(QIODevice::ReadOnly))
	{
		qsXmlIn = InFile.readAll();
		InFile.close();
	}

	CHwFileSystemInfo* pFsInfo = new CHwFileSystemInfo();
	iResult = pFsInfo->fromString(qsXmlIn);

	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsXmlOut = pFsInfo->toString();
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		if (OutFile.open(QIODevice::WriteOnly))
		{
			OutFile.write(qsXmlOut.toUtf8(), qsXmlOut.size());
			OutFile.close();
		}
	}
	delete pFsInfo;

	return iResult;
}

/**
 * Parses a XML file and fills a CHostHardwareInfo class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseHostHwInfo(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	QString qsXmlIn;
	if (InFile.open(QIODevice::ReadOnly))
	{
		qsXmlIn = InFile.readAll();
		InFile.close();
	}

	CHostHardwareInfo* pHostHwInfo = new CHostHardwareInfo();
	iResult = pHostHwInfo->fromString(qsXmlIn);

	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsXmlOut = pHostHwInfo->toString();
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		if (OutFile.open(QIODevice::WriteOnly))
		{
			OutFile.write(qsXmlOut.toUtf8(), qsXmlOut.size());
			OutFile.close();
		}
	}
	delete pHostHwInfo;

	return iResult;
}

/**
 * Parses a XML file and fills a CUpdaterConfig class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseUpdaterConfig(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CUpdaterConfig* pUpdCfg = new CUpdaterConfig();
	iResult = pUpdCfg->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		iResult = pUpdCfg->saveToFile(&OutFile);
	}
	delete pUpdCfg;

	return iResult;
}

/**
 * Parses a XML file and fills a CUpdates class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseUpdates(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CUpdates* pUpds = new CUpdates();
	iResult = pUpds->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		iResult = pUpds->saveToFile(&OutFile);
	}
	delete pUpds;

	return iResult;
}

/**
 * Parses a XML file and fills a CVaConfig class.
 * @param qsFileName A name of input XML file.
 * @return Error code.
 */
int ParseVaConfig(const QString& qsFileName)
{
	int iResult = 0;

	QFile InFile(qsFileName);
	CVaConfig* pCfg = new CVaConfig();
	iResult = pCfg->loadFromFile(&InFile);
	if (iResult == PRL_ERR_SUCCESS)
	{
		QString qsOut = "Out_" + QFileInfo(qsFileName).fileName();
		QFile OutFile(qsOut);
		iResult = pCfg->saveToFile(&OutFile);
	}
	delete pCfg;

	return iResult;
}

/**
 * Main function.
 */
int main(int argc, const char **argv)
{
    if (argc < 2)
        usage();

	QString qsFileName;
	bool bVmConfig = false;
	bool bDispConfig = false;
	bool bProbRep = false;
	bool bVmDirs = false;
	bool bVmEvent = false;
	bool bHostHwInfo = false;
	bool bHostFsInfo = false;
	bool bClientPrefs = false;
	bool bUpdConfig = false;
	bool bUpdates = false;
	bool bVaConfig = false;

 	for (int i = 1; i < argc; ++i)
	{
        QString arg = argv[i];
        if (arg == "-VmConfig")
            bVmConfig = true;
        else if (arg == "-DispConfig")
            bDispConfig = true;
        else if (arg == "-ProbRep")
            bProbRep = true;
        else if (arg == "-VmDirs")
            bVmDirs = true;
        else if (arg == "-VmEvent")
            bVmEvent = true;
        else if (arg == "-HostHwInfo")
            bHostHwInfo = true;
        else if (arg == "-HostFsInfo")
            bHostFsInfo = true;
        else if (arg == "-ClientPrefs")
			bClientPrefs = true;
        else if (arg == "-UpdConfig")
			bUpdConfig = true;
        else if (arg == "-Updates")
			bUpdates = true;
		else if (arg == "-VaConfig")
			bVaConfig = true;
     else if (qsFileName.isNull())
            qsFileName = arg;
        else
            usage();
    }

	if (qsFileName.isEmpty())
        usage();

    int iResult = 0;

	if (bVmConfig)
		iResult = ParseVmConfig(qsFileName);
	else if (bDispConfig)
		iResult = ParseDispConfig(qsFileName);
	else if (bProbRep)
		iResult = ParseProblemReport(qsFileName);
	else if (bVmDirs)
		iResult = ParseVmDirs(qsFileName);
	else if (bVmEvent)
		iResult = ParseVmEvent(qsFileName);
	else if (bHostHwInfo)
		iResult = ParseHostHwInfo(qsFileName);
	else if (bHostFsInfo)
		iResult = ParseHostFileSystemInfo(qsFileName);
	else if (bClientPrefs)
		iResult = ParseClientPreferences(qsFileName);
	else if (bUpdConfig)
		iResult = ParseUpdaterConfig(qsFileName);
	else if (bUpdates)
		iResult = ParseUpdates(qsFileName);
	else if (bVaConfig)
		iResult = ParseVaConfig(qsFileName);
	else
		usage();

	qerr << "iResult = " << iResult << "\n";

	return iResult;
};
