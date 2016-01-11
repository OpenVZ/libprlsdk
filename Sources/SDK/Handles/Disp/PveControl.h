/*
 * PveControl.h: Main interface for processing Client<->Dispatcher
 * protocol messages
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


#ifndef PVECONTROL_H
#define PVECONTROL_H

#include <QHash>
#include <QMutex>
#include <QObject>
#include <QStringList>
#include <prlcommon/Interfaces/ParallelsNamespace.h>
#include <prlcommon/Std/SmartPtr.h>
#include <prlcommon/IOService/IOCommunication/IOClient.h>
#include "Libraries/ProtoSerializer/CProtoSerializer.h"
#include "SDK/Include/PrlIOStructs.h"
#include "PrlHandleBase.h"

using namespace IOService;
using Parallels::CProtoCommandPtr;

/**
 * PVE control connection timeout - legacy.
 * Why would we need that at all ?
 */
#define PVE_CONNECTION_TIMEOUT 60000

/**
 * Main interface for processing Client<->Dispatcher proto messages
 */
class CPveControl : public QObject
{
	Q_OBJECT

public:
	/**
	 * Class constructor
	 * @param sign whether SSL must to be using
	 * @param pointer to object that will be processing all receiving packages and connection state changing
	 */
	CPveControl(bool bUseSSL, QObject *pEventReceiverObj);

	/**
	 * Class destructor
	 */
	~CPveControl ();

	/**
	 * Blocks current thread until answer to specified job will be received
	 * @param processing job UUID
	 * @param wait timeout in milliseconds
	 */
	bool WaitForRequestComplete(const QString &sReqId, quint32 nWaitTimeout = UINT_MAX);

	/**
	 * This method order to finalizing transport work from the native SDK event loop thread
	 */
	void stopTransport();

	/**
	 * Returns true if connection is remote and peer server has runtime
	 * display encodings support.
	 */
	bool IsDisplayEncodingsSupported () const;

	/**
	 * Returns true if connection is remote and peer server has runtime
	 * YUV display encodings support.
	 */
	bool IsYUVDisplayEncodingsSupported () const;

	IOClient *GetIoClient() const { return m_ioClient; }

public://Protocol methods
	/**
	 * Performs remote login request
	 * @param pointer to remote login job helper
	 * @param remote host name
	 * @param connSec connection security level
	 * @param timeout which specifies how long time it's necessary to wait for connection establishment
	 */
	void DspCmdUserLogin(	PrlHandleLoginHelperJobPtr pJob,
							const char *strRemoteHost,
							PRL_SECURITY_LEVEL connSec,
							quint32 nConnectionTimeout,
							PRL_UINT32& outPortNumber,
							QString& outHostName,
							PRL_CONST_STR certificate = 0,
							PRL_CONST_STR privateKey = 0);


	/**
	 * Provides a possibility to change user authority
	 * @param user name
	 * @param password
	 * @param previous session ID (in case of restore lost session)
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserRelogin(	const char *sUser,
							const char *sPassword,
							const char *sPrevSessionUuid,
							PRL_UINT32 nFlags);


	/**
	 * Performs first stage request of local login operation
	 * @param platform dependent user id (under UNIX based OSes it's UID;
	 *        under Windows based systems PID of current execution process)
	 * @param previous session uuid to restore session string(all string are utf8)
	 * @param connSec connection security level
	 * @param timeout which specifies how long time it's necessary to wait for connection establishment
	 * @param connection port number
	 * @param flags
	 * @return id of performed asynchronous request
	 */
	void DspCmdUserLoginLocal(PrlHandleLoginLocalHelperJobPtr pJob,
							  PRL_SECURITY_LEVEL connSec,
							  quint32 nPort,
							  PRL_UINT32& outPortNumber);

	/**
	 * Performs logoff operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserLogoff();

	/**
	 * Sets noninteractive session mode
	 * @param bNonInteractive noninteractive session mode
	 * @param nFlags reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdSetNonInteractiveSession(bool bNonInteractive, quint32 nFlags);

	QString DspCmdSetSessionConfirmationMode(
		bool bEnableConfirmationMode,
		const char *strUserName,
		const char *strPassword,
		quint32 nFlags);

	/**
	 * Performs start VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmStart(const char *strVmUUID);

	/**
	 * Performs start VM operation in the specified mode
	 * @param VM UUID
	 * @param start mode
	 * @param reserved parameter
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmStartEx(const char *strVmUUID, quint32 nStartMode, quint32 nReserved);

	/**
	 * Performs request to VM process on opening guest OS user session
	 * @param VM UUID
	 * @param user name
	 * @param user password
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmLoginInGuest(const char *strVmUUID, const char *sUserName,
									const char *sUserPassword, quint32 nFlags);

	/**
	 * Performs request to VM process authorizing user through guest OS security database
	 * @param VM UUID
	 * @param user name
	 * @param user password
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmAuthWithGuestSecurityDb(const char *strVmUUID, const char *sUserName,
									const char *sUserPassword, quint32 nFlags);

	/**
	 * Performs request to VM process on start VNC server
	 * @param VM UUID
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmStartVncServer(const char *strVmUUID,  quint32 nFlags);

	/**
	 * Performs request to VM process on stop VNC server
	 * @param VM UUID
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmStopVncServer(const char *strVmUUID,  quint32 nFlags);

	/**
	 * Performs request on exclusive VM lock
	 * @param VM UUID
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmLock(const char *strVmUUID,  quint32 nFlags);

	/**
	 * Performs request on unlock exclusively locked VM
	 * @param VM UUID
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmUnlock(const char *strVmUUID,  quint32 nFlags);

	/**
	 * Performs request to VM process on closing guest OS user session
	 * @param VM UUID
	 * @param VM session UUID
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGuestLogout(const char *strVmUUID, const char *strVmGuestUuid, quint32 nFlags);

	/**
	 * Performs request to VM process on getting network settings of guest OS
	 * @param VM UUID
	 * @param VM session UUID
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGuestGetNetworkSettings(const char *strVmUUID, const char *strVmGuestUuid, quint32 nFlags);

	/**
	 * Performs request to VM process on program execution at guest OS session
	 * @param VM UUID
	 * @param VM session UUID
	 * @param program name
	 * @param program arguments list
	 * @param program environment variables
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGuestRunProgram(const char *strVmUUID, const char *strVmGuestUuid, const char *strProgramName,
									const QStringList &lstArgs, const QStringList &lstVars, quint32 nFlags);

	/**
	 * Performs request to VM process on reset guest user account password
	 * @param VM UUID
	 * @param VM session UUID
	 * @param user login name
	 * @param user password
	 * @param flags bitset
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGuestSetUserPasswd(const char *strVmUUID, const char *strVmGuestUuid, const char *strUserLoginName,
									const char *strUserPassword, quint32 nFlags);

	QString DspCmdVmResizeDiskImage(const char *strVmUuid, const QString &sDiskImage, quint32 nNewSize, quint32 nFlags);

	/**
	 * Performs restart VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmRestartGuest(const char *strVmUUID);

	/**
	 * Performs stop VM operation
	 * @param VM UUID
	 * @param sign specifying whether ACPI must to be usage to power off VM
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmStop(const char *strVmUUID, PRL_UINT32 nStopMode, PRL_UINT32 nFlags);

	/**
	 * Performs pause VM operation
	 * @param VM UUID
	 * @param sign specifying whether ACPI must to be usage to pause VM (hibernate emulation)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmPause(const char *strVmUUID, bool bAcpi = false);

	/**
	 * Performs suspend VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmSuspend(const char *strVmUUID);

	/**
	 * Performs suspended VM screen operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGetSuspendedScreen(const char *strVmUUID);

	/**
	 * Performs resume VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmResume(const char *strVmUUID);

	/**
	* Drop suspendeded VM state
	* @param VM UUID
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmDropSuspendedState(const char *strVmUUID);

	/**
	* Start creation procedure of the new snapshot
	* @param VM UUID
	* @param Snapshot name
	* @param Snapshot description
	* @param UUID of the state to create
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmCreateSnapshot( const char *strVmUUID,
									const char *strName,
									const char *strDescription,
									const char *strStateUuid);

	/**
	* Start switching to state procedure
	* @param VM UUID
	* @param UUID of the state to switch to
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmSwitchToSnapshot(const char *strVmUUID, const char *strStateUuid,
			PRL_UINT32 nFlags);

	/**
	* Start delete state procedure
	* @param VM UUID
	* @param UUID of the state to delete
	* @param Merge snapshot or pure delete
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmDeleteSnapshot(const char *strVmUUID, const char *strStateUuid, const bool bMerge);

	/**
	* Performs get snapshot three operation
	* @param VM UUID
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmGetSnapshotsTree(const char *strVmUUID, PRL_UINT32 nFlags);

	/**
	* Sets new snapshot name and description
	* @param VM UUID
	* @param UUID of the state to switch to
	* @param New name
	* @param New description
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmUpdateSnapshotData(const char *strVmUUID, const char *strStateUuid,
									   const char *strNewName, const char *strNewDescription);

	/**
	 * Performs reset VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmReset(const char *strVmUUID);


	/**
	* Performs Internal VM operation
	* @param VM UUID
	* @param VM cmd
	* @param VM prm1
	* @param VM prm2
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmInternal(const char *strVmUUID,
								   const char *strCmd,
								   const QStringList &lstArgs
								   );

	/**
	 * Performs subscribe to VM statistics receiving operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmSubscribeToGuestStatistics(const char *strVmUUID);

	/**
	 * Performs unsubscribe from VM statistics receiving operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmUnsubscribeFromGuestStatistics(const char *strVmUUID);

	/**
	* Begin some utility installation
	* @param VM UUID
	* @param utility string id
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmInstallUtility(const char *strVmUUID, const char *strId);

	/**
	* Begin the Tools installation process
	* @param VM UUID
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmInstallTools(const char *strVmUUID);

	/**
	* Run the Compressor process
	* @param VM UUID
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmRunCompressor(const char *strVmUUID);

	/**
	* Cancel the Compressor process
	* @param VM UUID
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmCancelCompressor(const char *strVmUUID);

	/**
	* Creates and sends request on machine SID change to VM
	* @param VM UUID
	* @param flags bitset
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmChangeSid(const char *strVmUUID, PRL_UINT32 nFlags);

	/**
	* Creates and sends request on reset uptime counter to VM
	* @param VM UUID
	* @param flags bitset
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmResetUptime(const char *strVmUUID, PRL_UINT32 nFlags);

	/**
	* Update Vm Tools Section
	* @param VM UUID
	* @param Tools section in VM config
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmUpdateToolsSection(const char *strVmUUID, const char* strToolsSection);

	/**
	* Performs register VM operation
	* @param path to VM home dir
	* @param Vm UUID
	* @param with bitmask of flags
	* @return id of performed asynchronous request
	*/
	QString DspCmdDirRegVm(const char *strVmDirPath, const char *strVmUuid, quint32 bFlags = 0);

	/**
	* Performs register 3rd party VM operation
	* @param path to VM config file
	* @param path to VM root home dir
	* @param with bitmask of flags
	* @return id of performed asynchronous request
	*/
	QString DspCmdDirReg3rdPartyVm(const char *strVmConfig, const char *strVmRootDirPath, quint32 bFlags = 0);

	/**
	 * Performs unregister VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirUnregVm(const char *strVmUUID);

	/**
	 * Performs restore VM operation
	 * @param path to VM home dir
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirRestoreVm(const char *strVmUUID);

	/**
	 * Performs delete VM operation
	 * @param VM configuration XML representation string (CVmConfiguration XmlModel class)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirVmDelete(const char *strVmConfig,const QStringList & lstDevices);

	/**
	 * Performs create VM operation
	 * @param VM configuration XML representation string (CVmConfiguration XmlModel class)
	 * @param path to VM home dir (can be empty - it's mean to create VM at default VM directory path)
	 * @param flags set
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirVmCreate(const char *strVmConfig, const char *strVmHomePath, PRL_UINT32 nFlags );

	/**
	 * Performs clone VM operation
	 * @param cloning VM configuration XML representation string (CVmConfiguration XmlModel class)
	 * @param new VM name
	 * @param path to new VM home dir (can be empty - it's mean to create VM at default VM directory path)
	 * @param sign whether VM template must be created
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirVmClone(const char *strVmUuid,
		const char *strVmName,
		const char *strNewVmUuid,
		const char *strVmHomePath,
		PRL_UINT32 nFlags);

	/**
	 * Performs migrate VM operation
	 * @param migrating VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target server VM name
	 * @param target server VM home path
	 * @param VM migration type
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirVmMigrate(const char *strVmUuid,
		const char *strTargetServerHostname,
		quint32 nTargetServerPort,
		const char *strTargetServerSessionUuid,
		const char *sTargetVmName,
		const char *strTargetVmHomePath,
		quint32 nMigrationFlags,
		quint32 nReservedFlags,
		bool bForceOperation);

	/**
	 * Performs begin edit VM config operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdDirVmEditBegin(const char *strVmUUID);

	/**
	 * Performs commit changed VM config operation
	 * @param changed VM configuration XML representation string (CVmConfiguration XmlModel class)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirVmEditCommit(const char *strVmConfig, PRL_UINT32 nFlags);

	/**
	 * Performs get VM list operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirGetVmList(PRL_UINT32 nFlags);

	/**
	 * Performs start search VM configurations operation
	 * @param list of directories where try to find VM configurations
	 *        (may be empty - it specifies to dispatcher searching VMs at all local disks)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdStartSearchConfig(const QStringList &searchDirsList);

	/**
	* Performs start convert old hdds operation
	* @param list of paths hdd to convert
	* @return id of performed asynchronous request
	*/
	QString DspCmdConvertOldHdd(const QStringList &lstHddPathsToConvert);

	/**
	* updates usb assoc list on server
	* @param PRL_HANDLE hStringsList - usb assoc list
	* @return id of performed asynchronous request
	*/
	QString DspCmdUpdateUsbAssocList(const QStringList &lstUsbList,
										quint32 nListVersion,
										quint32 nFlags);

	/**
	 * Performs get server host hardware info operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserGetHostHwInfo();

	/**
	 * Performs get server disks list operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsGetDiskList();

	/**
	 * Performs get current server directory operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsGetCurrentDirectory();

	/**
	 * Performs get directory entries operation
	 * @param target directory absolute path
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsGetDirectoryEntries(const char *strSourceDir);

	/**
	 * Performs create remote directory operation
	 * @param creating directory absolute path
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsCreateDirectory(const char *strDir);

	/**
	 * Performs rename entry operation
	 * @param old entry name
	 * @param new entry name
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsRenameEntry(const char *strOldEntryName, const char *strNewEntryName);

	/**
	 * Performs remove entry operation
	 * @param removing entry name
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsRemoveEntry(const char *strEntryName);

	/**
	 * Performs checking whether current user can create specified file operation
	 * @param checking file absolute path
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsCanCreateFile(const char *strEntryName);

	/**
	 * Performs generate entry name operation
	 * @param target directory path
	 * @param filename prefix
	 * @param filename suffix
	 * @param delimiter between prefix and generated index
	 * @return id of performed asynchronous request
	 */
	QString DspCmdFsGenerateEntryName(
				const char *strDirPath,
				const char *strFilenamePrefix,
				const char *strFilenameSuffix,
				const char *strIndexDelimiter
				);

	/**
	 * Performs get current user profile operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserGetProfile();

	/**
	 * Performs get list information of all users
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserInfoList();

	/**
	 * Performs get user information
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserInfo(const char* strUserId);

	/**
	 * Performs stand by all guests with generic PCI devices.
	 * @return id of performed asynchronous request
	 */
	QString DspCmdPrepareForHibernate(PRL_UINT32 nFlags);

	/**
	* Performs to wake up generic PCI devices after host resume.
	* @return id of performed asynchronous request
	*/
	QString DspCmdAfterHostResume(PRL_UINT32 nFlags);

	/**
	 * Performs get list information of all virtual networks
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetVirtualNetworkList(PRL_UINT32 nFlags);

	/**
	 * Performs add virtual network
	 * @return id of performed asynchronous request
	 */
	QString DspCmdAddVirtualNetwork(const char* strVirtNet, PRL_UINT32 nFlags);

	/**
	 * Performs update virtual network
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUpdateVirtualNetwork(const char* strVirtNet, PRL_UINT32 nFlags);

	/**
	 * Performs delete virtual network
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDeleteVirtualNetwork(const char* strVirtNet, PRL_UINT32 nFlags);

	QString DspCmdUpdateOfflineService(const char* str, PRL_UINT32 nFlags);
	QString DspCmdDeleteOfflineService(const char* str, PRL_UINT32 nFlags);
	QString DspCmdGetOfflineServicesList(PRL_UINT32 nFlags);

	QString DspCmdUpdateNetworkClassesConfig(const char* str, PRL_UINT32 nFlags);
	QString DspCmdGetNetworkClassesConfig(PRL_UINT32 nFlags);

	QString DspCmdUpdateNetworkShapingConfig(const char* str, PRL_UINT32 nFlags);
	QString DspCmdGetNetworkShapingConfig(PRL_UINT32 nFlags);
	QString DspCmdRestartNetworkShaping(PRL_UINT32 nFlags);

	/**
	 * Performs configure generic PCI devices
	 * @return id of performed asynchronous request
	 */
	QString DspCmdConfigureGenericPci(const char* strDevList, PRL_UINT32 nFlags);

	/**
	 * Performs get list information of all host users
	 * @return id of performed asynchronous request
	 */
	QString DspCmdAllHostUsers();

	/**
	 * Performs begin edit current user profile operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserProfileBeginEdit();

	/**
	 * Performs commit current user profile operation
	 * @param changed user profile XML representation string (CDispUser XmlModel class)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserProfileCommit(const char *strUserProfile);

	/**
	 * Performs subscribe to receiving host statistics operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdSubscribeToHostStatistics();

	/**
	 * Performs unsubscribe from receiving host statistics operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUnsubscribeFromHostStatistics();

	/**
	 * Performs get common server preferences operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetHostCommonInfo();

	/**
	 * Performs get host statistics operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetHostStatistics();

	/**
	 * Performs begin edit common server preferences operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdHostCommonInfoBeginEdit();

	/**
	 * Performs commit common server preferences operation
	 * @param common server preferences XML representation string (CDispCommonPreferences XmlModel class)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdHostCommonInfoCommit(const char *strHostCommonInfo, PRL_UINT32 nFlags);

	/**
	 * Performs cancel remote task operation
	 * @param remote request UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserCancelOperation(const char *strCancellingRequestUuid);

	/**
	 * Performs send answer on VM question operation
	 * @param answer object XML representation string (CVmEvent XmlModel class)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmAnswer(const char *strEvent);

	/**
	 * Performs connect VM device operation
	 * @param VM UUID
	 * @param device type
	 * @param device index
	 * @param device XML representation string (CVmDevice inheritance class of XmlModel)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmDevConnect(const char *strVmUUID,
							   PRL_DEVICE_TYPE nDeviceType,
							   int nDeviceIndex,
							   const char *strDeviceConfig);

	/**
	 * Performs disconnect VM device operation
	 * @param VM UUID
	 * @param device type
	 * @param device index
	 * @param device XML representation string (CVmDevice inheritance class of XmlModel)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmDevDisconnect(const char *strVmUUID,
								  PRL_DEVICE_TYPE nDeviceType,
								  int nDeviceIndex,
								  const char *strDeviceConfig);

	/**
	 * Performs check virtual HDD password request
	 * @param VM UUID
	 * @param device type
	 * @param device index
	 * @param device XML representation string (CVmDevice inheritance class of XmlModel)
	 * @param bitset of flags
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmDevHdCheckPassword(const QString &sVmUUID,
							   PRL_DEVICE_TYPE nDeviceType,
							   int nDeviceIndex,
							   const QString &sDeviceConfig,
							   PRL_UINT32 nFlags);

	/**
	 * Performs requesting VM problem report operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGetProblemReport(const char *strVmUUID);

	/**
	* Performs requesting VM problem report operation
	* @param VM UUID
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmGetPackedProblemReport(const char *strVmUUID,PRL_UINT32 nFlags);

    /**
    * Performs sending problem report operation
    * @param report's data
    * @param VM UUID
    * @param flags associated with the operation
    * @return id of the performed asynchronous request
    */
    QString DspCmdSendProblemReport(const QString& report,
        const QString &vmUuid,
        PRL_UINT32 nFlags);

	/**
	 * Performs create unattended floppy operation
	 * @param VM UUID
	 * @param installing guest OS type
	 * @param serial key
	 * @param user name
	 * @param company name
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmCreateUnattendedFloppy(
		/* [in] */ const char *strVmUUID,
		/* [in] */ PRL_GUEST_DISTRO_VERSION_ENUM nGuestDistroType,
		/* [in] */ const char *strKey,
		/* [in] */ const char *strUser,
		/* [in] */ const char *strCompany);

	/**
	 * Performs operation of creation bootable ISO-image for unattended guest OS installation
	 * @param guest OS version from the (PVS_GUEST_VER_...)
	 * @param user name
	 * @param password
	 * @param full user name
	 * @param full path to the OS distribution image or folder
	 * @param full path to output ISO-image file
	 */
	QString DspCmdCreateUnattendedCd(
		/* [in] */ PRL_UINT32 nGuestType,
		/* [in] */ const char *sUserName,
		/* [in] */ const char *sPasswd,
		/* [in] */ const char *sFullUserName,
		/* [in] */ const char *sOsDistroPath,
		/* [in] */ const char *sOutImagePath);

	/**
	* Initiates sending notifications about devices states by VM
	* @param VM UUID
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmInitiateDevStateNotifications(const char *strVmUUID);
	/**
	* Performs update VM security operation
	* @param VM UUID
	* @param VM security
	* @return id of performed asynchronous request	*/
	QString DspCmdVmUpdateSecurity(const char *strVmUUID, const char *strVmSecurity);

	/**
	* Validates VM configuration
	* @param VM configuration
	* @param Section identification in VM configuration
	* @return id of performed asynchronous request
	*/
	QString DspCmdVmSectionValidateConfig(const char *strVmConfig, PRL_VM_CONFIG_SECTIONS nSection);

	/**
	 * Performs cancel VM migration operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmMigrateCancel(const char *strVmUUID);

	/**
	 * Create Vm backup
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param description
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	QString DspCmdCreateVmBackup(
		const char *strVmUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		const char *strDescription,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation);

	/**
	 * Restore Vm backup
	 * @param VM UUID
	 * @param Backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target VM home path
	 * @param flags
	 * @param reserved flags
	 */
	QString DspCmdRestoreVmBackup(
		const char *strVmUuid,
		const char *strBackupUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		const char *strTargetVmHomePath,
		const char *strTargetVmName,
		quint32 nRestoreFlags,
		quint32 nReservedFlags);

	/**
	 * Get backups tree
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	QString DspCmdGetBackupTree(
		const char *strVmUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation);

	/**
	 * Remove Vm backup
	 * @param VM UUID
	 * @param Backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	QString DspCmdRemoveVmBackup(
		const char *strVmUuid,
		const char *strBackupUuid,
		const char *strServerHostname,
		quint32 nServerPort,
		const char *strServerSessionUuid,
		quint32 nRestoreFlags,
		quint32 nReservedFlags,
		bool bForceOperation);

	/**
	 * Performs create image for VM device operation
	 * @param VM UUID
	 * @param VM device config XML representation string (CVmDevice inheritance XmlModel class)
	 * @param sign whether non interactive mode using
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirCreateImage(const char *strVmUUID,
								 const char *strImageConfig,
								 bool bRecreateIsAllowed,
								 bool bNonInteractiveMode);
	/**
	 * Performs copy image of specified VM device to custom location
	 * @param VM UUID
	 * @param VM device config XML representation string (CVmDevice inheritance XmlModel class)
	 * @param new image short name
	 * @param new location
	 * @param reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirCopyImage(	const char *strVmUUID,
								const char *strImageConfig,
								const char *strNewImageName,
								const char *strTargetPath,
								quint32 nFlags);
	/**
	* Performs update VM device information
	* @param VM device config XML representation string (CVmDevice inheritance XmlModel class)
	* @return id of performed asynchronous request
	*/
	QString DspCmdUpdateDeviceInfo(const char * strImageConfig);
	/**
	 * Not understand what is it
	 */
	QString DspCmdDirInstallGuestOS(const char *strUserName, const char *strCompanyName, const char *strSerialNo);

	/**
	 * Performs update VM configuration operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGetConfig(const char *strVmUUID, quint32 nFlags);

	/* Requect for default Vm configuration
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetDefaultVmConfig(const QString &sParam, quint32 nFlags);
	/**
	 * Performs get VM statistics operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmGetStatistics(const char *strVmUUID, PRL_UINT32 nFlags);

	/**
	 * Performs get VM current state operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdGetVmInfo(const char *strVmUUID);

	/**
	 * Extracts VM Tools current state
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdGetVmToolsInfo(const char *strVmUUID);

	/**
	 * Performs get dispatcher runtime info operation
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCGetDispatcherRTInfo();

	/**
	 * Performs get command history for specified VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCGetCommandHistoryByVm(const char *strVmUUID);

	/**
	 * Performs get command history for specified user session operation
	 * @param user session UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCGetCommandHistoryByUser(const char *strUserUUID);

	/**
	 * Performs shutdown dispatcher operation
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCShutdownDispatcher(PRL_UINT32 nFlags);

	/**
	 * Performs restart dispatcher operation
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCRestartDispatcher();

	/**
	 * Performs close specified user session operation
	 * @param user session UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCDisconnectUser(const char *strUserUUID);

	/**
	 * Performs disconnect all users operation
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCDisconnectAllUsers();

	/**
	 * Performs cancel remote task operation operation
	 * @param remote task UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCCancelUserCommand(const char *strCancelRequestUuid);

	/**
	 * Performs shutdown VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCShutdownVm(const char *strVmUUID);

	/**
	 * Performs restart VM operation
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
   QString DspCmdSMCRestartVm(const char *strVmUUID);

	/**
	 * Performs start Parallels network service operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdNetPrlNetworkServiceStart();

	/**
	 * Performs stop Parallels network service operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdNetPrlNetworkServiceStop();

	/**
	 * Performs restart Parallels network service operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdNetPrlNetworkServiceRestart();

	/**
	 * Performs restore defaults of Parallels network service
	 * @return id of performed asynchronous request
	 */
	QString DspCmdNetPrlNetworkServiceRestoreDefaults();

	/**
	 * Performs getting Parallels Net service status
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetNetServiceStatus();

	/**
	 * Performs adding Parallels Network Adapter
	* @param Network Adapter config XML representation string
	 * @return id of performed asynchronous request
	 */
	QString DspCmdAddNetAdapter(const char *strNetAdapter);

	/**
	 * Performs deleting Parallels Network Adapter
	* @param Network Adapter index
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDeleteNetAdapter(unsigned int nIndex);

	/**
	 * Performs updating Parallels Network Adapter
	* @param Network Adapter config XML representation string
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUpdateNetAdapter(const char *strNetAdapter);

	/**
	 * Performs update user license operation
	 * @param serial key
	 * @param user name
	 * @param company name
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserUpdateLicense(
		/* [in] */ const char *strKey,
		/* [in] */ const char *strUser,
		/* [in] */ const char *strCompany,
		/* [in] */ PRL_UINT32 nFlags );

	/**
	 * Performs get license info operation
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUserGetLicenseInfo();

	/**
	 * Returns connection current state
	 * @param pointer to object that will be processing request execution result
	 * @return connection current state
	 */
	IOSender::State GetState();

	/**
	* Performs attach to lost task
	* @param task uuid
	* @return id of performed asynchronous request
	*/
	QString DspCmdAttachToLostTask(const char *strTaskId );


	/**
	 * Performs virtual hard disk optimization
	 * @param VM UUID
	 * @param Bit mask of really hard disk devices (little endian order)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmCompact(const char *strVmUUID, quint32 uMask, quint32 nFlags);

	/**
	 * Performs cancelling virtual hard disk optimization
	 * @param VM UUID
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmCancelCompact(const char *strVmUUID);

	/**
	 * Performs virtual hard disk conversion
	 * @param VM UUID
	 * @param Bit mask of really hard disk devices (little endian order)
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmConvertDisks(const char *strVmUUID, quint32 uMask, quint32 nFlags);

	QString DspCmdSubscribePerfStats(const char *filter, const char *strVmUUID) ;
	QString DspCmdUnsubscribePerfStats(const char *strVmUUID) ;
	QString DspCmdGetPerfStats(const char *filter, const char *strVmUUID) ;

	QString DspCmdStorageSetValue( const char *sKey, const char *sValue, PRL_UINT32 nFlags );
	QString DspCmdVmStorageSetValue( const char *sVmUuid, const char *sKey, const char *sValue, PRL_UINT32 nFlags );

	/**
	* send to server client statistics
	* @param sStatistics - statistics as string
	* @return job UUID
	*/
	QString DspCmdSendClientStatistics( const char *sStatistics, PRL_UINT32 nFlags );

	/**
	 * install appliance from download service
	 * @param PRL_HANDLE hAppCfg - appliance config
	 * @param nFlags reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdInstallAppliance(const char* strAppCfg, quint32 nFlags);

	/**
	 * Performs request on client session authorization against encrypted VM
	 * @param VM UUID
	 * @param password
	 * @param nFlags reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmAuthorise( const char* strVmUUID, const char *strPassword, quint32 nFlags );

	/**
	 * Performs request on change password for encrypted VM
	 * @param VM UUID
	 * @param old password
	 * @param new password
	 * @param nFlags reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmChangePassword( const char* strVmUUID, const char *strOldPassword, const char *strNewPassword, quint32 nFlags );

	/**
	 * Performs request on VM encryption
	 * @param VM UUID
	 * @param password
	 * @param cipher plugin UUID
	 * @param nFlags reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmEncrypt( const char* strVmUUID, const char *strPassword, const char *strCipherPluginUuid, quint32 nFlags );

	/**
	 * Performs request on VM decryption
	 * @param VM UUID
	 * @param password
	 * @param nFlags reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmDecrypt( const char* strVmUUID, const char *strPassword, quint32 nFlags );

	/**
	 * Performs request on set VM protection
	 * @param VM UUID
	 * @param protection password
	 * @param reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmSetProtection( const char* strVmUUID,
								   const char *strProtectionPassword, quint32 nReserved );

	/**
	 * Performs request on remove VM protection
	 * @param VM UUID
	 * @param protection password
	 * @param reserved
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmRemoveProtection( const char* strVmUUID,
									  const char *strProtectionPassword, quint32 nReserved );

	/**
	 * Gets list of all Container templates
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetCtTemplateList(PRL_UINT32 nFlags);

	/**
	 * Sends prepared package to server side
	 * @param pointer to sending package object
	 * @return UUID on sended job
	 */
	QString SendRequestToServer(const SmartPtr<IOPackage> &pPackage);

	/**
	 * Removes Container template
	 * @return id of performed asynchronous request
	 */
	QString DspCmdRemoveCtTemplate(PRL_CONST_STR sName,
		PRL_CONST_STR sOsTmplName, PRL_UINT32 nFlags);

	/**
	 * Copy Container template
	 * @param template name
	 * @param os template name
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param command flags
	 * @param reserved command flags
	 * @return id of performed asynchronous request
	 */
	QString DspCmdCopyCtTemplate(
			const char *sName,
			const char *sOsTmplName,
			const char *sTargetServerHostname,
			quint32 nTargetServerPort,
			const char *sTargetServerSessionUuid,
			quint32 nFlags,
			quint32 nReservedFlags);

	/**
	 * Performs add private network
	 * @return id of performed asynchronous request
	 */
	QString DspCmdAddIPPrivateNetwork(const char *strPrivNet, PRL_UINT32 nFlags);

	/**
	 * Performs removal of private network
	 * @return id of performed asynchronous request
	 */
	QString DspCmdRemoveIPPrivateNetwork(const char *strPrivNet, PRL_UINT32 nFlags);

	/**
	 * Performs update private network
	 * @return id of performed asynchronous request
	 */
	QString DspCmdUpdateIPPrivateNetwork(const char *strPrivNet, PRL_UINT32 nFlags);

	/**
	 * Performs get list information of all private networks
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetIPPrivateNetworksList(PRL_UINT32 nFlags);

	/**
	 * Performs refresh states of installed parallels plugins.
	 * @return id of performed asynchronous request
	 */
	QString DspCmdRefreshPlugins(PRL_UINT32 nFlags);

	/**
	 * Performs get plugin list.
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetPluginsList(const char *strClassId, PRL_UINT32 nFlags);

	/**
	 * Performs Vm mount action
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmMount(PRL_CONST_STR strVmUUID, PRL_CONST_STR sMntPath, PRL_UINT32 nFlags);

	/**
	 * Performs Vm umount action
	 * @return id of performed asynchronous request
	 */
	QString DspCmdVmUmount(PRL_CONST_STR strVmUUID, PRL_UINT32 nFlags);

	/**
	 * Get available disk space for path
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetDiskFreeSpace(PRL_CONST_STR sPath, PRL_UINT32 nFlags);

	/**
	 * Move bundle of virtual machine or private area of container.
	 * @return id of performed asynchronous request
	 */
	QString DspCmdDirVmMove(PRL_CONST_STR sVmUuid, PRL_CONST_STR sNewHomePath, PRL_UINT32 nFlags);

	/**
	 * Find VM by given uuid or name
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetVmConfigById(PRL_CONST_STR sSearchId, PRL_UINT32 nFlags);

	/**
	 * Get CPU pools list
	 * @return id of performed asynchronous request
	 */
	QString DspCmdGetCPUPoolsList();

	/**
	 * Move to another cpu pool
	 * @return id of performed asynchronous request
	 */
	QString DspCmdMoveToCPUPool(PRL_CONST_STR strPoolName);

	/**
	 * Recalculate cpu pool features
	 * @return id of performed asynchronous request
	 */
	QString DspCmdRecalculateCPUPool(PRL_CONST_STR strPoolName);

private:
	/**
	 * Checks connection status
	 * @return sign whether connection established
	 */
	bool CheckConnectionStatus();

	/**
	 * Checks connection status and send result on failed
	 * @return false if failed
	 */
	bool CheckConnectionStatusHelper(IOSender::State _state,
									 const QString& strUuid,
									 bool bDelayedConnection,
									 const QString& qsHost = "localhost");

	/**
	 * Posts error result to receiver
	 * @param remote request UUID
	 * @param error source string
	 * @param error code
	 */
	void PostFailedResult(const QString &strUuid,
		const QString &strErrorSource,
		PRL_RESULT event_code,
                QStringList params = QStringList() );

	/**
	 * Unregistries sended job handle from handles hash
	 * @param unregistering job handle
	 */
	void UnregisterJobHandle(IOSendJob::Handle hJob);

	/**
	 * Registries sended job handle from handles hash
	 * @param registering job handle
	 */
	void RegisterJobHandle(IOSendJob::Handle hJob);

	/**
	 * Cleanups jobs handles hash
	 */
	void ClearJobHandles();

	/**
	 * Returns job handle by specified UUID
	 * @param job UUID
	 * @return requested job handle
	 */
	IOSendJob::Handle GetJobHandleByUuid(const QString &sJobUuid);

	/**
	 * Notifies all awaiting jobs that connection to dispatcher was lost
	 */
	void NotifyJobsThatConnectionWasLost();

	/**
	 * Sends specified request to server side
	 * @param pointer to sending request object
	 * @return UUID on sended job
	 */
	QString SendRequestToServer(CProtoCommandPtr pRequest);

signals:
	/**
	 * Signal helper that using in finalize transport work mech
	 */
	void finalizeTransportWork();
	/**
	 * Signal helper that using for safe login helper job cleanup
	 */
	void cleanupLoginHelperJob();

private slots:
	/**
	 * Slot that processing received event package
	 * @param pointer to received package object
	 */
	void handleEventPackage ( const SmartPtr<IOPackage> _pkg);
	/**
	 * Slot that processing received response package
	 * @param handle to request job
	 * @param pointer to received package object
	 */
	void handleResponsePackage ( IOSendJob::Handle hJob, const SmartPtr<IOPackage> _pkg);
	/**
	 * Slot that processing state changing events
	 * @param chaged state value
	 */
	void connectionStateChanged ( IOSender::State _state);
	/**
	 * Slot helper that usage in finalize transport mech
	 */
	void onFinalizeTransportWork();
	/**
	 * Slot helper that using for safe login helper job cleanup
	 */
	void onCleanupLoginHelperJob();

private:
	/** Sign whether SSL must be using */
	bool m_bUseSSL;
	/** Storing pointer to events processing object */
	QObject *m_pEventReceiverObj;
	/** Sended jobs handles hash */
	QHash<QString, IOSendJob::Handle> m_JobsHandlesHash;
	/** Question requests packages hash */
	QHash<QString, SmartPtr<IOPackage> > m_QuestionsRequestPkgsHash;
	/** Sended jobs handles hash access synchronization object */
	QMutex m_JobsHandlesHashMutex;
	/** Pointer to client connection object */
	IOClient* m_ioClient;
	/** Remote login helper object */
	PrlHandleLoginHelperJobPtr m_pLoginHelperJob;
	/** Local login helper object */
	PrlHandleLoginLocalHelperJobPtr m_pLoginLocalHelperJob;
	/** Loging limit rating object */
	LogRateLimit	m_rl;
};

#endif // PVECONTROL_H
