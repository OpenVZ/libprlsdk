/*
 * PrlHandleServerDisp.h
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


#ifndef __VIRTUOZZO_HANDLE_SERVER_DISP_H__
#define __VIRTUOZZO_HANDLE_SERVER_DISP_H__

#include "PrlHandleServer.h"


/**
 * Base handle object - all objects representation on the client
 * side should follow it's interface by the means of inheritance.
 */
class PrlHandleServerDisp : public PrlHandleServer
{
public:

	/**
	 * For internal use
	 */
	void SetNonInteractiveSession(PRL_BOOL bNonInteractive);

	/**
	* For internal use
	*/
	void SetConfirmationMode(PRL_BOOL bConfirmationModeEnabled);

	/**
	 * Returns available for current connection questions list
	 */
	PrlQuestionsList& GetQuestionsList();

	/**
	 * Perform login to the remote server.
	 *
	 * @param host name string (all string are utf8)
	 * @param user name string (all string are utf8)
	 * @param user password string (all string are utf8)
	 * @param previous session uuid to restore session string(all string are utf8)
	 * @param port number to transfer commands data
	 * @param timeout in milliseconds for wait physical TCP connection establishment (if 0 specified then internal default timeout using)
	 * @param security level
	 * @param flags
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr Login(
					PRL_CONST_STR host,
					PRL_CONST_STR user,
					PRL_CONST_STR passwd,
					PRL_CONST_STR prevSessionUuid,
					PRL_UINT32 port_cmd,
					PRL_UINT32 timeout,
					PRL_SECURITY_LEVEL security_level,
					PRL_UINT32 flags
					);

	/**
	 * Perform logoff from the server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr Logoff();

	/**
	 * Perform send answer on server request.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr SendAnswer(PRL_HANDLE answer);

	/**
    * Send problem report to server
    * @param hProblemReport - handle of type PHT_PROBLEM_REPORT to a client-side report to be sent
    * @param nFlags - flags supported by the report system
    * @return pointer to generated job instance
    */
    PrlHandleJobPtr SrvSendProblemReport( PRL_HANDLE hProblemReport, PRL_UINT32 nFlags );

	/**
	 * Perform get list information of all host users.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetAllHostUsers();

	/**
	 * Perform login to the local server operation.
	 *
	 * @param previous session uuid to restore session string(all string are utf8)
	 * @param connection port number
	 * @param security level
	 * @param flags
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr LoginLocal( PRL_CONST_STR sPrevSessionUuid, PRL_UINT32 port,
				PRL_SECURITY_LEVEL security_level, PRL_UINT32 flags );

	/**
	 * Sets noninteractive session mode.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr SetNonInteractiveSession(PRL_BOOL bNonInteractive, PRL_UINT32 nFlags);

	/**
	 * Returns noninteractive session status.
	 *
	 * @param [out] noninteractive status
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsNonInteractiveSession(PRL_BOOL_PTR pbNonInteractive);

	/**
	* Enable confirmation mode for session.
	*
	* @return pointer to the class that abstracts the job process
	*/
	PrlHandleJobPtr EnableConfirmationMode( PRL_UINT32 nFlags);

	/**
	* Disable confirmation mode for session.
	*
	* @return pointer to the class that abstracts the job process
	*/
	PrlHandleJobPtr DisableConfirmationMode(
		PRL_CONST_STR sUser,
		PRL_CONST_STR sPasswd,
		PRL_UINT32 nFlags);

	/**
	* Returns confirmation mode for session.
	*
	* @param [out] confirmation mode
	* @return PRL_RESULT
	*/
	PRL_RESULT IsConfirmationModeEnabled(PRL_BOOL_PTR pbConfirmationModeEnabled);

	/**
	 * Perform get server config to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetSrvConfig();

	/**
	 * Perform get server common preferences to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetCommonPrefs();

	/**
	 * Perform server common preferences edit transaction begin command to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr CommonPrefsBeginEdit();

	/**
	 * Perform server common preferences edit transaction commit command to the remote server.
	 * @param handle of PHT_DISP_CONFIG type
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr CommonPrefsCommit(PRL_HANDLE hDispCfg, PRL_UINT32 nFlags);

	/**
	 * Perform get currently logged in user profile command to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetUserProfile();

	/**
	 * Perform get list information of all users.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetUserInfoList();

	/**
	 * Perform user information.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetUserInfo(PRL_CONST_STR sUserId);

	/**
	 * Perform configure generic PCI devices.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr ConfigureGenericPci(const QList<PrlHandleBasePtr>& dev_list, PRL_UINT32 nFlags);

	/**
	 * Perform currently logged in user profile begin edit command to the remote server.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr UserProfileBeginEdit();

	/**
	 * Perform currently logged in user profile commit command to the remote server.
	 * @param handle to modified user profile object
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr UserProfileCommit(PRL_HANDLE hUserProfile);

	/**
	 * Checks whether connection to server alive.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsConnected(PRL_BOOL_PTR pbConnected);

	/**
	 * Returns server information object.
	 *
	 * @param pointer to buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetServerInfo(PRL_HANDLE_PTR phServerInfo);

	/**
	 * Performs shutdown server procedure.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr Shutdown(PRL_UINT32 nFlags);

	/**
	 * Perform get server disk list.
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr FsGetDiskList();

	/**
	 * Perform get remote dir entries.
	 * @param remote dir path
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr FsGetDirEntries(PRL_CONST_STR path);

	/**
	 * Perform new remote dir creation.
	 * @param remote dir path
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr FsCreateDir(PRL_CONST_STR path);

	/**
	 * Perform remote FS entry removing.
	 * @param remote FS entry path
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr FsRemoveEntry(PRL_CONST_STR path);

	/**
	* Perform checking if an user can create  a file.
	* @param remote full path
	*
	* @return pointer to the class that abstracts the job process
	*/
	PrlHandleJobPtr FsCanCreateFile(PRL_CONST_STR path);

	/**
	 * Perform remote FS entry renaming.
	 * @param old remote FS entry path
	 * @param new remote FS entry path
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr FsRenameEntry(PRL_CONST_STR oldPath, PRL_CONST_STR newPath);

	/**
	 * Perform remote FS generate entry name operation.
	 * @param target remote directory absolute path
	 * @param generating entry name prefix
	 * @param generating entry name suffix
	 * @param delimiter between prefix and generated index
	 *
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr FsGenerateEntryName(
						PRL_CONST_STR sDirPath,
						PRL_CONST_STR sFilenamePrefix,
						PRL_CONST_STR sFilenameSuffix,
						PRL_CONST_STR sIndexDelimiter
						);

	/**
	* Gets VM current state
	* @param sKey
	* @param sUser
	* @param sCompany
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdUserUpdateLicense( PRL_CONST_STR sKey, PRL_CONST_STR sUser,  PRL_CONST_STR sCompany, PRL_UINT32 nFlags);

	/**
	* Gets VM current state
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr DspCmdUserGetLicenseInfo();

	/**
	 * Perform refresh states of installed plugins.
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr RefreshPlugins(PRL_UINT32 nFlags);

	/**
	 * Perform get plugin list.
	 * @return pointer to the class that abstracts the job process
	 */
	PrlHandleJobPtr GetPluginsList(PRL_CONST_STR sClassId, PRL_UINT32 nFlags);

    /**
	* Get common problem report from server
	* @param none
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr SrvGetProblemReport();

	/**
	* Get common packed problem report from server
	* @param nFlags - flags supported by the report system
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr SrvGetPackedProblemReport( PRL_UINT32 nFlags );

	/**
	* Attach to executed task on the server
	* @param valid task_id
	* @return pointer to generated job instance
	*/
	PrlHandleJobPtr SrvAttachToLostTask(PRL_CONST_STR sTaskId);

	/**
	 * Checks whether specified feature supported by remote side
	 * @param requesting feature identifier
	 * @param pointer to the buffer to storing result
	 */
	PRL_RESULT IsFeatureSupported(PRL_FEATURES_MATRIX nFeatureId, PRL_BOOL_PTR pbIsSupported);

	/**
	 * Get available disk space for path
	 * @param path
	 * @param flags
	 */
	PrlHandleJobPtr GetFreeDiskSpace(PRL_CONST_STR sPath, PRL_UINT32 nFlags);

	/**
	 * Get vcmmd config
	 * @param nFlags
	 */
	PrlHandleJobPtr GetVcmmdConfig(PRL_UINT32 nFlags);

	/**
	 * Get cpu mask support
	 * @param nFlags
	 */
	PrlHandleJobPtr GetCpuMaskSupport(PRL_UINT32 nFlags);

	/**
	 * Set vcmmd config
	 * @param sConfig
	 * @param nFlags
	 */
	PrlHandleJobPtr SetVcmmdConfig(PRL_CONST_STR sConfig, PRL_UINT32 nFlags);

};

#endif // __VIRTUOZZO_HANDLE_SERVER_DISP_H__
