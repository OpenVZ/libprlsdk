/*
 * PrlHandleEventParam.cpp
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


#include "XmlModel/VmConfig/CVmSecurity.h"
#include "PrlHandleDispConfig.h"
#include "PrlHandleVmDeviceFloppy.h"
#include "PrlHandleVmDeviceHardDrive.h"
#include "PrlHandleVmDeviceOpticalDisk.h"
#include "PrlHandleVmDeviceSerialPort.h"
#include "PrlHandleVmDeviceParallelPort.h"
#include "PrlHandleVmDeviceUsbPort.h"
#include "PrlHandleVmDeviceNetAdapter.h"
#include "PrlHandleVmDeviceSound.h"
#include "PrlHandleVmDeviceGenericPci.h"
#include "PrlHandleVmDeviceGenericScsi.h"
#include "PrlHandleVmDeviceDisplay.h"
#include "PrlHandleSystemStatistics.h"
#include "PrlHandleFoundVmInfo.h"
#include "PrlHandleAccessRights.h"
#include "PrlHandleLicense.h"

#include "PrlHandleEventParam.h"
#include "XmlModel/Messaging/CVmEventParameter.h"
#include "XmlModel/Messaging/CVmBinaryEventParameter.h"
#include "PrlHandleVm.h"
#include "PrlHandleServer.h"
#include "Libraries/Logging/Logging.h"
#include "Libraries/Std/SmartPtr.h"
#include "Libraries/Std/PrlAssert.h"

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

#define SYNCHRO_PARENT_VM_EVENT \
	QMutexLocker _lock(m_pVmEvent.isValid() ? m_pVmEvent->GetSynchroObject() : &m_HandleMutex);

#define CHECK_EVENT_PARAM_OBJECT\
	{\
		if (!m_pEventParam)\
		{\
			if (m_pVmEvent)\
			{\
				foreach(CVmEventParameter *pEventParam, m_pVmEvent->GetEventObject().m_lstEventParameters)\
				{\
					if (GetHashCode() == qHash(pEventParam->getParamName()))\
					{\
						m_pEventParam = pEventParam;\
						/* Reregister instance with new pointer */\
						PrlControlValidity::RegisterInstance(GetHandle(), m_pEventParam);\
						break;\
					}\
				}\
				if (!m_pEventParam)\
					return (PRL_ERR_OBJECT_WAS_REMOVED);\
			}\
			else\
				return (PRL_ERR_OBJECT_WAS_REMOVED);\
		}\
	}

PrlHandleEventParam::PrlHandleEventParam(const PrlHandleVmEventPtr &pVmEvent, CVmEventParameter *pEventParam)
: PrlHandleBase(PHT_EVENT_PARAMETER), PrlControlValidity(GetHandle()), m_pEventParam(pEventParam), m_pVmEvent(pVmEvent)
{
	PrlControlValidity::RegisterInstance(GetHandle(), m_pEventParam);
}

PrlHandleEventParam::PrlHandleEventParam(class CVmEventParameter *pEventParam)
: PrlHandleBase(PHT_EVENT_PARAMETER), PrlControlValidity(GetHandle()), m_pEventParam(pEventParam)
{
}

PrlHandleEventParam::~PrlHandleEventParam()
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if ( ! m_pVmEvent.isValid() )
		delete m_pEventParam;
	m_pEventParam = NULL;
}

PRL_RESULT PrlHandleEventParam::GetName(PRL_STR sEventParamNameBuf, PRL_UINT32_PTR pnEventParamNameBufLength)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	return CopyStringValue(m_pEventParam->getParamName(), sEventParamNameBuf, pnEventParamNameBufLength);
}

PRL_RESULT PrlHandleEventParam::GetParamType(PRL_PARAM_FIELD_DATA_TYPE_PTR pnParamType)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT

	PRL_PARAM_FIELD_DATA_TYPE nParamType = PFD_ENTITY;

	if (CVmEventParameter::Binary == m_pEventParam->getEventParameterClassType())
		nParamType = PFD_BINARY;
	else
	{
		PVE::ParamFieldDataType nEnumParamType = m_pEventParam->getParamType();
		switch (nEnumParamType)
		{
			case PVE::UnsignedInt:
				nParamType = PFD_UINT32;
			break;

			case PVE::Int64:
				nParamType = PFD_INT64;
			break;

			case PVE::UInt64:
				nParamType = PFD_UINT64;
			break;

			case PVE::VmReturnCode:
			case PVE::UserResponseOnQuestion:
			case PVE::Integer:
				nParamType = PFD_INT32;
			break;

			case PVE::CData: nParamType = PFD_CDATA; break;

			case PVE::MessageShowNextTime:
			case PVE::Boolean:
				nParamType = PFD_BOOLEAN;
			break;

			case PVE::Text:
			case PVE::String:
			case PVE::Uuid:
				nParamType = PFD_STRING;
			break;

			case PVE::VmConfiguration:
			case PVE::VmLicense:
				nParamType = PFD_ENTITY;
			break;

			case PVE::VmRuntimeState:
			case PVE::VmSmartKernelState:
			case PVE::VmGuestToolActiveState:
			case PVE::VmDeviceStateData:
				nParamType = PFD_ENUMERATION;
			break;

			default:
				nParamType = PFD_UNKNOWN;
			break;
		}
	}

	*pnParamType = nParamType;

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleEventParam::ToString(PRL_STR sEventParamValueBuf, PRL_UINT32_PTR pnEventParamValueBufLength)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT

	return CopyStringValue(m_pEventParam->getParamValue(), sEventParamValueBuf, pnEventParamValueBufLength);
}

PRL_RESULT PrlHandleEventParam::GetBuffer(PRL_VOID_PTR pEventParamValue, PRL_UINT32_PTR pnEventParamValueBufLength)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	if (CVmEventParameter::Binary == m_pEventParam->getEventParameterClassType())
	{
		CVmBinaryEventParameter *pVmBinaryEventParameter = static_cast<CVmBinaryEventParameter *>(m_pEventParam);

		return CopyByteArrayValue(pVmBinaryEventParameter->GetByteArray(), pEventParamValue, pnEventParamValueBufLength);
	}

	return CopyByteArrayValue(m_pEventParam->getParamValue().toUtf8(), pEventParamValue, pnEventParamValueBufLength);
}

PRL_RESULT PrlHandleEventParam::ToCData(PRL_STR sEventParamValue, PRL_UINT32_PTR pnEventParamValueBufLength)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	return CopyStringValue(m_pEventParam->getCdata(), sEventParamValue, pnEventParamValueBufLength);
}

void PrlHandleEventParam::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pEventParam = NULL;
}

PRL_RESULT PrlHandleEventParam::ToUint32(PRL_UINT32_PTR pnParamValue)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	*pnParamValue = m_pEventParam->getParamValue().toUInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleEventParam::ToInt32(PRL_INT32_PTR pnParamValue)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	*pnParamValue = m_pEventParam->getParamValue().toInt();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleEventParam::ToUint64(PRL_UINT64_PTR pnParamValue)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	*pnParamValue = m_pEventParam->getParamValue().toULongLong();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleEventParam::ToInt64(PRL_INT64_PTR pnParamValue)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	*pnParamValue = m_pEventParam->getParamValue().toLongLong();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleEventParam::ToBoolean(PRL_BOOL_PTR pbParamValue)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	*pbParamValue = PRL_BOOL(m_pEventParam->getParamValue().toUInt());
	return (PRL_ERR_SUCCESS);
}

namespace {
/**
 * Creates VM object by specified VM config
 * @param pointer to the server object
 * @param pointer to buffer for storing result object handle
 * @param VM configuration
 * @return PRL_RESULT
 */
PRL_RESULT CreateVmConfigObject(PrlHandleServerPtr pServer, PRL_HANDLE_PTR phParamValue, const QString &sVmConfig)
{
	PrlHandleVmPtr pVm = pServer->GetVmByConfig(sVmConfig);
	if (!pVm)
	{
		pVm = new PrlHandleVm(pServer);
		PRL_RESULT rc = pVm->fromString(sVmConfig.toUtf8().data());
		if ( PRL_FAILED(rc) )
			return rc;
	} else
		pVm->AddRef();
	*phParamValue = pVm->GetHandle();
	return (PRL_ERR_SUCCESS);
}

/**
* Creates VM object by specified VM event
* @param pointer to buffer for storing result object handle
* @param VM event as string
* @return PRL_RESULT
*/
PRL_RESULT CreateVmEventObject(PrlHandleServerPtr pServer,PRL_HANDLE_PTR phParamValue, const QString & strVmEvent)
{
	PrlHandleVmPtr pVm = pServer->GetVmHandleByUuid(strVmEvent);

	PrlHandleVmEvent *pVmEvent = new PrlHandleVmEvent(pVm);
	if (!pVmEvent)
		return (PRL_ERR_OUT_OF_MEMORY);
	pVmEvent->FromString(strVmEvent.toUtf8().constData());
	*phParamValue = pVmEvent->GetHandle();
	return (PRL_ERR_SUCCESS);
}

/**
 * Creates dispatcher config common preferences object by specified input data
 * @param pointer to buffer for storing result object handle
 * @param dispatcher config common preferences XML representation
 * @return PRL_RESULT
 */
PRL_RESULT CreateDispCommonPrefsObject(PRL_HANDLE_PTR phParamValue, const QString &sDispConfig)
{
	// #116344
	const QString sNetworkConfig = "";

	PrlHandleDispConfig *pDispConfig = new PrlHandleDispConfig(sDispConfig, sNetworkConfig );
	if (!pDispConfig)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phParamValue = pDispConfig->GetHandle();
	return (PRL_ERR_SUCCESS);
}

#define CREATE_VM_DEVICE(sdk_type, xml_model_type)\
	{\
		xml_model_type *pXmlModelDevice = new xml_model_type;\
		if (!StringToElement<xml_model_type *>(pXmlModelDevice, sVmDevConfig))\
		{\
			LOG_MESSAGE(DBG_FATAL, "Failed to concrete instance parse VM device config: [%s]", sVmDevConfig.toUtf8().data());\
			return (PRL_ERR_INVALID_ARG);\
		}\
		pVmDev = new sdk_type(PrlHandleVmPtr((PrlHandleVm *)PRL_INVALID_HANDLE), pXmlModelDevice);\
	}

/**
 * Creates VM device object by specified input data
 * @param pointer to buffer for storing result object handle
 * @param VM device XML representation
 * @return PRL_RESULT
 */
PRL_RESULT CreateVmDevObject(PRL_HANDLE_PTR phParamValue, const QString &sVmDevConfig)
{
	SmartPtr<CVmDevice> pDevice( new CVmDevice );
	if (!StringToElement<CVmDevice *>(pDevice.getImpl(), sVmDevConfig))
	{
		LOG_MESSAGE(DBG_FATAL, "Failed to parse VM device config: [%s]", sVmDevConfig.toUtf8().data());
		return (PRL_ERR_INVALID_ARG);
	}
	PrlHandleVmDevice *pVmDev = NULL;

	switch (pDevice->getDeviceType())
	{
		case PDE_FLOPPY_DISK:
			CREATE_VM_DEVICE(PrlHandleVmDeviceFloppy, CVmFloppyDisk)
		break;
		case PDE_HARD_DISK:
			CREATE_VM_DEVICE(PrlHandleVmDeviceHardDrive, CVmHardDisk)
		break;
		case PDE_OPTICAL_DISK:
			CREATE_VM_DEVICE(PrlHandleVmDeviceOpticalDisk, CVmOpticalDisk)
		break;
		case PDE_SERIAL_PORT:
			CREATE_VM_DEVICE(PrlHandleVmDeviceSerialPort, CVmSerialPort)
		break;
		case PDE_PARALLEL_PORT:
			CREATE_VM_DEVICE(PrlHandleVmDeviceParallelPort, CVmParallelPort)
		break;
		case PDE_USB_DEVICE:
			CREATE_VM_DEVICE(PrlHandleVmDeviceUsbPort,CVmUsbDevice)
		break;
		case PDE_SOUND_DEVICE:
			CREATE_VM_DEVICE(PrlHandleVmDeviceSound, CVmSoundDevice)
		break;
		case PDE_GENERIC_NETWORK_ADAPTER:
			CREATE_VM_DEVICE(PrlHandleVmDeviceNetAdapter, CVmGenericNetworkAdapter)
		break;
		case PDE_GENERIC_PCI_DEVICE:
			CREATE_VM_DEVICE(PrlHandleVmDeviceGenericPci, CVmGenericPciDevice)
		break;
		case PDE_GENERIC_SCSI_DEVICE:
			CREATE_VM_DEVICE(PrlHandleVmDeviceGenericScsi, CVmGenericScsiDevice)
		break;
		case PDE_PCI_VIDEO_ADAPTER:
			CREATE_VM_DEVICE(PrlHandleVmDeviceDisplay, CVmPciVideoAdapter)
		break;
		default:
		{
			LOG_MESSAGE(DBG_FATAL, "Unknown VM device XML representation: [%s]", sVmDevConfig.toUtf8().data());
			return (PRL_ERR_INVALID_ARG);
		}
		break;
	}

	*phParamValue = pVmDev->GetHandle();
	return (PRL_ERR_SUCCESS);
}

/**
 * Creates system statistics object by specified input data
 * @param pointer to buffer for storing result object handle
 * @param system statistics XML representation
 * @return PRL_RESULT
 */
PRL_RESULT CreateSystemStatisticsObject(PRL_HANDLE_PTR phParamValue, CVmEventParameter *pParam)
{
	PrlHandleSystemStatistics *pSystemStatistics = NULL;
	if (pParam->getEventParameterClassType() == CVmEventParameter::BaseType)
		pSystemStatistics = new PrlHandleSystemStatistics(pParam->getParamValue());
	else if (pParam->getEventParameterClassType() == CVmEventParameter::Binary)
	{
		CVmBinaryEventParameter *pVmBinaryEventParameter = static_cast<CVmBinaryEventParameter *>(pParam);
		pSystemStatistics = new PrlHandleSystemStatistics(*pVmBinaryEventParameter->getBinaryDataStream().getImpl());
	}
	if (!pSystemStatistics)
	{
		WRITE_TRACE(DBG_FATAL, "Fatal error - incorrect event parameter type for statistics object initialization");
		return (PRL_ERR_UNEXPECTED);
	}
	*phParamValue = pSystemStatistics->GetHandle();
	return (PRL_ERR_SUCCESS);
}

/**
* Creates found VM info object by specified input data
* @param pointer to buffer for storing result object handle
* @param CVmEvent XML representation
* @return PRL_RESULT
*/
PRL_RESULT CreateSearchLostConfigObject(PRL_HANDLE_PTR phParamValue, const QString &searchInfo)
{
	SmartPtr<CVmEvent> pTmpEvent( new CVmEvent(searchInfo) );
	if (!pTmpEvent.getImpl())
		return (PRL_ERR_OUT_OF_MEMORY);

	PrlHandleFoundVmInfo *pVMInfo = new PrlHandleFoundVmInfo(pTmpEvent.getImpl());
	if (!pVMInfo)
		return (PRL_ERR_OUT_OF_MEMORY);

	*phParamValue = pVMInfo->GetHandle();
	return (PRL_ERR_SUCCESS);
}

/**
* Creates VM security object by specified input data
* @param pointer to buffer for storing result object handle
* @param CVmEvent XML representation
* @return PRL_RESULT
*/
PRL_RESULT CreateVmSecurityObject(PRL_HANDLE_PTR phParamValue, const QString &sVmSecurity)
{
	CVmSecurity _vm_security;
	PRL_RESULT _ret = _vm_security.fromString(sVmSecurity);
	if (PRL_FAILED(_ret))
		return (_ret);

	PrlHandleAccessRights *pVmSecurity = new PrlHandleAccessRights(_vm_security);

	*phParamValue = pVmSecurity->GetHandle();
	return (PRL_ERR_SUCCESS);
}

/**
* Creates license object by specified input data
* @param pointer to buffer for storing result object handle
* @param CVmEvent XML representation
* @return PRL_RESULT
*/
PRL_RESULT CreateLicenseObject( PRL_HANDLE_PTR phParamValue, const QString &sLicense )
{
	CVmEvent _license;
	PRL_RESULT _ret = _license.fromString( sLicense );
	if ( PRL_FAILED( _ret ) )
		return ( _ret );

	PrlHandleLicense *pLicense = new PrlHandleLicense( &_license );

	*phParamValue = pLicense->GetHandle();
	return ( PRL_ERR_SUCCESS );
}

}

PRL_RESULT PrlHandleEventParam::ToHandle(PRL_HANDLE_PTR phParamValue)
{
	SYNCHRO_PARENT_VM_EVENT
	CHECK_EVENT_PARAM_OBJECT
	if (EVT_PARAM_VM_CONFIG == m_pEventParam->getParamName() ||
		EVT_PARAM_COMPLEX_EVENT == m_pEventParam->getParamName())
	{
		PrlHandleServerPtr pServer = m_pVmEvent->GetServer();
		if (pServer)
		{
			if (EVT_PARAM_VM_CONFIG == m_pEventParam->getParamName())
				return CreateVmConfigObject(pServer, phParamValue, m_pEventParam->getParamValue());
			else if (EVT_PARAM_COMPLEX_EVENT == m_pEventParam->getParamName())
				return CreateVmEventObject(pServer,phParamValue, m_pEventParam->getParamValue());
		}
		else
		{
			WRITE_TRACE(DBG_FATAL, "Event object not bint to server object. Type: %.8X, code: %.8X", m_pVmEvent->GetEventObject().getEventType(), m_pVmEvent->GetEventObject().getEventCode());
			return (PRL_ERR_INVALID_ARG);
		}

	}
	else if (EVT_PARAM_DISP_COMMON_PREFERENSES == m_pEventParam->getParamName())
		return CreateDispCommonPrefsObject(phParamValue, m_pEventParam->getParamValue());
	else if (EVT_PARAM_VM_CONFIG_DEV_STATE == m_pEventParam->getParamName())
		return CreateVmDevObject(phParamValue, m_pEventParam->getParamValue());
	else if (EVT_PARAM_STATISTICS == m_pEventParam->getParamName())
		return CreateSystemStatisticsObject(phParamValue, m_pEventParam);
	else if (EVT_PARAM_VM_SEARCH_INFO == m_pEventParam->getParamName())
		return CreateSearchLostConfigObject(phParamValue, m_pEventParam->getParamValue());
	else if (EVT_PARAM_VM_SECURITY_INFO == m_pEventParam->getParamName())
		return CreateVmSecurityObject(phParamValue, m_pEventParam->getParamValue());
	else if (EVT_PARAM_VM_LICENSE == m_pEventParam->getParamName())
		return CreateLicenseObject(phParamValue, m_pEventParam->getParamValue());
	else
	{
		WRITE_TRACE(DBG_FATAL, "PrEvtPrm_ToHandle(): unsupported event parameter with name '%s'", QSTR2UTF8(m_pEventParam->getParamName()));
		return (PRL_ERR_INVALID_ARG);
	}

	return (PRL_ERR_SUCCESS);
}

quint32 PrlHandleEventParam::ConcreteGenerateHashCode()
{
	SYNCHRO_PARENT_VM_EVENT
	return (qHash(m_pEventParam ? m_pEventParam->getParamName() : QString("")));
}
