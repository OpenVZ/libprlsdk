/*
 * PrlHandleResult.cpp
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


#include "PrlHandleFoundVmInfo.h"
#include "PrlHandleVmToolsInfo.h"
#include "PrlHandleSrvConfig.h"
#include "PrlHandleUserProfile.h"
#include "PrlHandleDispConfig.h"
#include "PrlHandleRemoteFsInfo.h"
#include "PrlHandleSystemStatistics.h"
#include "PrlHandleLicense.h"
#include "PrlHandleNetServiceStatus.h"
#include "PrlHandleAccessRights.h"
#include "PrlHandleUserInfo.h"
#include "PrlHandleVmGuest.h"
#include "PrlHandleVirtNet.h"
#include "PrlHandleProblemReport.h"
#include "PrlHandleOfflineService.h"
#include "PrlHandleNetworkClass.h"
#include "PrlHandleNetworkShapingConfig.h"
#include "PrlHandleVmDeviceHardDrive.h"
#include "PrlHandleCtTemplate.h"
#include "PrlHandleIPPrivNet.h"
#include "PrlHandlePluginInfo.h"
#include "PrlHandleBackupResult.h"
#include "PrlHandleCpuPool.h"

#include "PrlHandleServer.h"
#include "PrlHandleServerJob.h"
#include "PrlHandleResult.h"
#include "PrlHandleVm.h"
#include "PrlHandleVmInfo.h"
#include "PrlHandleLoginResponse.h"
#include "PrlHandleServerInfo.h"
#include "PrlHandleVmEvent.h"

#include <prlcommon/Messaging/CResult.h>
#include <prlcommon/Messaging/CVmEvent.h>
#include <prlcommon/Messaging/CVmBinaryEventParameter.h>
#include <prlcommon/ProtoSerializer/CProtoCommands.h>

#include "Libraries/ProblemReportUtils/CPackedProblemReport.h"

using namespace Parallels;

#ifdef ENABLE_MALLOC_DEBUG
    // By adding this interface we enable allocations tracing in the module
    #include "Interfaces/Debug.h"
#else
    // We're not allowed to throw exceptions from the library -
    // so we need to prevent operator ::new from doing this
    #include <new>
    using std::nothrow;
    #define new new(nothrow)
#endif


/**
 * Class default constructor.
 */
PrlHandleResult::PrlHandleResult(const PrlHandleServerPtr& server, const PrlHandleVmPtr &pVm, const CResult& result) :
	PrlHandleBase( PHT_RESULT ),
	m_Result ( result ),
	m_pServer( server ),
	m_pVm( pVm )
{
}


/**
 * Class destructor.
 */
PrlHandleResult::~PrlHandleResult()
{

}


/**
 * Get count of parameters
 * @param specifies a pointer to the count of parameters
 * @return PRL_ERR_SUCCESS if the method succeeds
 */
PRL_RESULT PrlHandleResult::GetParamsCount(PRL_UINT32_PTR pCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pCount = InternalGetParamsCount();
  return PRL_ERR_SUCCESS;
}

namespace {
/**
 * Casts specified proto command id into result hash id
 * @param dispatcher command id
 * @return corresponding result hash id
 */
PVE::IDspMethodsArgs GetHashIdByOperationCode(PVE::IDispatcherCommands _cmd)
{
	switch (_cmd)
	{
		//case PVE::DspCmdUserLogin: return PVE::DspCmdUserLogin_strAccessToken;
		case PVE::DspCmdUserGetHostHwInfo: return PVE::DspCmdUserGetHostHwInfo_strHostHwInfo;
		case PVE::DspCmdFsGetDiskList: return PVE::DspCmdFsGetDiskList_strDiskList;
		case PVE::DspCmdFsGetCurrentDirectory: return PVE::DspCmdFsGetCurrentDirectory_strDir;
		case PVE::DspCmdFsGetDirectoryEntries: return PVE::DspCmdFsGetDirectoryEntries_strEntriesList;
		case PVE::DspCmdFsGetFileList: return PVE::DspCmdFsGetFileList_strFileList;
		case PVE::DspCmdFsCreateDirectory: return PVE::DspCmdFsCreateDirectory_strDirEntries;
		case PVE::DspCmdFsRenameEntry: return PVE::DspCmdFsRenameEntry_strDirEntries;
		case PVE::DspCmdFsRemoveEntry: return PVE::DspCmdFsRemoveEntry_strDirEntries;
		case PVE::DspCmdUserGetProfile: return PVE::DspCmdUserGetProfile_strUserProfile;
		case PVE::DspCmdUserInfo: return PVE::DspCmdUserInfo_strUserInfo;
		case PVE::DspCmdGetHostCommonInfo: return PVE::DspCmdGetHostCommonInfo_strCommonInfo;
		case PVE::DspCmdVmGetProblemReport: return PVE::DspCmdVmGetProblemReport_strReport;
		case PVE::DspCmdVmGetPackedProblemReport: return PVE::DspCmdVmGetProblemReport_strReport;
		case PVE::DspCmdVmGetSnapshotsTree: return PVE::DspCmdVmGetSnapshotsTree_strSnapshotsTree;
		case PVE::DspCmdGetBackupTree: return PVE::DspCmdGetBackupTree_strBackupsTree;
		case PVE::DspCmdVmGetConfig:
		case PVE::DspCmdGetVmConfigById:
			return PVE::DspCmdVmGetConfig_strVmConfig;
		case PVE::DspCmdGetVmInfo: return PVE::DspCmdGetVmInfo_strContainer;
		case PVE::DspCmdGetVmToolsInfo: return PVE::DspCmdGetVmToolsInfo_strContainer;
		case PVE::DspCmdSMCGetDispatcherRTInfo: return PVE::DspCmdSMCGetDispatcherRTInfo_strEventContainer;
		case PVE::DspCmdSMCGetCommandHistoryByVm: return PVE::DspCmdSMCGetCommandHistoryByVm_strContainer;
		case PVE::DspCmdSMCGetCommandHistoryByUser: return PVE::DspCmdSMCGetCommandHistoryByUser_strContainer;
		case PVE::DspCmdGetHostStatistics: return PVE::DspCmdGetHostStatistics_strSystemStatistics;
		case PVE::DspCmdVmGetStatistics: return PVE::DspCmdVmGetStatistics_strSystemStatistics;
		case PVE::DspCmdDirRegVm:
		case PVE::DspCmdDirReg3rdPartyVm:
		case PVE::DspCmdDirRestoreVm:
		case PVE::DspCmdDirVmClone:
		case PVE::DspCmdInstallAppliance:
		case PVE::DspCmdGetDefaultVmConfig:
		case PVE::DspCmdDirVmEditBegin:
			return PVE::DspCmdDirRegVm_strVmConfig;
		case PVE::DspCmdUserGetLicenseInfo:
		case PVE::DspCmdUserUpdateLicense:
			return PVE::DspCmdUserGetLicenseInfo_strLicenseInfo;
		case PVE::DspCmdUpdateDeviceInfo: return PVE::DspCmdUpdateDeviceInfo_strDeviceInfo;
		case PVE::DspCmdGetNetServiceStatus: return PVE::DspCmdGetNetServiceStatus_strNetServiceStatus;
		case PVE::DspCmdUserLogin: return (PVE::DspCmdUserLogin_strLoginInfo);
		case PVE::DspCmdUserLoginLocalStage2: return (PVE::DspCmdUserLoginLocalStage2_strLoginInfo);
		case PVE::DspCmdVmUpdateSecurity: return (PVE::DspCmdVmUpdateSecurity_strSecurityInfo);
		case PVE::DspCmdPerfomanceStatistics: return (PVE::DspCmdPerfomanceStatistics_strPerfStats) ;
		default: return PVE::MultipleArgStart;
	}
}

}

PRL_HANDLE PrlHandleResult::ResultAsHandle()
{
	PRL_HANDLE res_handle = PRL_INVALID_HANDLE;
	switch( m_Result.getOpCode() )
	{
		case PVE::DspCmdUserLogin:
		case PVE::DspCmdUserLoginLocal:
		case PVE::DspCmdUserLoginLocalStage2:
		{
			CVmEvent evtLoginInfo( m_Result.m_hashResultSet[ PVE::DspCmdUserLogin_strLoginInfo ] );
			PrlHandleLoginResponse *pLoginInfo = new PrlHandleLoginResponse( &evtLoginInfo ) ;
			if( pLoginInfo )
				res_handle = pLoginInfo->GetHandle();
		}
		break;
		case PVE::DspCmdFsGetDiskList:
		case PVE::DspCmdFsGetCurrentDirectory:
		case PVE::DspCmdFsGetDirectoryEntries:
		case PVE::DspCmdFsGetFileList:
		case PVE::DspCmdFsCreateDirectory:
		case PVE::DspCmdFsRenameEntry:
		case PVE::DspCmdFsRemoveEntry:
		{
			QString sFsInfo = m_Result.m_hashResultSet[GetHashIdByOperationCode(m_Result.getOpCode())];
			PrlHandleRemoteFsInfo *pFsInfo = new PrlHandleRemoteFsInfo(sFsInfo);
			if (pFsInfo)
				res_handle = pFsInfo->GetHandle();
		}
		break;

		case PVE::DspCmdUserGetHostHwInfo:
		{
			QString sHostHwInfo = m_Result.m_hashResultSet[PVE::DspCmdUserGetHostHwInfo_strHostHwInfo];
			PrlHandleSrvConfig *pSrvConfig = new PrlHandleSrvConfig(sHostHwInfo);
			if (pSrvConfig)
				res_handle = pSrvConfig->GetHandle();
		}
		break;

		case PVE::DspCmdGetNetServiceStatus:
		{
			QString sNetStatus = m_Result.m_hashResultSet[PVE::DspCmdGetNetServiceStatus_strNetServiceStatus];
			PrlHandleNetServiceStatus *pNetStatus = new PrlHandleNetServiceStatus(sNetStatus);
			if (pNetStatus)
				res_handle = pNetStatus->GetHandle();
		}
		break;
		case PVE::DspCmdGetVmInfo:
		{
			//XXX: eliminate stack XML object usage code
			CVmEvent e(m_Result.m_hashResultSet[PVE::DspCmdGetVmInfo_strContainer]);
			PrlHandleVmInfo *pVmInfo = new PrlHandleVmInfo(&e);
			if (pVmInfo)
				res_handle = pVmInfo->GetHandle();
		}
		break;
		case PVE::DspCmdGetVmToolsInfo:
		{
			//XXX: eliminate stack XML object usage code
			CVmEvent e(m_Result.m_hashResultSet[PVE::DspCmdGetVmToolsInfo_strContainer]);
			PrlHandleVmToolsInfo *pVmToolsInfo = new PrlHandleVmToolsInfo(&e);
			if (pVmToolsInfo)
				res_handle = pVmToolsInfo->GetHandle();
		}
		break;

		case PVE::DspCmdUserGetProfile:
		{
			QString sUserProfile = m_Result.m_hashResultSet[PVE::DspCmdUserGetProfile_strUserProfile];
			PrlHandleUserProfile *pUserProfile = new PrlHandleUserProfile(sUserProfile);
			if (pUserProfile)
				res_handle = pUserProfile->GetHandle();
		}
		break;

		case PVE::DspCmdUserInfo:
		{
			QString sUserInfo = m_Result.m_hashResultSet[PVE::DspCmdUserInfo_strUserInfo];
			PrlHandleUserInfo *pUserInfo = new PrlHandleUserInfo(sUserInfo);
			if (pUserInfo)
				res_handle = pUserInfo->GetHandle();
		}
		break;

		case PVE::DspCmdGetHostCommonInfo:
		{
			QString sDispConfig = m_Result.m_hashResultSet[PVE::DspCmdGetHostCommonInfo_strCommonInfo];
			QString sNetworkConfig = m_Result.m_hashResultSet[PVE::DspCmdGetHostCommonInfo_strNetworkInfo];

			PrlHandleDispConfig *pDispConfig = new PrlHandleDispConfig(sDispConfig, sNetworkConfig);
			if (pDispConfig)
				res_handle = pDispConfig->GetHandle();
		}
		break;

		case PVE::DspCmdVmGetStatistics:
		case PVE::DspCmdGetHostStatistics:
		{
			QString sStatistics = m_Result.m_hashResultSet[m_Result.getOpCode() == PVE::DspCmdGetHostStatistics ?
						PVE::DspCmdGetHostStatistics_strSystemStatistics : PVE::DspCmdVmGetStatistics_strSystemStatistics];
			PrlHandleSystemStatistics *pStatistics = new PrlHandleSystemStatistics(sStatistics);
			if (pStatistics)
				res_handle = pStatistics->GetHandle();
		}
		break;
		case PVE::DspCmdUserGetLicenseInfo:
		case PVE::DspCmdUserUpdateLicense:
		{
			CVmEvent e(m_Result.m_hashResultSet[ PVE::DspCmdUserGetLicenseInfo_strLicenseInfo ]);
			PrlHandleLicense *pLicense = new PrlHandleLicense( &e );
			if (pLicense)
				res_handle = pLicense->GetHandle();
		}
		break;

		case PVE::DspCmdStartSearchConfig:
		{
			QString str = m_Result.m_hashResultSet[PVE::MultipleArgStart];
			CVmEvent e( str );
			PrlHandleFoundVmInfo* pVmInfo = new PrlHandleFoundVmInfo( &e );
			if (pVmInfo)
				res_handle = pVmInfo->GetHandle();
		}
		break;

		case PVE::DspCmdVmUpdateSecurity:
		{
			CVmSecurity _vm_security;
			QString str = m_Result.m_hashResultSet[PVE::DspCmdVmUpdateSecurity_strSecurityInfo];
			PRL_RESULT nParseResult = _vm_security.fromString( str );
			if (PRL_SUCCEEDED(nParseResult))
			{
				PrlHandleAccessRights* pAccessRights = new PrlHandleAccessRights( _vm_security );
				if (pAccessRights)
					res_handle = pAccessRights->GetHandle();
			}
			else
				WRITE_TRACE(DBG_FATAL, "Wrong VM security data: error code: %.8X '%s' data: [%s]", nParseResult, PRL_RESULT_TO_STRING(nParseResult), str.toUtf8().constData());
		}
		break;

		case PVE::DspCmdVmLoginInGuest:
		{
			QString sSessionUuid = m_Result.m_hashResultSet[PVE::MultipleArgStart];
			PrlHandleVmGuest* pVmGuestHandle = new PrlHandleVmGuest( m_pVm, sSessionUuid );
			if (pVmGuestHandle)
				res_handle = pVmGuestHandle->GetHandle();
		}
		break;
		case PVE::DspCmdInstallAppliance:
		case PVE::DspCmdDirRegVm:
		case PVE::DspCmdDirReg3rdPartyVm:
		case PVE::DspCmdDirRestoreVm:
		case PVE::DspCmdDirVmClone:
		case PVE::DspCmdVmGetConfig:
		case PVE::DspCmdGetVmConfigById:
		case PVE::DspCmdGetDefaultVmConfig:
		case PVE::DspCmdDirVmEditBegin:
		{
			QString str = m_Result.m_hashResultSet[GetHashIdByOperationCode(m_Result.getOpCode())];

			PrlHandleVmPtr pVm;
			pVm = m_pServer->GetVmByConfig( str ) ;
			if (!pVm)
				pVm = new PrlHandleVm(m_pServer);
			else
				pVm->AddRef();

			PRL_RESULT rc = pVm->fromString(str.toUtf8().data());
			if ( PRL_FAILED(rc) )
				WRITE_TRACE(DBG_FATAL, "An error occurred on VM parsing: %.8X '%s'", rc, PRL_RESULT_TO_STRING(rc));

			res_handle = pVm->GetHandle();
		}
		break;

		case PVE::DspCmdPerfomanceStatistics:
		{
            SmartPtr<CVmEvent> pEvent( new CVmEvent(m_Result.m_hashResultSet[
                                                        GetHashIdByOperationCode(m_Result.getOpCode())]) ) ;
            if ( !pEvent.getImpl() )
                return PRL_INVALID_HANDLE ;

            PrlHandleVmPtr pVm;
            PrlHandleVmEvent* pVmEvent = new PrlHandleVmEvent(pVm, pEvent.getImpl()) ;
			if (pVmEvent)
                res_handle =  pVmEvent->GetHandle();
		}
		break;
		case PVE::DspCmdVmGuestRunProgram:
		{
            SmartPtr<CVmEvent> pEvent( new CVmEvent(m_Result.m_hashResultSet[
                                                        GetHashIdByOperationCode(m_Result.getOpCode())]) ) ;
            if ( !pEvent.getImpl() )
                return PRL_INVALID_HANDLE ;

            PrlHandleVmPtr pVm;
            PrlHandleVmEvent* pVmEvent = new PrlHandleVmEvent(pVm, pEvent.getImpl()) ;
			if (pVmEvent)
                res_handle =  pVmEvent->GetHandle();
		}
		break;

		case PVE::DspCmdVmGuestGetNetworkSettings:
		{
			QString sHostHwInfo = m_Result.m_hashResultSet[GetHashIdByOperationCode(m_Result.getOpCode())];
			PrlHandleSrvConfig *pSrvConfig = new PrlHandleSrvConfig(sHostHwInfo);
			if (pSrvConfig)
				res_handle = pSrvConfig->GetHandle();
		}
		break;

		case PVE::DspCmdVmGetProblemReport:
		{
			QString sReportData = m_Result.m_hashResultSet[GetHashIdByOperationCode(m_Result.getOpCode())];
			SmartPtr<CProblemReport> pProblemReport(new CProblemReport);
			pProblemReport->fromString(sReportData);
			PrlHandleProblemReport *pReport = new PrlHandleProblemReport(pProblemReport);
			if (pReport)
				res_handle = pReport->GetHandle();
		}
		break;

		case PVE::DspCmdGetNetworkClassesConfig:
		{
			QString sConf = m_Result.m_hashResultSet[GetHashIdByOperationCode(m_Result.getOpCode())];

			CNetworkClassesConfig Conf;
			Conf.fromString(sConf);

			PrlHandleHandlesList *lst = new PrlHandleHandlesList;
			foreach(CNetworkClass *pEntry, Conf.m_lstNetworkClasses)
			{
				PrlHandleBasePtr phEntry(new PrlHandleNetworkClass(pEntry->toString()));
				lst->AddItem(phEntry);
			}
			res_handle = lst->GetHandle();
			break;
		}
		case PVE::DspCmdGetNetworkShapingConfig:
		{
			QString sConf = m_Result.m_hashResultSet[GetHashIdByOperationCode(m_Result.getOpCode())];

			PrlHandleNetworkShapingConfig *pConfig = new PrlHandleNetworkShapingConfig(sConf);
			if ( !pConfig )
				return PRL_INVALID_HANDLE;

			res_handle = pConfig->GetHandle();
			break;
		}
		case PVE::DspCmdSMCGetDispatcherRTInfo:
		case PVE::DspCmdSMCGetCommandHistoryByVm:
		case PVE::DspCmdSMCGetCommandHistoryByUser:
		case PVE::DspCmdVmGetPackedProblemReport:
		case PVE::DspCmdDirGetVmList:
		case PVE::DspCmdVmSectionValidateConfig:
		case PVE::DspCmdUserInfoList:
		case PVE::DspCmdGetVirtualNetworkList:
		case PVE::DspCmdVmGetSuspendedScreen:
		case PVE::DspCmdVmCaptureScreen:
		case PVE::DspCmdAllHostUsers:
		case PVE::DspCmdConfigureGenericPci:
		case PVE::DspCmdPrepareForHibernate:
		case PVE::DspCmdAfterHostResume:
		case PVE::DspCmdGetOfflineServicesList:
		case PVE::DspCmdRestartNetworkShaping:
		case PVE::DspCmdGetCtTemplateList:
		case PVE::DspCmdGetIPPrivateNetworksList:
		case PVE::DspCmdGetPluginsList:
		case PVE::DspCmdGetCPUPoolsList:
		default:
		break;
	}

	return (res_handle);
}

/**
 * Get param token by index
 *
 * @param index of parameter
 * @param pointer to a command specific object that contains parameter value
 * @return PRL_ERR_SUCCESS if the method succeeds
 */
PRL_RESULT PrlHandleResult::GetParamToken(PRL_UINT32 index, PRL_VOID_PTR_PTR data_ptr)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString str					= m_Result.GetParamToken(index);
	QString sHashResult;

	switch( m_Result.getOpCode() )
	{
	case PVE::DspCmdDirGetVmList:
	{
		PrlHandleVmPtr pVm;
		pVm = m_pServer->GetVmByConfig(str);
		if (!pVm)
		{
			pVm = new PrlHandleVm(m_pServer);
			if ( !pVm )
				return PRL_ERR_OUT_OF_MEMORY;

			PRL_RESULT rc = pVm->fromString(str.toUtf8().data());
			if ( PRL_FAILED(rc) )
				return rc;
		} else
			pVm->AddRef();
		*data_ptr = (void *)pVm->GetHandle();
		break;
	}
	case PVE::DspCmdUserGetHostHwInfo:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdUserGetHostHwInfo_strHostHwInfo];
		break;
	case PVE::DspCmdFsGetDiskList:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdFsGetDiskList_strDiskList];
		break;
	case PVE::DspCmdFsGetCurrentDirectory:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdFsGetCurrentDirectory_strDir];
		break;
	case PVE::DspCmdFsGetDirectoryEntries:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdFsGetDirectoryEntries_strEntriesList];
		break;
	case PVE::DspCmdFsGetFileList:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdFsGetFileList_strFileList];
		break;
	case PVE::DspCmdFsCreateDirectory:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdFsCreateDirectory_strDirEntries];
		break;
	case PVE::DspCmdFsRenameEntry:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdFsRenameEntry_strDirEntries];
		break;
	case PVE::DspCmdFsRemoveEntry:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdFsRemoveEntry_strDirEntries];
		break;
	case PVE::DspCmdUserGetProfile:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdUserGetProfile_strUserProfile];
		break;
	case PVE::DspCmdGetHostCommonInfo:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdGetHostCommonInfo_strCommonInfo];
		break;
	case PVE::DspCmdVmGetProblemReport:
	case PVE::DspCmdVmGetPackedProblemReport:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdVmGetProblemReport_strReport];
		break;
	case PVE::DspCmdVmGetSnapshotsTree:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdVmGetSnapshotsTree_strSnapshotsTree];
		break;
	case PVE::DspCmdGetBackupTree:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdGetBackupTree_strBackupsTree];
		break;
	case PVE::DspCmdVmGetConfig:
	case PVE::DspCmdGetVmConfigById:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdVmGetConfig_strVmConfig];
		break;
	case PVE::DspCmdGetVmInfo:
	case PVE::DspCmdGetVmToolsInfo:
	{
		*data_ptr = (void *)ResultAsHandle();
		break;
	}
	case PVE::DspCmdSMCGetDispatcherRTInfo:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdSMCGetDispatcherRTInfo_strEventContainer];
		break;
	case PVE::DspCmdSMCGetCommandHistoryByVm:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdSMCGetCommandHistoryByVm_strContainer];
		break;
	case PVE::DspCmdSMCGetCommandHistoryByUser:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdSMCGetCommandHistoryByUser_strContainer];
		break;
	case PVE::DspCmdGetHostStatistics:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdGetHostStatistics_strSystemStatistics];
		break;
	case PVE::DspCmdVmGetStatistics:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdVmGetStatistics_strSystemStatistics];
		break;
	case PVE::DspCmdDirRegVm:
	case PVE::DspCmdDirReg3rdPartyVm:
	case PVE::DspCmdDirRestoreVm:
	case PVE::DspCmdDirVmClone:
	case PVE::DspCmdInstallAppliance:
	case PVE::DspCmdGetDefaultVmConfig:
	case PVE::DspCmdDirVmEditBegin:
	{
		str = m_Result.m_hashResultSet[PVE::DspCmdDirRegVm_strVmConfig];

		PrlHandleVmPtr pVm;
		pVm = m_pServer->GetVmByConfig( str ) ;
		if (!pVm)
		{
			pVm = new PrlHandleVm(m_pServer);
			if ( !pVm )
				return PRL_ERR_OUT_OF_MEMORY;
		}
		else
			pVm->AddRef();

		PRL_RESULT rc = pVm->fromString(str.toUtf8().data());
		if ( PRL_FAILED(rc) )
			return rc;
		*data_ptr = (void *)pVm->GetHandle();
	}
	break;
	case PVE::DspCmdUserGetLicenseInfo:
	case PVE::DspCmdUserUpdateLicense:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdUserGetLicenseInfo_strLicenseInfo];
		break;
	case PVE::DspCmdUpdateDeviceInfo:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdUpdateDeviceInfo_strDeviceInfo];
		break;
	case PVE::DspCmdGetNetServiceStatus:
		sHashResult = m_Result.m_hashResultSet[PVE::DspCmdGetNetServiceStatus_strNetServiceStatus];
		break;
	default:
		*data_ptr = (void *)strdup(str.toUtf8().data());
	}
	if (sHashResult.size())
		*data_ptr = (void *)strdup(sHashResult.toUtf8().data());

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleResult::GetError(PRL_VOID_PTR_PTR data_ptr)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_Result.GetError()) {
		*data_ptr = 0;
		return (PRL_ERR_NO_DATA);
	}
	*data_ptr = strdup(m_Result.GetError()->toString().toUtf8().data());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleResult::GetResultAsHandle(PRL_HANDLE_PTR pHandle)
{
	return (GetParamByIndex(0, pHandle));
}

PRL_RESULT PrlHandleResult::GetParamByIndex(PRL_UINT32 nIndex, PRL_HANDLE_PTR phParam)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= InternalGetParamsCount())
		return (PRL_ERR_INVALID_ARG);
	//Special case for binary responses
	if (m_Result.getBinaryContent())
	{
		if (
			PVE::DspCmdUserLogin == m_Result.getOpCode() ||
			PVE::DspCmdUserLoginLocal == m_Result.getOpCode() ||
			PVE::DspCmdUserLoginLocalStage2 == m_Result.getOpCode()
			)
		{
			PrlHandleLoginResponse *pLoginInfo = new PrlHandleLoginResponse( m_Result.getBinaryContent() ) ;
			if (!pLoginInfo)
				return (PRL_ERR_OUT_OF_MEMORY);
			*phParam = pLoginInfo->GetHandle();
		}
		else if (PVE::DspCmdVmGetPackedProblemReport == m_Result.getOpCode())
		{
			CVmEvent *pEvent = m_Result.getBinaryContent();
			CVmBinaryEventParameter *pVmBinaryEventParameter =
				dynamic_cast<CVmBinaryEventParameter *>(pEvent->getEventParameter(EVT_PARAM_VM_PROBLEM_REPORT));
			if (!pVmBinaryEventParameter)
				return (PRL_ERR_INVALID_ARG);
			QByteArray _buffer = pVmBinaryEventParameter->GetByteArray();
			CVmEventParameter *pDirPathEventParam = pEvent->getEventParameter(EVT_PARAM_VM_PROBLEM_REPORT_DIR_PATH);
			CPackedProblemReport *pProblemReport = NULL;
			PRL_RESULT nResult =
				CPackedProblemReport::createInstance(CPackedProblemReport::ClientSide, &pProblemReport, _buffer, pDirPathEventParam->getParamValue());
			if (PRL_FAILED(nResult))
				return (nResult);
			PrlHandleProblemReport *pReport = new PrlHandleProblemReport(SmartPtr<CProblemReport>(pProblemReport));
			if (!pReport)
				return (PRL_ERR_OUT_OF_MEMORY);
			*phParam = pReport->GetHandle();
		}
		else
		{
			PrlHandleVmEvent* pVmEvent = new PrlHandleVmEvent(m_pServer, m_Result.getBinaryContent()->getEventType(), m_Result.getBinaryContent());
			if (!pVmEvent)
				return PRL_ERR_OUT_OF_MEMORY;

			*phParam = pVmEvent->GetHandle();
		}
	}
	else if ( m_Result.getOpCode() == PVE::DspCmdDirGetVmList )
	{
		QString str	= m_Result.GetParamToken(nIndex);
		PrlHandleVmPtr pVm;
		pVm = m_pServer->GetVmByConfig(str);
		if (!pVm)
		{
			pVm = new PrlHandleVm(m_pServer);
			if ( !pVm )
				return PRL_ERR_OUT_OF_MEMORY;
		}
		else
			pVm->AddRef();

		PRL_RESULT rc = pVm->fromString(str.toUtf8().data());
		if ( PRL_FAILED(rc) )
			return rc;

		*phParam = pVm->GetHandle();
	}
	else if ( m_Result.getOpCode() == PVE::DspCmdLookupParallelsServers )
	{
		PrlHandleServerInfo *pServerInfo = new PrlHandleServerInfo(m_Result.GetParamToken(nIndex));
		*phParam = pServerInfo->GetHandle();
	}
	else if ( m_Result.getOpCode() == PVE::DspCmdStartSearchConfig )
	{
		QString str	= m_Result.GetParamToken(nIndex);

		SmartPtr<CVmEvent> pTmpEvent( new CVmEvent(str) );
		if ( !pTmpEvent.getImpl() )
			return PRL_ERR_OUT_OF_MEMORY;

		PrlHandleFoundVmInfo* pVmInfo = new PrlHandleFoundVmInfo(pTmpEvent.getImpl());
		if (!pVmInfo)
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pVmInfo->GetHandle();
	}
	else if (   m_Result.getOpCode() == PVE::DspCmdVmSectionValidateConfig
			 || m_Result.getOpCode() == PVE::DspCmdVmStart
			 || m_Result.getOpCode() == PVE::DspCmdVmStartEx
			 || m_Result.getOpCode() == PVE::DspCmdDirVmEditCommit
			 || m_Result.getOpCode() == PVE::DspCmdAllHostUsers
			 || m_Result.getOpCode() == PVE::DspCmdConfigureGenericPci
			 || m_Result.getOpCode() == PVE::DspCmdDirVmMigrate
			 || m_Result.getOpCode() == PVE::DspCmdVmCompact )
	{
		QString str	= m_Result.GetParamToken(nIndex);

		if( m_Result.getOpCode() == PVE::DspCmdDirVmEditCommit && PRL_SUCCEEDED( m_Result.getReturnCode() ) )
		{
			if( str.isEmpty() )
				return PRL_ERR_NO_DATA; // old schema

			PrlHandleVmPtr pVm (new PrlHandleVm(m_pServer) );
			if( !pVm )
				return PRL_ERR_OUT_OF_MEMORY;

			PRL_RESULT rc = pVm->fromString(str.toUtf8().data());
			if ( PRL_FAILED(rc) )
				WRITE_TRACE(DBG_FATAL, "An error occurred on VM parsing: %.8X '%s'", rc, PRL_RESULT_TO_STRING(rc));

			*phParam = pVm->GetHandle();
		}
		else
		{
			SmartPtr<CVmEvent> pEvent( new CVmEvent(str) );
			if ( !pEvent.getImpl() )
				return PRL_ERR_OUT_OF_MEMORY;

			PrlHandleVmPtr pVm;
			PrlHandleVmEvent* pVmEvent = new PrlHandleVmEvent(pVm, pEvent.getImpl());
			if (!pVmEvent)
				return PRL_ERR_OUT_OF_MEMORY;

			*phParam = pVmEvent->GetHandle();
		}

	}
	else if (m_Result.getOpCode() == PVE::DspCmdUserInfoList)
	{
		QString sUserInfo = m_Result.GetParamToken(nIndex);
		PrlHandleUserInfo* pUserInfo = new PrlHandleUserInfo(sUserInfo);
		if ( !pUserInfo )
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pUserInfo->GetHandle();
	}
	else if (m_Result.getOpCode() == PVE::DspCmdGetVirtualNetworkList)
	{
		QString sVirtNet = m_Result.GetParamToken(nIndex);
		PrlHandleVirtNet* pVirtNet = new PrlHandleVirtNet(sVirtNet);
		if ( !pVirtNet )
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pVirtNet->GetHandle();
	}
	else if (m_Result.getOpCode() == PVE::DspCmdGetOfflineServicesList)
	{
		QString sOffSrv = m_Result.GetParamToken(nIndex);
		PrlHandleOfflineService* pOffSrv = new PrlHandleOfflineService(sOffSrv);
		if ( !pOffSrv)
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pOffSrv->GetHandle();
	}
	else if ((m_Result.getOpCode() == PVE::DspCmdVmGetSuspendedScreen) ||
			(m_Result.getOpCode() == PVE::DspCmdVmCaptureScreen))
	{
		QString qsScreen = m_Result.GetParamToken(nIndex);

		SmartPtr<CVmEvent> pEvent( new CVmEvent() );
		if ( !pEvent.getImpl() )
			return PRL_ERR_OUT_OF_MEMORY;

		pEvent->addEventParameter( new CVmEventParameter(PVE::String, qsScreen) );

		PrlHandleVmPtr pVm;
		PrlHandleVmEvent* pVmEvent = new PrlHandleVmEvent(pVm, pEvent.getImpl());
		if (!pVmEvent)
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pVmEvent->GetHandle();
	}
	else if (PVE::DspCmdUpdateDeviceInfo == m_Result.getOpCode())
	{
		QString sXml = m_Result.m_hashResultSet[PVE::DspCmdUpdateDeviceInfo_strDeviceInfo];
		PrlHandleVmDeviceHardDrive *pHdd = new PrlHandleVmDeviceHardDrive;
		if (!pHdd)
			return PRL_ERR_OUT_OF_MEMORY;
		PRL_RESULT nRes = pHdd->fromStringCommon(QSTR2UTF8(sXml));
		if (PRL_FAILED(nRes)) {
			delete pHdd;
			return nRes;
		}
		*phParam = pHdd->GetHandle();
	}
	else if (m_Result.getOpCode() == PVE::DspCmdGetCtTemplateList)
	{
		QString sCtTemplate = m_Result.GetParamToken(nIndex);
		PrlHandleCtTemplate* pCtTemplate = new PrlHandleCtTemplate(sCtTemplate);
		if ( !pCtTemplate )
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pCtTemplate->GetHandle();
	}
	else if (m_Result.getOpCode() == PVE::DspCmdGetIPPrivateNetworksList)
	{
		QString sPrivNet = m_Result.GetParamToken(nIndex);
		PrlHandleIPPrivNet* pPrivNet = new PrlHandleIPPrivNet(sPrivNet);
		if ( !pPrivNet )
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pPrivNet->GetHandle();
	}
	else if (m_Result.getOpCode() == PVE::DspCmdGetPluginsList)
	{
		QString sPluginInfo = m_Result.GetParamToken(nIndex);
		PrlHandlePluginInfo* pPluginInfo = new PrlHandlePluginInfo(sPluginInfo);
		if ( !pPluginInfo )
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pPluginInfo->GetHandle();
	}
	else if (PVE::DspCmdCreateVmBackup == m_Result.getOpCode())
	{
		PrlHandleBackupResult* pBackupResult = new PrlHandleBackupResult(m_Result.GetParamToken(nIndex));
		if ( !pBackupResult )
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pBackupResult->GetHandle();
	}
	else if (m_Result.getOpCode() == PVE::DspCmdGetCPUPoolsList)
	{
		QString sCpuPool = m_Result.GetParamToken(nIndex);
		PrlHandleCpuPool* pCpuPool = new PrlHandleCpuPool(sCpuPool);
		if ( !pCpuPool )
			return PRL_ERR_OUT_OF_MEMORY;

		*phParam = pCpuPool->GetHandle();
	}
	else
	{
		*phParam = ResultAsHandle();
		if (*phParam == PRL_INVALID_HANDLE)
			return (PRL_ERR_NO_DATA);
	}
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleResult::GetParamByIndexAsString(PRL_UINT32 nIndex, PRL_STR sParamBuf, PRL_UINT32_PTR pnParamBufSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= InternalGetParamsCount())
		return (PRL_ERR_INVALID_ARG);
	//Special case for binary responses
	if (m_Result.getBinaryContent())
		return (PRL_ERR_INVALID_ARG);
	QString sParamValue;
	PVE::IDspMethodsArgs nHashId = GetHashIdByOperationCode(m_Result.getOpCode());
	if (nHashId == PVE::MultipleArgStart)
	{
		sParamValue = m_Result.GetParamToken(nIndex);
	}
	else
	{
		sParamValue = m_Result.m_hashResultSet[nHashId];
	}
	return (CopyStringValue(sParamValue, sParamBuf, pnParamBufSize));
}

PRL_RESULT PrlHandleResult::FromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CProtoCommandPtr pCmd = CProtoSerializer::ParseCommand(PVE::DspWsResponse, QString::fromLocal8Bit(sXml));
	if (!pCmd->IsValid())
		return (PRL_ERR_INVALID_ARG);
	CProtoCommandDspWsResponse *pResponseCommand = CProtoSerializer::CastToProtoCommand<CProtoCommandDspWsResponse>(pCmd);
	pResponseCommand->FillResult(&m_Result);
	return (PRL_ERR_SUCCESS);
}

PRL_UINT32 PrlHandleResult::InternalGetParamsCount()
{
	//Special case for binary responses
	if (m_Result.getBinaryContent())
		return (1);

	PVE::IDspMethodsArgs nHashId = GetHashIdByOperationCode(m_Result.getOpCode());

	//If operation do not have static identifier at result hash then return count of dynamic hash parameters
	if (nHashId == PVE::MultipleArgStart)
		return (m_Result.GetParamsCount());

	//If static field for operation not filled return 0 else return 1
	if (!m_Result.m_hashResultSet.contains(nHashId))
		return 0;
	else
		return 1;
}

