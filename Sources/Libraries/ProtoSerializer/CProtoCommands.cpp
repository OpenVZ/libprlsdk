/*
 * CProtoCommands.cpp: Implementation of project protocol commands
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


#include "CProtoCommands.h"
#include "XmlModel/Messaging/CVmEventParameterList.h"
#include <prlcommon/Logging/Logging.h>
#include <prlcommon/PrlUuid/Uuid.h>
#include <prlcommon/Std/PrlAssert.h>


// By adding this interface we enable allocations tracing in the module
#include "Interfaces/Debug.h"


namespace Parallels
{

CProtoCommandBase::CProtoCommandBase(bool bForceQuestionsSign, quint32 nFlags)
: m_pProtoPackage(new CVmEvent)
{
	SetUnsignedIntParamValue( quint32(bForceQuestionsSign), EVT_PARAM_PROTO_FORCE_QUESTIONS_SIGN );
	SetUnsignedIntParamValue( nFlags, EVT_PARAM_PROTO_CMD_FLAGS );
}

QString CProtoCommandBase::GetVmUuid() const
{
	return (GetStringParamValue(EVT_PARAM_BASIC_VM_CMD_VM_UUID));
}

QString CProtoCommandBase::GetFirstStrParam()
{
	return (GetStringParamValue(EVT_PARAM_PROTO_FIRST_STR_PARAM));
}

SmartPtr<CVmEvent> CProtoCommandBase::GetCommand()
{
	return (m_pProtoPackage);
}

const SmartPtr<CVmEvent>& CProtoCommandBase::GetCommand() const
{
	return (m_pProtoPackage);
}

void CProtoCommandBase::ParsePackage(const QString &sCmdBuffer)
{
	m_pProtoPackage->fromString(sCmdBuffer);
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_PROTO_FORCE_QUESTIONS_SIGN);
	if (!pParam)
		m_pProtoPackage->addEventParameter(new CVmEventParameter(PVE::UnsignedInt,
			QString::number(quint32(false)), EVT_PARAM_PROTO_FORCE_QUESTIONS_SIGN));
	pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_PROTO_CMD_FLAGS);
	if (!pParam)
		m_pProtoPackage->addEventParameter(new CVmEventParameter(PVE::UnsignedInt,
			QString::number(quint32(0)), EVT_PARAM_PROTO_CMD_FLAGS));
}

bool CProtoCommandBase::GetForceQuestionsSign()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_PROTO_FORCE_QUESTIONS_SIGN));
}

quint32 CProtoCommandBase::GetCommandFlags()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_PROTO_CMD_FLAGS));
}

// protected //
bool CProtoCommandBase::CheckWhetherParamPresents(const QString &sParamName, PVE::ParamFieldDataType nParamType) const
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(sParamName);
	if (pParam)
		return (pParam->getParamType() == nParamType);
	return (false);
}

QString CProtoCommandBase::GetStringParamValue(const QString &sParamName) const
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(sParamName);
	if (pParam)
		return (pParam->getParamValue());
	return (QString());
}

quint32 CProtoCommandBase::GetUnsignedIntParamValue(const QString &sParamName) const
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(sParamName);
	if (pParam)
		return (pParam->getParamValue().toUInt());
	return (0);
}

quint64 CProtoCommandBase::GetUnsignedInt64ParamValue(const QString &sParamName) const
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(sParamName);
	if (pParam)
		return (pParam->getParamValue().toULongLong());
	return (0);
}

QStringList CProtoCommandBase::GetStringListParamValue(const QString &sParamName) const
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(sParamName);
	if (pParam)
		return (pParam->getValuesList());
	return (QStringList());
}

void CProtoCommandBase::SetStringParamValue(const QString &sParamValue, const QString &sParamName)
{
	// FIXME: NEED ENABLE PRL_ASSERT to fix UNSUPPORTED BEHAVIOUR !!!

	// CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(sParamName) ;
	// PRL_ASSERT( !pParam );
	// if (pParam)
	//	pParam->setParamValue(sParamValue) ;
	// else
	{
		CVmEventParameter *pParam = new CVmEventParameter(PVE::String, sParamValue, sParamName) ;
		m_pProtoPackage->addEventParameter(pParam) ;
	}
}

void CProtoCommandBase::SetStringListParamValue(const QStringList &lstParamValue, const QString &sParamName)
{
	QMutableListIterator<CVmEventParameter* > it( m_pProtoPackage->m_lstEventParameters );
	while( it.hasNext() )
	{
		CVmEventParameter* pParam = it.next();
		PRL_ASSERT(pParam->getParamName() != sParamName);
		if( pParam->getParamName() == sParamName )
		{
			it.remove();
			delete pParam;
		}
	}

	CVmEventParameter* pParam = new CVmEventParameterList(PVE::String
		, lstParamValue
		, sParamName);
	m_pProtoPackage->addEventParameter(pParam) ;
}

void CProtoCommandBase::SetUnsignedIntParamValue(quint32 nParamValue, const QString &sParamName)
{
	// FIXME: NEED ENABLE PRL_ASSERT to fix UNSUPPORTED BEHAVIOUR !!!

	//CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(sParamName) ;
	//PRL_ASSERT( !pParam );
	//if (pParam)
	//{
	//	PRL_ASSERT( pParam->getParamType() == PVE::UnsignedInt );
	//	if( pParam->getParamType() != PVE::UnsignedInt )
	//		pParam->setParamType( PVE::UnsignedInt );
	//	pParam->setParamValue( QString("%1").arg(nParamValue) ) ;
	//}
	//else
	{
		CVmEventParameter *pParam = new CVmEventParameter(PVE::UnsignedInt
			, QString("%1").arg(nParamValue)
			, sParamName) ;
		m_pProtoPackage->addEventParameter(pParam) ;
	}
}

void CProtoCommandBase::SetUnsignedInt64ParamValue(quint64 nParamValue, const QString &sParamName)
{
	CVmEventParameter *pParam = new CVmEventParameter(PVE::UInt64
			, QString("%1").arg(nParamValue)
			, sParamName) ;
	m_pProtoPackage->addEventParameter(pParam) ;
}

//+++++++++++++++++++++++++++++++++++++++DspCmdUserLogin command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCommandDspCmdUserLogin::CProtoCommandDspCmdUserLogin(
		const QString &sUsername,
		const QString &sPassword,
		const QString &sPrevSessionUuid,
                quint32 nFlags
	)
: CProtoCommand(PVE::DspCmdUserLogin, false, nFlags)
{
	SetStringParamValue( sUsername, EVT_PARAM_LOGIN_CMD_USERNAME );
	SetStringParamValue( sPassword, EVT_PARAM_LOGIN_CMD_PASSWORD );
	SetStringParamValue( sPrevSessionUuid, EVT_PARAM_LOGIN_CMD_SESSION_TO_RESTORE );
}

bool CProtoCommandDspCmdUserLogin::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_LOGIN_CMD_USERNAME, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_LOGIN_CMD_PASSWORD, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_LOGIN_CMD_SESSION_TO_RESTORE, PVE::String);
}

QString CProtoCommandDspCmdUserLogin::GetUserLoginName()
{
	return (GetStringParamValue(EVT_PARAM_LOGIN_CMD_USERNAME));
}

QString CProtoCommandDspCmdUserLogin::GetPassword()
{
	return (GetStringParamValue(EVT_PARAM_LOGIN_CMD_PASSWORD));
}

QString CProtoCommandDspCmdUserLogin::GetPrevSessionUuid()
{
	return ( GetStringParamValue( EVT_PARAM_LOGIN_CMD_SESSION_TO_RESTORE ) );
}

//++++++++++++++++++++++++++++++++++DspCmdUserLoginLocal command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCommandDspCmdUserLoginLocal::CProtoCommandDspCmdUserLoginLocal(
		quint32 nUserId,
		PRL_APPLICATION_MODE appMode,
		quint64	nProcessId,
		quint32 flags
)
: CProtoCommand(PVE::DspCmdUserLoginLocal, false, flags)
{
	SetUnsignedIntParamValue(nUserId, EVT_PARAM_LOGIN_LOCAL_CMD_USER_ID);
	SetUnsignedIntParamValue(appMode, EVT_PARAM_LOGIN_LOCAL_CMD_APPLICATION_MODE);
	SetUnsignedInt64ParamValue(nProcessId, EVT_PARAM_LOGIN_LOCAL_CMD_PROCESS_ID);
}

bool CProtoCommandDspCmdUserLoginLocal::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_LOGIN_LOCAL_CMD_USER_ID, PVE::UnsignedInt));
}

quint32 CProtoCommandDspCmdUserLoginLocal::GetUserId()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_LOGIN_LOCAL_CMD_USER_ID));
}

quint64 CProtoCommandDspCmdUserLoginLocal::GetProcessId()
{
	return (GetUnsignedInt64ParamValue(EVT_PARAM_LOGIN_LOCAL_CMD_PROCESS_ID));
}

PRL_APPLICATION_MODE CProtoCommandDspCmdUserLoginLocal::GetApplicationMode()
{
	PRL_APPLICATION_MODE mode =
		static_cast<PRL_APPLICATION_MODE>(GetUnsignedIntParamValue(EVT_PARAM_LOGIN_LOCAL_CMD_APPLICATION_MODE));

	switch(mode)
	{
	case PAM_SERVER:
		return mode;
	default:
		return PAM_SERVER;
	}
}

//+++++++++++++++++++++++++++++++Proto command without params class implementation+++++++++++++++++++++++++++++++++++++
bool CProtoCommandWithoutParams::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_PROTO_FORCE_QUESTIONS_SIGN, PVE::UnsignedInt) &&
			CheckWhetherParamPresents(EVT_PARAM_PROTO_CMD_FLAGS, PVE::UnsignedInt));
}

//+++++++++++++++++++++++++++++++Proto command with access token and one string parameter class implementation+++++++++
CProtoCommandWithOneStrParam::CProtoCommandWithOneStrParam(
										PVE::IDispatcherCommands nCmdIdentifier,
										const QString &sFirstParam,
										bool bForceQuestionsSign,
										quint32 nFlags)
: CProtoCommandWithoutParams(nCmdIdentifier, bForceQuestionsSign, nFlags)
{
	SetStringParamValue( sFirstParam, EVT_PARAM_PROTO_FIRST_STR_PARAM );
}

bool CProtoCommandWithOneStrParam::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_PROTO_FIRST_STR_PARAM, PVE::String) &&
					CProtoCommandWithoutParams::IsValid());
}

//+++++++++++++++++++++++++++++++Proto command with two string params implementation+++++++++++++++++++++++++
CProtoCommandWithTwoStrParams::CProtoCommandWithTwoStrParams(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sParam1,
	const QString &sParam2,
	bool bForceQuestionsSign,
	quint32 nFlags
	)
	: CProtoCommandWithOneStrParam( nCmdIdentifier, sParam1, bForceQuestionsSign, nFlags )
{
	SetStringParamValue( sParam2, EVT_PARAM_PROTO_SECOND_STR_PARAM );
}

bool CProtoCommandWithTwoStrParams::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_PROTO_SECOND_STR_PARAM, PVE::String) &&
		CProtoCommandWithOneStrParam::IsValid());
}

QString CProtoCommandWithTwoStrParams::GetSecondStrParam()
{
	return GetStringParamValue(EVT_PARAM_PROTO_SECOND_STR_PARAM);
}

//+++++++++++++++++++++++++++++++DspCmdDirVmCreate proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoVmCreateCommand::CProtoVmCreateCommand(
	const QString &sVmConfig,
	const QString &sVmHomePath,
	quint32 nFlags
)
: CProtoCommandWithoutParams(PVE::DspCmdDirVmCreate, (nFlags & PACF_NON_INTERACTIVE_MODE), nFlags)
{
	AddOwnParams(sVmConfig, sVmHomePath);
}

CProtoVmCreateCommand::CProtoVmCreateCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmConfig,
	const QString &sVmHomePath
)
: CProtoCommandWithoutParams(nCmdIdentifier)
{
	AddOwnParams(sVmConfig, sVmHomePath);
}

void CProtoVmCreateCommand::AddOwnParams(const QString &sVmConfig, const QString &sVmHomePath)
{
	SetStringParamValue( sVmConfig, EVT_PARAM_VM_CREATE_CMD_VM_CONFIG );
	SetStringParamValue( sVmHomePath, EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH );
}

bool CProtoVmCreateCommand::IsValid()
{
	return (	CheckWhetherParamPresents(EVT_PARAM_VM_CREATE_CMD_VM_CONFIG, PVE::String) &&
				CheckWhetherParamPresents(EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH, PVE::String) &&
				CProtoCommandWithoutParams::IsValid());
}

QString CProtoVmCreateCommand::GetVmConfig()
{
	return (GetStringParamValue(EVT_PARAM_VM_CREATE_CMD_VM_CONFIG));
}

QString CProtoVmCreateCommand::GetVmHomePath()
{
	return (GetStringParamValue(EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH));
}

//+++++++++++++++++++++++++++++++DspCmdDirVmDelete proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoVmDeleteCommand::CProtoVmDeleteCommand(
	const QString &sVmUuid,
	const QStringList& lstDevices
	)
	: CProtoBasicVmCommand(PVE::DspCmdDirVmDelete, sVmUuid)
{
	SetStringListParamValue( lstDevices, EVT_PARAM_VM_DELETE_CMD_VM_DEVICES_LIST );
}

bool CProtoVmDeleteCommand::IsValid()
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_VM_DELETE_CMD_VM_DEVICES_LIST);
	return (pParam && pParam->isList() && pParam->getParamType() == PVE::String &&
		CProtoBasicVmCommand::IsValid());
}

QStringList CProtoVmDeleteCommand::GetVmDevicesList()
{
	return (GetStringListParamValue(EVT_PARAM_VM_DELETE_CMD_VM_DEVICES_LIST));
}

//+++++++++++++++++++++++++++++++DspCmdDirCreateImage proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCreateImageCommand::CProtoCreateImageCommand(
	const QString &sVmUuid,
	const QString &sImageConfig,
	bool bRecreateIsAllowed,
	bool bForceQuestionsSign
)
: CProtoBasicVmCommand(PVE::DspCmdDirCreateImage, sVmUuid, bForceQuestionsSign)
{
	SetStringParamValue( sImageConfig, EVT_PARAM_CREATE_IMAGE_CMD_IMAGE_CONFIG );
	SetUnsignedIntParamValue((quint32)bRecreateIsAllowed, EVT_PARAM_CREATE_IMAGE_CMD_RECREATE_SIGN );
}

bool CProtoCreateImageCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_CREATE_IMAGE_CMD_IMAGE_CONFIG, PVE::String) &&
					CheckWhetherParamPresents(EVT_PARAM_CREATE_IMAGE_CMD_RECREATE_SIGN, PVE::UnsignedInt) &&
					CProtoBasicVmCommand::IsValid());
}

QString CProtoCreateImageCommand::GetImageConfig()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_IMAGE_CMD_IMAGE_CONFIG));
}

bool CProtoCreateImageCommand::IsRecreateAllowed()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_CREATE_IMAGE_CMD_RECREATE_SIGN));
}

//+++++++++++++++++++++++++++++++DspCmdDirCopyImage proto command class implementation+++++++++++++++++++++
CProtoCopyImageCommand::CProtoCopyImageCommand(
	const QString &sVmUuid,
	const QString &sImageConfig,
	const QString &sNewImageName,
	const QString &sTargetPath,
	quint32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdDirCopyImage, sVmUuid, false, nFlags)
{
	SetStringParamValue( sImageConfig, EVT_PARAM_COPY_IMAGE_CMD_IMAGE_CONFIG );
	SetStringParamValue( sNewImageName, EVT_PARAM_COPY_IMAGE_CMD_NEW_IMAGE_NAME );
	SetStringParamValue( sTargetPath, EVT_PARAM_COPY_IMAGE_CMD_TARGET_PATH );
}

bool CProtoCopyImageCommand::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_COPY_IMAGE_CMD_IMAGE_CONFIG, PVE::String)
			&& CheckWhetherParamPresents(EVT_PARAM_COPY_IMAGE_CMD_NEW_IMAGE_NAME, PVE::String)
			&& CheckWhetherParamPresents(EVT_PARAM_COPY_IMAGE_CMD_TARGET_PATH, PVE::String)
			&& CProtoBasicVmCommand::IsValid();
}

QString CProtoCopyImageCommand::GetImageConfig()
{
	return GetStringParamValue(EVT_PARAM_COPY_IMAGE_CMD_IMAGE_CONFIG);
}

QString CProtoCopyImageCommand::GetNewImageName()
{
	return GetStringParamValue(EVT_PARAM_COPY_IMAGE_CMD_NEW_IMAGE_NAME);
}

QString CProtoCopyImageCommand::GetTargetPath()
{
	return GetStringParamValue(EVT_PARAM_COPY_IMAGE_CMD_TARGET_PATH);
}

//+++++++++++++++++++++++++++++++DspCmdFsRenameEntry proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoFsRenameEntryCommand::CProtoFsRenameEntryCommand(
	const QString &sOldEntryName,
	const QString &sNewEntryName
)
: CProtoCommandWithoutParams(PVE::DspCmdFsRenameEntry)
{
	SetStringParamValue( sOldEntryName, EVT_PARAM_FS_RENAME_ENTRY_CMD_OLD_ENTRY_NAME );
	SetStringParamValue( sNewEntryName, EVT_PARAM_FS_RENAME_ENTRY_CMD_NEW_ENTRY_NAME );
}

bool CProtoFsRenameEntryCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_FS_RENAME_ENTRY_CMD_OLD_ENTRY_NAME, PVE::String) &&
					CheckWhetherParamPresents(EVT_PARAM_FS_RENAME_ENTRY_CMD_NEW_ENTRY_NAME, PVE::String) &&
					CProtoCommandWithoutParams::IsValid());
}

QString CProtoFsRenameEntryCommand::GetOldEntryName()
{
	return (GetStringParamValue(EVT_PARAM_FS_RENAME_ENTRY_CMD_OLD_ENTRY_NAME));
}

QString CProtoFsRenameEntryCommand::GetNewEntryName()
{
	return (GetStringParamValue(EVT_PARAM_FS_RENAME_ENTRY_CMD_NEW_ENTRY_NAME));
}

//+++++++++++++++++++++++++++++++Proto command with serial number class implementation+++++++++++++++++++++++++++++++++++++++
CProtoSerialNumCommand::CProtoSerialNumCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sUserName,
	const QString &sCompanyName,
	const QString &sSerialNumber,
	quint32 nFlags
)
: CProtoCommandWithoutParams(nCmdIdentifier, false, nFlags )
{
	SetStringParamValue( sUserName, EVT_PARAM_SERIAL_NUM_CMD_USER_NAME );
	SetStringParamValue( sCompanyName, EVT_PARAM_SERIAL_NUM_CMD_COMPANY_NAME );
	SetStringParamValue( sSerialNumber, EVT_PARAM_SERIAL_NUM_CMD_SERIAL_NUMBER );
}

bool CProtoSerialNumCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_SERIAL_NUM_CMD_USER_NAME, PVE::String) &&
					CheckWhetherParamPresents(EVT_PARAM_SERIAL_NUM_CMD_COMPANY_NAME, PVE::String) &&
					CheckWhetherParamPresents(EVT_PARAM_SERIAL_NUM_CMD_SERIAL_NUMBER, PVE::String) &&
					CProtoCommandWithoutParams::IsValid());
}

QString CProtoSerialNumCommand::GetUserLoginName()
{
	return (GetStringParamValue(EVT_PARAM_SERIAL_NUM_CMD_USER_NAME));
}

QString CProtoSerialNumCommand::GetCompanyName()
{
	return (GetStringParamValue(EVT_PARAM_SERIAL_NUM_CMD_COMPANY_NAME));
}

QString CProtoSerialNumCommand::GetSerialNumber()
{
	return (GetStringParamValue(EVT_PARAM_SERIAL_NUM_CMD_SERIAL_NUMBER));
}

//++++++++++++++++++++++++++++++++++++CProtoCreateUnattendedCdCommand class implementation+++++++++++++++++++++++++++++
CProtoCreateUnattendedCdCommand::CProtoCreateUnattendedCdCommand(
		quint32 nGuestType,
		const QString &sUserName,
		const QString &sPasswd,
		const QString &sFullUserName,
		const QString &sOsDistroPath,
		const QString &sOutImagePath
)
: CProtoCommandWithoutParams(PVE::DspCmdCreateUnattendedCd)
{
	SetUnsignedIntParamValue(nGuestType, EVT_PARAM_CREATE_UNATTENDED_CD_GUEST_TYPE);
	SetStringParamValue(sUserName, EVT_PARAM_CREATE_UNATTENDED_CD_USERNAME);
	SetStringParamValue(sPasswd, EVT_PARAM_CREATE_UNATTENDED_CD_PASSWORD);
	SetStringParamValue(sFullUserName, EVT_PARAM_CREATE_UNATTENDED_CD_FULL_USERNAME);
	SetStringParamValue(sOsDistroPath, EVT_PARAM_CREATE_UNATTENDED_CD_OS_DISTROE_PATH);
	SetStringParamValue(sOutImagePath, EVT_PARAM_CREATE_UNATTENDED_CD_OUT_IMAGE_PATH);
}

bool CProtoCreateUnattendedCdCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_CREATE_UNATTENDED_CD_GUEST_TYPE, PVE::UnsignedInt) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_UNATTENDED_CD_USERNAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_UNATTENDED_CD_PASSWORD, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_UNATTENDED_CD_FULL_USERNAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_UNATTENDED_CD_OS_DISTROE_PATH, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_UNATTENDED_CD_OUT_IMAGE_PATH, PVE::String) &&
			CProtoCommandWithoutParams::IsValid());
}

quint32 CProtoCreateUnattendedCdCommand::GetGuestType()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_CREATE_UNATTENDED_CD_GUEST_TYPE));
}

QString CProtoCreateUnattendedCdCommand::GetUserLoginName()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_UNATTENDED_CD_USERNAME));
}

QString CProtoCreateUnattendedCdCommand::GetPasswd()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_UNATTENDED_CD_PASSWORD));
}

QString CProtoCreateUnattendedCdCommand::GetFullUserName()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_UNATTENDED_CD_FULL_USERNAME));
}

QString CProtoCreateUnattendedCdCommand::GetOsDistroPath()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_UNATTENDED_CD_OS_DISTROE_PATH));
}

QString CProtoCreateUnattendedCdCommand::GetOutImagePath()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_UNATTENDED_CD_OUT_IMAGE_PATH));
}

//+++++++++++++++++++++++++++++++DspCmdVmCreateUnattendedFloppy proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCreateUnattendedFloppyCommand::CProtoCreateUnattendedFloppyCommand(
	const QString &sVmUuid,
	const QString &sUserName,
	const QString &sCompanyName,
	const QString &sSerialNumber,
	PRL_GUEST_DISTRO_VERSION_ENUM nGuestType
)
: CProtoSerialNumCommand(PVE::DspCmdVmCreateUnattendedFloppy, sUserName, sCompanyName, sSerialNumber)
{
	SetStringParamValue( sVmUuid, EVT_PARAM_BASIC_VM_CMD_VM_UUID );
	SetUnsignedIntParamValue(quint32(nGuestType), EVT_PARAM_CREATE_UNATTENDED_FLOPPY_CMD_GUEST_DISTRO_TYPE );
}

bool CProtoCreateUnattendedFloppyCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_BASIC_VM_CMD_VM_UUID, PVE::String) &&
					CheckWhetherParamPresents(EVT_PARAM_CREATE_UNATTENDED_FLOPPY_CMD_GUEST_DISTRO_TYPE, PVE::UnsignedInt) &&
					CProtoSerialNumCommand::IsValid());
}

PRL_GUEST_DISTRO_VERSION_ENUM CProtoCreateUnattendedFloppyCommand::GetGuestDistroType()
{
	return ((PRL_GUEST_DISTRO_VERSION_ENUM)GetUnsignedIntParamValue(EVT_PARAM_CREATE_UNATTENDED_FLOPPY_CMD_GUEST_DISTRO_TYPE));
}

//+++++++++++++++++++++++++++++++DspCmdStartSearchConfig proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoStartSearchConfigCommand::CProtoStartSearchConfigCommand(
	const QStringList &lstSearchDirs
)
: CProtoCommandWithoutParams(PVE::DspCmdStartSearchConfig)
{
	SetStringListParamValue( lstSearchDirs, EVT_PARAM_START_SEARCH_CONFIG_CMD_SEARCH_DIRS );
}

bool CProtoStartSearchConfigCommand::IsValid()
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_START_SEARCH_CONFIG_CMD_SEARCH_DIRS);
	return (pParam && pParam->isList() && pParam->getParamType() == PVE::String &&
					CProtoCommandWithoutParams::IsValid());
}

QStringList CProtoStartSearchConfigCommand::GetSearchDirs()
{
	return (GetStringListParamValue(EVT_PARAM_START_SEARCH_CONFIG_CMD_SEARCH_DIRS));
}

//+++++++++++++++++++++++++++++++DspCmdDirVmClone proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoVmCloneCommand::CProtoVmCloneCommand(
	const QString &sVmUuid,
	const QString &sVmName,
	const QString &sNewVmUuid,
	const QString &sVmHomePath,
	PRL_UINT32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdDirVmClone, sVmUuid, false, nFlags)
{
	SetStringParamValue( sVmName, EVT_PARAM_VM_CLONE_CMD_VM_NAME);
	if (!sNewVmUuid.isEmpty())
		SetStringParamValue( sNewVmUuid, EVT_PARAM_VM_CLONE_CMD_VM_UUID);
	SetUnsignedIntParamValue( quint32(nFlags & PCVF_CLONE_TO_TEMPLATE), EVT_PARAM_VM_CLONE_CMD_CREATE_TEMPLATE );
	SetStringParamValue( sVmHomePath, EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH );
}

bool CProtoVmCloneCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_CLONE_CMD_VM_NAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_VM_CLONE_CMD_CREATE_TEMPLATE, PVE::UnsignedInt) &&
			CheckWhetherParamPresents(EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH, PVE::String) &&
			CProtoBasicVmCommand::IsValid());
}

QString CProtoVmCloneCommand::GetVmName()
{
	return (GetStringParamValue(EVT_PARAM_VM_CLONE_CMD_VM_NAME));
}

QString CProtoVmCloneCommand::GetVmHomePath()
{
	return (GetStringParamValue(EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH));
}

bool CProtoVmCloneCommand::IsCreateTemplate()
{
	return ((bool)GetUnsignedIntParamValue(EVT_PARAM_VM_CLONE_CMD_CREATE_TEMPLATE));
}

QString CProtoVmCloneCommand::GetNewVmUuid()
{
	return (GetStringParamValue(EVT_PARAM_VM_CLONE_CMD_VM_UUID));
}

//+++++++++++++++++++++++++++++++++++++++Basic VM command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoBasicVmCommand::CProtoBasicVmCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	bool bForceQuestionsSign,
	quint32 nFlags
)
: CProtoCommandWithoutParams(nCmdIdentifier, bForceQuestionsSign, nFlags)
{
	SetStringParamValue( sVmUuid, EVT_PARAM_BASIC_VM_CMD_VM_UUID );
}

bool CProtoBasicVmCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_BASIC_VM_CMD_VM_UUID, PVE::String) &&
					CProtoCommandWithoutParams::IsValid());
}

//+++++++++++++++++++++++++++++++++++++++VM command with ACPI sign class implementation+++++++++++++++++++++++++++++++++++++++
CProtoVmCommandWithAcpiSign::CProtoVmCommandWithAcpiSign(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	bool bUseAcpi
)
: CProtoBasicVmCommand(nCmdIdentifier, sVmUuid)
{
	m_pProtoPackage->addEventParameter( new CVmEventParameter( PVE::Boolean, QString("%1").arg(bUseAcpi), EVT_PARAM_VM_CMD_WITH_ACPI_SIGN_IS_USE_ACPI ));
}

bool CProtoVmCommandWithAcpiSign::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_CMD_WITH_ACPI_SIGN_IS_USE_ACPI, PVE::Boolean) &&
					CProtoBasicVmCommand::IsValid());
}

bool CProtoVmCommandWithAcpiSign::IsUseAcpi()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_VM_CMD_WITH_ACPI_SIGN_IS_USE_ACPI));
}

//+++++++++++++++++++++++++++++++++++++++VM stop command  class implementation+++++++++++++++++++++++++++++++++++++++
CProtoVmCommandStop::CProtoVmCommandStop(
	const QString &sVmUuid,
	PRL_UINT32 nStopMode,
	PRL_UINT32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdVmStop, sVmUuid, false, nFlags)
{
	m_pProtoPackage->addEventParameter( new CVmEventParameter( PVE::UnsignedInt,
						QString("%1").arg(nStopMode),
						EVT_PARAM_VM_CMD_STOP_MODE));
	// Compatobility issue for old client processing CProtoVmCommandWithAcpiSign
	m_pProtoPackage->addEventParameter( new CVmEventParameter( PVE::Boolean,
				QString("%1").arg(((nStopMode & PRL_VM_STOP_MODE_MASK) != PSM_KILL)),
				EVT_PARAM_VM_CMD_WITH_ACPI_SIGN_IS_USE_ACPI ));
}

bool CProtoVmCommandStop::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_VM_CMD_WITH_ACPI_SIGN_IS_USE_ACPI, PVE::Boolean) &&
					CProtoBasicVmCommand::IsValid();
}

PRL_UINT32 CProtoVmCommandStop::GetStopMode()
{
	if (CheckWhetherParamPresents(EVT_PARAM_VM_CMD_STOP_MODE, PVE::UnsignedInt))
	{
		PRL_UINT32 nStopFlags = GetCommandFlags();
		PRL_UINT32 nStopMode = GetUnsignedIntParamValue(EVT_PARAM_VM_CMD_STOP_MODE);

		return ((nStopMode & PRL_VM_STOP_MODE_MASK) | (nStopFlags & ~PRL_VM_STOP_MODE_MASK));
	}
	else
		// Processing old schema
		return GetUnsignedIntParamValue(EVT_PARAM_VM_CMD_WITH_ACPI_SIGN_IS_USE_ACPI) ? PSM_SHUTDOWN : PSM_KILL;
}

//+++++++++++++++++++++++++++++++++++++++VM command with one string param implementation++++++++++++++++++++++++++
CProtoVmCommandWithOneStrParam::CProtoVmCommandWithOneStrParam(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	const QString &sParam,
	quint32 nFlags
	)
	: CProtoBasicVmCommand(nCmdIdentifier, sVmUuid, false, nFlags)
{
	SetStringParamValue( sParam, EVT_PARAM_PROTO_FIRST_STR_PARAM );
}

bool CProtoVmCommandWithOneStrParam::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_PROTO_FIRST_STR_PARAM, PVE::String) &&
		CProtoBasicVmCommand::IsValid());
}

//+++++++++++++++++++++++++++++++++++++++VM command with two string params implementation+++++++++++++++++++++++++
CProtoVmCommandWithTwoStrParams::CProtoVmCommandWithTwoStrParams(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	const QString &sParam1,
	const QString &sParam2,
	quint32 nFlags
	)
	: CProtoVmCommandWithOneStrParam( nCmdIdentifier, sVmUuid, sParam1, nFlags )
{
	SetStringParamValue( sParam2, EVT_PARAM_VM_CMD_SECOND_STR_PARAM );
}

bool CProtoVmCommandWithTwoStrParams::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_CMD_SECOND_STR_PARAM, PVE::String) &&
		CProtoVmCommandWithOneStrParam::IsValid());
}

QString CProtoVmCommandWithTwoStrParams::GetSecondStrParam()
{
	return GetStringParamValue(EVT_PARAM_VM_CMD_SECOND_STR_PARAM);
}

//+++++++++++++++++++++++++++++++++++++++Update VM security command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoVmUpdateSecurityCommand::CProtoVmUpdateSecurityCommand(
	const QString &sVmUuid,
	const QString &sVmSecurity
)
: CProtoBasicVmCommand(PVE::DspCmdVmUpdateSecurity, sVmUuid)
{
	SetStringParamValue( sVmSecurity, EVT_PARAM_VM_UPDATE_SECURITY_CMD_SECURITY_INFO );
}

bool CProtoVmUpdateSecurityCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_UPDATE_SECURITY_CMD_SECURITY_INFO, PVE::String) &&
					CProtoBasicVmCommand::IsValid());
}

QString CProtoVmUpdateSecurityCommand::GetVmSecurity()
{
	return (GetStringParamValue(EVT_PARAM_VM_UPDATE_SECURITY_CMD_SECURITY_INFO));
}

//+++++++++++++++++++++++++++++++++++++++VM device command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoVmDeviceCommand::CProtoVmDeviceCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	PRL_DEVICE_TYPE nDevType,
	quint32 nDevIndex,
	const QString &sDevConfig,
	PRL_UINT32 nFlags
)
: CProtoBasicVmCommand(nCmdIdentifier, sVmUuid, false, nFlags)
{
	SetUnsignedIntParamValue(quint32(nDevType), EVT_PARAM_VM_DEV_CMD_DEVICE_TYPE );
	SetUnsignedIntParamValue(nDevIndex, EVT_PARAM_VM_DEV_CMD_DEVICE_INDEX );
	SetStringParamValue( sDevConfig, EVT_PARAM_VM_DEV_CMD_DEVICE_CONFIG );
}

bool CProtoVmDeviceCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_DEV_CMD_DEVICE_TYPE, PVE::UnsignedInt) &&
					CheckWhetherParamPresents(EVT_PARAM_VM_DEV_CMD_DEVICE_INDEX, PVE::UnsignedInt) &&
					CheckWhetherParamPresents(EVT_PARAM_VM_DEV_CMD_DEVICE_CONFIG, PVE::String) &&
					CProtoBasicVmCommand::IsValid());
}

PRL_DEVICE_TYPE CProtoVmDeviceCommand::GetDeviceType()
{
	return ((PRL_DEVICE_TYPE)GetUnsignedIntParamValue(EVT_PARAM_VM_DEV_CMD_DEVICE_TYPE));
}

quint32 CProtoVmDeviceCommand::GetDeviceIndex()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_VM_DEV_CMD_DEVICE_INDEX));
}

QString CProtoVmDeviceCommand::GetDeviceConfig()
{
	return (GetStringParamValue(EVT_PARAM_VM_DEV_CMD_DEVICE_CONFIG));
}

//+++++++++++++++++++++++++++++++++++++++DspWsResponse command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCommandDspWsResponse::CProtoCommandDspWsResponse(
		PVE::IDispatcherCommands nRequestOpCode,
		PRL_RESULT nRetCode
	)
: CProtoCommand(PVE::DspWsResponse)
{
	SetUnsignedIntParamValue((quint32)nRequestOpCode, EVT_PARAM_PROTO_REQUEST_OP_CODE );
	SetRetCode(nRetCode);
}

#define FILL_EVT_STRING_PARAM_VALUE(param_name, param_value) \
    set_param_value(param_name, param_value) ;

CVmEventParameter* CProtoCommandDspWsResponse::set_param_value(const QString &param_name, const QString &param_value)
{
    CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(param_name) ;
	if (pParam)
		pParam->setParamValue(param_value) ;
    else {
		pParam = new CVmEventParameter(PVE::String, param_value, param_name) ;
		m_pProtoPackage->addEventParameter(pParam) ;
	}
	return pParam ;
}

void CProtoCommandDspWsResponse::SetHostHardwareInfo(const QString &sHostHardwareInfo)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_HOST_HARDWARE_INFO, sHostHardwareInfo)
}

void CProtoCommandDspWsResponse::SetHwFileSystemInfo(const QString &sHwFileSystemInfo)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_HW_FILE_SYSTEM_INFO, sHwFileSystemInfo)
}

void CProtoCommandDspWsResponse::SetUserProfile(const QString &sUserProfile)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_USER_PROFILE, sUserProfile)
}

void CProtoCommandDspWsResponse::SetUserInfo(const QString &sUserInfo)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_USER_INFO, sUserInfo)
}

void CProtoCommandDspWsResponse::SetHostCommonInfo(const QString &sHostCommonInfo, const QString& sNetworkInfo)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_HOST_COMMON_INFO, sHostCommonInfo)
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_HOST_COMMON_INFO_NETWORK_CONFIG, sNetworkInfo)
}

void CProtoCommandDspWsResponse::SetProblemReport(const QString &sProblemReport)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_PROBLEM_REPORT, sProblemReport)
}

void CProtoCommandDspWsResponse::SetSnapshotsTree(const QString &sSnapshotsTree)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_GET_SNAPSHOTS_TREE, sSnapshotsTree)
}

void CProtoCommandDspWsResponse::SetBackupsTree(const QString &sBackupsTree)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_GET_BACKUPS_TREE, sBackupsTree)
}

void CProtoCommandDspWsResponse::SetVmEvent(const QString &sVmEvent)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_VM_EVENT, sVmEvent)
}

void CProtoCommandDspWsResponse::SetVmConfig(const QString &sVmConfig)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_VM_CONFIG, sVmConfig)
}

void CProtoCommandDspWsResponse::SetSystemStatistics(const QString &sSystemStatistics)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_SYSTEM_STATISTICS, sSystemStatistics)
}

void CProtoCommandDspWsResponse::SetError(const QString &sError)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_ERROR, sError)
}

void CProtoCommandDspWsResponse::SetVmDevice(const QString &sVmDevice)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_VM_DEVICE, sVmDevice)
}

void CProtoCommandDspWsResponse::SetNetServiceStatus(const QString &sNetServiceStatus)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_NET_SERVICE_STATUS, sNetServiceStatus)
}

void CProtoCommandDspWsResponse::SetLoginResponse(const QString &sLoginEvent)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_LOGIN_EVENT, sLoginEvent)
}

void CProtoCommandDspWsResponse::SetVmSecurity(const QString &sVmSecurity)
{
	FILL_EVT_STRING_PARAM_VALUE(EVT_PARAM_WS_RESPONSE_CMD_SECURITY_INFO, sVmSecurity)
}

void CProtoCommandDspWsResponse::SetParamsList(const QStringList &lstParams)
{
	for (QStringList::const_iterator it = lstParams.begin(); it != lstParams.end(); ++it)
		SetStringParamValue( *it, EVT_PARAM_WS_RESPONSE_CMD_STANDARD_PARAM	);
}

void CProtoCommandDspWsResponse::SetRetCode(PRL_RESULT nRetCode)
{
	m_pProtoPackage->setEventCode(nRetCode);
}

PRL_RESULT CProtoCommandDspWsResponse::GetRetCode()
{
	return (m_pProtoPackage->getEventCode());
}

#define INITIALIZE_RESULT_HASH_VALUE(pResult, param_name, hash_id)\
	{\
		CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(param_name);\
		if (pParam)\
			pResult->m_hashResultSet[hash_id] = pParam->getParamValue();\
	}

void CProtoCommandDspWsResponse::FillResult(CResult *pResult)
{
	pResult->setReturnCode(m_pProtoPackage->getEventCode());
	pResult->setRequestId(m_pProtoPackage->getInitRequestId());
	pResult->setOpCode((PVE::IDispatcherCommands)GetUnsignedIntParamValue(EVT_PARAM_PROTO_REQUEST_OP_CODE));
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_HOST_HARDWARE_INFO, PVE::DspCmdUserGetHostHwInfo_strHostHwInfo)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_USER_PROFILE, PVE::DspCmdUserGetProfile_strUserProfile)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_USER_INFO, PVE::DspCmdUserInfo_strUserInfo)

	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_HOST_COMMON_INFO, PVE::DspCmdGetHostCommonInfo_strCommonInfo)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_HOST_COMMON_INFO_NETWORK_CONFIG \
			, PVE::DspCmdGetHostCommonInfo_strNetworkInfo);

	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_PROBLEM_REPORT, PVE::DspCmdVmGetProblemReport_strReport)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_GET_SNAPSHOTS_TREE, PVE::DspCmdVmGetSnapshotsTree_strSnapshotsTree)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_GET_BACKUPS_TREE,
								PVE::DspCmdGetBackupTree_strBackupsTree)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_NET_SERVICE_STATUS, PVE::DspCmdGetNetServiceStatus_strNetServiceStatus)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_LOGIN_EVENT, PVE::DspCmdUserLogin_strLoginInfo)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_LOGIN_EVENT, PVE::DspCmdUserLoginLocalStage2_strLoginInfo)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_SECURITY_INFO, PVE::DspCmdVmUpdateSecurity_strSecurityInfo)
	INITIALIZE_RESULT_HASH_VALUE(pResult, EVT_PARAM_WS_RESPONSE_CMD_GET_PERFSTATS,
                                 PVE::DspCmdPerfomanceStatistics_strPerfStats)
	FillFsCommandsParams(pResult);
	FillVmConfigParam(pResult);
	FillVmEventParam(pResult);
	FillSystemStatisticsParam(pResult);
	FillDeviceInfo(pResult);
	//Fills result object with standard params
	foreach(CVmEventParameter *pParam, m_pProtoPackage->m_lstEventParameters)
		if (pParam->getParamName() == EVT_PARAM_WS_RESPONSE_CMD_STANDARD_PARAM)
			pResult->addParamToken(pParam->getParamValue());
	FillErrorData(pResult);
}

namespace {

/**
 * Returns corresponding result hash id for specified operation code
 */
PVE::IDspMethodsArgs GetHashIdByOpCode(PVE::IDispatcherCommands nOpCode)
{
	switch (nOpCode)
	{
		//case PVE::DspCmdUserLogin: return (PVE::DspCmdUserLogin_strAccessToken);
		case PVE::DspCmdUserGetHostHwInfo: return (PVE::DspCmdUserGetHostHwInfo_strHostHwInfo);
		case PVE::DspCmdFsGetDiskList: return (PVE::DspCmdFsGetDiskList_strDiskList);
		case PVE::DspCmdFsGetCurrentDirectory: return (PVE::DspCmdFsGetCurrentDirectory_strDir);
		case PVE::DspCmdFsGetDirectoryEntries: return (PVE::DspCmdFsGetDirectoryEntries_strEntriesList);
		case PVE::DspCmdFsGetFileList: return (PVE::DspCmdFsGetFileList_strFileList);
		case PVE::DspCmdFsCreateDirectory: return (PVE::DspCmdFsCreateDirectory_strDirEntries);
		case PVE::DspCmdFsRenameEntry: return (PVE::DspCmdFsRenameEntry_strDirEntries);
		case PVE::DspCmdFsRemoveEntry: return (PVE::DspCmdFsRemoveEntry_strDirEntries);
		case PVE::DspCmdUserGetProfile: return (PVE::DspCmdUserGetProfile_strUserProfile);
		case PVE::DspCmdUserInfo: return (PVE::DspCmdUserInfo_strUserInfo);
		case PVE::DspCmdGetHostCommonInfo: return (PVE::DspCmdGetHostCommonInfo_strCommonInfo);
		case PVE::DspCmdVmGetProblemReport: return (PVE::DspCmdVmGetProblemReport_strReport);
		case PVE::DspCmdVmGetPackedProblemReport: return (PVE::DspCmdVmGetProblemReport_strReport);
        case PVE::DspCmdSendProblemReport: return (PVE::DspCmdSendProblemReport_strReport);
		case PVE::DspCmdVmGetSnapshotsTree: return (PVE::DspCmdVmGetSnapshotsTree_strSnapshotsTree);
		case PVE::DspCmdGetBackupTree: return (PVE::DspCmdGetBackupTree_strBackupsTree);
		case PVE::DspCmdVmGetConfig:
		case PVE::DspCmdGetVmConfigById:
			return (PVE::DspCmdVmGetConfig_strVmConfig);
		case PVE::DspCmdGetVmInfo: return (PVE::DspCmdGetVmInfo_strContainer);
		case PVE::DspCmdGetVmToolsInfo: return (PVE::DspCmdGetVmToolsInfo_strContainer);
		case PVE::DspCmdSMCGetDispatcherRTInfo: return (PVE::DspCmdSMCGetDispatcherRTInfo_strEventContainer);
		case PVE::DspCmdSMCGetCommandHistoryByVm: return (PVE::DspCmdSMCGetCommandHistoryByVm_strContainer);
		case PVE::DspCmdSMCGetCommandHistoryByUser: return (PVE::DspCmdSMCGetCommandHistoryByUser_strContainer);
		case PVE::DspCmdGetHostStatistics: return (PVE::DspCmdGetHostStatistics_strSystemStatistics);
		case PVE::DspCmdVmGetStatistics: return (PVE::DspCmdVmGetStatistics_strSystemStatistics);
		case PVE::DspCmdDirVmCreate:
		case PVE::DspCmdDirRegVm:
		case PVE::DspCmdDirReg3rdPartyVm:
		case PVE::DspCmdDirRestoreVm:
		case PVE::DspCmdDirVmClone:
		case PVE::DspCmdInstallAppliance:
		case PVE::DspCmdGetDefaultVmConfig:
		case PVE::DspCmdDirVmEditBegin:
			return (PVE::DspCmdDirRegVm_strVmConfig);
		case PVE::DspCmdUserGetLicenseInfo:
		case PVE::DspCmdUserUpdateLicense:
			return (PVE::DspCmdUserGetLicenseInfo_strLicenseInfo);
		case PVE::DspCmdUpdateDeviceInfo: return (PVE::DspCmdUpdateDeviceInfo_strDeviceInfo);
		case PVE::DspCmdGetNetServiceStatus: return (PVE::DspCmdGetNetServiceStatus_strNetServiceStatus);
		case PVE::DspCmdUserLogin: return (PVE::DspCmdUserLogin_strLoginInfo);
		case PVE::DspCmdUserLoginLocalStage2: return (PVE::DspCmdUserLoginLocalStage2_strLoginInfo);
		case PVE::DspCmdPerfomanceStatistics: return (PVE::DspCmdPerfomanceStatistics_strPerfStats) ;
		default: return PVE::IDspMethodsArgs(0);
	}
}

}

void CProtoCommandDspWsResponse::FillFsCommandsParams(CResult *pResult)
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_WS_RESPONSE_CMD_HW_FILE_SYSTEM_INFO);
	if (pParam)
	{
		PVE::IDispatcherCommands nCmdNum = (PVE::IDispatcherCommands)GetUnsignedIntParamValue(EVT_PARAM_PROTO_REQUEST_OP_CODE);
		switch (nCmdNum)
		{
			case PVE::DspCmdFsGetDiskList:
			case PVE::DspCmdFsGetCurrentDirectory:
			case PVE::DspCmdFsGetDirectoryEntries:
			case PVE::DspCmdFsGetFileList:
			case PVE::DspCmdFsCreateDirectory:
			case PVE::DspCmdFsRenameEntry:
			case PVE::DspCmdFsRemoveEntry:
			{
				pResult->m_hashResultSet[GetHashIdByOpCode(nCmdNum)] = pParam->getParamValue();
			}
			break;

			default:
			{
				WRITE_TRACE(DBG_FATAL, "Hw file system info received for non FS command type: %d", nCmdNum);
			}
			break;
		}
	}
}

void CProtoCommandDspWsResponse::FillVmConfigParam(CResult *pResult)
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_WS_RESPONSE_CMD_VM_CONFIG);
	if (pParam)
	{
		PVE::IDispatcherCommands nCmdNum = (PVE::IDispatcherCommands)GetUnsignedIntParamValue(EVT_PARAM_PROTO_REQUEST_OP_CODE);
		switch (nCmdNum)
		{
			case PVE::DspCmdVmGetConfig:
			case PVE::DspCmdDirRegVm:
			case PVE::DspCmdDirReg3rdPartyVm:
			case PVE::DspCmdDirRestoreVm:
			case PVE::DspCmdDirVmClone:
			case PVE::DspCmdInstallAppliance:
			case PVE::DspCmdDirVmCreate:
			case PVE::DspCmdGetDefaultVmConfig:
			case PVE::DspCmdDirVmEditBegin:
			case PVE::DspCmdGetVmConfigById:
			{
				pResult->m_hashResultSet[GetHashIdByOpCode(nCmdNum)] = pParam->getParamValue();
			}
			break;

			default:
			{
				WRITE_TRACE(DBG_FATAL, "VM config received for non known command type: %d", nCmdNum);
			}
			break;
		}
	}
}

void CProtoCommandDspWsResponse::FillDeviceInfo(CResult *pResult)
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_WS_RESPONSE_CMD_VM_DEVICE);
	if (pParam)
	{
		PVE::IDispatcherCommands nCmdNum = (PVE::IDispatcherCommands)GetUnsignedIntParamValue(EVT_PARAM_PROTO_REQUEST_OP_CODE);
		switch (nCmdNum)
		{
		case PVE::DspCmdUpdateDeviceInfo:
			{
				pResult->m_hashResultSet[GetHashIdByOpCode(nCmdNum)] = pParam->getParamValue();
			}
			break;

		default:
			{
				WRITE_TRACE(DBG_FATAL, "VM config received for non known command type: %d", nCmdNum);
			}
			break;
		}
	}
}
void CProtoCommandDspWsResponse::FillVmEventParam(CResult *pResult)
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_WS_RESPONSE_CMD_VM_EVENT);
	if (pParam)
	{
		PVE::IDispatcherCommands nCmdNum = (PVE::IDispatcherCommands)GetUnsignedIntParamValue(EVT_PARAM_PROTO_REQUEST_OP_CODE);
		switch (nCmdNum)
		{
			case PVE::DspCmdGetVmInfo:
			case PVE::DspCmdGetVmToolsInfo:
			case PVE::DspCmdSMCGetDispatcherRTInfo:
			case PVE::DspCmdSMCGetCommandHistoryByVm:
			case PVE::DspCmdSMCGetCommandHistoryByUser:
			case PVE::DspCmdUserGetLicenseInfo:
			case PVE::DspCmdUserUpdateLicense:
			case PVE::DspCmdGetBackupTree:
			{
				pResult->m_hashResultSet[GetHashIdByOpCode(nCmdNum)] = pParam->getParamValue();
			}
			break;

			default:
			{
				WRITE_TRACE(DBG_FATAL, "VM event received for non known command type: %d", nCmdNum);
			}
			break;
		}
	}
}

void CProtoCommandDspWsResponse::FillSystemStatisticsParam(CResult *pResult)
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_WS_RESPONSE_CMD_SYSTEM_STATISTICS);
	if (pParam)
	{
		PVE::IDispatcherCommands nCmdNum = (PVE::IDispatcherCommands)GetUnsignedIntParamValue(EVT_PARAM_PROTO_REQUEST_OP_CODE);
		switch (nCmdNum)
		{
			case PVE::DspCmdGetHostStatistics:
			case PVE::DspCmdVmGetStatistics:
			{
				pResult->m_hashResultSet[GetHashIdByOpCode(nCmdNum)] = pParam->getParamValue();
			}
			break;

			default:
			{
				WRITE_TRACE(DBG_FATAL, "System statistics received for non known command type: %d", nCmdNum);
			}
			break;
		}
	}
}

void CProtoCommandDspWsResponse::FillErrorData(CResult *pResult)
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_WS_RESPONSE_CMD_ERROR);
	if (pParam)
		pResult->setError(new CVmEvent(pParam->getParamValue()));
}

void CProtoCommandDspWsResponse::AddStandardParam(const QString &sParamValue)
{
	SetStringParamValue( sParamValue, EVT_PARAM_WS_RESPONSE_CMD_STANDARD_PARAM);
}

quint32 CProtoCommandDspWsResponse::GetStandardParamsCount()
{
	quint32 nParamsCount = 0;
	foreach(CVmEventParameter *pParam, m_pProtoPackage->m_lstEventParameters)
		if (pParam->getParamName() == EVT_PARAM_WS_RESPONSE_CMD_STANDARD_PARAM)
			nParamsCount++;
	return (nParamsCount);
}

QString CProtoCommandDspWsResponse::GetStandardParam(quint32 nIndex)
{
	quint32 nTmpIndex = 0;
	foreach(CVmEventParameter *pParam, m_pProtoPackage->m_lstEventParameters)
	{
		if (pParam->getParamName() == EVT_PARAM_WS_RESPONSE_CMD_STANDARD_PARAM)
		{
			if (nTmpIndex == nIndex)
				return (pParam->getParamValue());
			else
				nTmpIndex++;
		}
	}
	return (QString());
}

//+++++++++++++++++++++++++++++++DspCmdConvertOldHdd proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoStartConvertHardDisksCommand::CProtoStartConvertHardDisksCommand(
	const QStringList &lstHddPathsToConvert
	)
	: CProtoCommandWithoutParams(PVE::DspCmdConvertOldHdd)
{
	SetStringListParamValue( lstHddPathsToConvert, EVT_PARAM_CONVERT_OLD_HDD_LIST );
}

bool CProtoStartConvertHardDisksCommand::IsValid()
{
	CVmEventParameter *pParam = m_pProtoPackage->getEventParameter(EVT_PARAM_CONVERT_OLD_HDD_LIST);
	return (pParam && pParam->isList() && pParam->getParamType() == PVE::String &&
		CProtoCommandWithoutParams::IsValid());
}

QStringList CProtoStartConvertHardDisksCommand::GetHddPathsToConvert()
{
	return (GetStringListParamValue(EVT_PARAM_CONVERT_OLD_HDD_LIST));
}

//+++++++++++++++++++++++++++++++DspCmdUpdateUsbAssocList proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoUpdateUsbAssocListCommand::CProtoUpdateUsbAssocListCommand(
	const QStringList &lstUsbList,
	quint32 nListVersion,
	quint32 nFlags
	)
	: CProtoCommandWithoutParams(PVE::DspCmdUpdateUsbAssocList,nFlags)
{
	SetStringListParamValue( lstUsbList, EVT_PARAM_USB_ASSOC_LIST );
	SetUnsignedIntParamValue( nListVersion, EVT_PARAM_USB_ASSOC_LIST_VERSION );
}

bool CProtoUpdateUsbAssocListCommand::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_USB_ASSOC_LIST, PVE::String) &&
		CheckWhetherParamPresents(EVT_PARAM_USB_ASSOC_LIST_VERSION, PVE::UnsignedInt );
}

QStringList CProtoUpdateUsbAssocListCommand::GetUsbList()
{
	return GetStringListParamValue(EVT_PARAM_USB_ASSOC_LIST);
}

quint32 CProtoUpdateUsbAssocListCommand::GetListVersion()
{
	return GetUnsignedIntParamValue( EVT_PARAM_USB_ASSOC_LIST_VERSION );
}

//+++++++++++++++++++++++++++++++++++++++DspCmdFsGenerateEntryName command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCommandDspCmdFsGenerateEntryName::CProtoCommandDspCmdFsGenerateEntryName(
		const QString &sDirPath,
		const QString &sFilenamePrefix,
		const QString &sFilenameSuffix,
		const QString &sIndexDelimiter
	)
: CProtoCommand(PVE::DspCmdFsGenerateEntryName)
{
	SetStringParamValue( sDirPath, EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_DIRPATH );
	SetStringParamValue( sFilenamePrefix, EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_PREFIX );
	SetStringParamValue( sFilenameSuffix, EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_SUFFIX );

	//Workaround for QDom lib issue: can't to pass through string value with white spaces
	SetStringParamValue( "*" + sIndexDelimiter,	EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_INDEX_DELIMITER);
}

bool CProtoCommandDspCmdFsGenerateEntryName::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_DIRPATH, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_PREFIX, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_SUFFIX, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_INDEX_DELIMITER, PVE::String);
}

QString CProtoCommandDspCmdFsGenerateEntryName::GetDirPath()
{
	return (GetStringParamValue(EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_DIRPATH));
}

QString CProtoCommandDspCmdFsGenerateEntryName::GetFilenamePrefix()
{
	return (GetStringParamValue(EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_PREFIX));
}

QString CProtoCommandDspCmdFsGenerateEntryName::GetFilenameSuffix()
{
	return ( GetStringParamValue( EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_SUFFIX ) );
}

QString CProtoCommandDspCmdFsGenerateEntryName::GetIndexDelimiter()
{
	return ( GetStringParamValue( EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_INDEX_DELIMITER ).remove(0, 1) );
}

//+++++++++++++++++++++++++++++++++++++++DspCmdVmSectionValidateConfig command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCreateVmValidateConfigCommand::CProtoCreateVmValidateConfigCommand()
: CProtoCommandWithOneStrParam(PVE::DspCmdVmSectionValidateConfig)
{
}

CProtoCreateVmValidateConfigCommand::CProtoCreateVmValidateConfigCommand(const QString &sVmConfig, PRL_VM_CONFIG_SECTIONS nSection)
: CProtoCommandWithOneStrParam(PVE::DspCmdVmSectionValidateConfig, sVmConfig)
{
	SetUnsignedIntParamValue( nSection, EVT_PARAM_VM_CONFIG_SECTION);
}

PRL_VM_CONFIG_SECTIONS CProtoCreateVmValidateConfigCommand::GetSection()
{
	return (PRL_VM_CONFIG_SECTIONS )GetUnsignedIntParamValue(EVT_PARAM_VM_CONFIG_SECTION);
}

bool CProtoCreateVmValidateConfigCommand::IsValid()
{
	return CProtoCommandWithOneStrParam::IsValid()
			&& CheckWhetherParamPresents(EVT_PARAM_VM_CONFIG_SECTION, PVE::UnsignedInt);
}

//++++++++++++++++++++++++++++++++++++++++++++DspCmdVmMigrate command class implementation+++++++++++++++++++++++++++++
CProtoVmMigrateCommand::CProtoVmMigrateCommand(
	const QString &sVmUuid,
	const QString &sTargetServerHostname,
	quint32 nTargetServerPort,
	const QString &sTargetServerSessionUuid,
	const QString &sTargetVmName,
	const QString &sTargetVmHomePath,
	quint32 nMigrationFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
: CProtoBasicVmCommand(PVE::DspCmdDirVmMigrate, sVmUuid, bForceOperation, nMigrationFlags)
{
	SetUnsignedIntParamValue(MIGRATE_CLIENT_PROTO_VERSION, EVT_PARAM_MIGRATE_PROTO_VERSION);
	SetStringParamValue( sTargetServerHostname, EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_HOSTNAME);
	SetUnsignedIntParamValue( nTargetServerPort, EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_PORT);
	SetStringParamValue( sTargetServerSessionUuid, EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_SESSION_UUID );
	SetStringParamValue( sTargetVmName, EVT_PARAM_MIGRATE_CMD_TARGET_VM_NAME );
	SetStringParamValue( sTargetVmHomePath, EVT_PARAM_MIGRATE_CMD_TARGET_VM_HOME_PATH );
	SetUnsignedIntParamValue( nReservedFlags, EVT_PARAM_MIGRATE_CMD_RESERVED_FLAGS);
}

bool CProtoVmMigrateCommand::IsValid()
{
	/* Skip EVT_PARAM_MIGRATE_CMD_TARGET_VM_NAME check in order to preserve backward compatibility. */
	return (CheckWhetherParamPresents(EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_HOSTNAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_PORT, PVE::UnsignedInt) &&
			CheckWhetherParamPresents(EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_SESSION_UUID, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_MIGRATE_CMD_RESERVED_FLAGS, PVE::UnsignedInt) &&
			CheckWhetherParamPresents(EVT_PARAM_MIGRATE_CMD_TARGET_VM_HOME_PATH, PVE::String) &&
			CProtoBasicVmCommand::IsValid());
}

quint32 CProtoVmMigrateCommand::GetVersion()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_MIGRATE_PROTO_VERSION));
}

QString CProtoVmMigrateCommand::GetTargetServerHostname()
{
	return (GetStringParamValue(EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_HOSTNAME));
}

quint32 CProtoVmMigrateCommand::GetTargetServerPort()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_PORT));
}

QString CProtoVmMigrateCommand::GetTargetServerSessionUuid()
{
	return (GetStringParamValue(EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_SESSION_UUID));
}

QString CProtoVmMigrateCommand::GetTargetServerVmName()
{
	return (GetStringParamValue(EVT_PARAM_MIGRATE_CMD_TARGET_VM_NAME));
}

QString CProtoVmMigrateCommand::GetTargetServerVmHomePath()
{
	return (GetStringParamValue(EVT_PARAM_MIGRATE_CMD_TARGET_VM_HOME_PATH));
}

quint32 CProtoVmMigrateCommand::GetMigrationFlags()
{
	return GetCommandFlags();
}

quint32 CProtoVmMigrateCommand::GetReservedFlags()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_MIGRATE_CMD_RESERVED_FLAGS));
}
//++++++++++++++++++++++++++++++++++++++++++++DspCmdVmStartEx command class implementation+++++++++++++++++++++++++++++
CProtoVmStartExCommand::CProtoVmStartExCommand(const QString &sVmUuid, quint32 nStartMode, quint32 nReserved)
: CProtoBasicVmCommand(PVE::DspCmdVmStartEx, sVmUuid)
{
	SetUnsignedIntParamValue( nStartMode, EVT_PARAM_VM_START_EX_CMD_START_MODE);
	SetUnsignedIntParamValue( nReserved, EVT_PARAM_VM_START_EX_CMD_RESERVED );
}

bool CProtoVmStartExCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_START_EX_CMD_START_MODE, PVE::UnsignedInt) &&
			CheckWhetherParamPresents(EVT_PARAM_VM_START_EX_CMD_RESERVED, PVE::UnsignedInt) &&
			CProtoBasicVmCommand::IsValid());
}

quint32 CProtoVmStartExCommand::GetStartMode()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_VM_START_EX_CMD_START_MODE));
}

quint32 CProtoVmStartExCommand::GetReservedParameter()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_VM_START_EX_CMD_RESERVED));
}

/*****************************************
 * class CProtoPerfStatsCommand
 ****************************************/
CProtoPerfStatsCommand::CProtoPerfStatsCommand(CProtoPerfStatsCommand::PerfStatsAction action,
                                               const QString &sFilter, const QString &sVmUuid)
    :ancestor(PVE::DspCmdPerfomanceStatistics, sVmUuid)
{
	SetUnsignedIntParamValue( quint32(action), EVT_PARAM_PERFSTATS_CMD_ACTION );
	SetStringParamValue( sFilter, EVT_PARAM_PERFSTATS_CMD_FILTER);
}

bool CProtoPerfStatsCommand::IsValid()
{
	return ancestor::IsValid() && CheckWhetherParamPresents(EVT_PARAM_PERFSTATS_CMD_FILTER, PVE::String) &&
        normalize_action(GetUnsignedIntParamValue(EVT_PARAM_PERFSTATS_CMD_ACTION))!= PSA_INVALID ;
}


CProtoPerfStatsCommand::PerfStatsAction CProtoPerfStatsCommand::GetAction()
{
	return normalize_action(GetUnsignedIntParamValue(EVT_PARAM_PERFSTATS_CMD_ACTION)) ;
}

QString CProtoPerfStatsCommand::GetFilter()
{
	return GetStringParamValue( EVT_PARAM_PERFSTATS_CMD_FILTER ) ;
}

//+++++++++++++++++++++++++++++++DspCmdVmSwitchToSnapshot proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoSwitchToSnapshotCommand::CProtoSwitchToSnapshotCommand(
	const QString &sVmUuid,
	const QString &sSnapshotUuid,
	quint32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdVmSwitchToSnapshot, sVmUuid, false, nFlags)
{
	SetStringParamValue( sSnapshotUuid, EVT_PARAM_SWITCH_TO_SNAPSHOT_UUID );
}

bool CProtoSwitchToSnapshotCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_SWITCH_TO_SNAPSHOT_UUID, PVE::String) &&
		CProtoCommandWithoutParams::IsValid());
}

QString CProtoSwitchToSnapshotCommand::GetSnapshotUuid()
{
	return (GetStringParamValue(EVT_PARAM_SWITCH_TO_SNAPSHOT_UUID));
}

//+++++++++++++++++++++++++++++++DspCmdVmInternal proto command class implementation+++++++++++++++++++++++++++++++++
CProtoVmInternalCommand::CProtoVmInternalCommand(
	const QString &sVmUuid,
	const QString &sCmd,
	const QStringList &lstArgs
	)
	: CProtoBasicVmCommand(PVE::DspCmdVmInternal, sVmUuid)
{
	SetStringParamValue( sCmd, EVT_PARAM_INTERNAL_CMD_NAME );
	SetStringListParamValue( lstArgs, EVT_PARAM_INTERNAL_CMD_ARGUMENTS );
}

bool CProtoVmInternalCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_INTERNAL_CMD_NAME, PVE::String) &&
		CProtoCommandWithoutParams::IsValid());
}

QString CProtoVmInternalCommand::GetCommandName()
{
	return (GetStringParamValue(EVT_PARAM_INTERNAL_CMD_NAME));
}

QStringList CProtoVmInternalCommand::GetCommandArguments()
{
	return (GetStringListParamValue(EVT_PARAM_INTERNAL_CMD_ARGUMENTS));
}

//+++++ CProtoVmInternalCommand proto command class implementation ++++++++
CProtoVmCommitDiskUnfinished::CProtoVmCommitDiskUnfinished(
		const QString &sVmUuid,
		const QString &sDiskSysName,
		const QString &sSnapshotUuid
		)
: CProtoBasicVmCommand(PVE::DspCmdCtlVmCommitDiskUnfinished, sVmUuid)
{
	SetStringParamValue( sDiskSysName, EVT_PARAM_PROTO_FIRST_STR_PARAM );
	SetStringParamValue( sSnapshotUuid, EVT_PARAM_PROTO_SECOND_STR_PARAM );
}
bool CProtoVmCommitDiskUnfinished::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_PROTO_FIRST_STR_PARAM, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_PROTO_SECOND_STR_PARAM, PVE::String)
		&& CProtoBasicVmCommand::IsValid() );
}

QString CProtoVmCommitDiskUnfinished::GetDiskSysName()
{
	return GetStringParamValue( EVT_PARAM_PROTO_FIRST_STR_PARAM );
}

QString CProtoVmCommitDiskUnfinished::GetSnapshotUuid()
{
	return GetStringParamValue( EVT_PARAM_PROTO_SECOND_STR_PARAM );
}

//+++++++++++++++++++++++++++++++DspCmdVmDeleteSnapshot proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoDeleteSnapshotCommand::CProtoDeleteSnapshotCommand(
	const QString &sVmUuid,
	const QString &sSnapshotUuid,
	bool bChild,
	quint32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdVmDeleteSnapshot, sVmUuid, false, nFlags)
{
	SetStringParamValue( sSnapshotUuid, EVT_PARAM_DELETE_SNAPSHOT_UUID );
	SetUnsignedIntParamValue( quint32(bChild), EVT_PARAM_DELETE_SNAPSHOT_CHILD_SIGN );
}

bool CProtoDeleteSnapshotCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_DELETE_SNAPSHOT_UUID, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_DELETE_SNAPSHOT_CHILD_SIGN, PVE::UnsignedInt) &&
			CProtoCommandWithoutParams::IsValid());
}

QString CProtoDeleteSnapshotCommand::GetSnapshotUuid()
{
	return (GetStringParamValue(EVT_PARAM_DELETE_SNAPSHOT_UUID));
}

bool CProtoDeleteSnapshotCommand::GetChild()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_DELETE_SNAPSHOT_CHILD_SIGN));
}

void CProtoDeleteSnapshotCommand::SetMerge(bool bMerge)
{
	SetUnsignedIntParamValue( quint32(bMerge), EVT_PARAM_DELETE_SNAPSHOT_MERGE_SIGN );
}

bool CProtoDeleteSnapshotCommand::GetMerge()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_DELETE_SNAPSHOT_MERGE_SIGN));
}

quint32 CProtoDeleteSnapshotCommand::GetStepsCount()
{
	return GetUnsignedIntParamValue(EVT_PARAM_PROTO_FIRST_STR_PARAM);
}

void CProtoDeleteSnapshotCommand::SetStepsCount(quint32 uiStepsCount)
{
	SetUnsignedIntParamValue(uiStepsCount, EVT_PARAM_PROTO_FIRST_STR_PARAM);
}

quint32 CProtoDeleteSnapshotCommand::GetDeletionStep()
{
	return GetUnsignedIntParamValue(EVT_PARAM_PROTO_SECOND_STR_PARAM);
}

void CProtoDeleteSnapshotCommand::SetDeletionStep(quint32 uiStep)
{
	SetUnsignedIntParamValue(uiStep, EVT_PARAM_PROTO_SECOND_STR_PARAM);
}

//+++++++++++++++++++++++++++++++DspCmdVmCreateSnapshot proto command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCreateSnapshotCommand::CProtoCreateSnapshotCommand(
	const QString &sVmUuid,
	const QString &sName,
	const QString &sDescription,
	const QString &sSnapshotUuid,
	const QString &sSnapshotCreator,
	const QString &sSnapshotPath,
	quint32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdVmCreateSnapshot, sVmUuid, false, nFlags)
{
	SetStringParamValue( sName, EVT_PARAM_CREATE_SNAPSHOT_NAME );
	SetStringParamValue( sDescription, EVT_PARAM_CREATE_SNAPSHOT_DESCRIPTION );
	SetStringParamValue( sSnapshotUuid, EVT_PARAM_CREATE_SNAPSHOT_UUID );
	SetStringParamValue( sSnapshotCreator, EVT_PARAM_CREATE_SNAPSHOT_CREATOR );
	SetStringParamValue( sSnapshotPath, EVT_PARAM_CREATE_SNAPSHOT_PATH );
}

bool CProtoCreateSnapshotCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_CREATE_SNAPSHOT_NAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_SNAPSHOT_DESCRIPTION, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_SNAPSHOT_UUID, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_CREATE_SNAPSHOT_CREATOR, PVE::String) &&
			CProtoCommandWithoutParams::IsValid());
}

QString CProtoCreateSnapshotCommand::GetName()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_SNAPSHOT_NAME));
}

QString CProtoCreateSnapshotCommand::GetDescription()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_SNAPSHOT_DESCRIPTION));
}

QString CProtoCreateSnapshotCommand::GetSnapshotUuid()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_SNAPSHOT_UUID));
}

QString CProtoCreateSnapshotCommand::GetSnapshotCreator()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_SNAPSHOT_CREATOR));
}

QString CProtoCreateSnapshotCommand::GetSnapshotPath()
{
	return (GetStringParamValue(EVT_PARAM_CREATE_SNAPSHOT_PATH));
}

//+++++++++++++++++++++++++++++++DspCmdVmUpdateSnapshotData proto command class implementation++++++++++++++++
CProtoUpdateSnapshotDataCommand::CProtoUpdateSnapshotDataCommand(
	const QString &sVmUuid,
	const QString &sSnapshotUuid,
	const QString &sNewName,
	const QString &sNewDescription
	)

	: CProtoBasicVmCommand(PVE::DspCmdVmUpdateSnapshotData, sVmUuid)
{
	SetStringParamValue( sSnapshotUuid, EVT_PARAM_UPDATE_SNAPSHOT_DATA_UUID );
	SetStringParamValue( sNewName, EVT_PARAM_UPDATE_SNAPSHOT_DATA_NAME );
	SetStringParamValue( sNewDescription, EVT_PARAM_UPDATE_SNAPSHOT_DATA_DESCRIPTION );
}

bool CProtoUpdateSnapshotDataCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_UPDATE_SNAPSHOT_DATA_UUID, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_UPDATE_SNAPSHOT_DATA_NAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_UPDATE_SNAPSHOT_DATA_DESCRIPTION, PVE::String) &&
		CProtoCommandWithoutParams::IsValid());
}

QString CProtoUpdateSnapshotDataCommand::GetSnapshotUuid()
{
	return (GetStringParamValue(EVT_PARAM_UPDATE_SNAPSHOT_DATA_UUID));
}

QString CProtoUpdateSnapshotDataCommand::GetName()
{
	return (GetStringParamValue(EVT_PARAM_UPDATE_SNAPSHOT_DATA_NAME));
}

QString CProtoUpdateSnapshotDataCommand::GetDescription()
{
	return (GetStringParamValue(EVT_PARAM_UPDATE_SNAPSHOT_DATA_DESCRIPTION));
}

//+++++++++++++++++++++++++++++++DspCmdVmUpdateSnapshotData proto command class implementation++++++++++++++++
CProtoVmLoginInGuestCommand::CProtoVmLoginInGuestCommand(PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid, const QString &sUserName,
	const QString &sUserPassword, quint32 nFlags)
:
CProtoBasicVmCommand(nCmdIdentifier, sVmUuid, false, nFlags)
{
	SetStringParamValue( sUserName, EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_LOGIN );
	SetStringParamValue( sUserPassword, EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_PASSWORD );
}

bool CProtoVmLoginInGuestCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_LOGIN, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_PASSWORD, PVE::String) &&
			CProtoBasicVmCommand::IsValid());
}

QString CProtoVmLoginInGuestCommand::GetUserLoginName()
{
	return (GetStringParamValue(EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_LOGIN));
}

QString CProtoVmLoginInGuestCommand::GetUserPassword()
{
	return (GetStringParamValue(EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_PASSWORD));
}

//+++++++++++++++++++++++++++++++Basic VM guest OS session functionality proto command class implementation++++++++++++
CProtoBasicVmGuestCommand::CProtoBasicVmGuestCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	const QString &sVmGuestUuid,
	quint32 nFlags
)
:
CProtoBasicVmCommand(nCmdIdentifier, sVmUuid, false, nFlags)
{
	SetStringParamValue( sVmGuestUuid, EVT_PARAM_VM_GUEST_CMD_SESSION_ID );
}

bool CProtoBasicVmGuestCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_GUEST_CMD_SESSION_ID, PVE::String) &&
			CProtoBasicVmCommand::IsValid());
}

QString CProtoBasicVmGuestCommand::GetVmSessionUuid()
{
	return (GetStringParamValue(EVT_PARAM_VM_GUEST_CMD_SESSION_ID));
}

//+++++++++++++++++++++++++++++++DspCmdVmGuestRunProgram proto command class implementation++++++++++++++++++++++++++++
CProtoVmGuestRunProgramCommand::CProtoVmGuestRunProgramCommand(
	const QString &sVmUuid,
	const QString &sVmGuestUuid,
	const QString &sProgramName,
	const QStringList &lstArgs,
	const QStringList &lstVars,
	quint32 nFlags
)
:
CProtoBasicVmGuestCommand(PVE::DspCmdVmGuestRunProgram, sVmUuid, sVmGuestUuid, nFlags)
{
	SetStringParamValue( sProgramName, EVT_PARAM_VM_GUEST_RUN_APP_CMD_PROGRAM_NAME );
	SetStringListParamValue( lstArgs, EVT_PARAM_VM_GUEST_RUN_APP_CMD_ARGUMENTS );
	SetStringListParamValue( lstVars, EVT_PARAM_VM_GUEST_RUN_APP_CMD_ENV_VARS );
}

bool CProtoVmGuestRunProgramCommand::IsValid()
{
	CVmEventParameter *pParamArgs = m_pProtoPackage->getEventParameter(EVT_PARAM_VM_GUEST_RUN_APP_CMD_ARGUMENTS);
	CVmEventParameter *pParamEnvVars = m_pProtoPackage->getEventParameter(EVT_PARAM_VM_GUEST_RUN_APP_CMD_ENV_VARS);
	return (pParamArgs && pParamArgs->isList() && pParamArgs->getParamType() == PVE::String &&
			pParamEnvVars && pParamEnvVars->isList() && pParamEnvVars->getParamType() == PVE::String &&
			CheckWhetherParamPresents(EVT_PARAM_VM_GUEST_RUN_APP_CMD_PROGRAM_NAME, PVE::String) &&
			CProtoBasicVmGuestCommand::IsValid());
}

QString CProtoVmGuestRunProgramCommand::GetProgramName()
{
	return (GetStringParamValue(EVT_PARAM_VM_GUEST_RUN_APP_CMD_PROGRAM_NAME));
}

QStringList CProtoVmGuestRunProgramCommand::GetProgramArguments()
{
	return (GetStringListParamValue(EVT_PARAM_VM_GUEST_RUN_APP_CMD_ARGUMENTS));
}

QStringList CProtoVmGuestRunProgramCommand::GetProgramEnvVars()
{
	return (GetStringListParamValue(EVT_PARAM_VM_GUEST_RUN_APP_CMD_ENV_VARS));
}

//+++++++++++++++++++++++++++++++DspCmdVmGuestSetUserPasswd proto command class implementation++++++++++++++++++++++++++
CProtoVmGuestSetUserPasswdCommand::CProtoVmGuestSetUserPasswdCommand(
	const QString &sVmUuid,
	const QString &sVmGuestUuid,
	const QString &sUserLoginName,
	const QString &sUserPassword,
	quint32 nFlags
)
:
CProtoBasicVmGuestCommand(PVE::DspCmdVmGuestSetUserPasswd, sVmUuid, sVmGuestUuid, nFlags)
{
	SetStringParamValue( sUserLoginName, EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_LOGIN_NAME );
	SetStringParamValue( sUserPassword, EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_PASSWD );
}

bool CProtoVmGuestSetUserPasswdCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_LOGIN_NAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_PASSWD, PVE::String) &&
			CProtoBasicVmGuestCommand::IsValid());
}

QString CProtoVmGuestSetUserPasswdCommand::GetUserLoginName()
{
	return (GetStringParamValue(EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_LOGIN_NAME));
}

QString CProtoVmGuestSetUserPasswdCommand::GetUserPassword()
{
	return (GetStringParamValue(EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_PASSWD));
}

//++++++++++++++++++++++++++++++++++++Base DspCmd*Backup* commands class implementation+++++++++++++++++++++++++++++
CProtoBackupCommand::CProtoBackupCommand(
	PVE::IDispatcherCommands nCmdIdentifier,
	const QString &sVmUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	quint32 nFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
: CProtoBasicVmCommand(nCmdIdentifier, sVmUuid, bForceOperation, nFlags)
{
	SetUnsignedIntParamValue(BACKUP_PROTO_VERSION, EVT_PARAM_BACKUP_PROTO_VERSION);
	SetStringParamValue(sServerHostname, EVT_PARAM_BACKUP_CMD_SERVER_HOSTNAME);
	SetUnsignedIntParamValue(nServerPort, EVT_PARAM_BACKUP_CMD_SERVER_PORT);
	SetStringParamValue(sServerSessionUuid, EVT_PARAM_BACKUP_CMD_SERVER_SESSION_UUID);
	SetUnsignedIntParamValue(nReservedFlags, EVT_PARAM_BACKUP_CMD_RESERVED_FLAGS);
}

bool CProtoBackupCommand::IsValid()
{
	/*
	Skip EVT_PARAM_BACKUP_PROTO_VERSION check: In PSBM-beta this field does not exist
	To add check in RC-Beta compatibility will cancel
	*/
	return (CheckWhetherParamPresents(EVT_PARAM_BACKUP_CMD_SERVER_HOSTNAME, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_BACKUP_CMD_SERVER_PORT, PVE::UnsignedInt) &&
			CheckWhetherParamPresents(EVT_PARAM_BACKUP_CMD_SERVER_SESSION_UUID, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_BACKUP_CMD_RESERVED_FLAGS, PVE::UnsignedInt) &&
			CProtoBasicVmCommand::IsValid());
}

quint32 CProtoBackupCommand::GetVersion()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_BACKUP_PROTO_VERSION));
}

QString CProtoBackupCommand::GetServerHostname()
{
	QString server = GetStringParamValue(EVT_PARAM_BACKUP_CMD_SERVER_HOSTNAME);
	return server.isEmpty() ? "127.0.0.1" : server;
}

quint32 CProtoBackupCommand::GetServerPort()
{
	quint32 port = GetUnsignedIntParamValue(EVT_PARAM_BACKUP_CMD_SERVER_PORT);
	return port ? port : PRL_DISPATCHER_LISTEN_PORT;
}

QString CProtoBackupCommand::GetServerSessionUuid()
{
	return (GetStringParamValue(EVT_PARAM_BACKUP_CMD_SERVER_SESSION_UUID));
}

quint32 CProtoBackupCommand::GetFlags()
{
	return GetCommandFlags();
}

//+++++++++++++++++++++++++++++++CProtoCreateVmBackupCommand proto command class implementation++++++++++++++++
CProtoCreateVmBackupCommand::CProtoCreateVmBackupCommand(
	const QString &sVmUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	const QString &sDescription,
	quint32 nBackupFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
: CProtoBackupCommand(
	PVE::DspCmdCreateVmBackup,
	sVmUuid,
	sServerHostname,
	nServerPort,
	sServerSessionUuid,
	nBackupFlags,
	nReservedFlags,
	bForceOperation)
{
	SetStringParamValue(sDescription, EVT_PARAM_BACKUP_CMD_DESCRIPTION);
	/* will redefine snapshot uuid on source dispatcher */
	SetStringParamValue("", EVT_PARAM_BACKUP_CMD_BACKUP_UUID);
}

bool CProtoCreateVmBackupCommand::IsValid()
{
	/*
	Skip EVT_PARAM_BACKUP_CMD_DESCRIPTION check:
	In PSBM-beta this field does not exist and this field is not a mandatory
	To add check in RC-Beta compatibility will cancel
	*/
	return (CProtoBackupCommand::IsValid());
}

QString CProtoCreateVmBackupCommand::GetDescription()
{
	return (GetStringParamValue(EVT_PARAM_BACKUP_CMD_DESCRIPTION));
}

//+++++++++++++++++++++++++++++++CProtoRestoreVmBackupCommand proto command class implementation++++++++++++++++
CProtoRestoreVmBackupCommand::CProtoRestoreVmBackupCommand(
	const QString &sVmUuid,
	const QString &sBackupUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	const QString &sTargetVmHomePath,
	const QString &sTargetVmName,
	quint32 nRestoreFlags,
	quint32 nReservedFlags
)
: CProtoBackupCommand(
	PVE::DspCmdRestoreVmBackup,
	sVmUuid,
	sServerHostname,
	nServerPort,
	sServerSessionUuid,
	nRestoreFlags,
	nReservedFlags,
	PRL_TRUE)
{
	SetStringParamValue(sBackupUuid, EVT_PARAM_BACKUP_CMD_BACKUP_UUID);
	SetStringParamValue(sTargetVmHomePath, EVT_PARAM_BACKUP_CMD_TARGET_VM_HOME_PATH);
	SetStringParamValue(sTargetVmName, EVT_PARAM_BACKUP_CMD_TARGET_VM_NAME);
}

/* EVT_PARAM_BACKUP_CMD_TARGET_VM_NAME field can absent due to backward compatibility */
bool CProtoRestoreVmBackupCommand::IsValid()
{
		return (CheckWhetherParamPresents(EVT_PARAM_BACKUP_CMD_BACKUP_UUID, PVE::String) &&
			CheckWhetherParamPresents(EVT_PARAM_BACKUP_CMD_TARGET_VM_HOME_PATH, PVE::String) &&
			CProtoBackupCommand::IsValid());
}

QString CProtoRestoreVmBackupCommand::GetTargetVmHomePath()
{
	return (GetStringParamValue(EVT_PARAM_BACKUP_CMD_TARGET_VM_HOME_PATH));
}

QString CProtoRestoreVmBackupCommand::GetTargetVmName()
{
	return (GetStringParamValue(EVT_PARAM_BACKUP_CMD_TARGET_VM_NAME));
}

QString CProtoRestoreVmBackupCommand::GetBackupUuid()
{
	return (GetStringParamValue(EVT_PARAM_BACKUP_CMD_BACKUP_UUID));
}

//+++++++++++++++++++++++++++++++CProtoRemoveVmBackupCommand proto command class implementation++++++++++++++++
CProtoRemoveVmBackupCommand::CProtoRemoveVmBackupCommand(
	const QString &sVmUuid,
	const QString &sBackupUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	quint32 nRestoreFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
: CProtoBackupCommand(
	PVE::DspCmdRemoveVmBackup,
	sVmUuid,
	sServerHostname,
	nServerPort,
	sServerSessionUuid,
	nRestoreFlags,
	nReservedFlags,
	bForceOperation)
{
	SetStringParamValue(sBackupUuid, EVT_PARAM_BACKUP_CMD_BACKUP_UUID);
}

bool CProtoRemoveVmBackupCommand::IsValid()
{
		return (CheckWhetherParamPresents(EVT_PARAM_BACKUP_CMD_BACKUP_UUID, PVE::String) &&
			CProtoBackupCommand::IsValid());
}

QString CProtoRemoveVmBackupCommand::GetBackupUuid()
{
	return (GetStringParamValue(EVT_PARAM_BACKUP_CMD_BACKUP_UUID));
}

//+++++++++++++++++++++++++++++++CProtoGetBackupTreeCommand proto command class implementation++++++++++++++++
CProtoGetBackupTreeCommand::CProtoGetBackupTreeCommand(
	const QString &sVmUuid,
	const QString &sServerHostname,
	quint32 nServerPort,
	const QString &sServerSessionUuid,
	quint32 nBackupFlags,
	quint32 nReservedFlags,
	bool bForceOperation
)
: CProtoBackupCommand(
	PVE::DspCmdGetBackupTree,
	sVmUuid,
	sServerHostname,
	nServerPort,
	sServerSessionUuid,
	nBackupFlags,
	nReservedFlags,
	bForceOperation)
{
}

bool CProtoGetBackupTreeCommand::IsValid()
{
	return (CProtoBackupCommand::IsValid());
}

//+++++++++++++++++++++++++++++++CProtoVmResizeDiskImageCommand proto command class implementation++++++++++++++++
CProtoVmResizeDiskImageCommand::CProtoVmResizeDiskImageCommand(
	const QString &sVmUuid,
	const QString &sDiskImage,
	quint32 nNewSize,
	quint32 nFlags
	)

	: CProtoBasicVmCommand(PVE::DspCmdVmResizeDisk, sVmUuid)
{
	SetStringParamValue(sDiskImage, EVT_PARAM_RESIZE_DISK_CMD_IMAGE);
	SetUnsignedIntParamValue(nNewSize, EVT_PARAM_RESIZE_DISK_CMD_SIZE);
	SetUnsignedIntParamValue(nFlags, EVT_PARAM_RESIZE_DISK_CMD_RESERVED_FLAGS);
}

bool CProtoVmResizeDiskImageCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_RESIZE_DISK_CMD_IMAGE, PVE::String) &&
		CheckWhetherParamPresents(EVT_PARAM_RESIZE_DISK_CMD_SIZE, PVE::UnsignedInt) &&
		CProtoCommandWithoutParams::IsValid());
}

QString CProtoVmResizeDiskImageCommand::GetDiskImage()
{
	return GetStringParamValue(EVT_PARAM_RESIZE_DISK_CMD_IMAGE);
}

quint32 CProtoVmResizeDiskImageCommand::GetSize()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_RESIZE_DISK_CMD_SIZE));
}

quint32 CProtoVmResizeDiskImageCommand::GetFlags()
{
	return (GetUnsignedIntParamValue(EVT_PARAM_RESIZE_DISK_CMD_RESERVED_FLAGS));
}


//+++++++++++++++++++++++++++++++++++++++CProtoCommandDspCmdSetSessionConfirmationMode command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoCommandDspCmdSetSessionConfirmationMode::CProtoCommandDspCmdSetSessionConfirmationMode(
	bool bEnable,
	const QString &sUsername,
	const QString &sPassword,
	quint32 nReservedFlags
	)
	: CProtoCommand(PVE::DspCmdSetSessionConfirmationMode, false, nReservedFlags)
{
	SetStringParamValue( sUsername, EVT_PARAM_COMFIRMATION_MODE_CMD_USERNAME );
	SetStringParamValue( sPassword, EVT_PARAM_COMFIRMATION_MODE_CMD_PASSWORD );
	SetUnsignedIntParamValue( (unsigned int)bEnable, EVT_PARAM_COMFIRMATION_MODE_CMD_ENABLE_FLAG );
}

bool CProtoCommandDspCmdSetSessionConfirmationMode::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_COMFIRMATION_MODE_CMD_USERNAME, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_COMFIRMATION_MODE_CMD_PASSWORD, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_COMFIRMATION_MODE_CMD_ENABLE_FLAG, PVE::UnsignedInt);
}

QString CProtoCommandDspCmdSetSessionConfirmationMode::GetUserLoginName()
{
	return (GetStringParamValue(EVT_PARAM_COMFIRMATION_MODE_CMD_USERNAME));
}

QString CProtoCommandDspCmdSetSessionConfirmationMode::GetPassword()
{
	return (GetStringParamValue(EVT_PARAM_COMFIRMATION_MODE_CMD_PASSWORD));
}

bool CProtoCommandDspCmdSetSessionConfirmationMode::GetEnabledSign()
{
	return ((bool)GetUnsignedIntParamValue(EVT_PARAM_COMFIRMATION_MODE_CMD_ENABLE_FLAG));
}

///
CProtoDspCmdStorageSetValueCommand::CProtoDspCmdStorageSetValueCommand(
	const QString &sKey,
	const QString &sValue,
	quint32 nReservedFlags
	)
	: CProtoCommand(PVE::DspCmdStorageSetValue, false, nReservedFlags)
{
	SetStringParamValue( sKey, EVT_PARAM_STORAGE_CMD_KEY );
	SetStringParamValue( sValue, EVT_PARAM_STORAGE_CMD_VALUE );
}

bool CProtoDspCmdStorageSetValueCommand::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_STORAGE_CMD_KEY, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_STORAGE_CMD_VALUE, PVE::String);
}

QString CProtoDspCmdStorageSetValueCommand::GetKey()
{
	return (GetStringParamValue(EVT_PARAM_STORAGE_CMD_KEY));
}

QString CProtoDspCmdStorageSetValueCommand::GetValue()
{
	return (GetStringParamValue(EVT_PARAM_STORAGE_CMD_VALUE));
}

///
CProtoDspCmdVmStorageSetValueCommand::CProtoDspCmdVmStorageSetValueCommand(
	const QString &sVmUuid,
	const QString &sKey,
	const QString &sValue,
	quint32 nReservedFlags
	)
	: CProtoBasicVmCommand(PVE::DspCmdVmStorageSetValue,  sVmUuid, false, nReservedFlags)
{
	SetStringParamValue( sKey, EVT_PARAM_STORAGE_CMD_KEY );
	SetStringParamValue( sValue, EVT_PARAM_STORAGE_CMD_VALUE );
}

bool CProtoDspCmdVmStorageSetValueCommand::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_STORAGE_CMD_KEY, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_STORAGE_CMD_VALUE, PVE::String);
}

QString CProtoDspCmdVmStorageSetValueCommand::GetKey()
{
	return (GetStringParamValue(EVT_PARAM_STORAGE_CMD_KEY));
}

QString CProtoDspCmdVmStorageSetValueCommand::GetValue()
{
	return (GetStringParamValue(EVT_PARAM_STORAGE_CMD_VALUE));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++CProtoDspCmdDirReg3rdPartyVmCommand command class implementation+++++++++++++++++++++++++++++++++++++++
CProtoDspCmdDirReg3rdPartyVmCommand::CProtoDspCmdDirReg3rdPartyVmCommand(
	const QString &sVmConfigPath,
	const QString &sVmRootDirPath,
	quint32 nReservedFlags
)
: CProtoCommand(PVE::DspCmdDirReg3rdPartyVm, false, nReservedFlags)
{
	SetStringParamValue( sVmConfigPath, EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_CONFIG );
	SetStringParamValue( sVmRootDirPath, EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_ROOT_DIR );
}

bool CProtoDspCmdDirReg3rdPartyVmCommand::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_CONFIG, PVE::String)
		&& CheckWhetherParamPresents(EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_ROOT_DIR, PVE::String);
}

QString CProtoDspCmdDirReg3rdPartyVmCommand::GetPathToVmConfig()
{
	return (GetStringParamValue(EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_CONFIG));
}

QString CProtoDspCmdDirReg3rdPartyVmCommand::GetPathToVmRootDir()
{
	return (GetStringParamValue(EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_ROOT_DIR));
}

//*++++++++++++CProtoDspCmdNetworkPrefsChangedCommand command class implementation++++++++++++++++
CProtoDspCmdNetworkPrefsChangedCommand::CProtoDspCmdNetworkPrefsChangedCommand(
		const QString &sNetworkConfig,
		const QString &sOldNetworkID,
		const QString &sNewNetworkID,
		quint32 nFlags
)
: CProtoCommand(PVE::DspCmdDirReg3rdPartyVm, false, nFlags)
{
	SetStringParamValue( sNetworkConfig, EVT_PARAM_NETWORK_PREFERENSES);
	SetStringParamValue( sOldNetworkID, EVT_PARAM_VMCFG_OLD_VNETWORK_ID);
	SetStringParamValue( sNewNetworkID, EVT_PARAM_VMCFG_NEW_VNETWORK_ID);
}

bool CProtoDspCmdNetworkPrefsChangedCommand::IsValid()
{
	return CheckWhetherParamPresents(EVT_PARAM_NETWORK_PREFERENSES, PVE::String);
}

QString CProtoDspCmdNetworkPrefsChangedCommand::GetNetworkConfig()
{
	return (GetStringParamValue(EVT_PARAM_NETWORK_PREFERENSES));
}

QString CProtoDspCmdNetworkPrefsChangedCommand::GetOldNetworkID()
{
	return (GetStringParamValue(EVT_PARAM_VMCFG_OLD_VNETWORK_ID));
}

QString CProtoDspCmdNetworkPrefsChangedCommand::GetNewNetworkID()
{
	return (GetStringParamValue(EVT_PARAM_VMCFG_NEW_VNETWORK_ID));
}


//*++++++++++++DspCmdCopyCtTemplate command class implementation++++++++++++++++
CProtoCopyCtTemplateCommand::CProtoCopyCtTemplateCommand(
		const QString &sTmplName,
		const QString &sOsTmplName,
		const QString &sTargetServerHostname,
		quint32 nTargetServerPort,
		const QString &sTargetServerSessionUuid,
		quint32 nFlags,
		quint32 nReservedFlags)
:CProtoCommandWithoutParams(PVE::DspCmdCopyCtTemplate, false, nFlags)
{
	SetUnsignedIntParamValue(COPY_CT_TMPL_PROTO_VERSION, EVT_PARAM_COPY_CT_TMPL_PROTO_VERSION);
	SetStringParamValue(sTmplName, EVT_PARAM_COPY_CT_TMPL_TMPL_NAME);
	SetStringParamValue(sOsTmplName, EVT_PARAM_COPY_CT_TMPL_OS_TMPL_NAME);
	SetStringParamValue( sTargetServerHostname, EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_HOSTNAME);
	SetUnsignedIntParamValue( nTargetServerPort, EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_PORT);
	SetStringParamValue( sTargetServerSessionUuid, EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_SESSION_UUID );
	SetUnsignedIntParamValue( nReservedFlags, EVT_PARAM_COPY_CT_TMPL_RESERVED_FLAGS);
}

bool CProtoCopyCtTemplateCommand::IsValid()
{
	return (
		CheckWhetherParamPresents(EVT_PARAM_COPY_CT_TMPL_PROTO_VERSION, PVE::UnsignedInt) &&
		CheckWhetherParamPresents(EVT_PARAM_COPY_CT_TMPL_TMPL_NAME, PVE::String) &&
		CheckWhetherParamPresents(EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_HOSTNAME, PVE::String) &&
		CheckWhetherParamPresents(EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_PORT, PVE::UnsignedInt) &&
		CheckWhetherParamPresents(EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_SESSION_UUID, PVE::String) &&
		CheckWhetherParamPresents(EVT_PARAM_COPY_CT_TMPL_RESERVED_FLAGS, PVE::UnsignedInt) &&
		CProtoCommandWithoutParams::IsValid());
}

quint32 CProtoCopyCtTemplateCommand::GetVersion()
{
	return GetUnsignedIntParamValue(EVT_PARAM_COPY_CT_TMPL_PROTO_VERSION);
}

QString CProtoCopyCtTemplateCommand::GetTmplName()
{
	return GetStringParamValue(EVT_PARAM_COPY_CT_TMPL_TMPL_NAME);
}

QString CProtoCopyCtTemplateCommand::GetOsTmplName()
{
	return GetStringParamValue(EVT_PARAM_COPY_CT_TMPL_OS_TMPL_NAME);
}

QString CProtoCopyCtTemplateCommand::GetTargetServerHostname()
{
	return GetStringParamValue(EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_HOSTNAME);
}

quint32 CProtoCopyCtTemplateCommand::GetTargetServerPort()
{
	return GetUnsignedIntParamValue(EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_PORT);
}

QString CProtoCopyCtTemplateCommand::GetTargetServerSessionUuid()
{
	return GetStringParamValue(EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_SESSION_UUID);
}

quint32 CProtoCopyCtTemplateCommand::GetReservedFlags()
{
	return GetUnsignedIntParamValue(EVT_PARAM_COPY_CT_TMPL_RESERVED_FLAGS);
}

//+++++++++++++++++++++++++++++++DspCmdDirVmMove proto command class implementation++++++++++++++++++++++++++++++++++++
CProtoVmMoveCommand::CProtoVmMoveCommand(
	const QString &sVmUuid,
	const QString &sNewHomePath,
	PRL_UINT32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdDirVmMove, sVmUuid, false, nFlags)
{
	SetStringParamValue( sNewHomePath, EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH );
}

bool CProtoVmMoveCommand::IsValid()
{
	return (CheckWhetherParamPresents(EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH, PVE::String) &&
			CProtoBasicVmCommand::IsValid());
}

QString CProtoVmMoveCommand::GetNewHomePath()
{
	return (GetStringParamValue(EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH));
}

//++++++DspCmdSendProblemReport proto command class implementation+++++++++++
CProtoSendProblemReport::CProtoSendProblemReport(
    const QString &report,
    const QString &vmUuid,
    PRL_UINT32 nFlags
)
: CProtoBasicVmCommand(PVE::DspCmdSendProblemReport, vmUuid, false, nFlags)
{
    SetStringParamValue( report, EVT_PARAM_SRV_SEND_PROBLEM_REPORT_DATA );
}

bool CProtoSendProblemReport::IsValid()
{
    return CheckWhetherParamPresents(EVT_PARAM_SRV_SEND_PROBLEM_REPORT_DATA, PVE::String) &&
            CProtoBasicVmCommand::IsValid();
}

QString CProtoSendProblemReport::GetReportData()
{
    return GetStringParamValue(EVT_PARAM_SRV_SEND_PROBLEM_REPORT_DATA);
}

}//namespace Parallels

