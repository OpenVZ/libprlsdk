/*
 * CProtoCommands.h: Implementation of project protocol commands
 * serializer helpers.
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


#ifndef CProtoCommands_H
#define CProtoCommands_H

#include <QString>
#include <QStringList>
#include "Interfaces/ParallelsNamespace.h"
#include "Libraries/Std/SmartPtr.h"
#include "XmlModel/Messaging/CVmEvent.h"
#include "XmlModel/Messaging/CResult.h"

namespace Parallels
{

/**
* Abstraction of protocol command
*/
class CProtoCommandBase
{
public:
	/**
	 * Class constructor
	 * @param protocol command identifier
	 * @param sign whether questions must be forced during task progress
	 * @param command flags bitset
	 */
	CProtoCommandBase(bool bForceQuestionsSign, quint32 nFlags = 0);

	/** Virtual class destructor for correct child instances destruction */
	virtual ~CProtoCommandBase()
	{}

	/** Returns VM UUID */
	QString GetVmUuid() const;

	/** Returns command first string parameter */
	QString GetFirstStrParam();

	/**
	 * Returns generated proto package for external purposes.
	 */
	SmartPtr<CVmEvent> GetCommand();

	/**
	 * Returns generated proto package for external purposes (const variant).
	 */
	const SmartPtr<CVmEvent> &GetCommand() const;

	/**
	 * Parses specified command string buffer in internal proto package container
	 * @param command string buffer
	 */
	void ParsePackage(const QString &sCmdBuffer);

	/** Returns sign whether task questions need to be forced */
	bool GetForceQuestionsSign();

	/** Returns command flags bitset */
	quint32 GetCommandFlags();

public:
	/** Abstract method that let to determine whether protocol command valid */
	virtual bool IsValid() = 0;

protected:
	/**
	 * Checks whether specified parameter presents at proto package container
	 * @param parameter name
	 * @param parameter type
	 */
	bool CheckWhetherParamPresents(const QString &sParamName, PVE::ParamFieldDataType nParamType) const;

	/**
	 * Extracts corresponding parameter string value
	 * @param parameter name
	 */
	QString GetStringParamValue(const QString &sParamName) const;

	/**
	 * Extracts corresponding parameter unsigned int value
	 * @param parameter name
	 */
	quint32 GetUnsignedIntParamValue(const QString &sParamName) const;

	/**
	 * Extracts corresponding parameter unsigned long long value
	 * @param parameter name
	 */
	quint64 GetUnsignedInt64ParamValue(const QString &sParamName) const;

	/**
	 * Extracts corresponding strings list parameter
	 * @param parameter name
	 */
	QStringList GetStringListParamValue(const QString &sParamName) const;

	/**
	* Add/set string parameter
	* @param parameter value
	* @param parameter name
	*/
	void SetStringParamValue(const QString &sParamValue, const QString &sParamName);

	/**
	* Add/set string list parameter
	* @param parameter value
	* @param parameter name
	*/
	void SetStringListParamValue(const QStringList &lstParamValue, const QString &sParamName);

	/**
	* Add/set unsigned int parameter
	* @param parameter value
	* @param parameter name
	*/
	void SetUnsignedIntParamValue(quint32 nParamValue, const QString &sParamName);

	/**
	* Add/set unsigned long long parameter
	* @param parameter value
	* @param parameter name
	*/
	void SetUnsignedInt64ParamValue(quint64 nParamValue, const QString &sParamName);

protected:
	/** Pointer to proto package */
	SmartPtr<CVmEvent> m_pProtoPackage;
};

/**
 * Abstraction of protocol command
 */
template <typename TCmdIdentifier>
class CProtoCommandBaseTempl : public CProtoCommandBase
{
public:
	/**
	 * Class constructor
	 * @param protocol command identifier
	 * @param sign whether questions must be forced during task progress
	 * @param command flags bitset
	 */
	CProtoCommandBaseTempl(TCmdIdentifier nCmdIdentifier, bool bForceQuestionsSign = false, quint32 nFlags = 0)
	: CProtoCommandBase(bForceQuestionsSign, nFlags), m_nCmdIdentifier(nCmdIdentifier)
	{
	}

	/** Returns proto command id */
	inline TCmdIdentifier GetCommandId() const {return (m_nCmdIdentifier);}

private:
	/** Command identifier */
	TCmdIdentifier m_nCmdIdentifier;
};

typedef CProtoCommandBaseTempl<PVE::IDispatcherCommands> CProtoCommand;
typedef SmartPtr<CProtoCommand> CProtoCommandPtr;

/**
 * Stub for DspIllegalCommand
 */
class CProtoCommandIllegalCommand : public CProtoCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCommandIllegalCommand()
	: CProtoCommand(PVE::DspIllegalCommand)
	{}
	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid()
	{
		return (false);
	}
};

/**
 * Serializer helper class that let to generate and process DspCmdUserLogin command
 */
class CProtoCommandDspCmdUserLogin : public CProtoCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCommandDspCmdUserLogin()
	: CProtoCommand(PVE::DspCmdUserLogin, false)
	{}
	/**
	 * Class constructor.
	 * @param user login
	 * @param user password
	 * @param uuid of session to restore
	 */
	CProtoCommandDspCmdUserLogin(
		const QString &sUsername,
		const QString &sPassword,
		const QString &sPrevSessionUuid,
                quint32 nFlags
	);
	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid();
	/** Returns username param value */
	QString GetUserLoginName();
	/** Returns password param value */
	QString GetPassword();
	/** Returns uuid of session to restore [ optional ]*/
	QString GetPrevSessionUuid();
};

/**
 * Serializer helper class that let to generate and process DspCmdUserLoginLocal command
 */
class CProtoCommandDspCmdUserLoginLocal : public CProtoCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCommandDspCmdUserLoginLocal()
	: CProtoCommand(PVE::DspCmdUserLoginLocal)
	{}
	/**
	 * Class constructor.
	 * @param user identifier
	 */
	CProtoCommandDspCmdUserLoginLocal(
		quint32 nUserId,
		PRL_APPLICATION_MODE appMode,
		quint64	nProcessId,
		quint32 flags
	);
	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid();
	/** Returns user identifier */
	quint32 GetUserId();
	/** Returns application mode, if none PAM_SERVER will be returned */
	PRL_APPLICATION_MODE GetApplicationMode();
	/** Returns client process id for internal purposes ( #PWE-5753 )**/
	/** If procerssId was not sent ( client may use old SDK ) it returns 0 **/
	quint64	GetProcessId();
};

/**
 * Serializer helper class that let to generate and process DspCmdFsGenerateEntryName command
 */
class CProtoCommandDspCmdFsGenerateEntryName : public CProtoCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCommandDspCmdFsGenerateEntryName()
	: CProtoCommand(PVE::DspCmdFsGenerateEntryName, false)
	{}
	/**
	 * Class constructor.
	 * @param target directory path
	 * @param filename prefix
	 * @param filename suffix
	 * @param delimiter between prefix and generated index
	 */
	CProtoCommandDspCmdFsGenerateEntryName(
		const QString &sDirPath,
		const QString &sFilenamePrefix,
		const QString &sFilenameSuffix,
		const QString &sIndexDelimiter
	);
	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid();
	/** Returns target directory path param value */
	QString GetDirPath();
	/** Returns filename prefix param value */
	QString GetFilenamePrefix();
	/** Returns filename suffix param value */
	QString GetFilenameSuffix();
	/** Returns delimiter between prefix and generated index value */
	QString GetIndexDelimiter();
};

/**
 * Serializer helper class that let to generate basic protocol command contains no additional parameters
 */
class CProtoCommandWithoutParams : public CProtoCommand
{
public:
	/**
	 * Class default constructor.
	 * @param protocol command identifier
	 * @param sign whether questions must be forced during task progress
	 * @param command flags bitset
	 */
	CProtoCommandWithoutParams(PVE::IDispatcherCommands nCmdIdentifier, bool bForceQuestionsSign = false,
								quint32 nFlags = 0)
	: CProtoCommand(nCmdIdentifier, bForceQuestionsSign, nFlags)
	{}
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
};

/**
 * Serializer helper class that let to generate basic protocol command contains user access token and one string parameters
 */
class CProtoCommandWithOneStrParam : public CProtoCommandWithoutParams
{
public:
	/**
	 * Class default constructor.
	 * @param protocol command identifier
	 */
	CProtoCommandWithOneStrParam(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoCommandWithoutParams(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param proto command identifier
	 * @param first string parameter value
	 * @param sign whether questions must be forced during task progress
	 */
	CProtoCommandWithOneStrParam(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sFirstParam,
		bool bForceQuestionsSign = false,
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
};

/**
 * Serializer helper class that let to generate basic protocol command contains
 * user access token and two string parameters
 */
class CProtoCommandWithTwoStrParams : public CProtoCommandWithOneStrParam
{
public:
	/**
	 * Class default constructor.
	 * @param protocol command identifier
	 */
	CProtoCommandWithTwoStrParams(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoCommandWithOneStrParam(nCmdIdentifier)
	{}

	/**
	 * Class constructor.
	 * @param proto command identifier
	 * @param first string parameter value
	 * @param second string parameter value
	 * @param sign whether questions must be forced during task progress
	 */
	CProtoCommandWithTwoStrParams(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sParam1,
		const QString &sParam2,
		bool bForceQuestionsSign = false,
		quint32 nFlags = 0
	);

	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();

	/** Gets second string parameter */
	QString GetSecondStrParam();
};

/**
 * Serializer helper class that let to generate DspCmdDirVmCreate protocol command
 */
class CProtoVmCreateCommand : public CProtoCommandWithoutParams
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoVmCreateCommand()
	: CProtoCommandWithoutParams(PVE::DspCmdDirVmCreate)
	{}
	/**
	 * Class constructor.
	 * @param XML representation string of VM configuration
	 * @param path to VM home dir
	 * @param flags set
	 */
	CProtoVmCreateCommand(
		const QString &sVmConfig,
		const QString &sVmHomePath,
		quint32 nFlags
	);
	/**
	 * Special constructor for child classes. Let to pass own child command identifier up to classes hierarchy
	 * @param command identifier
	 */
	CProtoVmCreateCommand(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoCommandWithoutParams(nCmdIdentifier)
	{}
	/**
	 * Special constructor for child classes. Let to pass own child command identifier up to classes hierarchy
	 * @param command identifier
	 * @param XML representation string of VM configuration
	 * @param path to VM home dir
	 */
	CProtoVmCreateCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmConfig,
		const QString &sVmHomePath
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns XML representation string of VM configuration */
	QString GetVmConfig();
	/** Returns path to VM home dir */
	QString GetVmHomePath();

private:
	/**
	 * Adds own command parameters
	 * @param XML representation string of VM configuration
	 * @param path to VM home dir
	 */
	void AddOwnParams(const QString &sVmConfig, const QString &sVmHomePath);
};

/**
 * Serializer helper class that let to generate DspCmdFsRenameEntry protocol command
 */
class CProtoFsRenameEntryCommand : public CProtoCommandWithoutParams
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoFsRenameEntryCommand()
	: CProtoCommandWithoutParams(PVE::DspCmdFsRenameEntry)
	{}
	/**
	 * Class constructor.
	 * @param old entry name
	 * @param new entry name
	 */
	CProtoFsRenameEntryCommand(
		const QString &sOldEntryName,
		const QString &sNewEntryName
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns old entry name */
	QString GetOldEntryName();
	/** Returns new entry name */
	QString GetNewEntryName();
};

/**
 * Serializer helper class that let to generate protocol commands with serial number (DspCmdDirInstallGuestOS/DspCmdUserUpdateLicense)
 */
class CProtoSerialNumCommand : public CProtoCommandWithoutParams
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoSerialNumCommand(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoCommandWithoutParams(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param proto command identifier
	 * @param user name
	 * @param company name
	 * @param serial number
	 * @param flags
	 */
	CProtoSerialNumCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sUserName,
		const QString &sCompanyName,
		const QString &sSerialNumber,
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns user name */
	QString GetUserLoginName();
	/** Returns company name */
	QString GetCompanyName();
	/** Returns serial number */
	QString GetSerialNumber();
};

/**
 * Serializer helper class that let to generate DspCmdVmCreateUnattendedFloppy protocol command
 */
class CProtoCreateUnattendedFloppyCommand : public CProtoSerialNumCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCreateUnattendedFloppyCommand()
	: CProtoSerialNumCommand(PVE::DspCmdVmCreateUnattendedFloppy)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param user name
	 * @param company name
	 * @param serial number string
	 * @param guest OS type
	 */
	CProtoCreateUnattendedFloppyCommand(
		const QString &sVmUuid,
		const QString &sUserName,
		const QString &sCompanyName,
		const QString &sSerialNumber,
		PRL_GUEST_DISTRO_VERSION_ENUM nGuestType
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns guest distro type */
	PRL_GUEST_DISTRO_VERSION_ENUM GetGuestDistroType();
};

/**
 * Serializer helper class that let to generate DspCmdStartSearchConfig protocol command
 */
class CProtoStartSearchConfigCommand : public CProtoCommandWithoutParams
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoStartSearchConfigCommand()
	: CProtoCommandWithoutParams(PVE::DspCmdStartSearchConfig)
	{}
	/**
	 * Class constructor.
	 * @param search dirs list
	 */
	CProtoStartSearchConfigCommand(
		const QStringList &lstSearchDirs
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns XML representation string of image configuration */
	QStringList GetSearchDirs();
};

/**
 * Serializer helper class that let to generate basic protocol command contains just user access token and VM UUID
 * params
 */
class CProtoBasicVmCommand : public CProtoCommandWithoutParams
{
public:
	/**
	 * Class default constructor.
	 * @param proto command identifier
	 */
	CProtoBasicVmCommand(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoCommandWithoutParams(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param proto command identifier
	 * @param VM UUID
	 * @param sign whether task questions need to be forced
	 * @param command flags bitset
	 */
	CProtoBasicVmCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		bool bForceQuestionsSign = false,
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
};

/**
* Serializer helper class that let to generate DspCmdDirVmDelete protocol command
*/
class CProtoVmDeleteCommand : public CProtoBasicVmCommand
{
public:
	/**
	* Class default constructor.
	*/
	CProtoVmDeleteCommand()
		: CProtoBasicVmCommand(PVE::DspCmdDirVmDelete)
	{}
	/**
	* Class constructor.
	* @param XML representation string of VM configuration
	* @param path to VM home dir
	* @param lstDevices list of devices which have to be created
	*/
	CProtoVmDeleteCommand(
		const QString &sVmUuid,
		const QStringList& lstDevices
		);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns devices list */
	QStringList GetVmDevicesList();
};

/**
 * Serializer helper class that let to generate DspCmdDirVmClone protocol command
 */
class CProtoVmCloneCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoVmCloneCommand()
	: CProtoBasicVmCommand(PVE::DspCmdDirVmClone)
	{}
	/**
	 * Class constructor.
	 * @param XML representation string of VM configuration
	 * @param new VM name
	 * @param path to VM home dir
	 * @param sign whether VM template must be created
	 */
	CProtoVmCloneCommand(
		const QString &sVmUuid,
		const QString &sVmName,
		const QString &sVmHomePath,
		PRL_UINT32 nFlags
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns new VM name */
	QString GetVmName();
	/** Returns sign whether VM template must be created */
	bool IsCreateTemplate();
	/** Returns sign whether changeSID procedure should be performed */
	bool IsChangeSID();
	/** Returns path to clone new vm*/
	QString GetVmHomePath();
};

/**
 * Serializer helper class that let to generate DspCmdDirCreateImage protocol command
 */
class CProtoCreateImageCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCreateImageCommand()
	: CProtoBasicVmCommand(PVE::DspCmdDirCreateImage)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param XML representation string of image config
	 * @param sign whether image recreate allowed
	 * @param sign whether task questions need to be forced
	 */
	CProtoCreateImageCommand(
		const QString &sVmUuid,
		const QString &sImageConfig,
		bool bRecreateIsAllowed,
		bool bForceQuestionsSign = false
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns XML representation string of image configuration */
	QString GetImageConfig();
	/** Returns sign whether image recreate operation allowed */
	bool IsRecreateAllowed();
};

/**
 * Serializer helper class that let to generate DspCmdDirCopyImage protocol command
 */
class CProtoCopyImageCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCopyImageCommand()
	: CProtoBasicVmCommand(PVE::DspCmdDirCopyImage)
	{}

	CProtoCopyImageCommand(
		const QString &sVmUuid,
		const QString &sImageConfig,
		const QString &sNewImageName,
		const QString &sTargetPath,
		quint32 nFlags = 0
	);

	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();

	QString GetImageConfig();
	QString GetNewImageName();
	QString GetTargetPath();

};

/**
 * Serializer helper class that let to generate protocol command contains user access token, VM UUID and use ACPI sign
 * params
 */
class CProtoVmCommandWithAcpiSign : public CProtoBasicVmCommand
{
public:
	/**
	 * Class constructor.
	 * @param proto command identifier
	 */
	CProtoVmCommandWithAcpiSign(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoBasicVmCommand(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param proto command identifier
	 * @param VM UUID
	 * @param use ACPI sign
	 */
	CProtoVmCommandWithAcpiSign(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		bool bUseAcpi
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns sign whether ACPI must be used */
	bool IsUseAcpi();
};

/**
 * Serializer helper class that let to generate Vm stop command
 * params
 */
class CProtoVmCommandStop : public CProtoBasicVmCommand
{
public:
	CProtoVmCommandStop(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoBasicVmCommand(nCmdIdentifier)
	{}

	CProtoVmCommandStop(
		const QString &sVmUuid,
		PRL_UINT32 nStopMode,
		PRL_UINT32 nFlags
		);
	virtual bool IsValid();
	PRL_UINT32 GetStopMode();
};


/**
* Serializer helper class that let to generate protocol command contains user access token, VM UUID and one String
* params
*/
class CProtoVmCommandWithOneStrParam : public CProtoBasicVmCommand
{
public:
	/**
	* Class constructor.
	* @param proto command identifier
	*/
	CProtoVmCommandWithOneStrParam(PVE::IDispatcherCommands nCmdIdentifier)
		: CProtoBasicVmCommand(nCmdIdentifier)
	{}
	/**
	* Class constructor.
	* @param proto command identifier
	* @param VM UUID
	* @param use ACPI sign
	*/
	CProtoVmCommandWithOneStrParam(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		const QString &sParam,
		quint32 nFlags = 0
		);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
};

/**
* Serializer helper class that let to generate protocol command contains user access token, VM UUID and one String
* params
*/
class CProtoVmCommandWithTwoStrParams : public CProtoVmCommandWithOneStrParam
{
public:
	/**
	* Class constructor.
	* @param proto command identifier
	*/
	CProtoVmCommandWithTwoStrParams(PVE::IDispatcherCommands nCmdIdentifier)
		: CProtoVmCommandWithOneStrParam(nCmdIdentifier)
	{}
	/**
	* Class constructor.
	* @param proto command identifier
	* @param VM UUID
	* @param use ACPI sign
	*/
	CProtoVmCommandWithTwoStrParams(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		const QString &sParam1,
		const QString &sParam2,
		quint32 nFlags = 0
		);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/**
	 * Returns second str param
	 */
	QString GetSecondStrParam();
};

/**
 * Serializer helper class that let to generate protocol command contains user access token, VM UUID and use ACPI sign
 * params
 */
class CProtoVmUpdateSecurityCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoVmUpdateSecurityCommand()
	: CProtoBasicVmCommand(PVE::DspCmdVmUpdateSecurity)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param VM security info
	 */
	CProtoVmUpdateSecurityCommand(
		const QString &sVmUuid,
		const QString &sVmSecurity
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns VM security info */
	QString GetVmSecurity();
};

/**
 * Serializer helper class that let to generate VM device protocol command
 */
class CProtoVmDeviceCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class constructor.
	 * @param proto command identifier
	 */
	CProtoVmDeviceCommand(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoBasicVmCommand(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param proto command identifier
	 * @param VM UUID
	 * @param VM device type
	 * @param VM device index
	 * @param VM device configuration XML string representation
	 * @param bitset of flags
	 */
	CProtoVmDeviceCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		PRL_DEVICE_TYPE nDevType,
		quint32 nDevIndex,
		const QString &sDevConfig,
		PRL_UINT32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns VM device type */
	PRL_DEVICE_TYPE GetDeviceType();
	/** Returns VM device index */
	quint32 GetDeviceIndex();
	/** Returns VM device config */
	QString GetDeviceConfig();
};

/**
 * Serializer helper class that let to generate DspCmdVmCreateSnapshot protocol command
 */
class CProtoCreateSnapshotCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCreateSnapshotCommand()
	: CProtoBasicVmCommand(PVE::DspCmdVmCreateSnapshot)
	{}
	/**
	 * Class constructor.
	 * @param VM uuid
	 * @param snapshot name
	 * @param snapshot description
	 */
	CProtoCreateSnapshotCommand(
		const QString &sVmUuid,
		const QString &sName,
		const QString &sDescription,
		const QString &sSnapshotUuid,
		const QString &sSnapshoCreator = QString(),
		const QString &sSnapshotPath = QString(),
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns snapshot name */
	QString GetName();
	/** Returns snapshot description */
	QString GetDescription();
	/** Returns state Uuid */
	QString GetSnapshotUuid();
	/** Returns state Uuid */
	QString GetSnapshotCreator();
	/** Return snapshot path */
	QString GetSnapshotPath();
};

/**
 * Serializer helper class that let to generate DspCmdVmSwitchToSnapshot protocol command
 */
class CProtoSwitchToSnapshotCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoSwitchToSnapshotCommand()
	: CProtoBasicVmCommand(PVE::DspCmdVmSwitchToSnapshot)
	{}
	/**
	 * Class constructor.
	 * @param VM uuid
	 * @param uuid of the state to switch to
	 */
	CProtoSwitchToSnapshotCommand(
		const QString &sVmUuid,
		const QString &sSnapshotUuid,
		quint32 nFlags
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns state Uuid */
	QString GetSnapshotUuid();
};


/**
 * Serializer helper class that let to generate DspCmdVmDeleteSnapshot protocol command
 */
class CProtoDeleteSnapshotCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoDeleteSnapshotCommand()
	: CProtoBasicVmCommand(PVE::DspCmdVmDeleteSnapshot)
	{}
	/**
	 * Class constructor.
	 * @param VM uuid
	 * @param uuid of the state to delete
	 * @param Delete childs of snapshot
	 */
	CProtoDeleteSnapshotCommand(
		const QString &sVmUuid,
		const QString &sSnapshotUuid,
		bool bChild,
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns state Uuid */
	QString GetSnapshotUuid();
	/** Returns child flag */
	bool GetChild();
	/** Returns merge flag*/
	bool GetMerge();
	/** Sets merge flag */
	void SetMerge(bool bMerge);
	/** Returns deleting steps count */
	quint32 GetStepsCount();
	/** Sets deleting steps count */
	void SetStepsCount(quint32 uiStepsCount);
	/** Returns first deletion sign */
	quint32 GetDeletionStep();
	/** Sets first deletion sign */
	void SetDeletionStep(quint32 uiStep);
};

/**
* Serializer helper class that let to generate DspCmdVmUpdateSnapshotData protocol command
*/
class CProtoUpdateSnapshotDataCommand : public CProtoBasicVmCommand
{
public:
	/**
	* Class default constructor.
	*/
	CProtoUpdateSnapshotDataCommand()
		: CProtoBasicVmCommand(PVE::DspCmdVmUpdateSnapshotData)
	{}
	/**
	* Class constructor.
	* @param VM uuid
	* @param uuid of the state
	* @param new name
	* @param new description
	*/
	CProtoUpdateSnapshotDataCommand(
		const QString &sVmUuid,
		const QString &sSnapshotUuid,
		const QString &sNewName,
		const QString &sNewDescription
		);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns state Uuid */
	QString GetSnapshotUuid();
	/** Returns state name */
	QString GetName();
	/** Returns state description */
	QString GetDescription();
};

/**
 * Serializer helper class that let to generate and process DspWsResponse command
 */
class CProtoCommandDspWsResponse : public CProtoCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCommandDspWsResponse()
	: CProtoCommand(PVE::DspWsResponse)
	{}
	/**
	 * Class constructor.
	 * @param initial request operation code
	 * @param operation return code
	 */
	CProtoCommandDspWsResponse(
		PVE::IDispatcherCommands nRequestOpCode,
		PRL_RESULT nRetCode
	);
	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid()
	{
		return (true);
	}
	/**
	 * Let to initialize response with list of params
	 * @param assigning list of params
	 */
	void SetParamsList(const QStringList &lstParams);
	/**
	 * Let to initialize host hardware info response field
	 * @param assigning value (expecting correct string representation of CHostHardwareInfo XML model class object)
	 */
	void SetHostHardwareInfo(const QString &sHostHardwareInfo);
	/**
	 * Let to initialize host filesystem info response field
	 * @param assigning value (expecting correct string representation of CHwFileSystemInfo XML model class object)
	 */
	void SetHwFileSystemInfo(const QString &sHwFileSystemInfo);
	/**
	 * Let to initialize user profile response field
	 * @param assigning value (expecting correct string representation of CDispUser XML model class object)
	 */
	void SetUserProfile(const QString &sUserProfile);
	/**
	 * Let to initialize user information response field
	 * @param assigning value (expecting correct string representation of UserInfo XML model class object)
	 */
	void SetUserInfo(const QString &sUserInfo);
	/**
	 * Let to initialize host common info response field
	 * @param assigning value (expecting correct string representation of CDispCommonPreferences XML model class object)
	 */
	void SetHostCommonInfo(const QString &sHostCommonInfo, const QString &sNetworkInfo);

	/**
	 * Let to initialize snapshots tree response field
	 * @param assigning value (expecting text data)
	 */
	void SetSnapshotsTree(const QString &sSnapshotsTree);

	/**
	 * Let to initialize backups tree response field
 	 * @param assigning value (expecting text data)
	 */
	void SetBackupsTree(const QString &sBackupsTree);

	/**
	 * Let to initialize problem report response field
	 * @param assigning value (expecting text data)
	 */
	void SetProblemReport(const QString &sProblemReport);
	/**
	 * Let to initialize VM event response field
	 * @param assigning value (expecting correct string representation of CVmEvent XML model class object)
	 */
	void SetVmEvent(const QString &sVmEvent);
	/**
	 * Let to initialize VM config response field
	 * @param assigning value (expecting correct string representation of CVmConfiguration XML model class object)
	 */
	void SetVmConfig(const QString &sVmConfig);
	/**
	 * Let to initialize system statistics response field
	 * @param assigning value (expecting correct string representation of CSystemStatistics XML model class object)
	 */
	void SetSystemStatistics(const QString &sSystemStatistics);
	/**
	 * Let to initialize Parallels Net service status response field
	 * @param assigning value (expecting correct string representation of Net service status)
	 */
	void SetNetServiceStatus(const QString &sNetServiceStatus);
	/**
	 * Let to initialize additional error data response field
	 * @param assigning value (expecting correct string representation of CVmEvent XML model class object)
	 */
	void SetError(const QString &sError);
	/**
	* Let to initialize vm device data response field
	* @param assigning value (expecting correct string representation of sVmDevice XML model class object)
	*/
	void SetVmDevice(const QString &sVmDevice);
	/**
	* Let to initialize login response field
	* @param assigning value (expecting correct string representation in XML format of VmEvent)
	*/
	void SetLoginResponse(const QString &sLoginEvent);
	/**
	* Let to initialize VM security result field
	* @param assigning value (expecting correct string representation in XML format of VmEvent)
	*/
	void SetVmSecurity(const QString &sVmSecurity);
	/** Returns operation return code */
	PRL_RESULT GetRetCode();
	/**
	 * Let to set response return code
	 * @param assigning return code value
	 */
	void SetRetCode(PRL_RESULT nRetCode);
	/**
	 * Fills specified result object with necessary data
	 * @param pointer to filling result object
	 */
	void FillResult(CResult *pResult);
	/**
	 * Let to add standard result parameter. This parameter will be transfered to the tokens params list
	 * at fill result stage.
	 * @param parameter value
	 */
	void AddStandardParam(const QString &sParamValue);
	/**
	 * Returns standard parameters count
	 */
	quint32 GetStandardParamsCount();
	/**
	 * Returns specified standard parameter value
	 * @param standard parameter index (if @ref nIndex out of range null string value will be returned)
	 */
	QString GetStandardParam(quint32 nIndex);

	/**
	 * Let to initialize system statistics response field
	 * @param assigning value (expecting correct string representation of CSystemStatistics XML model class object)
	 */
    void SetPerfStats(const QString &sValue)
    {
        set_param_value(EVT_PARAM_WS_RESPONSE_CMD_GET_PERFSTATS, sValue) ;
    }

private:

    CVmEventParameter* set_param_value(const QString &param_name, const QString &param_value) ;
	/**
	 * @ref FillResult() method helper that fills necessary result fields for FS proto commands
	 * @param pointer to processing result object
	 */
	void FillFsCommandsParams(CResult *pResult);
	/**
	 * @ref FillResult() method helper that fills necessary result fields for VM config fetching commands
	 * @param pointer to processing result object
	 */
	void FillVmConfigParam(CResult *pResult);
	/**
	 * @ref FillResult() method helper that fills necessary result fields for VM event fetching commands
	 * @param pointer to processing result object
	 */
	void FillVmEventParam(CResult *pResult);
	/**
	 * @ref FillResult() method helper that fills necessary result fields for system statistics fetching commands
	 * @param pointer to processing result object
	 */
	void FillSystemStatisticsParam(CResult *pResult);
	/**
	 * @ref FillResult() method helper that fills result object with additional error data
	 * @param pointer to processing result object
	 */
	void FillErrorData(CResult *pResult);
	/**
	* @ref FillDeviceInfo() method helper that fills result object with additional error data
	* @param pointer to processing result object
	*/
	void FillDeviceInfo(CResult *pResult);
};


/**
* Serializer helper class that let to generate Convert old hdd protocol command
*/
class CProtoStartConvertHardDisksCommand : public CProtoCommandWithoutParams
{
public:
	/**
	* Class default constructor.
	*/
	CProtoStartConvertHardDisksCommand()
		: CProtoCommandWithoutParams(PVE::DspCmdConvertOldHdd)
	{}
	/**
	* Class constructor.
	* @param search dirs list
	*/
	CProtoStartConvertHardDisksCommand(
		const QStringList &lstHddPathsToConvert
		);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns XML representation string of image configuration */
	QStringList GetHddPathsToConvert();
};

/**
* Serializer helper class that let to generate update usb assoc protocol command
*/
class CProtoUpdateUsbAssocListCommand : public CProtoCommandWithoutParams
{
public:
	/**
	* Class default constructor.
	*/
	CProtoUpdateUsbAssocListCommand()
		: CProtoCommandWithoutParams(PVE::DspCmdUpdateUsbAssocList)
	{}
	/**
	* Class constructor.
	* @param search dirs list
	*/
	CProtoUpdateUsbAssocListCommand(
		const QStringList &lstUsbList,
		quint32 nListVersion,
		quint32 nFlags
		);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns usb assoc list */
	QStringList GetUsbList();
	quint32 GetListVersion();
};

/**
* Serializer helper class that let to generate validate VM configuration command
*/
class CProtoCreateVmValidateConfigCommand : public CProtoCommandWithOneStrParam
{
public:

	CProtoCreateVmValidateConfigCommand();
	CProtoCreateVmValidateConfigCommand(const QString &sVmConfig, PRL_VM_CONFIG_SECTIONS nSection);

	PRL_VM_CONFIG_SECTIONS GetSection();

	virtual bool IsValid();
};

/**
 * Serializer helper class that let to generate DspCmdDirVmMigrate protocol command
 */
class CProtoVmMigrateCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoVmMigrateCommand()
	: CProtoBasicVmCommand(PVE::DspCmdDirVmMigrate)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target server VM home path
	 * @param VM migration flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	CProtoVmMigrateCommand(
		const QString &sVmUuid,
		const QString &sTargetServerHostname,
		quint32 nTargetServerPort,
		const QString &sTargetServerSessionUuid,
		const QString &sTargetVmHomePath,
		quint32 nMigrationFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	quint32 GetVersion();
	/** Returns target server hostname */
	QString GetTargetServerHostname();
	/** Returns target server port */
	quint32 GetTargetServerPort();
	/** Returns target server session UUID */
	QString GetTargetServerSessionUuid();
	/** Returns target server VM home path */
	QString GetTargetServerVmHomePath();
	/** Returns VM migration flags */
	quint32 GetMigrationFlags();
	/** Returns reserved flags */
	quint32 GetReservedFlags();
	/** Returns force operation sign */
	bool GetForceOperationSign();
};

/**
 * Base serializer helper class for CProtoCreateUnattendedCdCommand
 */
class CProtoCreateUnattendedCdCommand : public CProtoCommandWithoutParams
{
public:
	/**
	 * Class constructor.
	 */
	CProtoCreateUnattendedCdCommand()
	: CProtoCommandWithoutParams(PVE::DspCmdCreateUnattendedCd)
	{}
	/**
	 * Class constructor.
	 * @param guest OS version from the (PVS_GUEST_VER_...)
	 * @param user name
	 * @param password
	 * @param full user name
	 * @param full path to the OS distribution image or folder
	 * @param full path to output ISO-image file
	 */
	CProtoCreateUnattendedCdCommand(
		quint32 nGuestType,
		const QString &sUserName,
		const QString &sPasswd,
		const QString &sFullUserName,
		const QString &sOsDistroPath,
		const QString &sOutImagePath
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns guest OS version */
	quint32 GetGuestType();
	/** Returns user name */
	QString GetUserLoginName();
	/** Returns password */
	QString GetPasswd();
	/** Returns full user name */
	QString GetFullUserName();
	/** Returns a full path to the OS distribution image or folder */
	QString GetOsDistroPath();
	/** Returns a full path to the created ISO-image location */
	QString GetOutImagePath();
};

/**
 * Base serializer helper class for CProto*Backup*Command
 */
class CProtoBackupCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class constructor.
	 * @param command ID
	 */
	CProtoBackupCommand(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoBasicVmCommand(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param command ID
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	CProtoBackupCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		quint32 nFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/* Returns backup protocol version */
	quint32 GetVersion();
	/** Returns target server hostname */
	QString GetServerHostname();
	/** Returns target server port */
	quint32 GetServerPort();
	/** Returns target server session UUID */
	QString GetServerSessionUuid();
	/** Returns flags */
	quint32 GetFlags();
};

/**
 * Serializer helper class that let to generate DspCmdCreateVmBackup protocol command
 */
class CProtoCreateVmBackupCommand : public CProtoBackupCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCreateVmBackupCommand()
	: CProtoBackupCommand(PVE::DspCmdCreateVmBackup)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target VM home path
	 * @param backup description
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	CProtoCreateVmBackupCommand(
		const QString &sVmUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		const QString &sDescription,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns backup description */
	QString GetDescription();
};

/**
 * Serializer helper class that let to generate DspCmdRestoreVmBackup protocol command
 */
class CProtoRestoreVmBackupCommand : public CProtoBackupCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoRestoreVmBackupCommand()
	: CProtoBackupCommand(PVE::DspCmdRestoreVmBackup)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target VM home path
	 * @param target iSCSI storage for bundle
	 * @param flags
	 * @param reserved flags
	 */
	CProtoRestoreVmBackupCommand(
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
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns target Vm home path */
	QString GetTargetVmHomePath();
	/** Returns target Vm name */
	QString GetTargetVmName();
	/** Returns source backup uuid */
	QString GetBackupUuid();
};

/**
 * Serializer helper class that let to generate DspCmdRemoveVmBackup protocol command
 */
class CProtoRemoveVmBackupCommand : public CProtoBackupCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoRemoveVmBackupCommand()
	: CProtoBackupCommand(PVE::DspCmdRemoveVmBackup)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param backup UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	CProtoRemoveVmBackupCommand(
		const QString &sVmUuid,
		const QString &sBackupUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		quint32 nRestoreFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns source backup uuid */
	QString GetBackupUuid();
};

/**
 * Serializer helper class that let to generate DspCmdGetBackupTree protocol command
 */
class CProtoGetBackupTreeCommand : public CProtoBackupCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoGetBackupTreeCommand()
	: CProtoBackupCommand(PVE::DspCmdGetBackupTree)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param target VM home path
	 * @param flags
	 * @param reserved flags
	 * @param force operation sign
	 */
	CProtoGetBackupTreeCommand(
		const QString &sVmUuid,
		const QString &sServerHostname,
		quint32 nServerPort,
		const QString &sServerSessionUuid,
		quint32 nBackupFlags,
		quint32 nReservedFlags,
		bool bForceOperation
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
};

/**
 * Serializer helper class that let to generate DspCmdVmStartEx protocol command
 */
class CProtoVmStartExCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoVmStartExCommand()
	: CProtoBasicVmCommand(PVE::DspCmdVmStartEx)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param start mode
	 * @param reserved parameter
	 */
	CProtoVmStartExCommand(const QString &sVmUuid, quint32 nStartMode, quint32 nReserved);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns VM start mode flags */
	quint32 GetStartMode();
	/** Returns reserved parameter */
	quint32 GetReservedParameter();
};

/**
 * Serializer helper class that let to generate DspCmdPerfomanceStatistics protocol command
 */
class CProtoPerfStatsCommand : public CProtoBasicVmCommand
{
        typedef CProtoBasicVmCommand ancestor ;
public:
	enum PerfStatsAction {
        PSA_INVALID      = 0,
        PSA_SUBSCRIBE,
        PSA_UNSUBSCRIBE,
        PSA_GET
	} ;

    CProtoPerfStatsCommand():ancestor(PVE::DspCmdPerfomanceStatistics) {}

	CProtoPerfStatsCommand(PerfStatsAction action, const QString &sFilter, const QString &sVmUuid="") ;

	virtual bool IsValid() ;

	PerfStatsAction GetAction() ;

    QString GetFilter() ;

    bool IsVmCommand() { return !GetVmUuid().isEmpty() ; }
private:
    inline PerfStatsAction normalize_action(unsigned int action)
    {
        if (!action || action>PSA_GET)
            return PSA_INVALID ;
        return (PerfStatsAction)action ;
    }
} ;

/**
 * Serializer helper class that let to generate DspCmdVmLoginInGuest protocol command
 */
class CProtoVmLoginInGuestCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 * @param command identifier
	 */
	CProtoVmLoginInGuestCommand(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoBasicVmCommand(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param command identifier
	 * @param VM UUID
	 * @param user login
	 * @param user password
	 * @param flags bitset
	 */
	CProtoVmLoginInGuestCommand(PVE::IDispatcherCommands nCmdIdentifier, const QString &sVmUuid, const QString &sUserName,
		const QString &sUserPassword, quint32 nFlags);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns user name parameter */
	QString GetUserLoginName();
	/** Returns user password parameter */
	QString GetUserPassword();
};

/**
 * Serializer helper class that let to generate basic protocol command for guest OS session functionality
 */
class CProtoBasicVmGuestCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 * @param proto command identifier
	 */
	CProtoBasicVmGuestCommand(PVE::IDispatcherCommands nCmdIdentifier)
	: CProtoBasicVmCommand(nCmdIdentifier)
	{}
	/**
	 * Class constructor.
	 * @param proto command identifier
	 * @param VM UUID
	 * @param VM guest OS session UUID
	 * @param bitset of command flags
	 */
	CProtoBasicVmGuestCommand(
		PVE::IDispatcherCommands nCmdIdentifier,
		const QString &sVmUuid,
		const QString &sVmGuestUuid,
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns guest OS session UUID */
	QString GetVmSessionUuid();
};

/**
 * Serializer helper class that let to generate DspCmdVmGuestRunProgram protocol command
 */
class CProtoVmGuestRunProgramCommand : public CProtoBasicVmGuestCommand
{
public:
	/**
	 * Class default constructor.
	 * @param proto command identifier
	 */
	CProtoVmGuestRunProgramCommand()
	: CProtoBasicVmGuestCommand(PVE::DspCmdVmGuestRunProgram)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param VM guest OS session UUID
	 * @param program name
	 * @param program arguments list
	 * @param program environment variables list
	 * @param bitset of command flags
	 */
	CProtoVmGuestRunProgramCommand(
		const QString &sVmUuid,
		const QString &sVmGuestUuid,
		const QString &sProgramName,
		const QStringList &lstArgs,
		const QStringList &lstVars,
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns program name */
	QString GetProgramName();
	/** Returns program arguments list */
	QStringList GetProgramArguments();
	/** Returns program environment variables */
	QStringList GetProgramEnvVars();
};

/**
 * Serializer helper class that let to generate DspCmdVmGuestSetUserPasswd protocol command
 */
class CProtoVmGuestSetUserPasswdCommand : public CProtoBasicVmGuestCommand
{
public:
	/**
	 * Class default constructor.
	 * @param proto command identifier
	 */
	CProtoVmGuestSetUserPasswdCommand()
	: CProtoBasicVmGuestCommand(PVE::DspCmdVmGuestSetUserPasswd)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param VM guest OS session UUID
	 * @param user login name
	 * @param user password
	 * @param bitset of command flags
	 */
	CProtoVmGuestSetUserPasswdCommand(
		const QString &sVmUuid,
		const QString &sVmGuestUuid,
		const QString &sUserLoginName,
		const QString &sUserPassword,
		quint32 nFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns user login name */
	QString GetUserLoginName();
	/** Returns user password */
	QString GetUserPassword();
};

class CProtoVmResizeDiskImageCommand : public CProtoBasicVmCommand
{
public:
	CProtoVmResizeDiskImageCommand()
		: CProtoBasicVmCommand(PVE::DspCmdVmResizeDisk)
	{}
	/**
	 * Class constructor.
	 * @param VM UUID
	 * @param path to disk image
	 * @param new disk size
	 */

	CProtoVmResizeDiskImageCommand(
		const QString &sVmUuid,
		const QString &sDiskImage,
		quint32 nNewSize,
		quint32 nReservedFlags = 0
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();

	QString GetDiskImage();
	quint32 GetSize();
	quint32 GetFlags();
};

/**
* Serializer helper class that let to generate and process DspCmdSessionLock/Unlock commands
*/
class CProtoCommandDspCmdSetSessionConfirmationMode : public CProtoCommand
{
public:
	/**
	* Class default constructor.
	*/
	CProtoCommandDspCmdSetSessionConfirmationMode()
		: CProtoCommand( PVE::DspCmdSetSessionConfirmationMode, false)
	{}
	/**
	* Class constructor.
	* @param user login
	* @param user password
	*/
	CProtoCommandDspCmdSetSessionConfirmationMode(
		bool bEnable,
		const QString &sUsername,
		const QString &sPassword,
		quint32 nReservedFlags = 0
		);

	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid();

	QString GetUserLoginName();
	QString GetPassword();
	bool GetEnabledSign();
};

class CProtoDspCmdStorageSetValueCommand : public CProtoCommand
{
public:
	/**
	* Class default constructor.
	*/
	CProtoDspCmdStorageSetValueCommand()
		: CProtoCommand( PVE::DspCmdStorageSetValue, false)
	{}
	/**
	* Class constructor.
	* @param user login
	* @param user password
	*/
	CProtoDspCmdStorageSetValueCommand(
		const QString &sKey,
		const QString &sValue,
		quint32 nReservedFlags = 0
		);

	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid();

	QString GetKey();
	QString GetValue();
};

class CProtoDspCmdVmStorageSetValueCommand: public CProtoBasicVmCommand
{
public:
	/**
	* Class default constructor.
	*/
	CProtoDspCmdVmStorageSetValueCommand()
		: CProtoBasicVmCommand( PVE::DspCmdVmStorageSetValue )
	{}
	/**
	* Class constructor.
	* @param user login
	* @param user password
	*/
	CProtoDspCmdVmStorageSetValueCommand(
		const QString &sVmUuid,
		const QString &sKey,
		const QString &sValue,
		quint32 nReservedFlags = 0
		);

	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid();

	QString GetKey();
	QString GetValue();
};

/**
 * Implementation of register 3rd party VM proto command
 */
class CProtoDspCmdDirReg3rdPartyVmCommand: public CProtoCommand
{
public:
	/**
	* Class default constructor.
	*/
	CProtoDspCmdDirReg3rdPartyVmCommand()
		: CProtoCommand( PVE::DspCmdDirReg3rdPartyVm, false, 0)
	{}
	/**
	* Class constructor.
	* @param path to VM configuration file
	* @param path to VM root directory
	*/
	CProtoDspCmdDirReg3rdPartyVmCommand(
		const QString &sVmConfigPath,
		const QString &sVmRootDirPath,
		quint32 nReservedFlags = 0
		);

	/** Overridden method that let to determine whether protocol command valid */
	bool IsValid();
	/** Returns path to VM configuration file parameter */
	QString GetPathToVmConfig();
	/** Returns path to VM root directory parameter */
	QString GetPathToVmRootDir();
};

class CProtoDspCmdNetworkPrefsChangedCommand: public CProtoCommand
{
public:
	CProtoDspCmdNetworkPrefsChangedCommand()
		: CProtoCommand( PVE::DspEvtNetworkPrefChanged, false, 0)
	{}
	CProtoDspCmdNetworkPrefsChangedCommand(
		const QString &sNetworkConfig,
		const QString &sOldNetworkID,
		const QString &sNewNetworkID,
		quint32 nFlags = 0
		);

	bool IsValid();
	QString GetNetworkConfig();
	QString GetOldNetworkID();
	QString GetNewNetworkID();
};

class CProtoVmInternalCommand: public CProtoBasicVmCommand
{
public:
	CProtoVmInternalCommand()
		: CProtoBasicVmCommand(PVE::DspCmdVmInternal)
	{}
	CProtoVmInternalCommand(
		const QString &sVmUuid,
		const QString &sCmd,
		const QStringList &lstArgs
		);

	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	QString GetCommandName();
	QStringList GetCommandArguments();
};

class CProtoVmCommitDiskUnfinished: public CProtoBasicVmCommand
{
public:
	CProtoVmCommitDiskUnfinished()
		: CProtoBasicVmCommand(PVE::DspCmdCtlVmCommitDiskUnfinished)
	{}
	CProtoVmCommitDiskUnfinished(
		const QString &sVmUuid,
		const QString &sDiskSysName,
		const QString &sSnapshotUuid
		);

	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	QString GetDiskSysName();
	QString GetSnapshotUuid();
};


/**
 * Serializer helper class that let to generate DspCmdCopyCtTemplate protocol command
 */
class CProtoCopyCtTemplateCommand: public CProtoCommandWithoutParams
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoCopyCtTemplateCommand()
	: CProtoCommandWithoutParams(PVE::DspCmdCopyCtTemplate)
	{}
	/**
	 * Class constructor.
	 * @param template name
	 * @param os template name
	 * @param target server hostname
	 * @param target server port
	 * @param target server session UUID
	 * @param command flags
	 * @param reserved command flags
	 */
	CProtoCopyCtTemplateCommand(
		const QString &sTmplName,
		const QString &sOsTmplName,
		const QString &sTargetServerHostname,
		quint32 nTargetServerPort,
		const QString &sTargetServerSessionUuid,
		quint32 nFlags,
		quint32 nReservedFlags);

	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	quint32 GetVersion();
	QString GetTmplName();
	QString GetOsTmplName();
	QString GetTargetServerHostname();
	quint32 GetTargetServerPort();
	QString GetTargetServerSessionUuid();
	quint32 GetReservedFlags();
};

/**
 * Serializer helper class that let to generate DspCmdDirVmMove protocol command
 */
class CProtoVmMoveCommand : public CProtoBasicVmCommand
{
public:
	/**
	 * Class default constructor.
	 */
	CProtoVmMoveCommand()
	: CProtoBasicVmCommand(PVE::DspCmdDirVmMove)
	{}
	/**
	 * Class constructor.
	 * @param XML representation string of VM configuration
	 * @param path to VM home dir
	 * @param sign whether VM template must be created
	 */
	CProtoVmMoveCommand(
		const QString &sVmUuid,
		const QString &sNewHomePath,
		PRL_UINT32 nFlags
	);
	/** Overridden method that let to determine whether protocol command valid */
	virtual bool IsValid();
	/** Returns new path to move vm/ct */
	QString GetNewHomePath();
};

/**
 * Serializer helper class that let to generate and process
 * CProtoSendProblemReport command
 */
class CProtoSendProblemReport : public CProtoBasicVmCommand
{
public:
    /**
     * Class default constructor.
     */
    CProtoSendProblemReport()
    : CProtoBasicVmCommand(PVE::DspCmdSendProblemReport)
    {}
    /**
     * Class constructor.
     * @param report's xml data
     * @param [optional] vmUuid which report should be attached
     * should be supplied if vm report should be also attached to
     * the report
     * @param flags being associated with the operation
     */
    CProtoSendProblemReport(
        const QString &report,
        const QString /*optional=QString()*/ &vmUuid,
        quint32 nFlags
    );
    /** Overridden method that let to determine whether protocol command valid */
    bool IsValid();

    /** Get stringified problem report data */
    QString GetReportData();
};


}//namespace Parallels

#endif

