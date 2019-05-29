/*
 * PrlProblemReportAssembler.cpp: Implementation of problem report
 * sender mech
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#include "PrlProblemReportAssembler.h"
#include "Libraries/ProblemReportUtils/CProblemReportUtils.h"
#include <prlcommon/Std/PrlAssert.h>
#include <prlcommon/HostUtils/PrlMiscellaneous.h>
#include <prlxmlmodel/DispConfig/CDispatcherConfig.h>

using namespace PrlMiscellaneous;

namespace {
struct CSimpleReportWrapper
{
	CSimpleReportWrapper(const SmartPtr<CProblemReport> &pReport, QMutex *pMutex)
	: m_lock(pMutex), m_pProblemReport(pReport)
	{}
	CProblemReport &GetReportReference()
	{
		return (*m_pProblemReport.getImpl());
	}

private:
	QMutexLocker m_lock;
	SmartPtr<CProblemReport> m_pProblemReport;
};

}

PrlProblemReportAssembler::PrlProblemReportAssembler(const PrlHandleLocalJobPtr &pAssemblyReportJob,
		SmartPtr<CProblemReport> pProblemReport,
		QMutex *pMutex,
		PRL_UINT32 nFlags)
:
m_pAssemblyReportJob(pAssemblyReportJob), m_pProblemReport(pProblemReport), m_pMutex(pMutex), m_nFlags(nFlags),
m_bWasCancelled(false)
{}

void PrlProblemReportAssembler::concreteRun()
{
	#define CHECK_JOB_WAS_CANCELLED\
		if (m_bWasCancelled)\
			return;

	#define GET_LOCKED_REPORT_REFERENCE\
		CSimpleReportWrapper(m_pProblemReport, m_pMutex).GetReportReference()

	CHECK_CONNECT_DONE_PROPERLY(connect(m_pAssemblyReportJob.getHandle(), SIGNAL(jobWasCanceled()), SLOT(onJobWasCancelled()), Qt::DirectConnection))

	//Fill report with server part info
	if (PPRF_ADD_SERVER_PART & m_nFlags)
	{
		CProblemReportUtils::fillDesktopProblemReportOnDisconnectServer( GET_LOCKED_REPORT_REFERENCE );
	}

	CHECK_JOB_WAS_CANCELLED

	//Fill report with client part info
	if (PPRF_ADD_CLIENT_PART & m_nFlags)
	{
		CProblemReportUtils::addClientInformation( GET_LOCKED_REPORT_REFERENCE, PrlGetApiAppMode() );
		CHECK_JOB_WAS_CANCELLED
	} // if (PPRF_ADD_CLIENT_PART & m_nFlags)

	//Pack report if necessary
	QMutexLocker _lock(m_pMutex);
	PRL_RESULT nRetCode = PRL_ERR_SUCCESS;
	CPackedProblemReport *pReport = dynamic_cast<CPackedProblemReport *>(m_pProblemReport.getImpl());
	if (pReport)
	{
		QFile::remove(pReport->getArchivePath());
		nRetCode = pReport->packReport();
	}
	m_pAssemblyReportJob->SetReturnCode(nRetCode);

	#undef CHECK_JOB_WAS_CANCELLED
	#undef GET_LOCKED_REPORT_REFERENCE
}

void PrlProblemReportAssembler::onJobWasCancelled()
{
	m_bWasCancelled = true;
}
