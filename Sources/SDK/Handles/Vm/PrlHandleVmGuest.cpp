/*
 * PrlHandleVmGuest.cpp
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


#include "PrlHandleVmGuest.h"
#include "Libraries/Std/PrlAssert.h"
#include "PrlHandleStringsList.h"

PrlHandleVmGuest::PrlHandleVmGuest( const PrlHandleVmPtr &pVm, const QString &sVmGuestUuid )
:
PrlHandleBase(PHT_VM_GUEST_SESSION), m_pVm((PrlHandleVmSrv* )pVm.getHandle()), m_sVmGuestUuid(sVmGuestUuid)
{
	PRL_ASSERT(m_pVm.getHandle());
}

PrlHandleJobPtr PrlHandleVmGuest::Logout(PRL_UINT32 nFlags)
{
	PrlHandleJobPtr pJob = m_pVm->VmGuestLogout(m_sVmGuestUuid, nFlags);
	pJob->SetVmHandle(m_pVm->GetHandle());
	return (pJob);
}

PrlHandleJobPtr PrlHandleVmGuest::RunProgram(PRL_CONST_STR sProgramName, PRL_HANDLE hArgsList, PRL_HANDLE hEnvsList,
		PRL_UINT32 nFlags, PRL_FILE_DESC nStdin, PRL_FILE_DESC nStdout, PRL_FILE_DESC nStderr)
{
	QStringList lstArgs, lstEnvs;
	PrlHandleStringsListPtr pArgsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hArgsList);
	if (pArgsList.getHandle())
		lstArgs = pArgsList->GetStringsList();
	PrlHandleStringsListPtr pEnvsList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hEnvsList);
	if (pEnvsList.getHandle())
		lstEnvs = pEnvsList->GetStringsList();
	PrlHandleJobPtr pJob = m_pVm->VmGuestRunProgram(m_sVmGuestUuid, sProgramName, lstArgs, lstEnvs, nFlags,
													nStdin, nStdout, nStderr);
	pJob->SetVmHandle(m_pVm->GetHandle());
	return (pJob);
}

PrlHandleJobPtr PrlHandleVmGuest::GetNetworkSettings(PRL_UINT32 nFlags)
{
	PrlHandleJobPtr pJob = m_pVm->VmGuestGetNetworkSettings(m_sVmGuestUuid, nFlags);
	pJob->SetVmHandle(m_pVm->GetHandle());
	return (pJob);
}

PrlHandleJobPtr PrlHandleVmGuest::SetUserPasswd(PRL_CONST_STR sUserName, PRL_CONST_STR sUserPasswd, PRL_UINT32 nFlags)
{
	PrlHandleJobPtr pJob = m_pVm->VmGuestSetUserPasswd(m_sVmGuestUuid, sUserName, sUserPasswd, nFlags);
	pJob->SetVmHandle(m_pVm->GetHandle());
	return (pJob);
}

