/*
 * PrlHandleProblemReport.h
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


#ifndef __PARALLELS_HANDLE_PROBLEM_REPORT_H__
#define __PARALLELS_HANDLE_PROBLEM_REPORT_H__

#include "PrlHandleBase.h"
#include "PrlHandleProblemReportBase.h"

/**
 * Handle which wrapping problem report object.
 */
class PrlHandleProblemReport : public PrlHandleProblemReportBase
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * @param pointer to the object for initialization.
	 */
	PrlHandleProblemReport(const SmartPtr<CProblemReport> &pReportData);

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

	/**
	 * Returns problem report binary representation
	 */
	QByteArray GetBinaryData(PRL_RESULT *pError = NULL);

public:
	/**
	 * Instantiates and returns report data for specified scheme
	 * @param problem report scheme
	 */
	static SmartPtr<CProblemReport> CreateReportData(PRL_PROBLEM_REPORT_SCHEME nReportScheme);

public://API helpers
    /**
     * Returns problem report scheme
     * @param pointer to buffer for storing result
     *
     * @return PRL_RESULT. Possible values:
     * PRL_ERR_SUCCESS - operation completed successfully
     */
    virtual PRL_RESULT GetScheme(PRL_PROBLEM_REPORT_SCHEME_PTR pnReportScheme);

	/**
	 * Returns problem report archive file name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully string value)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetArchiveFileName(PRL_STR sArchiveFileName, PRL_UINT32_PTR pnBufLength);

	/**
	 * Returns problem report binary data
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetData(PRL_VOID_PTR pBuffer, PRL_UINT32_PTR pnBufLength);

	/**
	 * Assemblies problem report correspond with specified actions at parameter nFlags.
	 * Supported actions:
	 *		- PPRF_ADD_CLIENT_PART - add client part to problem report
	 *		- PPRF_ADD_SERVER_PART - add server part to problem report
	 */
	PRL_HANDLE Assembly(PRL_UINT32 nFlags);

	/**
	 * Create an archive with all collected data.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - report object is not initialized or cannot be packed
	 * * PRL_ERR_FAILURE - failed to pack report archive
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT Pack() const;
};

typedef PrlHandleSmartPtr<PrlHandleProblemReport> PrlHandleProblemReportPtr;

#endif // __PARALLELS_HANDLE_PROBLEM_REPORT_H__

