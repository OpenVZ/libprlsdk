/*
 * CProtoSerializer.h: Implementation of project protocol commands
 * serializer.
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


#ifndef CProtoSerializer_H
#define CProtoSerializer_H

#include "Libraries/ProtoSerializer/CProtoCommands.h"
#include <prlcommon/IOService/IOCommunication/IOProtocol.h>

namespace Parallels
{

namespace DispatcherPackage {

inline SmartPtr<IOService::IOPackage> duplicateInstance (
    const SmartPtr<IOService::IOPackage>& pkg,
    const QString& evt )
{
    QByteArray baUtf8 = evt.toUtf8();
    quint32 size = 0;
    if ( baUtf8.size() > 0 )
        size = baUtf8.size() + 1;
    else
        return SmartPtr<IOService::IOPackage>();

    SmartPtr<IOService::IOPackage> p =
        IOService::IOPackage::duplicateHeaderInstance( pkg, 1 );

    p->fillBuffer( 0, IOService::IOPackage::RawEncoding,
                   baUtf8.data(), size );
    return p;
}

inline SmartPtr<IOService::IOPackage> duplicateInstance (
    const SmartPtr<IOService::IOPackage>& pkg,
    const CVmEvent& evt )
{
    return duplicateInstance(pkg, evt.toString());
}

inline SmartPtr<IOService::IOPackage> duplicateInstance (
    const SmartPtr<IOService::IOPackage>& pkg,
    const CVmEvent* evt )
{
    if ( evt == 0 )
        return SmartPtr<IOService::IOPackage>();
    return duplicateInstance(pkg, evt->toString());
}

inline SmartPtr<IOService::IOPackage> createInstance (
    int cmdNumber,
    const QString& evt,
    const SmartPtr<IOService::IOPackage> &parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false,
    quint32 buffsNum = 1,
    bool duplicateHeader = false )
{
    QByteArray baUtf8 = evt.toUtf8();
    quint32 size = 0;
    if ( baUtf8.size() > 0 )
        size = baUtf8.size() + 1;
    else
        return SmartPtr<IOService::IOPackage>();

    SmartPtr<IOService::IOPackage> p;
    if ( !duplicateHeader || !parent )
    {
        p = IOService::IOPackage::createInstance( cmdNumber, buffsNum, parent, broadcastResponse );
    }
    else
    {
        p = IOService::IOPackage::duplicateHeaderInstance( parent, 1 );
        p->header.type = cmdNumber;
    }
    p->fillBuffer( 0, IOService::IOPackage::RawEncoding,
                   baUtf8.data(), size );
    return p;
}

inline SmartPtr<IOService::IOPackage> createInstance (
    int cmdNumber,
    const CVmEvent& evt,
    const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false )
{
    return createInstance(cmdNumber, evt.toString(),
                          parent, broadcastResponse);
}

inline SmartPtr<IOService::IOPackage> createInstance (
    int cmdNumber,
    const CVmEvent* evt,
    const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false )
{
    if ( evt == 0 )
        return SmartPtr<IOService::IOPackage>();
    return createInstance(cmdNumber, evt->toString(),
                          parent, broadcastResponse);
}

inline SmartPtr<IOService::IOPackage> createInstance (
    int cmdNumber,
    const CVmEvent& evt,
    const void* data,
    quint32 size,
    const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false )
{
    SmartPtr<IOService::IOPackage> p =
        createInstance(cmdNumber, evt.toString(),
                       parent, broadcastResponse, 2);
    p->fillBuffer( 1, IOService::IOPackage::RawEncoding,
                   reinterpret_cast<const char*>(data), size );
    return p;
}

inline SmartPtr<IOService::IOPackage> createInstance (
    int cmdNumber,
    const CVmEvent* evt,
    const void* data,
    quint32 size,
    const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false )
{
    if ( evt == 0 )
        return SmartPtr<IOService::IOPackage>();
    SmartPtr<IOService::IOPackage> p =
        createInstance(cmdNumber, evt->toString(),
                       parent, broadcastResponse, 2);
    p->fillBuffer( 1, IOService::IOPackage::RawEncoding,
                   reinterpret_cast<const char*>(data), size );
    return p;
}

inline SmartPtr<IOService::IOPackage> createInstance (
    int type,
    const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false )
{
    return IOService::IOPackage::createInstance( type, 1,
                                                 parent, broadcastResponse );
}

inline SmartPtr<IOService::IOPackage> createInstance (
    int cmdNumber,
    const CProtoCommandPtr& cmd,
    const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false,
    bool duplicateHeader = false )
{
    return createInstance( cmdNumber, cmd->GetCommand()->toString(),
                           parent, broadcastResponse, 1, duplicateHeader );
}

/**
 * Overloaded method to prevent possibility of incorrect above method usage with non native pointers
 */
inline SmartPtr<IOService::IOPackage> createInstance (
    int cmdNumber,
    const CProtoCommand *pCmd,
    const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
    bool broadcastResponse = false )
{
    return createInstance( cmdNumber, pCmd->GetCommand()->toString(),
                           parent, broadcastResponse );
}

inline SmartPtr<IOService::IOPackage> createInstance (
	int cmdNumber,
	QDataStream& out,
	quint32 size ,
	const SmartPtr<IOService::IOPackage>& parent = SmartPtr<IOService::IOPackage>(),
	bool bDublicateHeader = false)
{
	SmartPtr<IOService::IOPackage> ioPkg;
	if(parent.getImpl() == NULL)
	{
		// IO pkg with single buffer
		ioPkg = IOService::IOPackage::createInstance( cmdNumber, 1 );
	}
	else
	{
		if ( bDublicateHeader )
		{
			ioPkg =
				IOService::IOPackage::duplicateHeaderInstance( parent, 1 );
			ioPkg->header.type = cmdNumber;
		}
		else
		{
			ioPkg =
				IOService::IOPackage::createInstance( cmdNumber, 1,parent);
		}
	}

	// Create data buffer
	if ( size > 0 ) {
		SmartPtr<char> dataBuff( new char[ size ],
					 SmartPtrPolicy::ArrayStorage );
		out.readRawData( dataBuff.getImpl(), size );
		ioPkg->setBuffer( 0, IOService::IOPackage::RawEncoding, dataBuff, size );
	}

	return ioPkg;
}

}//namespace DispatcherPackage

/**
 * Couple of proto messages generation methods
 */
class CProtoSerializer
{
public:
	/**
	 * Parses specified command string buffer and returns pointer to proto command object of corresponding type
	 * @param proto command type identifier
	 * @param package string buffer for parsing
	 */
	static CProtoCommandPtr ParseCommand(PVE::IDispatcherCommands nCmdIdentifier, const QString &sPackage);

	/**
         * Same as above, just an overloaded method.
	 */
	static CProtoCommandPtr ParseCommand( const SmartPtr<IOService::IOPackage>& );

	/**
	 * Template class method that let to casting base proto command pointer to necessary command type
	 */
	template <typename T>
	static T *CastToProtoCommand(CProtoCommandPtr pCommand)
	{
		return (static_cast<T *>(pCommand.getImpl()));
	}
	/**
	 * Generates DspCmdUserLogin protocol command
	 * @param user login
	 * @param user password
	 */
	static CProtoCommandPtr CreateDspCmdUserLoginCommand(
		const QString &sUsername,
		const QString &sPassword,
		const QString &sPrevSessionUuid,
                quint32 nFlags
	);
	/**
	 * Generates DspCmdUserLoginLocal protocol command
	 * @param user identifier
	 * @param application mode
	 * @param client process id
	 * @param flags
	 */
	static CProtoCommandPtr CreateDspCmdUserLoginLocalCommand(
		quint32 nUserId,
		PRL_APPLICATION_MODE appMode,
		quint64 nProcessId,
		quint32 flags = 0
	);
	/**
	 * Generates protocol command that contains no params (DspCmdUserLogoff, DspCmdDirGetVmList and etc.)
	 * @param proto command identifier
	 * @param sign whether questions must be forced during task progress
	 */
	static CProtoCommandPtr CreateProtoCommandWithoutParams(
		PVE::IDispatcherCommands nCmdIdentifier,
		bool bForceQuestionsSign = false,
		quint32 mFlags = 0
	);
	/**
	 * Generates protocol command that contains just some string parameter (DspCmdVmAnswer, DspCmdFsCreateDirectory and etc.)
	 * @param proto command identifier
	 * @param first command string parameter value
	 * @param sign whether questions must be forced during task progress
	 */
	static CProtoCommandPtr CreateProtoCommandWithOneStrParam(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sFirstParam,
		bool bForceQuestionsSign = false,
		quint32 nFlags = 0
	);
	/**
	 * Generates protocol command that contains two string parameters
	 * @param proto command identifier
	 * @param first command string parameter value
	 * @param second command string parameter value
	 * @param sign whether questions must be forced during task progress
	 */
	static CProtoCommandPtr CreateProtoCommandWithTwoStrParams(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sFirstParam,
		const QString &sSecondParam,
		bool bForceQuestionsSign = false,
		quint32 nFlags = 0
	);
	/**
	 * Generates protocol DspCmdDirVmCreate command
	 * @param XML representation string of VM configuration
	 * @param path to VM home
	 * @param flags set
	 */
	static CProtoCommandPtr CreateVmCreateProtoCommand(
		const QString &sVmConfig,
		const QString &sVmHomePath,
		quint32 nFlags
	);
	/**
	* Generates protocol DspCmdDirVmDelete command
	* @param Uuid of VM
	* @param lstDevices Devices which have to be deleted in VM.
	*/
	static CProtoCommandPtr CreateVmDeleteProtoCommand(
		const QString &sVmUuid,
		const QStringList& lstDevices
		);

	/**
	 * Generates protocol DspCmdFsRenameEntry command
	 * @param old entry name
	 * @param new entry name
	 */
	static CProtoCommandPtr CreateFsRenameEntryProtoCommand(
		const QString &sOldEntryName,
		const QString &sNewEntryName
	);
	/**
	 * Generates protocol commands with serial num info (DspCmdDirInstallGuestOs/DspCmdUserUpdateLicense)
	 * @param proto command identifier
	 * @param user name
	 * @param company name
	 * @param serial number
	 */
	static CProtoCommandPtr CreateSerialNumProtoCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sUserName,
		const QString &sCompanyName,
		const QString &sSerialNumber,
		quint32 nFlags = 0
	);
	/**
	 * Generates protocol commands with serial num info (DspCmdDirInstallGuestOs/DspCmdUserUpdateLicense)
	 * @param VM UUID
	 * @param user name
	 * @param company name
	 * @param serial number
	 * @param guest distro type
	 */
	static CProtoCommandPtr CreateVmCreateUnattendedProtoCommand(
		const QString &sVmUuid,
		const QString &sUserName,
		const QString &sCompanyName,
		const QString &sSerialNumber,
		PRL_GUEST_DISTRO_VERSION_ENUM nGuestType
	);
	/**
	 * Generates protocol DspCmdCreateUnattendedCd command
	 * @param guest OS version from the (PVS_GUEST_VER_...)
	 * @param user name
	 * @param password
	 * @param full user name
	 * @param a full path to the OS distribution image or folder
	 * @param a full path to output ISO-image file
	 */
	static CProtoCommandPtr CreateSrvCreateUnattendedCdProtoCommand(
		PRL_UINT32 nGuestType,
		const QString &sUserName,
		const QString &sPasswd,
		const QString &sFullUserName,
		const QString &sOsDistroPath,
		const QString &sOutImagePath
	);

	/**
	 * Generates protocol DspCmdDirVmClone command
	 * @param VM UUID
	 * @param new VM name
	 * @param new VM uuid
	 * @param path to VM home
	 * @param clone flags (PACF_CLONEVM_TO_TEMPLATE, PACF_CLONEVM_CHANGE_SID)
	 */
	static CProtoCommandPtr CreateVmCloneProtoCommand(
		const QString &sVmUuid,
		const QString &sVmName,
		const QString &sNewVmUuid,
		const QString &sVmHomePath,
		PRL_UINT32 nFlags
	);

	/**
	 * Generates protocol DspCmdDirCreateImage command
	 * @param VM UUID
	 * @param XML representation string of image configuration
	 * @param image recreate allowed sign
	 * @param sign whether task questions need to be forced
	 */
	static CProtoCommandPtr CreateImageCreateProtoCommand(
		const QString &sVmUuid,
		const QString &sImageConfig,
		bool bRecreateIsAllowed,
		bool bForceQuestionsSign = false
	);

	/**
	 * Generates protocol DspCmdDirCopyImage command
	 * @param VM UUID
	 * @param XML representation string of image configuration
	 * @param new image short name
	 * @param new location
	 * @param reserved
	 */
	static CProtoCommandPtr CopyImageCreateProtoCommand(
		const QString &sVmUuid,
		const QString &sImageConfig,
		const QString &sNewImageName,
		const QString &sTargetPath,
		quint32 nFlags = 0 );

	/**
	 * Generates protocol DspCmdStartSearchConfig command
	 * @param search dirs list
	 */
	static CProtoCommandPtr CreateStartSearchConfigProtoCommand(
		const QStringList &lstSearchDirs
	);

	/**
	* Generates protocol DspCmdUpdateUsbAssocList command
	* @param usb assoc list
	* @param usb list version
	* @param reserved flags
	*/

	static CProtoCommandPtr CreateUpdateUsbAssocListProtoCommand(
		const QStringList &lstUsbList,
		quint32 nListVersion,
		quint32 nFlags
	);

	/**
	* Generates protocol DspCmdConvertOldHdd command
	* @param search paths list
	*/

	static CProtoCommandPtr CreateConvertHddProtoCommand(
		const QStringList &lstHddPathsToConvert
		);

	/**
	 * Generates protocol VM command that contains just VM UUID and user access token params (DspCmdVmStart,
	 * DspCmdVmReset and etc.)
	 * @param proto command identifier
	 * @param VM UUID
	 */
	static CProtoCommandPtr CreateProtoBasicVmCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		quint32 nFlags = 0
	);
	/**
	 * Generates protocol VM command that contains VM UUID, user access token and use ACPI sign params (DspCmdVmStop/DspCmdVmPause)
	 * @param proto command identifier
	 * @param VM UUID
	 * @param use ACPI sign
	 */
	static CProtoCommandPtr CreateProtoVmCommandWithAcpiSign(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		bool bUseAcpi
	);

	/**
	 * Generates protocol VM command to stop VM
	 * @param proto command identifier
	 * @param VM UUID
	 * @param Vm stop mode
	 */
	static CProtoCommandPtr CreateProtoVmCommandStop(
		const QString &sVmUuid,
                PRL_UINT32 nStopMode,
                PRL_UINT32 nFlags
	);

	/**
	* Generates protocol VM command that contains VM UUID, user access token and use one string params
	* @param proto command identifier
	* @param VM UUID
	* @param string param
	*/
	static CProtoCommandPtr CreateProtoVmCommandWithOneStrParam(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		const QString &sParam,
		quint32 nFlags = 0
		);

	/**
	 * Generates update security protocol VM command
	 * @param VM UUID
	 * @param VM security info
	 */
	static CProtoCommandPtr CreateVmUpdateSecurityCommand(
		const QString &sVmUuid,
		const QString &sVmSecurity
	);
	/**
	 * Generates protocol VM device command (DspCmdVmDevConnect/DspCmdVmDevDisconnect)
	 * @param proto command identifier
	 * @param VM UUID
	 * @param VM device type
	 * @param VM device index
	 * @param XML representation string of VM device configuration
	 * @param bitset of flags
	 */
	static CProtoCommandPtr CreateVmDeviceProtoCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		PRL_DEVICE_TYPE nDevType,
		quint32 nDevIndex,
		const QString &sDevConfig,
		PRL_UINT32 nFlags = 0
	);
	/**
	 * Generates DspWsResponse protocol command
	 * @param initial request operation code
	 * @param operation return code
	 */
	static CProtoCommandPtr CreateDspWsResponseCommand(
		PVE::IDispatcherCommands nOpCode,
		PRL_RESULT nRetCode
	);

	/**
	 * Generates DspWsResponse protocol command
	 * @param p package on which response should be made
	 * @param nRetCode operation return code
	 */
	static CProtoCommandPtr CreateDspWsResponseCommand(
                const SmartPtr<IOService::IOPackage>& p,
		PRL_RESULT nRetCode
	);
	/**
	 * Generates DspCmdFsGenerateEntryName protocol command
	 * @param target directory path
	 * @param entry filename prefix
	 * @param entry filename suffix
	 * @param delimiter between prefix and generated index
	 */
	static CProtoCommandPtr CreateDspCmdFsGenerateEntryNameCommand(
		const QString &sDirPath,
		const QString &sFilenamePrefix,
		const QString &sFilenameSuffix,
		const QString &sIndexDelimiter
	);

	/**
	 * Validates VM configuration
	 * @param VM configuration
	 * @param Section identification in VM configuration
	 */
	static CProtoCommandPtr CreateVmValidateConfigProtoCommand(
		const QString &sVmConfig,
		PRL_VM_CONFIG_SECTIONS nSection
	);

	/**
	 * Generates protocol DspCmdDirVmMigrate command
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target server VM name
	 * @param target server VM home path
	 * @param VM migration flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	static CProtoCommandPtr CreateVmMigrateProtoCommand(
		const QString &sVmUuid,
		const QString &sTargetServerHostname,
		quint32 nTargetServerPort,
		const QString &sTargetServerSessionUuid,
		const QString &sTargetVmName,
		const QString &sTargetVmHomePath,
		quint32 nMigrationFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/**
	 * Generates CreateVmBackup proto command
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param backup description
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	static CProtoCommandPtr CreateCreateVmBackupCommand(
		const QString &sVmUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		const QString &sDescription,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/**
	 * Generates RestoreVmBackup proto command
	 * @param VM UUID
	 * @param backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target VM home path
	 * @param flags
	 * @param reserved flags
	 */
	static CProtoCommandPtr CreateRestoreVmBackupCommand(
		const QString &sVmUuid,
		const QString &sBackupUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		const QString &sTargetVmHomePath,
		const QString &sTargetVmName,
		quint32 nRestoreFlags,
		quint32 nReservedFlags
	);
	/**
	 * Generates GetBackupTree proto command
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	static CProtoCommandPtr CreateGetBackupTreeCommand(
		const QString &sVmUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/**
	 * Generates RemoveVmBackup proto command
	 * @param VM UUID
	 * @param backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	static CProtoCommandPtr CreateRemoveVmBackupCommand(
		const QString &sVmUuid,
		const QString &sBackupUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		quint32 nRestoreFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);

	/**
	 * Generates protocol DspCmdVmStartEx command
	 * @param VM UUID
	 * @param start mode
	 * @param reserved parameter
	 */
	static CProtoCommandPtr CreateVmStartExProtoCommand(
		const QString &sVmUuid,
		quint32 nStartMode,
		quint32 nReserved
	);

	static CProtoCommandPtr CreatePerfStatsProtoCommand(
        CProtoPerfStatsCommand::PerfStatsAction action,
        const QString &sFilter,
        const QString &sVmUuid)
    {
        return CProtoCommandPtr(new CProtoPerfStatsCommand(action, sFilter, sVmUuid)) ;
    }

	/**
	 * Generates protocol DspCmdVmCreateSnapshot command
	 * @param VM uuid
	 * @param snapshot name
	 * @param snapshot description
	 * @param uuid of the state to create
	 */
	static CProtoCommandPtr CreateCreateSnapshotProtoCommand(
		const QString &sVmUuid,
		const QString &sName,
		const QString &sDescription,
		const QString &sSnapshotUuid,
		const QString &sSnapshotCreator = QString(),
		const QString &sSnapshotPath = QString(),
		quint32 nFlags = 0
	);

	/**
	 * Generates protocol DspCmdVmSwitchToSnapshot command
	 * @param VM uuid
	 * @param uuid of the state to switch to
	 */
	static CProtoCommandPtr CreateSwitchToSnapshotProtoCommand(
		const QString &sVmUuid,
		const QString &sSnapshotUuid,
		quint32 nFlags = 0
	);

	/**
	 * Generates protocol DspCmdVmInternal command
	 * @param VM uuid
	 * @param Command name
	 * @param prameter1
	 * @param prameter2
	 */
	static CProtoCommandPtr CreateVmInternalProtoCommand(
		const QString &sVmUuid,
		const QString &sCmd,
		const QStringList &lstArgs
	);

	/**
	 * Generates protocol DspCmdVmCommitDiskUnfinished command
	 * @param VM uuid
	 * @param diskname of the state to commit
	 * @param uuid of the state to commit
	 * @param
	 */
	static CProtoCommandPtr CreateVmCommitDiskUnfinished(
		const QString &sVmUuid,
		const QString &sDiskName,
		const QString &sSnapshotUuid
	);

	/**
	 * Generates protocol DspCmdVmDeleteSnapshot command
	 * @param VM uuid
	 * @param uuid of the state to delete
	 * @param
	 */
	static CProtoCommandPtr CreateDeleteSnapshotProtoCommand(
		const QString &sVmUuid,
		const QString &sSnapshotUuid,
		bool bChild,
		quint32 nFlags = 0
	);

	/**
	* Generates protocol DspCmdVmUpdateSnapshotData command
	* @param VM uuid
	* @param uuid of the state
	* @param new name
	* @param new description
	*/
	static CProtoCommandPtr CreateUpdateSnapshotDataProtoCommand(
		const QString &sVmUuid,
		const QString &sSnapshotUuid,
		const QString &sNewName,
		const QString &sNewDescription
		);

	/**
	 * Generates protocol DspCmdVmLoginInGuest command
	 * @param VM UUID
	 * @param user login
	 * @param user password
	 * @param flags bitset
	 */
	static CProtoCommandPtr CreateVmLoginInGuestProtoCommand(
		const QString &sVmUuid,
		const QString &sUserName,
		const QString &sUserPassword,
		quint32 nFlags
	);

	/**
	 * Generates protocol DspCmdVmAuthWithGuestSecurityDb command
	 * @param VM UUID
	 * @param user login
	 * @param user password
	 * @param flags bitset
	 */
	static CProtoCommandPtr CreateVmAuthWithGuestSecurityDbProtoCommand(
		const QString &sVmUuid,
		const QString &sUserName,
		const QString &sUserPassword,
		quint32 nFlags
	);

	/**
	 * Generates protocol guest OS session command without arguments
	 * @param command identifier
	 * @param VM UUID
	 * @param VM guest session UUID
	 * @param flags bitset
	 */
	static CProtoCommandPtr CreateBasicVmGuestProtoCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		const QString &sVmGuestUuid,
		quint32 nFlags = 0
	);

	/**
	 * Generates protocol DspCmdVmGuestRunProgram command
	 * @param command identifier
	 * @param VM UUID
	 * @param VM guest session UUID
	 * @param program name
	 * @param arguments list
	 * @param environment variables
	 * @param flags bitset
	 */
	static CProtoCommandPtr CreateVmGuestRunProgramProtoCommand(
		const QString &sVmUuid,
		const QString &sVmGuestUuid,
		const QString &sProgramName,
		const QStringList &lstArgs,
		const QStringList &lstVars,
		quint32 nFlags = 0
	);

	/**
	 * Generates protocol DspCmdVmGuestSetUserPasswd command
	 * @param command identifier
	 * @param VM UUID
	 * @param VM guest session UUID
	 * @param user name
	 * @param user password
	 * @param flags bitset
	 */
	static CProtoCommandPtr CreateVmGuestSetUserPasswdProtoCommand(
		const QString &sVmUuid,
		const QString &sVmGuestUuid,
		const QString &sUserLoginName,
		const QString &sUserPassword,
		quint32 nFlags = 0
	);

	static CProtoCommandPtr CreateVmResizeDiskImageProtoCommand(
		const QString &sVmUuid,
		const QString &sDiskImage,
		quint32 nNewSize,
		quint32 nFlags
	);

	static CProtoCommandPtr CreateSetSessionConfirmationModeProtoCommand(
		bool bEnable,
		const QString &sUsername,
		const QString &sPassword,
		quint32 nFlags
		);

	static CProtoCommandPtr CreateDspCmdStorageSetValueCommand(
		const QString &sKey,
		const QString &sValue,
		quint32 nReservedFlags
		);

	static CProtoCommandPtr CreateDspCmdVmStorageSetValueCommand(
		const QString &sVmUuid,
		const QString &sKey,
		const QString &sValue,
		quint32 nReservedFlags
		);

	/**
	 * Generates protocol DspCmdDirReg3rdPartyVm command
	 * @param path to VM configuration file
	 * @param path tp VM root directory
	 * @param flags bitset
	 */
	static CProtoCommandPtr CreateDspCmdDirReg3rdPartyVmProtoCommand(
		const QString &sVmConfigPath,
		const QString &sVmRootDirPath,
		quint32 nFlags = 0
	);
	static CProtoCommandPtr CreateDspCmdNetworkPrefsChanged(
		const QString &sNetworkConfig,
		const QString &sOldnetworkID,
		const QString &sNewNetworkID,
		quint32 nFlags = 0
	);

	/**
	 * Generates protocol VM commands with two str params
	 * @param protocol command identifier
	 * @param VM UUID
	 * @param first str param
	 * @param second str param
	 * @param bitset of flags
	 */
	static CProtoCommandPtr CreateProtoVmCommandWithTwoStrParams(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		const QString &sParam1,
		const QString &sParam2,
		quint32 nFlags = 0
		);

	/**
	 * Generates protocol DspCmdCopyCtTemplate command
	 * @param template name
	 * @param os template name
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param command flags
	 * @param reserved command flags
	 */
	static CProtoCommandPtr CreateCopyCtTemplateProtoCommand(
		const QString &sName,
		const QString &sOsTmplName,
		const QString &sTargetServerHostname,
		quint32 nTargetServerPort,
		const QString &sTargetServerSessionUuid,
		quint32 nFlags,
		quint32 nReservedFlags
	);

	/**
	 * Generates protocol DspCmdDirVmMove command
	 * @param VM UUID
	 * @param new VM name
	 * @param path to VM home
	 * @param clone flags (PACF_CLONEVM_TO_TEMPLATE, PACF_CLONEVM_CHANGE_SID)
	 */
	static CProtoCommandPtr CreateVmMoveProtoCommand(
		const QString &sVmUuid,
		const QString &sNewHomePath,
		PRL_UINT32 nFlags
	);

    /**
     * Generates protocol DspCmdSendProblemReport command
     * @param report data in stringified form
     * @param VM UUID
     * @param flags associated with the command
     */
    static CProtoCommandPtr CreateSendProblemReportProtoCommand(
        const QString &report,
        const QString &vmUuid,
        PRL_UINT32 nFlags
    );
};

}//namespace Parallels

#endif

