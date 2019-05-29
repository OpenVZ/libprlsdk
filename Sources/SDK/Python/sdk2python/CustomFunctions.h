/*
 * Copyright (c) 2016-2017, Parallels International GmbH
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

#ifndef CUSTOM_FUNCTIONS_H
#define CUSTOM_FUNCTIONS_H

#include <QStringList>
#define CUSTOM_FUNCS_CPP "sdk2python/CustomFunctions.cpp"

inline QStringList getCustomFunctions() {
	QStringList nonStdFuncs;
	nonStdFuncs.append("PrlOpTypeList_GetItem");
	nonStdFuncs.append("PrlReport_GetData");
	nonStdFuncs.append("PrlReport_Send");
	nonStdFuncs.append("PrlApi_SendProblemReport");
	nonStdFuncs.append("PrlApi_SendPackedProblemReport");
	nonStdFuncs.append("PrlSrv_GetDefaultVmConfig");
	nonStdFuncs.append("PrlVmCfg_GetMemGuaranteeSize");
	nonStdFuncs.append("PrlVmCfg_SetMemGuaranteeSize");
	nonStdFuncs.append("PrlDiskMap_Read");
	nonStdFuncs.append("PrlHandle_RegEventHandler");
	nonStdFuncs.append("PrlHandle_UnregEventHandler");
	nonStdFuncs.append("PrlSrv_RegEventHandler");
	nonStdFuncs.append("PrlSrv_UnregEventHandler");
	nonStdFuncs.append("PrlVm_RegEventHandler");
	nonStdFuncs.append("PrlVm_UnregEventHandler");
	nonStdFuncs.append("PrlOpTypeList_GetItem");
	nonStdFuncs.append("PrlApi_Init");
	nonStdFuncs.append("PrlApi_InitEx");
	nonStdFuncs.append("PrlApi_Deinit");
	nonStdFuncs.append("PrlVmCfg_GetIoLimit");
	nonStdFuncs.append("PrlVmCfg_SetIoLimit");
	nonStdFuncs.append("PrlVmCfg_GetCpuLimitEx");
	nonStdFuncs.append("PrlVmCfg_SetCpuLimitEx");
	return nonStdFuncs;
}

#endif //CUSTOM_FUNCTION_H
