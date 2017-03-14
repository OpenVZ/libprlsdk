/*
 * PrlHandleProblemReport.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#include "Libraries/ProblemReportUtils/CPackedProblemReport.h"
#include "PrlHandleProblemReport.h"
#include "PrlHandleLocalJob.h"
#include "PrlProblemReportAssembler.h"

SmartPtr<CProblemReport> PrlHandleProblemReport::CreateReportData(PRL_PROBLEM_REPORT_SCHEME nReportScheme)
{
	SmartPtr<CProblemReport> pProblemReport;
	switch (nReportScheme)
	{
		case PRS_NEW_PACKED:
		{
			CPackedProblemReport *pReport = NULL;
			PRL_RESULT nResult = CPackedProblemReport::createInstance(CPackedProblemReport::ClientSide, &pReport);
			if (PRL_FAILED(nResult))
				throw nResult;
			pProblemReport = SmartPtr<CProblemReport>(pReport);
		}
		break;

		case PRS_OLD_XML_BASED:
			pProblemReport = SmartPtr<CProblemReport>(new CProblemReport);
		break;

		default:
			WRITE_TRACE(DBG_FATAL, "Wrong report scheme: nReportScheme=%d", nReportScheme);
			throw PRL_ERR_INVALID_ARG;
		break;
	}

	// Set product name according to APP_MODE
	pProblemReport->setProductName(PrlGetApiProductName());
	return (pProblemReport);
}

PrlHandleProblemReport::PrlHandleProblemReport(const SmartPtr<CProblemReport> &pProblemReport)
: PrlHandleProblemReportBase(pProblemReport)
{
	CPackedProblemReport *pReport = dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl());
	if (pReport)
		pReport->setCleanupTempDir(true);
}

PRL_RESULT PrlHandleProblemReport::GetData(PRL_VOID_PTR pBuffer, PRL_UINT32_PTR pnBufLength)
{
	PRL_RESULT nRetCode = PRL_ERR_UNINITIALIZED;
	QByteArray _buffer = GetBinaryData(&nRetCode);
	if (PRL_FAILED(nRetCode))
		return (nRetCode);
	return (CopyByteArrayValue(_buffer, pBuffer, pnBufLength));
}

PRL_HANDLE PrlHandleProblemReport::Assembly(PRL_UINT32 nFlags)
{
	PrlHandleLocalJobPtr pAssemblyReportJob(new PrlHandleLocalJob(Uuid::createUuid().toString(),
													PJOC_REPORT_ASSEMBLY));
	if (!pAssemblyReportJob.getHandle())
		return (GENERATE_ERROR_HANDLE(PRL_ERR_OUT_OF_MEMORY, PJOC_REPORT_ASSEMBLY));
	PrlProblemReportAssembler *pReportAssembler =
		new PrlProblemReportAssembler(pAssemblyReportJob, m_pProblemReport, &m_HandleMutex, nFlags);
	if (!pReportAssembler)
		return (GENERATE_ERROR_HANDLE(PRL_ERR_OUT_OF_MEMORY, PJOC_REPORT_ASSEMBLY));
	pReportAssembler->start();
	return (pAssemblyReportJob->GetHandle());
}

PRL_RESULT PrlHandleProblemReport::Pack() const
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	CPackedProblemReport *pReport = dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl());
	if (!pReport)
		return PRL_ERR_INVALID_ARG;

	return (pReport->packReport());
}

PRL_RESULT PrlHandleProblemReport::GetArchiveFileName(PRL_STR sArchiveFileName, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CPackedProblemReport *pReport = dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl());
	if (!pReport)
		return (PRL_ERR_INVALID_ARG);
	QString strArchiveFileName = QFileInfo(pReport->getArchivePath()).fileName();
	return (CopyStringValue(strArchiveFileName, sArchiveFileName, pnBufLength));
}

PRL_PROBLEM_REPORT_SCHEME PrlHandleProblemReport::GetScheme()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl()) == NULL ? PRS_OLD_XML_BASED : PRS_NEW_PACKED);
}

QString PrlHandleProblemReport::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sReportData;
	CPackedProblemReport *pReport = dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl());
	if (!pReport)
		sReportData = m_pProblemReport->toString();
	else
		sReportData = pReport->convertToProblemReportOldFormat()->toString();
	return (sReportData);
}

PRL_RESULT PrlHandleProblemReport::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CPackedProblemReport *pReport = dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl());
	if (!pReport)
		return (m_pProblemReport->fromString(UTF8_2QSTR(sXml)));
	else
		return (pReport->fromBaseReport(UTF8_2QSTR(sXml)));
}

PRL_RESULT PrlHandleProblemReport::GetScheme(PRL_PROBLEM_REPORT_SCHEME_PTR pnReportScheme)
{
    *pnReportScheme = GetScheme();
    return (PRL_ERR_SUCCESS);
}

QByteArray PrlHandleProblemReport::GetBinaryData(PRL_RESULT *pError)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CPackedProblemReport *pReport = dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl());
	if (!pReport)
	{
		if (pError)
			*pError = PRL_ERR_INVALID_ARG;
		return (QByteArray());
	}
	QFile _file(pReport->getArchivePath());
	if (!_file.open(QIODevice::ReadOnly))
	{
		if (pError)
			*pError = PRL_ERR_NO_DATA;
		return (QByteArray());
	}
	if (pError)
		*pError = PRL_ERR_SUCCESS;
	return (_file.readAll());
}
