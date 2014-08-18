/*
 * PrlRunProgramInGuestJob.h
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


#ifndef __PARALLELS_CUSTOM_JOBS_H__
#define __PARALLELS_CUSTOM_JOBS_H__

#include "PrlHandleServerJob.h"

#ifndef _WIN_
#include <termios.h>
#include <unistd.h>
#endif

/**
 * Job handle object.
 */
class PrlRunProgramInGuestJob : public PrlHandleServerJob
{

public:

	/**
	 * Class constructor.
	 *
	 * @param pointer to the server object where action takes place
	 * @param job unique identifier
	 * @param file descriptor to the standard input
	 * @param file descriptor to the standard output
	 * @param file descriptor to the standard errors output
	 */
	PrlRunProgramInGuestJob(
		const PrlHandleServerPtr &server,
		const QString& job_uuid,
		PRL_FILE_DESC nStdin,
		PRL_FILE_DESC nStdout,
		PRL_FILE_DESC nStderr
		);
	/**
	 * Class destructor
	 */
	~PrlRunProgramInGuestJob();
	/**
	 * Returns standard input file descriptor
	 */
	PRL_FILE_DESC GetStdin() const;
	/**
	 * Returns standard output file descriptor
	 */
	PRL_FILE_DESC GetStdout() const;
	/**
	 * Returns standard errors output file descriptor
	 */
	PRL_FILE_DESC GetStderr() const;
	/**
	 * Overridden method
	 * Sets to job object with specified uuid command execution result
	 * @param result   - applying result
	 */
	void SetCResult( const CResult& result );
	/**
	 * Overridden method
	 * Lets to set job return code
	 * @param setting value
	 */
	void SetReturnCode(PRL_RESULT nRetCode);

	/**
	 * Change terminal settings for VM enter on/off
	 * @param setting value
	 */
	void MakeTerminalRaw(PRL_BOOL bOn);

private:
	/** File descriptor to the standard input */
	PRL_FILE_DESC m_nStdin;
	/** File descriptor to the standard output */
	PRL_FILE_DESC m_nStdout;
	/** File descriptor to the standard errors output */
	PRL_FILE_DESC m_nStderr;
	/** Do we need to restore tty settings */
	PRL_BOOL m_bIsTtyChanged;
#ifndef _WIN_
	/** Original tty settings */
	struct termios m_Tios;
#endif
};

#endif // __PARALLELS_CUSTOM_JOBS_H__

