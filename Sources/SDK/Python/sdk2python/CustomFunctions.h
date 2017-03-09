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
