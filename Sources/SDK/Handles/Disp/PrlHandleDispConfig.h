/*
 * PrlHandleDispConfig.h
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


#ifndef __PARALLELS_HANDLE_DISP_CONFIG_H__
#define __PARALLELS_HANDLE_DISP_CONFIG_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/DispConfig/CDispCommonPreferences.h>
#include <prlxmlmodel/NetworkConfig/CParallelsNetworkConfig.h>
#include "PrlHandleCpuFeatures.h"

#define SYNCHRO_PARENT_DISP_CONFIG QMutexLocker _lock(m_pDispCfg->GetSynchroObject());

/**
 * Handle object represents dispatcher common preferences
 */
class PrlHandleDispConfig : public PrlHandleBase
{

public:
	/**
	 * Class default constructor.
	 */
	PrlHandleDispConfig(const QString& sDispConfig, const QString& sNetworkConfig);

	/**
	 * Returns default VM dir
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultVmDir(PRL_STR sDefaultVmDir, PRL_UINT32_PTR pnDefaultVmDirBufLength);

	/**
	 * Returns default Ct dir
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully VM name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultCtDir(PRL_STR sDefaultDir, PRL_UINT32_PTR pnDefaultDirBufLength);

	/**
	 * Returns reserved for VMs memory size in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetReservedMemLimit(PRL_UINT32_PTR pnMemSize);


	/**
	 * Sets reserved for VMs memory size in megabytes
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetReservedMemLimit(PRL_UINT32 nMemSize);

	/**
	 * Returns minimal possible VM memory setting in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMinVmMem(PRL_UINT32_PTR pnMemSize);


	/**
	 * Sets minimal possible VM memory setting in megabytes
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMinVmMem(PRL_UINT32 nMemSize);

	/**
	 * Returns maximum possible VM memory setting in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMaxVmMem(PRL_UINT32_PTR pnMemSize);


	/**
	 * Sets maximum possible VM memory setting in megabytes
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMaxVmMem(PRL_UINT32 nMemSize);

	/**
	 * Returns recommended maximum VM memory setting in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRecommendMaxVmMem(PRL_UINT32_PTR pnMemSize);


	/**
	 * Sets recommended maximum VM memory setting in megabytes
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetRecommendMaxVmMem(PRL_UINT32 nMemSize);

	/**
	 * Returns maximum possible reserv memory for VMs size in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMaxReservMemLimit(PRL_UINT32_PTR pnMemSize);


	/**
	 * Sets maximum possible reserv memory for VMs size in megabytes
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMaxReservMemLimit(PRL_UINT32 nMemSize);

	/**
	 * Returns minimum possible reserv memory for VMs size in megabytes
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMinReservMemLimit(PRL_UINT32_PTR pnMemSize);


	/**
	 * Sets minimum possible reserv memory for VMs size in megabytes
	 * @param setting value (in megabytes)
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMinReservMemLimit(PRL_UINT32 nMemSize);

	/**
	 * Returns sign whether system must auto adjusting VMs memory size
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsAdjustMemAuto(PRL_BOOL_PTR pbAdjustMemAuto);

	/**
	 * Sets sign whether system must auto adjusting VMs memory size
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAdjustMemAuto(PRL_BOOL bAdjustMemAuto);

	/**
	 * Returns sign whether send statistic report mechanism activated
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsSendStatisticReport(PRL_BOOL_PTR pbSendStatisticReport);

	/**
	 * Sets sign whether send statistic report mechanism activated
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetSendStatisticReport(PRL_BOOL bSendStatisticReport);

	/**
	 * Returns base port for remote desktop
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRemoteDisplayBasePort(PRL_UINT32_PTR pnPort);

	/**
	 * Sets base port for remote desktop
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetRemoteDisplayBasePort(PRL_UINT32 nPort);

	/**
	 * Returns max port for remote desktop
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRemoteDisplayMaxPort(PRL_UINT32_PTR pnPort);

	/**
	 * Gets remote desktop base host name
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetRemoteDisplayBaseHostName(PRL_STR sHostNameBuf, PRL_UINT32_PTR pnHostNameBufLength);


	/**
	 * Sets remote desktop base host name
	 * @param [in] new remote desktop base host name
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - incorrect (empty) string value
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetRemoteDisplayBaseHostName(PRL_CONST_STR sNewHostName);

	/**
	 * Creates net adapter instance
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate net adapter object
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT CreateDispNet(PRL_HANDLE_PTR phDispNet);

	/**
	 * Returns pointer to internal dispatcher config data synchronization object
	 */
	inline QMutex *GetSynchroObject() {return (&m_HandleMutex);}

	/**
	 * Returns dispatcher configuration
	 */
	inline CDispCommonPreferences &GetDispConfig() {return (m_DispConfig);}

	/**
	 * Read object representation from string
	 */
	PRL_RESULT fromString(PRL_CONST_STR disp_config);

	/**
	 * Returns XML as string
	 */
	QString toString();

	/**
	 * Returns net adapters count
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDispNetCount(PRL_UINT32_PTR pnDispNetCount);


	/**
	 * Returns net adapter object
	 * @param [in] index of net adapter object
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - net adapter object index out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDispNet(PRL_UINT32 nDispNetIndex, PRL_HANDLE_PTR phDispNet);


	/**
	 * Returns sign whether a new user can change the common server parameters
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT CanChangeDefaultSettings(PRL_BOOL_PTR pbDefaultChangeSettings);

	/**
	 * Sets sign whether a new user can change the common server parameters
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetCanChangeDefaultSettings(PRL_BOOL bDefaultChangeSettings);


	/**
	 * Returns minimal security level for connection with server
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMinSecurityLevel(PRL_SECURITY_LEVEL_PTR pnMinSecurityLevel);

	/**
	 * Sets minimal security level for connection with server
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMinSecurityLevel(PRL_SECURITY_LEVEL nMinSecurityLevel);

	/**
	* Obtains a handles list of operations which required administrator confirmation
	* @param [out]	A pointer to a variable that receives the handle.
					The type of the handle is PHT_OPAQUE_TYPE_LIST
					( values in list consist in enum PRL_ALLOWED_VM_COMMAND )
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetConfirmationsList(PRL_HANDLE_PTR phConfirmationsList);

	/**
	* Obtains a handles list of operations which required administrator confirmation
	* @param  pOpaqueList values in list consist in enum PRL_ALLOWED_VM_COMMAND
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT SetConfirmationsList(PrlHandleOpTypeListPtr pList);

	/**
	 * Gets default backup server
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultBackupServer(PRL_STR sBackupServer, PRL_UINT32_PTR pnBackupServerBufLength);

	/**
	 * Sets default backup server
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDefaultBackupServer(PRL_CONST_STR sBackupServer);

  	/**
 	 * Gets backup temporary directory
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
 	 */
	PRL_RESULT GetBackupTmpDir(PRL_STR sBackupTmpDir, PRL_UINT32_PTR pnBackupTmpDirBufLength);

 	/**
 	 * Sets backup temporary directory
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
 	 */
	PRL_RESULT SetBackupTmpDir(PRL_CONST_STR sBackupTmpDir);

	/**
	 * Gets backup user login
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetBackupUserLogin(PRL_STR sUserLogin, PRL_UINT32_PTR pnUserLoginBufLength);

	/**
	 * Sets backup user login
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetBackupUserLogin(PRL_CONST_STR sUserLogin);

	/**
	 * Sets backup user password
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetBackupUserPassword(PRL_CONST_STR sUserPassword);

	/**
	 * Gets backup user password enabling sign
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsBackupUserPasswordEnabled(PRL_BOOL_PTR pbUserPasswordEnabled);

	/**
	 * Sets backup user password enabling sign
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetBackupUserPasswordEnabled(PRL_BOOL bUserPasswordEnabled);

	/**
	 * Gets default backup directory
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDefaultBackupDirectory(PRL_STR sBackupDirectory, PRL_UINT32_PTR pnBackupDirectoryBufLength);

	/**
	 * Sets default backup directory
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDefaultBackupDirectory(PRL_CONST_STR sBackupServer);

	/**
	 * Get backup timeout
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetBackupTimeout(PRL_UINT32_PTR nTimeout);

	/**
	 * Set backup timeout
	 * @param setting value
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetBackupTimeout(PRL_UINT32 nTimeout);

	/**
	* Determines whether the plugins enabled or not
	* @param [out]  Pointer to a variable that receives the result.
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_INVALID_ARG - incorrect passed buffer (NULL)
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT ArePluginsEnabled(PRL_BOOL_PTR pbIsPluginsAllowed);

	/**
	* Enable or disable plugins mech
	* @param [in] setting value
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT EnablePlugins(PRL_BOOL bEnablePluginsSupport);

	/**
	* Gets VM CPU limit type
	* @param [out] Pointer to a variable that receives the result.
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetVmCpuLimitType(PRL_UINT32_PTR pnVmCpuLimitType);

	/**
	* Sets VM CPU limit type
	* @param [in] setting value
	* @return PRL_RESULT. Possible values:
	* * PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT SetVmCpuLimitType(PRL_UINT32 nVmCpuLimitType);

	/**
	 * Gets default log level
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsVerboseLogEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets verbose log enabling sign
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetVerboseLogEnabled(PRL_BOOL bEnabled);

	/**
	 * Gets allow multiple PMC option
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsAllowMultiplePMC(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets allow multiple PMC option
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAllowMultiplePMC(PRL_BOOL bEnabled);

	/**
	 * Gets allow direct mobile option
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsAllowDirectMobile(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets allow direct mobile option
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAllowDirectMobile(PRL_BOOL bEnabled);

	/**
	 * Gets allow mobile clients option
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsAllowMobileClients(PRL_BOOL_PTR pbEnabled);

	/**
	 * Gets the number of USB devices
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUsbIdentityCount(PRL_UINT32_PTR pnUsbIdentityCount);

	/**
	 * Get handle to USB device information object
	 * @param [in] an index of USB device
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - index is out of range
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetUsbIdentity(PRL_UINT32 nUsbIdentIndex, PRL_HANDLE_PTR phUsbIdent);

	/**
	 * (Un)Associate USB device with VM
	 * @param [in] System name of USB device to associate
	 * @param [in] Uuid of VM to associate device with or empty to
	 *						terminate association
	 * @param [in] reserved
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - No device found with given name
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetUsbIdentAssociation(PRL_CONST_STR sSystemName, PRL_CONST_STR sVmUuid, PRL_UINT32 nFlags);

	/**
	 * To get the CPU features masks from dispatcher config
	 * @param [out] A pointer to strcucture with masks of processor features
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - invalid handle.
	 * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT GetCpuFeaturesMask(PRL_CPU_FEATURES_MASK_PTR pMasks);

	/**
	 * To set the CPU features masks on dispatcher config
	 * @param [in] A pointer to strcucture with masks of processor features
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_INVALID_ARG - invalid handle.
	 * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT SetCpuFeaturesMask(PRL_CONST_CPU_FEATURES_MASK_PTR pMasks);

	/**
	 * Returns CPU features masks.
	 * @param [out] a pointer to a variable that receives a handle of type PHT_CPU_FEATURES.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_OUT_OF_MEMORY - not enough memory to instantiate new object.
	 * * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT GetCpuFeaturesMaskEx(PRL_HANDLE_PTR phCpuFeatures);

	/**
	 * Sets value of CPU features mask.
	 * @param [in] a handle of type PHT_CPU_FEATURES identifying the features mask.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - function completed successfully.
	 */
	PRL_RESULT SetCpuFeaturesMaskEx(PRL_HANDLE hCpuFeatures);

	/**
	 * Gets log rotation enabled state.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsLogRotationEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets log rotation enabled state
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetLogRotationEnabled(PRL_BOOL bEnabled);

	/**
	 * Gets allow attach screenshots enabled state.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT IsAllowAttachScreenshotsEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets allow attach screenshots enabled state
	 * @param [in] setting value
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetAllowAttachScreenshotsEnabled(PRL_BOOL bEnabled);

	/**
	 * Get advanced authorization mode for mobile clients.
	 * @param [out] buffer for storing result
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetMobileAdvancedAuthMode(PRL_MOBILE_ADVANCED_AUTH_MODE_PTR pnMode);

	/**
	 * Set advanced authorization mode for mobile clients.
	 * @param [in] new advanced auth mode.
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetMobileAdvancedAuthMode(PRL_MOBILE_ADVANCED_AUTH_MODE nMode);
	/**
	 * Get proxy manager URL.
	 * @param [out] sURL buffer for storing result
	 * @param [out] pnURLBufLength buffer size (on in - how much symbols can be placed
	 * into buffer; on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result.
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetProxyManagerURL(PRL_STR sURL, PRL_UINT32_PTR pnURLBufLength);
	/**
	 * Set proxy manager URL.
	 * @param [in] sNewURL proxy manager url string.
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetProxyManagerURL(PRL_CONST_STR sNewURL);
	/**
	 * Get mobile host id.
	 * @param [out] sHostId buffer for storing result
	 * @param [out] pnHostIdBufLength buffer size (on in - how much symbols can be placed
	 * into buffer; on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result.
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetHostId(PRL_STR sHostId, PRL_UINT32_PTR pnHostIdBufLength);
	/**
	 * Set mobile host Id.
	 * @param [in] sHostId host id string. Empty string for resetting to hardware host id.
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetHostId(PRL_CONST_STR sHostId);

	/**
	* Obtains a handle to cpu pool information
	* @param [out]	A pointer to a variable that receives the handle.
					The type of the handle is PHT_CPU_POOL
	* @return PRL_RESULT. Possible values:
	* PRL_ERR_SUCCESS - operation completed successfully
	*/
	PRL_RESULT GetCpuPool(PRL_HANDLE_PTR phCpuPool);

private:

	PRL_RESULT SetCpuFeaturesMaskExImpl(const PrlHandleCpuFeatures &pFeatures);

	/// DispConfig object
	CDispCommonPreferences m_DispConfig;
	CParallelsNetworkConfig m_NetworkConfig;
};

typedef PrlHandleSmartPtr<PrlHandleDispConfig> PrlHandleDispConfigPtr;

#endif // __PARALLELS_HANDLE_DISP_CONFIG_H__
