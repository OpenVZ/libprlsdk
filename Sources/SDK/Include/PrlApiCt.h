/*
 * Copyright (C) 1999-2017 Parallels IP Holdings GmbH
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


#ifndef __VIRTUOZZO_API_CT_H__
#define __VIRTUOZZO_API_CT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Reinstall Container 
   Recreates Container's root virtual disk, installs all applications
   previously installed from application templates,
   copies credentials from the old container (unless the REINSTALL_RESET_PWDB
   flag is specified), and renames the old root directory '/' to '/old'
   (unless the REINSTALL_SKIP_BACKUP flag is specified). 

   To get the return code from the PHT_JOB object, use the
   PrlJob_GetRetCode function. Possible values are:
   PRL_ERR_INVALID_ARG - invalid handle was passed.
   PRL_ERR_SUCCESS - function completed successfully.
   Parameters
   hVm :          A handle of type PHT_VIRTUAL_MACHINE
                  identifying the virtual machine.
   sOsTemplate :  New OS template name.
   nFlags :       Bitset of flags - from enum PRL_CT_REINSTALL_OPTIONS

   Returns
   A handle of type PHT_JOB containing the results of this
   asynchronous operation or PRL_INVALID_HANDLE if there's not
   enough memory to instantiate the job object.

*/
PRL_ASYNC_SRV_METHOD_DECL(VIRTUOZZO_API_VER_7,
	PrlCt_Reinstall, (
		PRL_HANDLE hVm,
		PRL_CONST_STR sOsTemplate,
		PRL_UINT32 nFlags
	));


#ifdef __cplusplus
}
#endif

#endif // __VIRTUOZZO_API_CT_H_
