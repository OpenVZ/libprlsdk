/*
 * PrlApiDeprMacro.h: This file used in PrlApiMacro.h and sould not
 * use directly It defines depricated methods list NOTE: This file
 * is just a list of names. No static/dymamic type info declated
 * here at all.
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

#ifndef PRL_SDK_WRAP_FOR_EACH_DEPRECATED

#define PRL_SDK_WRAP_FOR_EACH_DEPRECATED() \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_FromString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_ToString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_PortForwardingFromString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_PortForwardingToString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlLic_FromString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlLic_ToString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlLic_Create ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlLoginResponse_ToString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlResult_GetError ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlResult_GetParamToken ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_FromString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_ToString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlUsrCfg_AddProxy ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmDev_FromString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmDev_ToString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmDev_UpdateInfo ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlUsrCfg_FromString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlUsrCfg_ToString )\
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlResult_FromString ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_SetCpuVtxEnabled ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_StartConvertHdd ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_CreateAnswerEvent ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_SmcGetRuntimeInfo ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlUsr_GetClientConfig ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_PrepareForHibernate ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_AfterHostResume ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_Remove ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_IsEnabled ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetEnabled ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_IsHidden ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetHidden ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_IsDhcpEnabled ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetDhcpEnabled ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetNetworkType ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetNetworkType ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetName ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetName ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetUuid ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetSysName ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetSysName ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetIndex ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetIndex ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetDhcpScopeStartIp ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetDhcpScopeStartIp ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetDhcpScopeEndIp ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetDhcpScopeEndIp ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_GetDhcpScopeMask ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispNet_SetDhcpScopeMask ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_AddNetAdapter ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_DeleteNetAdapter ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_UpdateNetAdapter ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_CreateDispNet ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_GetDispNetCount ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_GetDispNet ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlApi_SetLogPath ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetDevsList ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_StoreValueByKey ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_StoreValueByKey ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_SendClientStatistics ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_UpdateUsbDevicesAssociationsList ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_RunCompressor ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_CancelCompressor ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_InternalCommand ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_RestartNetworkShaping ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_UpdateNetworkShapingList ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_GetNetworkShapingList ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_SetDefaultGateway) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_SetDefaultGatewayIPv6) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetDefaultGateway) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetDefaultGatewayIPv6) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmDevHd_GetBlockSize ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmDevHd_SetBlockSize ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_Is3DAccelerationEnabled ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_Set3DAccelerationEnabled ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_IsClusterMode ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_SetClusterMode ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_StartClusterService ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_StopClusterService ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_SetHostId ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlDispCfg_GetHostId ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVm_SendProblemReport ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetEnvId) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetHostMemQuotaMin ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_SetHostMemQuotaMin ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetHostMemQuotaMax ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_SetHostMemQuotaMax ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetHostMemQuotaPriority ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_SetHostMemQuotaPriority ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_IsHostMemAutoQuota ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_SetHostMemAutoQuota ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_GetMaxBalloonSize ) \
PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlVmCfg_SetMaxBalloonSize )
#endif // PRL_SDK_WRAP_FOR_EACH_DEPRECATED
