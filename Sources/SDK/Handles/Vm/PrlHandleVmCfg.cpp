/*
 * PrlHandleVmCfg.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#include <prlcommon/PrlCommonUtilsBase/StringUtils.h>
#include <prlxmlmodel/ParallelsObjects/CVmProfileHelper.h>

#include "PrlHandleVmCfg.h"
#include "PrlHandleSrvConfig.h"
#include "PrlHandleStringsList.h"
#include "PrlHandleOpTypeList.h"
#include "PrlHandleVmInfo.h"
#include "PrlHandleVmDefaultConfig.h"
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
#include "PrlHandleVmShare.h"
#include "PrlHandleVmBootDev.h"
#include "PrlHandleNetworkRate.h"


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

namespace
{
/**
 * Returns number of VM devices
 */
PRL_UINT32 GetVmDevicesCount(CVmHardware *pHardware)
{
	return (pHardware->m_lstUsbDevices.count() + pHardware->m_lstSoundDevices.count() +
					pHardware->m_lstNetworkAdapters.count() + pHardware->m_lstParallelPorts.count() +
					pHardware->m_lstSerialPorts.count() + pHardware->m_lstHardDisks.count() +
					pHardware->m_lstOpticalDisks.count() + pHardware->m_lstFloppyDisks.count() +
					pHardware->m_lstGenericScsiDevices.count() + pHardware->m_lstGenericPciDevices.count() +
					pHardware->m_lstPciVideoAdapters.count());
}

}

PRL_RESULT PrlHandleVmCfg::SetConfig(PRL_HANDLE hVmCfg)
{
	Q_UNUSED(hVmCfg);

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetConfig(PRL_HANDLE_PTR phVmCfg)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	AddRef();
	*phVmCfg = GetHandle();
	return PRL_ERR_SUCCESS;
}

namespace {

template <typename T>
void ClearList(QList<T *> &lst)
{
	foreach(T *pItem, lst)
		delete pItem;
	lst.clear();
}

template <typename T>
void CopyList(const QList<T *> &sourceLst, QList<T *> &destLst)
{
	ClearList<T>(destLst);
	foreach(T *pItem, sourceLst)
		destLst.append(new T(*pItem));
}

}


PRL_RESULT PrlHandleVmCfg::SetDefaultConfig(PrlHandleSrvConfigPtr pSrvConfig,
										 PRL_UINT32 guestOsVerison,
										 PRL_BOOL needCreateDevices)
{
	PrlHandleVmDefaultConfig configGenerator ( pSrvConfig, PrlHandleVmPtr(this) );

	QString strCfg = configGenerator.CreateDefaultVmConfig( guestOsVerison, needCreateDevices );
	if ( !needCreateDevices )
	{
		CVmConfiguration _vm_conf(strCfg);
		SYNCHRO_INTERNAL_DATA_ACCESS
		CopyList<CVmFloppyDisk>(m_VmConfig.getVmHardwareList()->m_lstFloppyDisks, _vm_conf.getVmHardwareList()->m_lstFloppyDisks);
		CopyList<CVmOpticalDisk>(m_VmConfig.getVmHardwareList()->m_lstOpticalDisks, _vm_conf.getVmHardwareList()->m_lstOpticalDisks);
		CopyList<CVmHardDisk>(m_VmConfig.getVmHardwareList()->m_lstHardDisks, _vm_conf.getVmHardwareList()->m_lstHardDisks);
		CopyList<CVmSerialPort>(m_VmConfig.getVmHardwareList()->m_lstSerialPorts, _vm_conf.getVmHardwareList()->m_lstSerialPorts);
		CopyList<CVmParallelPort>(m_VmConfig.getVmHardwareList()->m_lstParallelPorts, _vm_conf.getVmHardwareList()->m_lstParallelPorts);
		CopyList<CVmGenericNetworkAdapter>(m_VmConfig.getVmHardwareList()->m_lstNetworkAdapters, _vm_conf.getVmHardwareList()->m_lstNetworkAdapters);
		CopyList<CVmSoundDevice>(m_VmConfig.getVmHardwareList()->m_lstSoundDevices, _vm_conf.getVmHardwareList()->m_lstSoundDevices);
		CopyList<CVmUsbDevice>(m_VmConfig.getVmHardwareList()->m_lstUsbDevices, _vm_conf.getVmHardwareList()->m_lstUsbDevices);
		CopyList<CVmPciVideoAdapter>(m_VmConfig.getVmHardwareList()->m_lstPciVideoAdapters, _vm_conf.getVmHardwareList()->m_lstPciVideoAdapters);
		CopyList<CVmGenericDevice>(m_VmConfig.getVmHardwareList()->m_lstGenericDevices, _vm_conf.getVmHardwareList()->m_lstGenericDevices);
		CopyList<CVmGenericPciDevice>(m_VmConfig.getVmHardwareList()->m_lstGenericPciDevices, _vm_conf.getVmHardwareList()->m_lstGenericPciDevices);
		CopyList<CVmGenericScsiDevice>(m_VmConfig.getVmHardwareList()->m_lstGenericScsiDevices, _vm_conf.getVmHardwareList()->m_lstGenericScsiDevices);
		return fromString( _vm_conf.toString().toUtf8().constData() );
	}
	else
	{
		return fromString( strCfg.toUtf8().constData() );
	}
}

PRL_RESULT PrlHandleVmCfg::GetConfigValidity(PRL_RESULT_PTR pnErrCode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnErrCode = m_VmConfig.getValidRc();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::AddDefaultDevice(PrlHandleSrvConfigPtr pSrvConfig, PRL_DEVICE_TYPE deviceType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleVmDefaultConfig deviceGenerator ( pSrvConfig, PrlHandleVmPtr(this) );

	if ( ! deviceGenerator.AddDefaultDevice( m_VmConfig, deviceType ) )
		return PRL_ERR_OPERATION_FAILED;

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::AddDefaultDeviceEx( PrlHandleSrvConfigPtr pSrvConfig, PRL_DEVICE_TYPE deviceType,
											PRL_HANDLE_PTR phVmDevice)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleVmDefaultConfig deviceGenerator ( pSrvConfig, PrlHandleVmPtr(this) );

	if ( ! deviceGenerator.AddDefaultDevice( m_VmConfig, deviceType, phVmDevice ) )
		return PRL_ERR_OPERATION_FAILED;

	if ( *phVmDevice == PRL_INVALID_HANDLE )
		return PRL_ERR_OUT_OF_MEMORY;

	return PRL_ERR_SUCCESS;
}

void PrlHandleVmCfg::UpdateSecurityFromVmSecurity(const QString &sVmSecurity)
{
	CVmSecurity _vm_security_checker;
	PRL_RESULT nParseResult = _vm_security_checker.fromString(sVmSecurity);
	if (PRL_SUCCEEDED(nParseResult))
	{
		SYNCHRO_INTERNAL_DATA_ACCESS
		m_VmConfig.getVmSecurity()->fromString(sVmSecurity);
	}
	else
		WRITE_TRACE(DBG_FATAL, "Try to update VM security from wrong VM security data: error code %.8X '%s' data: [%s]", nParseResult, PRL_RESULT_TO_STRING(nParseResult), sVmSecurity.toUtf8().constData());
}

void PrlHandleVmCfg::UpdateSecurityFromVmInfo(const QString &sVmInfo)
{
	CVmEvent _vm_info_checker;
	PRL_RESULT nParseResult = _vm_info_checker.fromString(sVmInfo);
	if (PRL_SUCCEEDED(nParseResult))
	{
		CVmEventParameter* pParam = _vm_info_checker.getEventParameter( EVT_PARAM_VMINFO_VM_SECURITY );
		if (pParam)
			UpdateSecurityFromVmSecurity(pParam->getParamValue());
		else
			WRITE_TRACE(DBG_FATAL, "VM info doesn't contain necessary security data: [%s]", sVmInfo.toUtf8().constData());
	}
	else
		WRITE_TRACE(DBG_FATAL, "Try to update VM security from wrong VM info data: error code %.8X '%s' data: [%s]", nParseResult, PRL_RESULT_TO_STRING(nParseResult), sVmInfo.toUtf8().constData());
}

QString PrlHandleVmCfg::GetUuid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return (m_VmConfig.getVmIdentification()->getVmUuid());
}

PRL_UINT32 PrlHandleVmCfg::GetCtId(PRL_STR sBuf, PRL_UINT32_PTR pnBufLength)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return CopyStringValue(m_VmConfig.getVmIdentification()->getCtId(), sBuf, pnBufLength);
}

/************************************VM devices operations**********************************************************/

PRL_RESULT PrlHandleVmCfg::GetDevicesCount(PRL_UINT32_PTR vmdev_handles_count)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmHardware *pHardware = m_VmConfig.getVmHardwareList();
	if (!pHardware)
		return (PRL_ERR_UNINITIALIZED);
	*vmdev_handles_count = GetVmDevicesCount(pHardware);
	return (PRL_ERR_SUCCESS);
}

#define ADD_VIRTUAL_DEVICES(dev_list, handle_vm_dev, vm_class) \
	foreach(vm_class* pVmDev, m_VmConfig.getVmHardwareList()->dev_list) \
	{ \
		handle_vm_dev* pDevHandle = new handle_vm_dev(PrlHandleVmPtr(this), pVmDev); \
		if ( ! pDevHandle ) \
			return PRL_ERR_OUT_OF_MEMORY; \
		list += PrlHandleBasePtr(pDevHandle); \
		pDevHandle->Release(); \
	}


PRL_RESULT PrlHandleVmCfg::GetAllDevices(PRL_HANDLE_PTR phDevsList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	QList<PrlHandleBasePtr> list;

	ADD_VIRTUAL_DEVICES(m_lstFloppyDisks, PrlHandleVmDeviceFloppy, CVmFloppyDisk);
	ADD_VIRTUAL_DEVICES(m_lstHardDisks, PrlHandleVmDeviceHardDrive, CVmHardDisk);
	ADD_VIRTUAL_DEVICES(m_lstOpticalDisks, PrlHandleVmDeviceOpticalDisk, CVmOpticalDisk);
	ADD_VIRTUAL_DEVICES(m_lstSerialPorts, PrlHandleVmDeviceSerialPort, CVmSerialPort);
	ADD_VIRTUAL_DEVICES(m_lstParallelPorts, PrlHandleVmDeviceParallelPort, CVmParallelPort);
	ADD_VIRTUAL_DEVICES(m_lstUsbDevices, PrlHandleVmDeviceUsbPort, CVmUsbDevice);
	ADD_VIRTUAL_DEVICES(m_lstNetworkAdapters, PrlHandleVmDeviceNetAdapter, CVmGenericNetworkAdapter);
	ADD_VIRTUAL_DEVICES(m_lstSoundDevices, PrlHandleVmDeviceSound, CVmSoundDevice);
	ADD_VIRTUAL_DEVICES(m_lstGenericPciDevices, PrlHandleVmDeviceGenericPci, CVmGenericPciDevice);
	ADD_VIRTUAL_DEVICES(m_lstGenericScsiDevices, PrlHandleVmDeviceGenericScsi, CVmGenericScsiDevice);
	ADD_VIRTUAL_DEVICES(m_lstPciVideoAdapters, PrlHandleVmDeviceDisplay, CVmPciVideoAdapter);

	PrlHandleHandlesList* pList = new PrlHandleHandlesList;
	if ( ! pList )
		return PRL_ERR_OUT_OF_MEMORY;

	pList->SetHandlesList(list);

	*phDevsList = pList->GetHandle();

	return PRL_ERR_SUCCESS;
}

#define PROCESS_VIRTUAL_DEVICES(vm_devices_list, vm_dev_handle_typename, xml_model_dev_typename)\
	foreach(xml_model_dev_typename *_device, vm_devices_list)\
	{\
		if (!nExpectedCount)\
			return (PRL_ERR_BUFFER_OVERRUN);\
		PrlHandleBase *pVmDevHandle = new vm_dev_handle_typename(PrlHandleVmPtr(this), _device);\
		if (!pVmDevHandle)\
			return (PRL_ERR_OUT_OF_MEMORY);\
		vmdev_handles[*vmdev_handles_count] = pVmDevHandle->GetHandle();\
		(*vmdev_handles_count)++;\
		nExpectedCount--;\
	}

PRL_RESULT PrlHandleVmCfg::GetDevicesList(PRL_HANDLE_PTR vmdev_handles, PRL_UINT32_PTR vmdev_handles_count)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmHardware *pHardware = m_VmConfig.getVmHardwareList();
	if (!pHardware)
		return (PRL_ERR_UNINITIALIZED);
	PRL_UINT32 nExpectedCount = *vmdev_handles_count;
	if (!nExpectedCount)
		return (PRL_ERR_INVALID_ARG);
	*vmdev_handles_count = 0;
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstFloppyDisks, PrlHandleVmDeviceFloppy, CVmFloppyDisk)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstHardDisks, PrlHandleVmDeviceHardDrive, CVmHardDisk)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstOpticalDisks, PrlHandleVmDeviceOpticalDisk, CVmOpticalDisk)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstSerialPorts, PrlHandleVmDeviceSerialPort, CVmSerialPort)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstParallelPorts, PrlHandleVmDeviceParallelPort, CVmParallelPort)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstUsbDevices, PrlHandleVmDeviceUsbPort, CVmUsbDevice)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstNetworkAdapters, PrlHandleVmDeviceNetAdapter, CVmGenericNetworkAdapter)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstSoundDevices, PrlHandleVmDeviceSound, CVmSoundDevice)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstGenericPciDevices, PrlHandleVmDeviceGenericPci, CVmGenericPciDevice)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstGenericScsiDevices, PrlHandleVmDeviceGenericScsi, CVmGenericScsiDevice)
	PROCESS_VIRTUAL_DEVICES(pHardware->m_lstPciVideoAdapters, PrlHandleVmDeviceDisplay, CVmPciVideoAdapter)
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDevsCountByType(PRL_DEVICE_TYPE vmDeviceType, PRL_UINT32_PTR pnDevicesCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QList<PRL_VOID_PTR >* pDevList
		= (QList<PRL_VOID_PTR >* )GetVmConfig().getVmHardwareList()->m_aDeviceLists[vmDeviceType];
	*pnDevicesCount = pDevList->size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDevByType( PRL_DEVICE_TYPE device_type, PRL_UINT32 nIndex, PRL_HANDLE_PTR phDevice )
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	*phDevice = NULL;

	PRL_RESULT prlResult = PRL_ERR_INVALID_ARG;
	switch( device_type )
	{
	case PDE_HARD_DISK:
		prlResult = GetHardDisk( nIndex, phDevice );
		break;
	case PDE_FLOPPY_DISK:
		prlResult = GetFloppyDisk( nIndex, phDevice );
		break;
	case PDE_OPTICAL_DISK:
		prlResult = GetOpticalDisk( nIndex, phDevice );
		break;
	case PDE_SERIAL_PORT:
		prlResult = GetSerialPort( nIndex, phDevice );
		break;
	case PDE_PARALLEL_PORT:
		prlResult = GetParallelPort( nIndex, phDevice );
		break;
	case PDE_USB_DEVICE:
		prlResult = GetUsbDevice( nIndex, phDevice );
		break;
	case PDE_GENERIC_NETWORK_ADAPTER:
		prlResult = GetNetAdapter( nIndex, phDevice );
		break;
	case PDE_SOUND_DEVICE:
		prlResult = GetSoundDev( nIndex, phDevice );
		break;
	case PDE_GENERIC_PCI_DEVICE:
		prlResult = GetGenericPciDev( nIndex, phDevice );
		break;
	case PDE_GENERIC_SCSI_DEVICE:
		prlResult = GetGenericScsiDev( nIndex, phDevice );
		break;
	case PDE_PCI_VIDEO_ADAPTER:
		prlResult = GetDisplayDev( nIndex, phDevice );
		break;
	default:
		return (PRL_ERR_UNSUPPORTED_DEVICE_TYPE);
	}
	return prlResult;
}

#define CREATE_DEVICE(sdk_device_name, xml_model_device_name)\
	if (nIndex >= PRL_UINT32(m_VmConfig.getVmHardwareList()->m_lst##xml_model_device_name##s.size()))\
		return (PRL_ERR_INVALID_ARG);\
	PrlHandleVmDevice##sdk_device_name *pDevice = new PrlHandleVmDevice##sdk_device_name(PrlHandleVmPtr(this), m_VmConfig.getVmHardwareList()->m_lst##xml_model_device_name##s.value(nIndex));\
	if (!pDevice)\
		return (PRL_ERR_OUT_OF_MEMORY);

PRL_RESULT PrlHandleVmCfg::GetFloppyDisksCount(PRL_UINT32_PTR pnFloppyDisksCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnFloppyDisksCount = m_VmConfig.getVmHardwareList()->m_lstFloppyDisks.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetFloppyDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phFloppyDisk)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(Floppy, FloppyDisk)
	*phFloppyDisk = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetHardDisksCount(PRL_UINT32_PTR pnHardDisksCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnHardDisksCount = m_VmConfig.getVmHardwareList()->m_lstHardDisks.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetHardDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phHardDisk)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(HardDrive, HardDisk)
	*phHardDisk = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetOpticalDisksCount(PRL_UINT32_PTR pnOpticalDisksCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnOpticalDisksCount = m_VmConfig.getVmHardwareList()->m_lstOpticalDisks.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetOpticalDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phOpticalDisk)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(OpticalDisk, OpticalDisk)
	*phOpticalDisk = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetParallelPortsCount(PRL_UINT32_PTR pnParallelPortsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnParallelPortsCount = m_VmConfig.getVmHardwareList()->m_lstParallelPorts.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetParallelPort(PRL_UINT32 nIndex, PRL_HANDLE_PTR phParallelPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(ParallelPort, ParallelPort)
	*phParallelPort = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSerialPortsCount(PRL_UINT32_PTR pnSerialPortsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSerialPortsCount = m_VmConfig.getVmHardwareList()->m_lstSerialPorts.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSerialPort(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSerialPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(SerialPort, SerialPort)
	*phSerialPort = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSoundDevsCount(PRL_UINT32_PTR pnSoundDevsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSoundDevsCount = m_VmConfig.getVmHardwareList()->m_lstSoundDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSoundDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSoundDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(Sound, SoundDevice)
	*phSoundDev = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetUsbDevicesCount(PRL_UINT32_PTR pnUsbDevicesCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnUsbDevicesCount = m_VmConfig.getVmHardwareList()->m_lstUsbDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetUsbDevice(PRL_UINT32 nIndex, PRL_HANDLE_PTR phUsbDevice)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(UsbPort, UsbDevice)
	*phUsbDevice = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetNetAdaptersCount(PRL_UINT32_PTR pnNetAdaptersCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnNetAdaptersCount = m_VmConfig.getVmHardwareList()->m_lstNetworkAdapters.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetNetAdapter(PRL_UINT32 nIndex, PRL_HANDLE_PTR phNetAdapter)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(NetAdapter, NetworkAdapter)
	*phNetAdapter = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetGenericPciDevsCount(PRL_UINT32_PTR pnGenericPciDevsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnGenericPciDevsCount = m_VmConfig.getVmHardwareList()->m_lstGenericPciDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetGenericPciDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericPciDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(GenericPci, GenericPciDevice)
	*phGenericPciDev = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetGenericScsiDevsCount(PRL_UINT32_PTR pnGenericScsiDevsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnGenericScsiDevsCount = m_VmConfig.getVmHardwareList()->m_lstGenericScsiDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetGenericScsiDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericScsiDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(GenericScsi, GenericScsiDevice)
	*phGenericScsiDev = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDisplayDevsCount(PRL_UINT32_PTR pnDisplayDevsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnDisplayDevsCount = m_VmConfig.getVmHardwareList()->m_lstPciVideoAdapters.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDisplayDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phDisplayDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(Display, PciVideoAdapter)
	*phDisplayDev = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

/************************************Shared folders operations**********************************************************/

PRL_RESULT PrlHandleVmCfg::CreateShare(PRL_HANDLE_PTR phVmShare)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(phVmShare);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::GetSharesCount(PRL_UINT32_PTR pnSharesCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(pnSharesCount);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::GetShare(PRL_UINT32 nShareIndex, PRL_HANDLE_PTR phVmShare)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(nShareIndex);
	Q_UNUSED(phVmShare);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::IsUserDefinedSharedFoldersEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(pbEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::SetUserDefinedSharedFoldersEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(bEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::IsSmartMountEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(pbEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::SetSmartMountEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(bEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::IsSmartMountRemovableDrivesEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(pbEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::SetSmartMountRemovableDrivesEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(bEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::IsSmartMountDVDsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(pbEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::SetSmartMountDVDsEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(bEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::IsSmartMountNetworkSharesEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(pbEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}

PRL_RESULT PrlHandleVmCfg::SetSmartMountNetworkSharesEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	Q_UNUSED(bEnabled);
	return PRL_ERR_UNIMPLEMENTED;
}
/************************************Smart guard operations********************************************************/

PRL_RESULT PrlHandleVmCfg::IsSmartGuardEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmAutoprotect()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetSmartGuardEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmAutoprotect()->setEnabled( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsSmartGuardNotifyBeforeCreation(PRL_BOOL_PTR pbNotifyBeforeCreation)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbNotifyBeforeCreation = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmAutoprotect()->isNotifyBeforeCreation();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetSmartGuardNotifyBeforeCreation(PRL_BOOL bNotifyBeforeCreation)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmAutoprotect()->setNotifyBeforeCreation( bNotifyBeforeCreation );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSmartGuardInterval(PRL_UINT32_PTR pnInterval)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnInterval = m_VmConfig.getVmSettings()->getVmAutoprotect()->getPeriod();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetSmartGuardInterval(PRL_UINT32 nInterval)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmAutoprotect()->setPeriod( nInterval );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSmartGuardMaxSnapshotsCount(PRL_UINT32_PTR pnMaxSnapshotsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMaxSnapshotsCount = m_VmConfig.getVmSettings()->getVmAutoprotect()->getTotalSnapshots();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetSmartGuardMaxSnapshotsCount(PRL_UINT32 nMaxSnapshotsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmAutoprotect()->setTotalSnapshots( nMaxSnapshotsCount );
	return (PRL_ERR_SUCCESS);
}

/************************************Shared profile operations******************************************************/

PRL_RESULT PrlHandleVmCfg::IsSharedProfileEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetSharedProfileEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->setEnabled( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsUseDesktop(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->isUseDesktop();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUseDesktop(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->setUseDesktop( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsUseDocuments(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->isUseDocuments();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUseDocuments(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->setUseDocuments( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsUsePictures(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->isUsePictures();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUsePictures(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->setUsePictures( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsUseMusic(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->isUseMusic();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUseMusic(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->setUseMusic( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsUseDownloads(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->isUseDownloads();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUseDownloads(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->setUseDownloads( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsUseMovies(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->isUseMovies();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUseMovies(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharedProfile()->setUseMovies( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetOptimizeModifiersMode(PRL_OPTIMIZE_MODIFIERS_MODE_PTR pnMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMode = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getOptimizeModifiers();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetOptimizeModifiersMode(PRL_OPTIMIZE_MODIFIERS_MODE nMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setOptimizeModifiers( nMode );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsShareClipboard(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getClipboardSync()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetShareClipboard(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getClipboardSync()->setEnabled( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsTimeSynchronizationEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getTimeSync()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetTimeSynchronizationEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getTimeSync()->setEnabled( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsTimeSyncSmartModeEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getTimeSync()->isKeepTimeDiff();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetTimeSyncSmartModeEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getTimeSync()->setKeepTimeDiff( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetTimeSyncInterval(PRL_UINT32_PTR pnTimeSyncInterval)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnTimeSyncInterval = m_VmConfig.getVmSettings()->getVmTools()->getTimeSync()->getSyncInterval();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetTimeSyncInterval(PRL_UINT32 nTimeSyncInterval)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getTimeSync()->setSyncInterval( nTimeSyncInterval );
	return (PRL_ERR_SUCCESS);
}

/******************************************Boot devices operations*******************************************************/

PRL_RESULT PrlHandleVmCfg::CreateBootDev(PRL_HANDLE_PTR phVmBootDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleVmBootDev *pBootDev = new PrlHandleVmBootDev(PrlHandleVmPtr(this));
	if (!pBootDev)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phVmBootDev = pBootDev->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetBootDevCount(PRL_UINT32_PTR pnBootDevCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnBootDevCount = m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetBootDev(PRL_UINT32 nBootDevIndex, PRL_HANDLE_PTR phVmBootDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nBootDevIndex >= PRL_UINT32(m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList.size()))
		return (PRL_ERR_INVALID_ARG);
	CVmStartupOptions::CVmBootDevice *pBootDeviceElem = m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList.value(nBootDevIndex);
	PrlHandleVmBootDev *pBootDev = new PrlHandleVmBootDev(PrlHandleVmPtr(this), pBootDeviceElem);
	if (!pBootDev)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phVmBootDev = pBootDev->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsAllowSelectBootDevice(PRL_BOOL_PTR pbAllowed)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbAllowed = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmStartupOptions()->isAllowSelectBootDevice();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetAllowSelectBootDevice(PRL_BOOL bAllowed)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setAllowSelectBootDevice( bAllowed );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsEfiEnabled(PRL_BOOL_PTR pbEfiEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEfiEnabled = PRL_BOOL(m_VmConfig.getVmSettings()->getVmStartupOptions()->getBios()->isEfiEnabled());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetEfiEnabled(PRL_BOOL bEfiEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->getBios()->setEfiEnabled(bEfiEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetExternalBootDevice(PRL_STR sSysName, PRL_UINT32_PTR pnSysNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmStartupOptions()->getExternalDeviceSystemName(),
						sSysName, pnSysNameBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetExternalBootDevice(PRL_CONST_STR sNewSysName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setExternalDeviceSystemName(UTF8_2QSTR(sNewSysName));
	return (PRL_ERR_SUCCESS);
}

/************************************VM configuration operations********************************************************/

PRL_RESULT PrlHandleVmCfg::GetName(PRL_STR sVmNameBuf, PRL_UINT32_PTR pnVmNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getVmName(), sVmNameBuf, pnVmNameBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetName(PRL_CONST_STR sNewVmName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmNameUtf8 = UTF8_2QSTR(sNewVmName);
	m_VmConfig.getVmIdentification()->setVmName(sNewVmNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetHostname(PRL_STR sVmHostnameBuf, PRL_UINT32_PTR pnVmHostnameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getGlobalNetwork()->getHostName(),
						sVmHostnameBuf, pnVmHostnameBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetHostname(PRL_CONST_STR sNewVmHostname)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmHostnameUtf8 = UTF8_2QSTR(sNewVmHostname);
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setHostName(sNewVmHostnameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDefaultGateway(PRL_STR sVmDefaultGatewayBuf, PRL_UINT32_PTR pnVmDefaultGatewayBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getGlobalNetwork()->getDefaultGateway(),
						sVmDefaultGatewayBuf, pnVmDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetDefaultGateway(PRL_CONST_STR sNewVmDefaultGateway)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmDefaultGatewayUtf8 = UTF8_2QSTR(sNewVmDefaultGateway);
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setDefaultGateway(sNewVmDefaultGatewayUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDefaultGatewayIPv6(PRL_STR sVmDefaultGatewayBuf, PRL_UINT32_PTR pnVmDefaultGatewayBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getGlobalNetwork()->getDefaultGatewayIPv6(),
						sVmDefaultGatewayBuf, pnVmDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetDefaultGatewayIPv6(PRL_CONST_STR sNewVmDefaultGateway)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmDefaultGatewayUtf8 = UTF8_2QSTR(sNewVmDefaultGateway);
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setDefaultGatewayIPv6(sNewVmDefaultGatewayUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetUuid(PRL_STR sVmUuidBuf, PRL_UINT32_PTR pnVmUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getVmUuid(), sVmUuidBuf, pnVmUuidBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetLinkedVmUuid(PRL_STR sVmUuidBuf, PRL_UINT32_PTR pnVmUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getLinkedVmUuid(), sVmUuidBuf, pnVmUuidBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetOsType(PRL_UINT32_PTR pnVmOsType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmSettings() || !m_VmConfig.getVmSettings()->getVmCommonOptions())
		return (PRL_ERR_NO_DATA);
	*pnVmOsType = m_VmConfig.getVmSettings()->getVmCommonOptions()->getOsType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetOsVersion(PRL_UINT32_PTR pnVmOsVersion)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmSettings() || !m_VmConfig.getVmSettings()->getVmCommonOptions())
		return (PRL_ERR_NO_DATA);
	*pnVmOsVersion = m_VmConfig.getVmSettings()->getVmCommonOptions()->getOsVersion();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetOsVersion(PRL_UINT32 nVmOsVersion)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (!IS_WINDOWS(nVmOsVersion) &&
			!IS_LINUX(nVmOsVersion) &&
			!IS_FREEBSD(nVmOsVersion) &&
			!IS_OS2(nVmOsVersion) &&
			!IS_MSDOS(nVmOsVersion) &&
			!IS_NETWARE(nVmOsVersion) &&
			!IS_SOLARIS(nVmOsVersion) &&
			!IS_MACOS(nVmOsVersion) &&
			!IS_CHROMEOS(nVmOsVersion) &&
			!IS_ANDROID(nVmOsVersion) &&
			!IS_OTHER(nVmOsVersion))
		return (PRL_ERR_INVALID_ARG);

	m_VmConfig.getVmSettings()->getVmCommonOptions()->setOsType(PVS_GET_GUEST_TYPE(nVmOsVersion));
	m_VmConfig.getVmSettings()->getVmCommonOptions()->setOsVersion(nVmOsVersion);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetRamSize(PRL_UINT32_PTR pnVmRamSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmHardwareList() || !m_VmConfig.getVmHardwareList()->getMemory())
		return (PRL_ERR_NO_DATA);
	*pnVmRamSize = m_VmConfig.getVmHardwareList()->getMemory()->getRamSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetRamSize(PRL_UINT32 nVmRamSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getMemory()->setRamSize(nVmRamSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsRamHotplugEnabled(PRL_BOOL_PTR pbVmRamHotplugEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmHardwareList() || !m_VmConfig.getVmHardwareList()->getMemory())
		return (PRL_ERR_NO_DATA);
	*pbVmRamHotplugEnabled = m_VmConfig.getVmHardwareList()->getMemory()->isEnableHotplug();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetRamHotplugEnabled(PRL_BOOL bVmRamHotplugEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getMemory()->setEnableHotplug(bVmRamHotplugEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetVideoRamSize(PRL_UINT32_PTR pnVmVideoRamSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmHardwareList() || !m_VmConfig.getVmHardwareList()->getMemory())
		return (PRL_ERR_NO_DATA);
	*pnVmVideoRamSize = m_VmConfig.getVmHardwareList()->getVideo()->getMemorySize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetVideoRamSize(PRL_UINT32 nVmVideoRamSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getVideo()->setMemorySize(nVmVideoRamSize);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetHostMemQuotaMin(PRL_UINT32_PTR pnHostMemQuotaMin)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmHardwareList() || !m_VmConfig.getVmHardwareList()->getMemory())
		return (PRL_ERR_NO_DATA);
	*pnHostMemQuotaMin = m_VmConfig.getVmHardwareList()->getMemory()->getHostMemQuotaMin();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetHostMemQuotaMin(PRL_UINT32 nHostMemQuotaMin)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getMemory()->setHostMemQuotaMin(nHostMemQuotaMin);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetHostMemQuotaMax(PRL_UINT32_PTR pnHostMemQuotaMax)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmHardwareList() || !m_VmConfig.getVmHardwareList()->getMemory())
		return (PRL_ERR_NO_DATA);
	*pnHostMemQuotaMax = m_VmConfig.getVmHardwareList()->getMemory()->getHostMemQuotaMax();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetHostMemQuotaMax(PRL_UINT32 nHostMemQuotaMax)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getMemory()->setHostMemQuotaMax(nHostMemQuotaMax);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetHostMemQuotaPriority(PRL_UINT32_PTR pnHostMemQuotaPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmHardwareList() || !m_VmConfig.getVmHardwareList()->getMemory())
		return (PRL_ERR_NO_DATA);
	*pnHostMemQuotaPriority =
		m_VmConfig.getVmHardwareList()->getMemory()->getHostMemQuotaPriority();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetHostMemQuotaPriority(PRL_UINT32 nHostMemQuotaPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getMemory()->setHostMemQuotaPriority(nHostMemQuotaPriority);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsHostMemAutoQuota(PRL_BOOL_PTR pbHostMemAutoQuota)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_VmConfig.getVmHardwareList() || !m_VmConfig.getVmHardwareList()->getMemory())
		return (PRL_ERR_NO_DATA);
	*pbHostMemAutoQuota = m_VmConfig.getVmHardwareList()->getMemory()->isAutoQuota();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetHostMemAutoQuota(PRL_BOOL bHostMemAutoQuota)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getMemory()->setAutoQuota(bHostMemAutoQuota);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetMaxBalloonSize(PRL_UINT32_PTR pnMaxBalloonSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMaxBalloonSize =
		m_VmConfig.getVmHardwareList()->getMemory()->getMaxBalloonSize();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetMaxBalloonSize(PRL_UINT32 nMaxBalloonSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getMemory()->setMaxBalloonSize(nMaxBalloonSize);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetMemGuaranteeSize(PRL_MEMGUARANTEE_DATA_PTR pMemGuaranteeSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	pMemGuaranteeSize->type =
		m_VmConfig.getVmHardwareList()->getMemory()->getMemGuaranteeType();
	pMemGuaranteeSize->value =
		m_VmConfig.getVmHardwareList()->getMemory()->getMemGuarantee();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetMemGuaranteeSize(PRL_CONST_MEMGUARANTEE_DATA_PTR pMemGuaranteeSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	switch(pMemGuaranteeSize->type)
	{
	case PRL_MEMGUARANTEE_AUTO:
		m_VmConfig.getVmHardwareList()->getMemory()->setMemGuarantee(0);
		break;
	case PRL_MEMGUARANTEE_PERCENTS:
		m_VmConfig.getVmHardwareList()->getMemory()->setMemGuarantee(pMemGuaranteeSize->value);
		break;
	default:
		return PRL_ERR_INVALID_MEMORY_GUARANTEE;
	}

	m_VmConfig.getVmHardwareList()->getMemory()->setMemGuaranteeType(pMemGuaranteeSize->type);

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetCpuCount(PRL_UINT32_PTR pnVmCpuCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmCpuCount = m_VmConfig.getVmHardwareList()->getCpu()->getNumber();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuCount(PRL_UINT32 nVmCpuCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setNumber(nVmCpuCount);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCpuMode(PRL_CPU_MODE_PTR pnVmCpuMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmCpuMode = m_VmConfig.getVmHardwareList()->getCpu()->getMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuMode(PRL_CPU_MODE nVmCpuMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setMode(nVmCpuMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCpuAccelLevel(PRL_VM_ACCELERATION_LEVEL_PTR pnVmCpuAccelLevel)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmCpuAccelLevel = m_VmConfig.getVmHardwareList()->getCpu()->getAccelerationLevel();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuAccelLevel(PRL_VM_ACCELERATION_LEVEL nVmCpuAccelLevel)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setAccelerationLevel(nVmCpuAccelLevel);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsCpuVtxEnabled(PRL_BOOL_PTR pbVmCpuVtxEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmCpuVtxEnabled = PRL_BOOL(m_VmConfig.getVmHardwareList()->getCpu()->getEnableVTxSupport());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuVtxEnabled(PRL_BOOL bVmCpuVtxEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setEnableVTxSupport(PVE::VTxSupportFlag(bVmCpuVtxEnabled));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsCpuHotplugEnabled(PRL_BOOL_PTR pbVmCpuHotplugEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmCpuHotplugEnabled = PRL_BOOL(m_VmConfig.getVmHardwareList()->getCpu()->isEnableHotplug());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuHotplugEnabled(PRL_BOOL bVmCpuHotplugEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setEnableHotplug((bool)bVmCpuHotplugEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::Is3DAccelerationEnabled(PRL_BOOL_PTR pbVm3DAccelerationEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVm3DAccelerationEnabled = PRL_BOOL(m_VmConfig.getVmHardwareList()->getVideo()->getEnable3DAcceleration() != P3D_DISABLED);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::Set3DAccelerationEnabled(PRL_BOOL bVm3DAccelerationEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getVideo()->setEnable3DAcceleration(bVm3DAccelerationEnabled ? P3D_ENABLED_HIGHEST : P3D_DISABLED);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::Get3DAccelerationMode(PRL_VIDEO_3D_ACCELERATION_PTR pn3DAccelerationMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pn3DAccelerationMode = m_VmConfig.getVmHardwareList()->getVideo()->getEnable3DAcceleration();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::Set3DAccelerationMode(PRL_VIDEO_3D_ACCELERATION n3DAccelerationMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getVideo()->setEnable3DAcceleration(n3DAccelerationMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsVerticalSynchronizationEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_VmConfig.getVmHardwareList()->getVideo()->isEnableVSync();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetVerticalSynchronizationEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getVideo()->setEnableVSync(bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsHighResolutionEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_VmConfig.getVmHardwareList()->getVideo()->isEnableHiResDrawing();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetHighResolutionEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getVideo()->setEnableHiResDrawing(bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsAdaptiveHypervisorEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isEnableAdaptiveHypervisor();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetAdaptiveHypervisorEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setEnableAdaptiveHypervisor(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsSwitchOffWindowsLogoEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isDisableWin7Logo();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetSwitchOffWindowsLogoEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setDisableWin7Logo(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsLongerBatteryLifeEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = PRL_BOOL( ! m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getOptimizePowerConsumptionMode() );
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetLongerBatteryLifeEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setOptimizePowerConsumptionMode(
		bEnable ? PVE::OptimizeBatteryLife : PVE::OptimizePerformance);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsBatteryStatusEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isShowBatteryStatus();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetBatteryStatusEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setShowBatteryStatus(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsNestedVirtualizationEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmHardwareList()->getCpu()->isVirtualizedHV();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetNestedVirtualizationEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setVirtualizedHV(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsPMUVirtualizationEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmHardwareList()->getCpu()->isVirtualizePMU();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetPMUVirtualizationEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setVirtualizePMU(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsLockGuestOnSuspendEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmSettings()->getVmTools()->isLockGuestOnSuspend();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetLockGuestOnSuspendEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->setLockGuestOnSuspend(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsIsolatedVmEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmSettings()->getVmTools()->isIsolatedVm();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetIsolatedVmEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->setIsolatedVm(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetCpuUnits(PRL_UINT32 nVmCpuUnits)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setCpuUnits(nVmCpuUnits);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCpuUnits(PRL_UINT32_PTR pnVmCpuUnits)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmCpuUnits = m_VmConfig.getVmHardwareList()->getCpu()->getCpuUnits();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuLimit(PRL_UINT32 nVmCpuLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setCpuLimitPercents(nVmCpuLimit);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCpuLimit(PRL_UINT32_PTR pnVmCpuLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmCpuLimit = m_VmConfig.getVmHardwareList()->getCpu()->getCpuLimit();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuLimitEx(PRL_CONST_CPULIMIT_DATA_PTR pVmCpuLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->setCpuLimitData(pVmCpuLimit);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCpuLimitEx(PRL_CPULIMIT_DATA_PTR pVmCpuLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getCpu()->getCpuLimitData(pVmCpuLimit);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetGuestCpuLimitType(PRL_UINT32_PTR npVmGuestCpuLimitType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*npVmGuestCpuLimitType = m_VmConfig.getVmHardwareList()->getCpu()->getGuestLimitType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCpuMask(PRL_CONST_STR sCpuMask)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sMask = sCpuMask == NULL ? "" : sCpuMask;
	CVmCpu *pVmCpu = m_VmConfig.getVmHardwareList()->getCpu();
	if (Parallels::parseCpuMask(QString(sMask)))
		return PRL_ERR_INVALID_ARG;
	pVmCpu->setCpuMask(sMask);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCpuMask(PRL_STR sMaskBuf, PRL_UINT32_PTR pnMaskBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmHardwareList()->getCpu()->getCpuMask(),
			sMaskBuf, pnMaskBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetNodeMask(PRL_CONST_STR sNodeMask)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	QString sMask = sNodeMask;

	if (Parallels::parseNodeMask(QString(sMask)))
		return PRL_ERR_INVALID_ARG;

	m_VmConfig.getVmHardwareList()->getCpu()->setNodeMask(sMask);

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetNodeMask(PRL_STR sMaskBuf, PRL_UINT32_PTR pnMaskBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	return CopyStringValue(m_VmConfig.getVmHardwareList()->getCpu()->getNodeMask(),
			sMaskBuf, pnMaskBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetIoPriority(PRL_UINT32 nVmIoPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setIoPriority(nVmIoPriority);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetIoPriority(PRL_UINT32_PTR pnVmIoPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmIoPriority = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getIoPriority();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetIoLimit(PRL_CONST_IOLIMIT_DATA_PTR pVmIoLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmIoLimit *pIoLimit = new CVmIoLimit(pVmIoLimit->type, pVmIoLimit->value);
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setIoLimit(pIoLimit);
	foreach (CVmHardDisk *d, m_VmConfig.getVmHardwareList()->m_lstHardDisks)
		d->setIoLimit(new CVmIoLimit(pVmIoLimit->type, pVmIoLimit->value));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetIoLimit(PRL_IOLIMIT_DATA_PTR pVmIoLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CVmIoLimit *pIoLimit = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getIoLimit();
	if (pIoLimit) {
		pVmIoLimit->type = pIoLimit->getIoLimitType();
		pVmIoLimit->value = pIoLimit->getIoLimitValue();
	} else {
		pVmIoLimit->type = PRL_IOLIMIT_BS;
		pVmIoLimit->value = 0;
	}
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetIopsLimit(PRL_UINT32 nVmIopsLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setIopsLimit(nVmIopsLimit);
	foreach (CVmHardDisk *d, m_VmConfig.getVmHardwareList()->m_lstHardDisks)
		d->setIopsLimit(nVmIopsLimit);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetIopsLimit(PRL_UINT32_PTR pnVmIopsLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmIopsLimit = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getIopsLimit();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsTemplate(PRL_BOOL_PTR pbVmIsTemplate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmIsTemplate = PRL_BOOL(m_VmConfig.getVmSettings()->getVmCommonOptions()->isTemplate());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetTemplateSign(PRL_BOOL bVmIsTemplate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmCommonOptions()->setTemplate(bool(bVmIsTemplate));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetServerUuid(PRL_STR sServerUuidBuf, PRL_UINT32_PTR pnServerUuidBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getServerUuid(), sServerUuidBuf, pnServerUuidBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetServerHost(PRL_STR sServerHostBuf, PRL_UINT32_PTR pnServerHostBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getServerHost(), sServerHostBuf, pnServerHostBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetHomePath(PRL_STR sHomePathBuf, PRL_UINT32_PTR pnHomePathBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getHomePath(), sHomePathBuf, pnHomePathBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetLocation(PRL_VM_LOCATION_PTR pnVmLocation)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmLocation = m_VmConfig.getVmIdentification()->getVmFilesLocation();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetIcon(PRL_STR sVmIconBuf, PRL_UINT32_PTR pnVmIconBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmCommonOptions()->getIcon(), sVmIconBuf, pnVmIconBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetIcon(PRL_CONST_STR sNewVmIcon)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmIconUtf8 = UTF8_2QSTR(sNewVmIcon);
	m_VmConfig.getVmSettings()->getVmCommonOptions()->setIcon(sNewVmIconUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDescription(PRL_STR sVmDescriptionBuf, PRL_UINT32_PTR pnVmDescriptionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmCommonOptions()->getVmDescription(), sVmDescriptionBuf,
													pnVmDescriptionBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetDescription(PRL_CONST_STR sNewVmDescription)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmDescriptionUtf8 = UTF8_2QSTR(sNewVmDescription);
	m_VmConfig.getVmSettings()->getVmCommonOptions()->setVmDescription(sNewVmDescriptionUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCustomProperty(PRL_STR sVmCustomPropertyBuf, PRL_UINT32_PTR pnVmCustomPropertyBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmCommonOptions()->getCustomProperty(), sVmCustomPropertyBuf,
		pnVmCustomPropertyBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetCustomProperty(PRL_CONST_STR sNewVmCustomProperty)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmCommonOptions()->setCustomProperty(
		UTF8_2QSTR(sNewVmCustomProperty));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetAutoStart(PRL_VM_AUTOSTART_OPTION_PTR pnVmAutoStart)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmAutoStart = m_VmConfig.getVmSettings()->getVmStartupOptions()->getAutoStart();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetAutoStart(PRL_VM_AUTOSTART_OPTION nVmAutoStart)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setAutoStart(nVmAutoStart);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetAutoStartDelay(PRL_UINT32_PTR pnVmAutoStartDelay)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmAutoStartDelay = m_VmConfig.getVmSettings()->getVmStartupOptions()->getAutoStartDelay();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetAutoStartDelay(PRL_UINT32 nVmAutoStartDelay)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setAutoStartDelay(nVmAutoStartDelay);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetStartLoginMode(PRL_VM_START_LOGIN_MODE_PTR pnVmStartLoginMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmStartLoginMode = m_VmConfig.getVmSettings()->getVmStartupOptions()->getVmStartLoginMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetStartLoginMode(PRL_VM_START_LOGIN_MODE nVmStartLoginMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setVmStartLoginMode(nVmStartLoginMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetStartUserLogin(PRL_STR sStartUserLogin, PRL_UINT32_PTR pnStartUserLoginBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmStartupOptions()->getVmStartAsUser(), sStartUserLogin,
													pnStartUserLoginBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetStartUserCreds(PRL_CONST_STR sStartUserLogin, PRL_CONST_STR sPassword)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setVmStartAsUser(sStartUserLogin);
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setVmStartAsPassword(sPassword);
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setChangedPassword(true);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetAutoStop(PRL_VM_AUTOSTOP_OPTION_PTR pnVmAutoStop)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmAutoStop = m_VmConfig.getVmSettings()->getShutdown()->getAutoStop();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetAutoStop(PRL_VM_AUTOSTOP_OPTION nVmAutoStop)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getShutdown()->setAutoStop(nVmAutoStop);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetActionOnWindowClose(PRL_VM_ACTION_ON_WINDOW_CLOSE_PTR pnActionOnWindowClose)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnActionOnWindowClose = m_VmConfig.getVmSettings()->getShutdown()->getOnVmWindowClose();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetActionOnWindowClose(PRL_VM_ACTION_ON_WINDOW_CLOSE nActionOnWindowClose)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getShutdown()->setOnVmWindowClose(nActionOnWindowClose);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetActionOnStopMode(PRL_VM_ACTION_ON_STOP_PTR pnMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMode = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getActionOnStop();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetActionOnStopMode(PRL_VM_ACTION_ON_STOP nMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setActionOnStop( nMode );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetWindowMode(PRL_VM_WINDOW_MODE_PTR pnVmWindowMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmWindowMode = m_VmConfig.getVmSettings()->getVmStartupOptions()->getWindowMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetWindowMode(PRL_VM_WINDOW_MODE nVmWindowMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setWindowMode(nVmWindowMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsStartInDetachedWindowEnabled(PRL_BOOL_PTR pbEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnable = m_VmConfig.getVmSettings()->getVmStartupOptions()->isStartInDetachedWindow();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetStartInDetachedWindowEnabled(PRL_BOOL bEnable)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setStartInDetachedWindow(bEnable);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsLockInFullScreenMode(PRL_BOOL_PTR pbValue)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbValue = m_VmConfig.getVmSettings()->getVmStartupOptions()->isLockInFullScreenMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetLockInFullScreenMode(PRL_BOOL bValue)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmStartupOptions()->setLockInFullScreenMode(bValue);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetLastModifiedDate(PRL_STR sVmLastModifiedDateBuf, PRL_UINT32_PTR pnVmLastModifiedDateBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getLastModifDate().toString(XML_DATETIME_FORMAT),
													sVmLastModifiedDateBuf, pnVmLastModifiedDateBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetLastModifierName(PRL_STR sVmLastModifierNameBuf, PRL_UINT32_PTR pnVmLastModifierNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getModifierName(), sVmLastModifierNameBuf,
													pnVmLastModifierNameBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetUptimeStartDate(PRL_STR sVmUptimeStartDateBuf, PRL_UINT32_PTR pnVmUptimeStartDateBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmIdentification()->getVmUptimeStartDateTime().toString(XML_DATETIME_FORMAT),
													sVmUptimeStartDateBuf, pnVmUptimeStartDateBufLength);
}

PRL_RESULT PrlHandleVmCfg::GetUptime(PRL_UINT64_PTR pnVmUptime)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmUptime = m_VmConfig.getVmIdentification()->getVmUptimeInSeconds();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetAccessRights(PRL_HANDLE_PTR phVmAcl)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleAccessRights *pVmAcl = new PrlHandleAccessRights(*m_VmConfig.getVmSecurity());
	*phVmAcl = pVmAcl->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsGuestSharingEnabled(PRL_BOOL_PTR pbVmGuestSharingEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmGuestSharingEnabled = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getGuestSharing()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetGuestSharingEnabled(PRL_BOOL bVmGuestSharingEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getGuestSharing()->setEnabled(bVmGuestSharingEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsGuestSharingAutoMount(PRL_BOOL_PTR pbVmGuestSharingAutoMount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmGuestSharingAutoMount = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getGuestSharing()->isAutoMount();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetGuestSharingAutoMount(PRL_BOOL bVmGuestSharingAutoMount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getGuestSharing()->setAutoMount(bVmGuestSharingAutoMount);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsGuestSharingEnableSpotlight(PRL_BOOL_PTR pbVmGuestSharingEnableSpotlight)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmGuestSharingEnableSpotlight = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getGuestSharing()->isEnableSpotlight();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetGuestSharingEnableSpotlight(PRL_BOOL bVmGuestSharingEnableSpotlight)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getGuestSharing()->setEnableSpotlight(bVmGuestSharingEnableSpotlight);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsHostSharingEnabled(PRL_BOOL_PTR pbVmHostSharingEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmHostSharingEnabled = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getHostSharing()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetHostSharingEnabled(PRL_BOOL bVmHostSharingEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getHostSharing()->setEnabled(bVmHostSharingEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsVirtualLinksEnabled(PRL_BOOL_PTR pbVirtualLinksEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVirtualLinksEnabled = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getHostSharing()->isVirtualLinks();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetVirtualLinksEnabled(PRL_BOOL bVirtualLinksEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->getHostSharing()->setVirtualLinks(bVirtualLinksEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsShareAllHostDisks(PRL_BOOL_PTR pbShareAllHostDisks)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbShareAllHostDisks = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->
		getHostSharing()->isShareAllMacDisks();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetShareAllHostDisks(PRL_BOOL bShareAllHostDisks)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->
		getHostSharing()->setShareAllMacDisks(bShareAllHostDisks);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsShareUserHomeDir(PRL_BOOL_PTR pbShareUserHomeDir)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbShareUserHomeDir = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->
		getHostSharing()->isShareUserHomeDir();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetShareUserHomeDir(PRL_BOOL bShareUserHomeDir)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->
		getHostSharing()->setShareUserHomeDir(bShareUserHomeDir);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsMapSharedFoldersOnLetters(PRL_BOOL_PTR pbMapSharedFoldersOnLetters)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbMapSharedFoldersOnLetters = m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->
		getHostSharing()->isMapSharedFoldersOnLetters();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetMapSharedFoldersOnLetters(PRL_BOOL bMapSharedFoldersOnLetters)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getVmSharing()->
		getHostSharing()->setMapSharedFoldersOnLetters(bMapSharedFoldersOnLetters);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetRemoteDisplayMode(PRL_VM_REMOTE_DISPLAY_MODE_PTR pnVmRemoteDisplayMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmRemoteDisplayMode = m_VmConfig.getVmSettings()->getVmRemoteDisplay()->getMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetRemoteDisplayMode(PRL_VM_REMOTE_DISPLAY_MODE nVmRemoteDisplayMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRemoteDisplay()->setMode(nVmRemoteDisplayMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetRemoteDisplayPassword(PRL_STR sVmRemoteDisplayPasswordBuf, PRL_UINT32_PTR pnVmRemoteDisplayPasswordBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmRemoteDisplay()->getPassword(), sVmRemoteDisplayPasswordBuf,
		pnVmRemoteDisplayPasswordBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetRemoteDisplayPassword(PRL_CONST_STR sNewVmRemoteDisplayPassword)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmRemoteDisplayPasswordUtf8 = UTF8_2QSTR(sNewVmRemoteDisplayPassword);
	m_VmConfig.getVmSettings()->getVmRemoteDisplay()->setPassword(sNewVmRemoteDisplayPasswordUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetRemoteDisplayHostName(PRL_STR sVmRemoteDisplayHostNameBuf, PRL_UINT32_PTR pnVmRemoteDisplayHostNameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmRemoteDisplay()->getHostName(), sVmRemoteDisplayHostNameBuf,
		pnVmRemoteDisplayHostNameBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetRemoteDisplayHostName(PRL_CONST_STR sNewVmRemoteDisplayHostName)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmRemoteDisplayHostNameUtf8 = UTF8_2QSTR(sNewVmRemoteDisplayHostName);
	m_VmConfig.getVmSettings()->getVmRemoteDisplay()->setHostName(sNewVmRemoteDisplayHostNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetRemoteDisplayPortNumber(PRL_UINT32_PTR pnVmRemoteDisplayPortNumber)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmRemoteDisplayPortNumber = m_VmConfig.getVmSettings()->getVmRemoteDisplay()->getPortNumber();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetRemoteDisplayPortNumber(PRL_UINT32 nVmRemoteDisplayPortNumber)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRemoteDisplay()->setPortNumber(nVmRemoteDisplayPortNumber);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetRemoteDisplayWebSocketPortNumber(PRL_UINT32_PTR pnVmRemoteDisplayWebSocketPortNumber)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmRemoteDisplayWebSocketPortNumber = m_VmConfig.getVmSettings()->getVmRemoteDisplay()->getWebSocketPortNumber();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsRemoteDisplayEncryptionEnabled(PRL_BOOL_PTR pbVmRemoteDisplayEncryptionEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmRemoteDisplayEncryptionEnabled = m_VmConfig.getVmSettings()->getVmRemoteDisplay()->isEncrypted();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsScrResEnabled(PRL_BOOL_PTR pbVmScrResEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmScrResEnabled = m_VmConfig.getVmHardwareList()->getVideo()->getVmScreenResolutions()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetScrResEnabled(PRL_BOOL bVmScrResEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmHardwareList()->getVideo()->getVmScreenResolutions()->setEnabled(bVmScrResEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsDiskCacheWriteBack(PRL_BOOL_PTR pbVmDiskCacheWriteBack)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmDiskCacheWriteBack = PRL_BOOL(m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getDiskCachePolicy());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetDiskCacheWriteBack(PRL_BOOL bVmDiskCacheWriteBack)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setDiskCachePolicy(PVE::DiskCacheWritebackOption(bVmDiskCacheWriteBack));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsOsResInFullScrMode(PRL_BOOL_PTR pbVmOsResInFullScrMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmOsResInFullScrMode = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isOsResolutionInFullScreen();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetOsResInFullScrMode(PRL_BOOL bVmOsResInFullScrMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setOsResolutionInFullScreen(bVmOsResInFullScrMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsCloseAppOnShutdown(PRL_BOOL_PTR pbVmCloseAppOnShutdown)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVmCloseAppOnShutdown = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isCloseAppOnShutdown();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetCloseAppOnShutdown(PRL_BOOL bVmCloseAppOnShutdown)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setCloseAppOnShutdown(bVmCloseAppOnShutdown);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSystemFlags(PRL_STR sVmSystemFlagsBuf, PRL_UINT32_PTR pnVmSystemFlagsBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getSystemFlags(),
													sVmSystemFlagsBuf, pnVmSystemFlagsBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetSystemFlags(PRL_CONST_STR sNewVmSystemFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	QString sNewVmSystemFlagsUtf8 = UTF8_2QSTR(sNewVmSystemFlags);
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setSystemFlags(sNewVmSystemFlagsUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsDisableAPIC(PRL_BOOL_PTR pbDisableAPIC)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbDisableAPIC = PRL_BOOL(m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isDisableAPIC());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetDisableAPICSign(PRL_BOOL bDisableAPIC)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setDisableAPIC(bool(bDisableAPIC));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsDisableSpeaker(PRL_BOOL_PTR pbDisableSpeaker)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbDisableSpeaker = PRL_BOOL(m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isDisableSpeaker());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetDisableSpeakerSign(PRL_BOOL bDisableSpeaker)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setDisableSpeaker(bool(bDisableSpeaker));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetUndoDisksMode(PRL_UNDO_DISKS_MODE_PTR pnUndoDisksMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnUndoDisksMode = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getUndoDisksMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUndoDisksMode(PRL_UNDO_DISKS_MODE nUndoDisksMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setUndoDisksMode(nUndoDisksMode);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetForegroundPriority(PRL_VM_PRIORITY_PTR pnVmForegroundPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmForegroundPriority = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getForegroundPriority();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetForegroundPriority(PRL_VM_PRIORITY nVmForegroundPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setForegroundPriority(nVmForegroundPriority);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetBackgroundPriority(PRL_VM_PRIORITY_PTR pnVmBackgroundPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVmBackgroundPriority = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getBackgroundPriority();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetBackgroundPriority(PRL_VM_PRIORITY nVmBackgroundPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setBackgroundPriority(nVmBackgroundPriority);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsUseDefaultAnswers(PRL_BOOL_PTR pbUseDefaultAnswers)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbUseDefaultAnswers = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmRuntimeOptions()->isUseDefaultAnswers();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetUseDefaultAnswers(PRL_BOOL bUseDefaultAnswers)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setUseDefaultAnswers( bUseDefaultAnswers );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsList *pSearchDomainsList =
		new PrlHandleStringsList(m_VmConfig.getVmSettings()->getGlobalNetwork()->getSearchDomains());
	if (pSearchDomainsList)
	{
		*phSearchDomainsList = pSearchDomainsList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmCfg::SetSearchDomains(const QStringList &lstSearchDomains)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setSearchDomains(lstSearchDomains);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetDnsServers(PRL_HANDLE_PTR phDnsServersList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsList *pDnsServersList =
		new PrlHandleStringsList(m_VmConfig.getVmSettings()->getGlobalNetwork()->getDnsIPAddresses());
	if (pDnsServersList)
	{
		*phDnsServersList = pDnsServersList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmCfg::SetDnsServers(const QStringList &lstDnsServers)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setDnsIPAddresses(lstDnsServers);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsAutoApplyIpOnly(PRL_BOOL_PTR pbAutoApplyIpOnly)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbAutoApplyIpOnly = m_VmConfig.getVmSettings()->getGlobalNetwork()->isAutoApplyIpOnly();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetAutoApplyIpOnly(PRL_BOOL bAutoApplyIpOnly)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setAutoApplyIpOnly(bAutoApplyIpOnly);
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmCfg::CleanBootDevicesList(CVmDevice *pDevice)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (pDevice->getDeviceType() == PDE_GENERIC_NETWORK_ADAPTER)//Special case for network adapter
	{
		if (m_VmConfig.getVmHardwareList()->m_lstNetworkAdapters.size() == 1)//last network adapter removing
		{
			QList<CVmStartupOptions::CVmBootDevice *> lstBootDevicesForDeletion;
			foreach(CVmStartupOptions::CVmBootDevice *pBootDeviceElem, m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList)
				if (pBootDeviceElem->getType() == PDE_GENERIC_NETWORK_ADAPTER)
					lstBootDevicesForDeletion.append(pBootDeviceElem);

			foreach(CVmStartupOptions::CVmBootDevice *pBootDeviceElem, lstBootDevicesForDeletion)
			{
				PrlControlValidity::MarkAsInvalid(pBootDeviceElem);
				m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList.removeAll(pBootDeviceElem);
				delete pBootDeviceElem;
			}
		}
	}
	else
	{
		QList<CVmStartupOptions::CVmBootDevice *> lstBootDevicesForDeletion;
		foreach(CVmStartupOptions::CVmBootDevice *pBootDeviceElem, m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList)
			if (pBootDeviceElem->getType() == pDevice->getDeviceType() && pBootDeviceElem->getIndex() == pDevice->getIndex())
				lstBootDevicesForDeletion.append(pBootDeviceElem);

		foreach(CVmStartupOptions::CVmBootDevice *pBootDeviceElem, lstBootDevicesForDeletion)
		{
			PrlControlValidity::MarkAsInvalid(pBootDeviceElem);
			m_VmConfig.getVmSettings()->getVmStartupOptions()->m_lstBootDeviceList.removeAll(pBootDeviceElem);
			delete pBootDeviceElem;
		}
	}
}

/* Offline management helpers set */
PRL_RESULT PrlHandleVmCfg::IsOfflineManagementEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getGlobalNetwork()->isOfflineManagementEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetOfflineManagementEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setOfflineManagementEnabled( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetToolsAutoUpdateEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmTools()->getAutoUpdate()->setEnabled( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsToolsAutoUpdateEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmTools()->getAutoUpdate()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetOfflineServices(PRL_HANDLE_PTR phOfflineServicesList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsList *pOfflineServicesList =
		new PrlHandleStringsList(m_VmConfig.getVmSettings()->getGlobalNetwork()->getOfflineServices());
	if (pOfflineServicesList)
	{
		*phOfflineServicesList = pOfflineServicesList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmCfg::SetOfflineServices(const QStringList &lstOfflineServices)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getGlobalNetwork()->setOfflineServices(lstOfflineServices);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetNetworkRates(PRL_HANDLE hHandleList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PrlHandleHandlesListPtr pHandlesList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hHandleList);
	QList<PrlHandleBasePtr> hList = pHandlesList->GetHandlesList();

	CVmNetworkRates *pNetworkRates = new CVmNetworkRates;
	// Preserve RateBound
	pNetworkRates->setRateBound(m_VmConfig.getVmSettings()->getGlobalNetwork()->getNetworkRates()->isRateBound());
	foreach(PrlHandleBasePtr ph, hList)
	{
		if (ph->GetType() != PHT_NETWORK_RATE)
		{
			delete pNetworkRates;
			return PRL_ERR_INVALID_ARG;
		}
		PrlHandleNetworkRatePtr pNetworkRate = PRL_OBJECT_BY_HANDLE<PrlHandleNetworkRate>(ph->GetHandle());

		pNetworkRates->m_lstNetworkRates += new CVmNetworkRate(pNetworkRate->m_NetworkRate);
	}

	m_VmConfig.getVmSettings()->getGlobalNetwork()->setNetworkRates(pNetworkRates);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetNetworkRates(PRL_HANDLE_PTR phHandleList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleHandlesList *pNetworkRateList = new PrlHandleHandlesList();
	if (!pNetworkRateList)
		return (PRL_ERR_OUT_OF_MEMORY);

	CVmNetworkRates *pNetworkRates = m_VmConfig.getVmSettings()->getGlobalNetwork()->getNetworkRates();
	foreach( CVmNetworkRate *pRate, pNetworkRates->m_lstNetworkRates) {
		PrlHandleBasePtr phRate(new PrlHandleNetworkRate(pRate->toString()));
		pNetworkRateList->AddItem(phRate);
	}
	*phHandleList = pNetworkRateList->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsRateBound(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getGlobalNetwork()->getNetworkRates()->isRateBound();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetRateBound(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getGlobalNetwork()->getNetworkRates()->setRateBound( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetConfirmationsList(PRL_HANDLE_PTR phConfirmationsList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;

	TOpaqueTypeList<PRL_ALLOWED_VM_COMMAND> *pOpaqueList = new TOpaqueTypeList<PRL_ALLOWED_VM_COMMAND>;
	if (!pOpaqueList)
		return (PRL_ERR_OUT_OF_MEMORY);

	QList<PRL_ALLOWED_VM_COMMAND> &lstRef = pOpaqueList->GetContainer();
	lstRef	= m_VmConfig.getVmSecurity()->getLockedOperationsList()->getLockedOperations();

	PrlHandleOpTypeList *pOpaqueListHandle = new PrlHandleOpTypeList(pOpaqueList);
	if (!pOpaqueListHandle)
	{
		delete pOpaqueList;
		return (PRL_ERR_OUT_OF_MEMORY);
	}

	*phConfirmationsList = pOpaqueListHandle->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetConfirmationsList(PrlHandleOpTypeListPtr pOpaqueList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;

	PRL_RESULT nErr;

	PRL_SIZE nTypeSize;
	nErr = pOpaqueList->GetTypeSize(&nTypeSize);
	if( PRL_FAILED(nErr) )
		return nErr;

	PRL_UINT8* p = new PRL_UINT8[nTypeSize];
	SmartPtr< PRL_UINT8 > pBuff( p, SmartPtrPolicy::ArrayStorage );

	PRL_UINT32 nCount;
	nErr = pOpaqueList->GetItemsCount(&nCount);
	if( PRL_FAILED(nErr) )
		return nErr;

	QList<PRL_ALLOWED_VM_COMMAND> lst;
	for( PRL_UINT32 idx=0; idx < nCount; idx++ )
	{
		nErr = pOpaqueList->GetItem(idx, pBuff.getImpl() );
		if( PRL_FAILED(nErr) )
			return nErr;

		PRL_ALLOWED_VM_COMMAND cmd;

		switch (nTypeSize)
		{
#define CASE_BUFF_TO_CMD(type) \
		case sizeof(type): \
			{ \
			type* p = reinterpret_cast< type* >(pBuff.getImpl()); \
			cmd = (PRL_ALLOWED_VM_COMMAND)(*p); \
			break; \
		}
			CASE_BUFF_TO_CMD(PRL_UINT8)
				CASE_BUFF_TO_CMD(PRL_UINT16);
			CASE_BUFF_TO_CMD(PRL_UINT32);
			CASE_BUFF_TO_CMD(PRL_UINT64);
			default: return (PRL_ERR_INVALID_ARG);
#undef CASE_BUFF_TO_CMD
		}

		lst.append( (PRL_ALLOWED_VM_COMMAND)cmd );
	}
	m_VmConfig.getVmSecurity()->getLockedOperationsList()->setLockedOperations( lst );

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::IsAutoCompressEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = (PRL_BOOL )m_VmConfig.getVmSettings()->getVmAutoCompress()->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetAutoCompressEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmAutoCompress()->setEnabled( bEnabled );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetAutoCompressInterval(PRL_UINT32_PTR pnInterval)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnInterval = m_VmConfig.getVmSettings()->getVmAutoCompress()->getPeriod();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetAutoCompressInterval(PRL_UINT32 nInterval)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmAutoCompress()->setPeriod( nInterval );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetFreeDiskSpaceRatio(PRL_DOUBLE_PTR pdFreeDiskSpaceRatio)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pdFreeDiskSpaceRatio = m_VmConfig.getVmSettings()->getVmAutoCompress()->getFreeDiskSpaceRatio();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetFreeDiskSpaceRatio(PRL_DOUBLE dFreeDiskSpaceRatio)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmAutoCompress()->setFreeDiskSpaceRatio( dFreeDiskSpaceRatio );
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetVmInfo(PRL_HANDLE_PTR phVmInfo)
{
	SYNCHRO_INTERNAL_DATA_ACCESS;
	CVmEvent *pVmEvent =
		m_VmConfig.getVmSettings()->getVmRuntimeOptions()
			->getInternalVmInfo()->getParallelsEvent();

	// XXX: #483252
	if (!pVmEvent || pVmEvent->m_lstEventParameters.empty())
		return PRL_ERR_NO_DATA;

	PrlHandleVmInfo *pVmInfo = new PrlHandleVmInfo(pVmEvent);
	if (pVmInfo)
	{
		*phVmInfo = pVmInfo->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmCfg::GetUnattendedInstallLocale(PRL_STR sLocale, PRL_UINT32_PTR pnLocaleBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getUnattendedInstallLocale(),
						sLocale, pnLocaleBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetUnattendedInstallLocale(PRL_CONST_STR sLocale)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setUnattendedInstallLocale(UTF8_2QSTR(sLocale));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetUnattendedInstallEdition(PRL_STR sEdition, PRL_UINT32_PTR pnEditionBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getUnattendedInstallEdition(),
						sEdition, pnEditionBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetUnattendedInstallEdition(PRL_CONST_STR sEdition)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->setUnattendedInstallEdition(UTF8_2QSTR(sEdition));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetVmType(PRL_VM_TYPE nType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.setVmType(nType);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetVmType(PRL_VM_TYPE_PTR pnType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnType = m_VmConfig.getVmType();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetOstemplate(PRL_CONST_STR sOstemplate)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getCtSettings()->setOsTemplate(UTF8_2QSTR(sOstemplate));
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetOstemplate(PRL_STR sOstemplate, PRL_UINT32_PTR pnBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_VmConfig.getCtSettings()->getOsTemplate(), sOstemplate, pnBufLength);
}

PRL_RESULT PrlHandleVmCfg::SetApplyConfigSample(PRL_CONST_STR sConfigSample)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmCommonOptions()->setConfigSampleName(UTF8_2QSTR(sConfigSample));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::SetResourceById(PRL_CT_RESOURCE nResourceId, PRL_UINT64 nBarrier, PRL_UINT64 nLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (nResourceId > PCR_LAST)
		return PRL_ERR_INVALID_ARG;
	CCtResource *pRes = new CCtResource;

	pRes->setResourceId(nResourceId);
	pRes->setBarrier(nBarrier);
	pRes->setLimit(nLimit);
	m_VmConfig.getCtSettings()->addResource(pRes);

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetResourceById(PRL_CT_RESOURCE nResourceId, PRL_UINT64_PTR pnBarrier, PRL_UINT64_PTR pnLimit)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (nResourceId > PCR_LAST)
		return PRL_ERR_INVALID_ARG;

	CCtResource *pRes = m_VmConfig.getCtSettings()->getResource(nResourceId);
	if (!pRes)
		return PRL_ERR_PARAM_NOT_FOUND;

	*pnBarrier = pRes->getBarrier();
	*pnLimit = pRes->getLimit();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetAppTemplateList(PRL_HANDLE_PTR phAppList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (m_VmConfig.getVmType() != PVT_CT)
		return PRL_ERR_INVALID_ARG;

	PrlHandleStringsList *pList = new PrlHandleStringsList(
		m_VmConfig.getCtSettings()->getAppTemplate());
	if (pList)
	{
		*phAppList = pList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}

	return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleVmCfg::SetAppTemplateList(PRL_HANDLE hAppList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	if (m_VmConfig.getVmType() != PVT_CT)
		return PRL_ERR_INVALID_ARG;

	PrlHandleStringsListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleStringsList>(hAppList);
	m_VmConfig.getCtSettings()->setAppTemplate(pList->GetStringsList());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmCfg::GetCapabilitiesMask(PRL_UINT32_PTR pnCapMask)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCapMask = m_VmConfig.getCtSettings()->getCapabilitiesMask();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetCapabilitiesMask(PRL_UINT32 nCapMask)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getCtSettings()->setCapabilitiesMask(nCapMask);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetNetfilterMode(PRL_NETFILTER_MODE_PTR pnMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	*pnMode = m_VmConfig.getCtSettings()->getNetfilterMode();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetNetfilterMode(PRL_NETFILTER_MODE nMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	m_VmConfig.getCtSettings()->setNetfilterMode(nMode);

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetFeaturesMask(PRL_UINT32_PTR pnOn, PRL_UINT32_PTR pnOff)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnOn = m_VmConfig.getCtSettings()->getFeaturesOnMask();
	*pnOff = m_VmConfig.getCtSettings()->getFeaturesOffMask();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetFeaturesMask(PRL_UINT32 nOn, PRL_UINT32 nOff)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getCtSettings()->setFeaturesOnMask(nOn);
	m_VmConfig.getCtSettings()->setFeaturesOffMask(nOff);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetProfile(PRL_VIRTUAL_MACHINE_PROFILE_PTR pnVmProfile)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	*pnVmProfile = m_VmConfig.getVmSettings()->getVmCommonOptions()
							->getProfile()->getType();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::IsCustomProfile(PRL_BOOL_PTR pbCustom)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	*pbCustom = m_VmConfig.getVmSettings()->getVmCommonOptions()
							->getProfile()->isCustom();

	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetProfile(PrlHandleSrvConfigPtr pSrvConfig,
									  PRL_VIRTUAL_MACHINE_PROFILE nVmProfile)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	return CVmProfileHelper::set_vm_profile(nVmProfile, pSrvConfig->GetSrvConfig(), m_VmConfig);
}

PRL_RESULT PrlHandleVmCfg::IsHighAvailabilityEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbEnabled = m_VmConfig.getVmSettings()->getHighAvailability()->isEnabled();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetHighAvailabilityEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getHighAvailability()->setEnabled(bEnabled);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetHighAvailabilityPriority(PRL_UINT32_PTR pnPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnPriority = m_VmConfig.getVmSettings()->getHighAvailability()->getPriority();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetHighAvailabilityPriority(PRL_UINT32 nPriority)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getHighAvailability()->setPriority(nPriority);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetOnCrashAction(PRL_VM_ON_CRASH_ACTION nAction)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getOnCrash()->setMode(nAction);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetOnCrashAction(PRL_VM_ON_CRASH_ACTION_PTR pnAction)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnAction = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getOnCrash()->getMode();
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::SetOnCrashOptions(PRL_UINT32 nFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getOnCrash()->setOptions(nFlags);
	return PRL_ERR_SUCCESS;
}

PRL_RESULT PrlHandleVmCfg::GetOnCrashOptions(PRL_UINT32_PTR pnFlags)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnFlags = m_VmConfig.getVmSettings()->getVmRuntimeOptions()->getOnCrash()->getOptions();
	return PRL_ERR_SUCCESS;
}
