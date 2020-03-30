/*
 * PrlHandleProblemReportBase.h: Divided from
 * PrlHandleProblemReport.h in order to cut old xml-based
 * functionality of problem reporting from newer "packed" reporting.
 * Basic necessity of this file is that it can be used without libs
 * such as prl_problem_report_utilities.
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


#ifndef __VIRTUOZZO_HANDLE_PROBLEM_REPORT_BASE_H__
#define __VIRTUOZZO_HANDLE_PROBLEM_REPORT_BASE_H__

#include "PrlHandleBase.h"

class CProblemReport;

/**
 * Handle which wrapping problem report object.
 */
class PrlHandleProblemReportBase : public PrlHandleBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * @param pointer to the object for initialization.
	 */
    PrlHandleProblemReportBase(const SmartPtr<CProblemReport> &pReportData);

	/** Class destructor */
    ~PrlHandleProblemReportBase();

	/**
	 * Returns report scheme
	 */
    virtual PRL_PROBLEM_REPORT_SCHEME GetScheme();

	/**
	 * Returns problem report string representation
	 */
    virtual QString toString();

	/**
	 * Initializes problem report object from XML string representation
	 */
    virtual PRL_RESULT fromString(PRL_CONST_STR sXml);

public:
	/**
	 * Instantiates and returns report data for specified scheme
	 * @param problem report scheme
	 */
    static SmartPtr<CProblemReport> CreateReportData();

public://API helpers
	/**
	 * Returns problem report scheme
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT GetScheme(PRL_PROBLEM_REPORT_SCHEME_PTR pnReportScheme);

	/**
	 * Sets problem report type
	 * @param setting value
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT SetType(PRL_PROBLEM_REPORT_TYPE nReportType);

	/**
	 * Returns problem report type
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT GetPRType(PRL_PROBLEM_REPORT_TYPE_PTR pnReportType);

	/**
	 * Sets problem report reason
	 * @param setting value
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT SetReason(PRL_PROBLEM_REPORT_REASON nReportReason);

	/**
	 * Returns problem report reason
	 * @param pointer to buffer for storing result
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT GetReason(PRL_PROBLEM_REPORT_REASON_PTR pnReportReason);

	/**
	 * Returns problem report user name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT GetUserName(PRL_STR sUserName, PRL_UINT32_PTR pnUserNameBufLength);

	/**
	 * Sets problem report user name
	 * @param setting value
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT SetUserName(PRL_CONST_STR sNewUserName);

	/**
	 * Returns problem report user E-mail
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT GetUserEmail(PRL_STR sUserEmail, PRL_UINT32_PTR pnUserEmailBufLength);

	/**
	 * Sets problem report user E-mail
	 * @param setting value
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT SetUserEmail(PRL_CONST_STR sNewUserEmail);

	/**
	 * Returns problem report descriprion
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT GetDescription(PRL_STR sDescription, PRL_UINT32_PTR pnDescriptionBufLength);

	/**
	 * Sets problem report description
	 * @param setting value
	 *
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT SetDescription(PRL_CONST_STR sNewDescription);

    /**
     * Sets problem report's combined id
     * @param setting value
     *
     * @return PRL_RESULT. Possible values:
     * PRL_ERR_SUCCESS - operation completed successfully
     */
    PRL_RESULT SetCombinedReportId(PRL_CONST_STR sCombinedReportId);

    /**
     * Returns problem report's combined id
     * @param [out] buffer for storing the result
     * @param [in out] buffer size (on in - how much symbols can be placed into buffer;
     * on out - how much were placed actually)
     * @return PRL_RESULT. Possible values:
     * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
     * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
     * * non fully string value)
     * * PRL_ERR_SUCCESS - operation completed successfully
     */
    PRL_RESULT GetCombinedReportId(PRL_STR sCombinedReportId, PRL_UINT32_PTR pnCombinedReportIdLength);

    /**
     * Sets rating
     */
    PRL_RESULT SetRating(PRL_UINT32 nRating);

    /**
     * Returns rating
     */
    PRL_RESULT GetRating(PRL_UINT32_PTR pnRating);

	/**
	 * Returns problem report XML based string representation
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
    PRL_RESULT AsString(PRL_STR sBuffer, PRL_UINT32_PTR pnBufLength);

protected:
	/** Pointer to the wrapping problem report object */
	SmartPtr<CProblemReport> m_pProblemReport;
};

typedef PrlHandleSmartPtr<PrlHandleProblemReportBase> PrlHandleProblemReportBasePtr;

#endif // __VIRTUOZZO_HANDLE_PROBLEM_REPORT_BASE_H__

