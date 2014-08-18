/*
 * PrlRunProgramInGuestJob.cpp
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


#include "PrlRunProgramInGuestJob.h"
#include "PrlFileDescriptorsMech.h"


PrlRunProgramInGuestJob::PrlRunProgramInGuestJob(
	const PrlHandleServerPtr &server,
	const QString& job_uuid,
	PRL_FILE_DESC nStdin,
	PRL_FILE_DESC nStdout,
	PRL_FILE_DESC nStderr
)
:
PrlHandleServerJob(server, job_uuid, PJOC_VM_GUEST_RUN_PROGRAM, true),
m_nStdin(nStdin),
m_nStdout(nStdout),
m_nStderr(nStderr),
m_bIsTtyChanged(PRL_FALSE)
{
	PrlFileDescriptorsMech::RegisterJob(this);
}

PrlRunProgramInGuestJob::~PrlRunProgramInGuestJob()
{
	PrlFileDescriptorsMech::UnregisterJob(this);
	MakeTerminalRaw(PRL_FALSE);
}

PRL_FILE_DESC PrlRunProgramInGuestJob::GetStdin() const
{
	return (m_nStdin);
}

PRL_FILE_DESC PrlRunProgramInGuestJob::GetStdout() const
{
	return (m_nStdout);
}

PRL_FILE_DESC PrlRunProgramInGuestJob::GetStderr() const
{
	return (m_nStderr);
}

void PrlRunProgramInGuestJob::SetCResult( const CResult& result )
{
	PrlFileDescriptorsMech::UnregisterJob(this);
	PrlHandleServerJob::SetCResult(result);
}

void PrlRunProgramInGuestJob::SetReturnCode(PRL_RESULT nRetCode)
{
	PrlFileDescriptorsMech::UnregisterJob(this);
	PrlHandleServerJob::SetReturnCode(nRetCode);
}

void PrlRunProgramInGuestJob::MakeTerminalRaw(PRL_BOOL bOn)
{
	if (bOn == PRL_TRUE) {
#ifndef _WIN_
		struct termios tios;

		if (tcgetattr(0, &tios) == -1)
			return;
		/* store original settings */
		memcpy(&m_Tios, &tios, sizeof(struct termios));
		cfmakeraw(&tios);
		if (tcsetattr(0, TCSADRAIN, &tios) == -1)
			return;
#endif
		m_bIsTtyChanged = PRL_TRUE;
	} else {
		if (m_bIsTtyChanged != PRL_TRUE)
			return;
#ifndef _WIN_
		tcsetattr(0, TCSADRAIN, &m_Tios);
#endif
		m_bIsTtyChanged = PRL_FALSE;
	}
}

