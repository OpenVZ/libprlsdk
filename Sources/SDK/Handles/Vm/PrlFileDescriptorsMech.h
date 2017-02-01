/*
 * PrlFileDescriptorsMech.h
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


#ifndef __PARALLELS_FILE_DESCRIPTORS_MECH_H__
#define __PARALLELS_FILE_DESCRIPTORS_MECH_H__

#include "PrlRunProgramInGuestJob.h"

/**
 * File descriptors work mode of PrlVmGuest_RunProgram SDK call
 * mechanism implementation.
 */
namespace PrlFileDescriptorsMech
{
///////////////////////////////////////////////////////////////////////////////
// struct Attached

struct Attached: PrlThread
{
public slots:
	virtual void reactQuit() = 0;

private:
	Q_OBJECT
};

	/**
	 * Registries job at file descriptors mech.
	 * @param pointer to the registering job instance
	 */
	void RegisterJob(PrlRunProgramInGuestJob *pJob);
	/**
	 * Unregistries job from file descriptors mech.
	 * @param pointer to the registering job instance
	 */
	void UnregisterJob(PrlRunProgramInGuestJob *pJob);
	/**
	 * Marks correspond job entry at stdin process mechanism
	 * that remote side ready to accept data
	 * @param identifier of parent request package
	 */
	void MarkReadyToAcceptStdin( const QString &sRequestId );
	/**
	 * Processes received from remote side stdout/stderr portion
	 * @param pointer to the received package
	 * @return sign whether package was properly processed
	 */
	bool ProcessStdoutStderrEvent( const SmartPtr<IOPackage> &p );
	/**
	 * Processes finalize stdin/stdout/stderr interoperation package
	 * @param pointer on the received package
	 */
	void ProcessFinPackage( const SmartPtr<IOPackage> &p );
	/**
	 * Processes tty settings request package
	 * @param pointer on the received package
	 */
	void ProcessTtyPackage( const SmartPtr<IOPackage> &p );
}//namespace PrlFileDescriptorsMech

using namespace PrlFileDescriptorsMech;

#endif

