/*
 * PrlHandleProblemReportBase.cpp: Divided from
 * PrlHandleProblemReportBase.h in order to cut old xml-based
 * functionality of problem reporting from newer "packed" reporting.
 * Basic necessity of this file is that it can be used without libs
 * such as prl_problem_report_utilities.
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#include <prlxmlmodel/ProblemReport/CProblemReport.h>
#include "PrlHandleProblemReportBase.h"
#include "PrlHandleLocalJob.h"

SmartPtr<CProblemReport> PrlHandleProblemReportBase::CreateReportData()
{
    return SmartPtr<CProblemReport>(new CProblemReport);
}

PrlHandleProblemReportBase::PrlHandleProblemReportBase(const SmartPtr<CProblemReport> &pProblemReport)
: PrlHandleBase(PHT_PROBLEM_REPORT), m_pProblemReport(pProblemReport)
{
}

PrlHandleProblemReportBase::~PrlHandleProblemReportBase()
{
}

PRL_RESULT PrlHandleProblemReportBase::GetScheme(PRL_PROBLEM_REPORT_SCHEME_PTR pnReportScheme)
{
	*pnReportScheme = GetScheme();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::SetType(PRL_PROBLEM_REPORT_TYPE nReportType)
{
	if (quint32(nReportType) > PRT_LAST)
		return (PRL_ERR_INVALID_ARG);
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pProblemReport->setReportType(nReportType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::GetPRType(PRL_PROBLEM_REPORT_TYPE_PTR pnReportType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnReportType = m_pProblemReport->getReportType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::SetReason(PRL_PROBLEM_REPORT_REASON nReportReason)
{
	if (quint32(nReportReason) > PRR_LAST)
		return (PRL_ERR_INVALID_ARG);
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pProblemReport->setReportReason(nReportReason);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::GetReason(PRL_PROBLEM_REPORT_REASON_PTR pnReportReason)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnReportReason = m_pProblemReport->getReportReason();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::GetUserName(PRL_STR sUserName, PRL_UINT32_PTR pnUserNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_pProblemReport->getContactInfo()->getName(), sUserName, pnUserNameBufLength);
}

PRL_RESULT PrlHandleProblemReportBase::SetUserName(PRL_CONST_STR sNewUserName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pProblemReport->getContactInfo()->setName(sNewUserName);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::GetUserEmail(PRL_STR sUserEmail, PRL_UINT32_PTR pnUserEmailBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_pProblemReport->getContactInfo()->getEMail(), sUserEmail, pnUserEmailBufLength);
}

PRL_RESULT PrlHandleProblemReportBase::SetUserEmail(PRL_CONST_STR sNewUserEmail)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pProblemReport->getContactInfo()->setEMail(sNewUserEmail);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::GetDescription(PRL_STR sDescription, PRL_UINT32_PTR pnDescriptionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_pProblemReport->getUserDefinedData()->getProblemDescription(), sDescription, pnDescriptionBufLength);
}

PRL_RESULT PrlHandleProblemReportBase::SetDescription(PRL_CONST_STR sNewDescription)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pProblemReport->getUserDefinedData()->setProblemDescription(sNewDescription);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::SetCombinedReportId(PRL_CONST_STR sCombinedReportId)
{
    SYNCHRO_INTERNAL_DATA_ACCESS
    m_pProblemReport->getClientInfo()->setCombinedReportId(sCombinedReportId);
    return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleProblemReportBase::GetCombinedReportId(PRL_STR sCombinedReportId,
                                                           PRL_UINT32_PTR pnCombinedReportIdLength)
{
    SYNCHRO_INTERNAL_DATA_ACCESS
    return CopyStringValue(m_pProblemReport->getClientInfo()->getCombinedReportId(),
                           sCombinedReportId, pnCombinedReportIdLength);
}

PRL_RESULT PrlHandleProblemReportBase::SetRating(PRL_UINT32 nRating)
{
    SYNCHRO_INTERNAL_DATA_ACCESS
    m_pProblemReport->getClientInfo()->setRating(nRating);
    return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleProblemReportBase::GetRating(PRL_UINT32_PTR pnRating)
{
    SYNCHRO_INTERNAL_DATA_ACCESS
	*pnRating = m_pProblemReport->getClientInfo()->getRating();
    return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleProblemReportBase::AsString(PRL_STR sBuffer, PRL_UINT32_PTR pnBufLength)
{
	return (CopyStringValue(toString(), sBuffer, pnBufLength));
}

PRL_PROBLEM_REPORT_SCHEME PrlHandleProblemReportBase::GetScheme()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
    return PRS_OLD_XML_BASED;
}

QString PrlHandleProblemReportBase::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
    return m_pProblemReport->toString();
}

PRL_RESULT PrlHandleProblemReportBase::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
    return (m_pProblemReport->fromString(UTF8_2QSTR(sXml)));
}
