/*
 * PrlProblemReportAssembler.h: Implementation of problem report
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


#ifndef PrlProblemReportAssembler_H
#define PrlProblemReportAssembler_H

#include "PrlHandleLocalJob.h"
#include "PrlCommon.h"

class CProblemReport;
/**
 * Problem report assembler thread implementation
 */
class PrlProblemReportAssembler : public Heappy
{
Q_OBJECT

public:
	/**
	 * Class constructor
	 * @param pointer to the local job object which should demonstrate problem report assembler execution progress
	 * @param pointer
	 */
	PrlProblemReportAssembler(const PrlHandleLocalJobPtr &pAssemblyReportJob,
			SmartPtr<CProblemReport> pProblemReport,
			QRecursiveMutex *pMutex,
			PRL_UINT32 nFlags);

private:
	/** Overridden thread function */
	void concreteRun();

private slots:
	/** Process job cancelation action */
	void onJobWasCancelled();

private:
	/** Pointer to the assembly problem report job object */
	PrlHandleLocalJobPtr m_pAssemblyReportJob;
	/** Pointer to the processing problem report object */
	SmartPtr<CProblemReport> m_pProblemReport;
	/** Pointer to the synchronization access to problem report object mutex */
	QRecursiveMutex *m_pMutex;
	/** Bitset of flags which contains actions to be applied to problem report object */
	PRL_UINT32 m_nFlags;
	/** Sign which indicates that job was cancelled */
	bool m_bWasCancelled;

};

#endif

