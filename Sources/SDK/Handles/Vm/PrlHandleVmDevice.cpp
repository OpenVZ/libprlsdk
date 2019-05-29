/*
 * PrlHandleVmDevice.cpp
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


#include "PrlHandleVmCfg.h"
#include "PrlHandleVmDevice.h"
#include "PrlHandleServer.h"
#include <prlxmlmodel/VmConfig/CVmMassStorageDevice.h>
#include <prlxmlmodel/VmConfig/CVmPort.h>
#include "PrlControlValidity.h"
#include "PrlHandleVmDeviceFloppy.h"
#include "PrlHandleVmDeviceHardDrive.h"
#include "PrlHandleVmDeviceOpticalDisk.h"
#include "PrlHandleVmDeviceSerialPort.h"
#include "PrlHandleVmDeviceParallelPort.h"
#include "PrlHandleVmDeviceUsbPort.h"
#include "PrlHandleVmDeviceNetAdapter.h"
#include "PrlHandleVmDeviceSound.h"
#include "PrlHandleVmDeviceDisplay.h"
#include "PrlHandleVmDeviceGenericPci.h"
#include "PrlHandleVmDeviceGenericScsi.h"
#include <prlcommon/Std/PrlAssert.h>

#define CHECK_VM(job_type)\
	if (!m_pVm)\
		return (CreateErrorHandle(PRL_ERR_UNINITIALIZED, (PRL_CONST_STR)__FUNCTION__, job_type));

#define CHECK_SERVER(job_type)\
	if (!m_pVm->GetServer())\
		return (CreateErrorHandle(PRL_ERR_UNINITIALIZED, (PRL_CONST_STR)__FUNCTION__, job_type));

#define GET_XML_MODEL_OBJECT\
	CVmDevice *pVmDev = GetVmDeviceElem();\
	if (!pVmDev)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmDevice::PrlHandleVmDevice( PRL_HANDLE_TYPE hType )
: PrlHandleBase(hType), PrlControlValidity(GetHandle()), m_pVm(0)
{}

PrlHandleVmDevice::PrlHandleVmDevice( const PrlHandleVmPtr &pVm, PRL_HANDLE_TYPE hType )
: PrlHandleBase(hType), PrlControlValidity(GetHandle()), m_pVm(pVm)
{}

PrlHandleVmDevice::~PrlHandleVmDevice()
{}

PRL_RESULT PrlHandleVmDevice::GetVm( PRL_HANDLE_PTR vm_handle )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVm)
	{
		*vm_handle = PRL_INVALID_HANDLE;
		return (PRL_ERR_INVALID_ARG);
	}
	*vm_handle = m_pVm->GetHandle();
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmDevice::SetVm( PrlHandleVmPtr pVm )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVm = pVm;
}

PrlHandleJobPtr PrlHandleVmDevice::Connect()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_VM(PJOC_VM_DEV_CONNECT)
	CHECK_SERVER(PJOC_VM_DEV_CONNECT)
	CVmDevice *pVmDev = GetVmDeviceElem();
	if (!pVmDev)
		return (CreateErrorHandle(PRL_ERR_OBJECT_WAS_REMOVED, __FUNCTION__, PJOC_VM_DEV_CONNECT));

	PVE::DeviceConnectedState bOldDevConnectedState = pVmDev->getConnected();
	//Setting right sign of device connected state in sending XML configuration
	pVmDev->setConnected(PVE::DeviceConnectedState(true));

	QString qsVmUuid = ((PrlHandleVmCfg* )m_pVm.getHandle())->GetUuid();
	PrlHandleJobPtr pJob = m_pVm->GetServerVm()->DspCmdVmDevConnect(QSTR2UTF8(qsVmUuid),
																  pVmDev->getDeviceType(),
																  pVmDev->getIndex(),
																  QSTR2UTF8( toString() ));

	//Restoring device connected state
	pVmDev->setConnected(bOldDevConnectedState);
	return (pJob);
}

PrlHandleJobPtr PrlHandleVmDevice::Disconnect()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CHECK_VM(PJOC_VM_DEV_DISCONNECT)
	CHECK_SERVER(PJOC_VM_DEV_DISCONNECT)
	CVmDevice *pVmDev = GetVmDeviceElem();
	if (!pVmDev)
		return (CreateErrorHandle(PRL_ERR_OBJECT_WAS_REMOVED, __FUNCTION__, PJOC_VM_DEV_DISCONNECT));

	PVE::DeviceConnectedState bOldDevConnectedState = pVmDev->getConnected();
	//Setting right sign of device connected state in sending XML configuration
	pVmDev->setConnected(PVE::DeviceConnectedState(false));

	QString qsVmUuid = ((PrlHandleVmCfg* )m_pVm.getHandle())->GetUuid();
	PrlHandleJobPtr pJob = m_pVm->GetServerVm()->DspCmdVmDevDisconnect(QSTR2UTF8(qsVmUuid),
																	 pVmDev->getDeviceType(),
																	 pVmDev->getIndex(),
																	 QSTR2UTF8( toString() ));

	//Restoring device connected state
	pVmDev->setConnected(bOldDevConnectedState);
	return (pJob);
}

PRL_RESULT PrlHandleVmDevice::GetIndex( PRL_UINT32_PTR pnIndex )
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	*pnIndex = pVmDev->getIndex();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetIndex( PRL_UINT32 nIndex )
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	pVmDev->setIndex(nIndex);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::ConcreteRemove()
{
	return (PRL_ERR_UNIMPLEMENTED);
}

PRL_RESULT PrlHandleVmDevice::GetDeviceType(PRL_DEVICE_TYPE_PTR pnDeviceType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	*pnDeviceType = pVmDev->getDeviceType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::IsConnected(PRL_BOOL_PTR pbConnected)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	*pbConnected = PRL_BOOL(pVmDev->getConnected());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetConnected(PRL_BOOL bConnected)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	pVmDev->setConnected(PVE::DeviceConnectedState(bConnected));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::IsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	*pbEnabled = PRL_BOOL(pVmDev->getEnabled());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	pVmDev->setEnabled(PVE::DeviceEnabledState(bEnabled));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::IsRemote(PRL_BOOL_PTR pbRemote)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	*pbRemote = PRL_BOOL(pVmDev->isRemote());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetRemote(PRL_BOOL bRemote)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	pVmDev->setRemote(bool(bRemote));
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::GetEmulatedType(PRL_VM_DEV_EMULATION_TYPE_PTR pnEmulatedType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	*pnEmulatedType = PRL_VM_DEV_EMULATION_TYPE(pVmDev->getEmulatedType());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	pVmDev->setEmulatedType(nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

CVmClusteredDevice *PrlHandleVmDevice::GetVmClusteredDeviceElem(PRL_RESULT &nRetCode)
{
	nRetCode = PRL_ERR_INVALID_ARG;
	return (NULL);
}

CVmStorageDevice *PrlHandleVmDevice::GetVmStorageDeviceElem(PRL_RESULT &nRetCode)
{
	nRetCode = PRL_ERR_INVALID_ARG;
	return (NULL);
}

CVmMassStorageDevice *PrlHandleVmDevice::GetVmMassStorageDeviceElem(PRL_RESULT &nRetCode)
{
	nRetCode = PRL_ERR_INVALID_ARG;
	return (NULL);
}

PRL_RESULT PrlHandleVmDevice::GetSysName(PRL_STR sSysName, PRL_UINT32_PTR pnSysNameBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	return CopyStringValue(pVmDev->getSystemName(), sSysName, pnSysNameBufLength);
}

PRL_RESULT PrlHandleVmDevice::SetSysName(PRL_CONST_STR sNewSysName)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	QString sNewSysNameUtf8 = UTF8_2QSTR(sNewSysName);
	pVmDev->setSystemName(sNewSysNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::GetFriendlyName(PRL_STR sFriendlyName, PRL_UINT32_PTR pnFriendlyNameBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	return CopyStringValue(pVmDev->getUserFriendlyName(), sFriendlyName, pnFriendlyNameBufLength);
}

PRL_RESULT PrlHandleVmDevice::SetFriendlyName(PRL_CONST_STR sNewFriendlyName)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	QString sNewFriendlyNameUtf8 = UTF8_2QSTR(sNewFriendlyName);
	pVmDev->setUserFriendlyName(sNewFriendlyNameUtf8);
	if (pVmDev->getEmulatedType() == PDT_USE_IMAGE_FILE)
		pVmDev->setSystemName(sNewFriendlyNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::GetDescription(PRL_STR sDescription, PRL_UINT32_PTR pnDescriptionBufLength)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	return CopyStringValue(pVmDev->getDescription(), sDescription, pnDescriptionBufLength);
}

PRL_RESULT PrlHandleVmDevice::SetDescription(PRL_CONST_STR sNewDescription)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_XML_MODEL_OBJECT
	QString sNewDescriptionUtf8 = UTF8_2QSTR(sNewDescription);
	pVmDev->setDescription(sNewDescriptionUtf8);
	return (PRL_ERR_SUCCESS);
}

#define GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT\
	PRL_RESULT nRetCode = PRL_ERR_UNINITIALIZED;\
	CVmMassStorageDevice *pVmDev = GetVmMassStorageDeviceElem(nRetCode);\
	if (!pVmDev)\
		return (nRetCode);

PRL_RESULT PrlHandleVmDevice::GetIfaceType(PRL_MASS_STORAGE_INTERFACE_TYPE_PTR pnIfaceType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	*pnIfaceType = pVmDev->getInterfaceType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetIfaceType(PRL_MASS_STORAGE_INTERFACE_TYPE nIfaceType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	if (PDE_GENERIC_SCSI_DEVICE == pVmDev->getDeviceType())
		if (PMS_IDE_DEVICE == nIfaceType)
			return (PRL_ERR_INVALID_ARG);
	pVmDev->setInterfaceType(nIfaceType);
	return SetDefaultStackIndex();
}

PRL_RESULT PrlHandleVmDevice::GetSubType(PRL_CLUSTERED_DEVICE_SUBTYPE_PTR pnSubType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	*pnSubType = pVmDev->getSubType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetSubType(PRL_CLUSTERED_DEVICE_SUBTYPE nSubType)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	pVmDev->setSubType(nSubType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::GetStackIndex( PRL_UINT32_PTR pnStackIndex )
{
	SYNCHRO_VMDEV_DATA_ACCESS
	{
		GET_XML_MODEL_OBJECT
		CVmGenericPciDevice *pGenericPci = dynamic_cast<CVmGenericPciDevice *>(pVmDev);
		if (pGenericPci)
		{
			*pnStackIndex = pGenericPci->getStackIndex();
			return (PRL_ERR_SUCCESS);
		}
	}
	PRL_RESULT nRetCode = PRL_ERR_UNIMPLEMENTED;
	CVmMassStorageDevice *pVmDev = GetVmMassStorageDeviceElem(nRetCode);
	if (!pVmDev)
	{
		*pnStackIndex = 0;
		return (nRetCode);
	}
	*pnStackIndex = pVmDev->getStackIndex();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetStackIndex( PRL_UINT32 nStackIndex )
{
	SYNCHRO_VMDEV_DATA_ACCESS
	{
		GET_XML_MODEL_OBJECT
		CVmGenericPciDevice *pGenericPci = dynamic_cast<CVmGenericPciDevice *>(pVmDev);
		if (pGenericPci)
		{
			pGenericPci->setStackIndex(nStackIndex);
			return (PRL_ERR_SUCCESS);
		}
	}
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	pVmDev->setStackIndex(nStackIndex);
	return (PRL_ERR_SUCCESS);
}

namespace {
/**
 * Template helper for stack index auto assign mech. Travels through passing devices list and returns sign whether specified stack index already
 * presents in it.
 * @param devices list
 * @param checking stack index value
 * @param pointer to processing device element (need to ignoring corresponding element at devices list)
 * @return sign whether stack index already presents at devices list
 */
template <typename T, typename T1>
bool IsStackIndexPresentsInDevicesList(const QList<T *> &lstDevices, PRL_UINT32 nStackIndex, T1 *pVmDev)
{
	foreach(const T *pDevice, lstDevices)
		if (pDevice != pVmDev && pDevice->getInterfaceType() == pVmDev->getInterfaceType() && pDevice->getStackIndex() == nStackIndex)
			return (true);

	return (false);
}
template <>
bool IsStackIndexPresentsInDevicesList(const QList<CVmGenericPciDevice *> &lstDevices, PRL_UINT32 nStackIndex, CVmGenericPciDevice *pVmDev)
{
	foreach(const CVmGenericPciDevice *pDevice, lstDevices)
		if (pDevice != pVmDev && pDevice->getStackIndex() == nStackIndex)
			return (true);

	return (false);
}

}

#define PROCESS_STACK_INDEX(devices_list)\
	{\
		if (IsStackIndexPresentsInDevicesList(devices_list, nStackIndex, pVmDev))\
		{\
			nStackIndex++;\
			if (nStackIndex == nMaxPossibleIndexValue)\
				return (PRL_ERR_NO_MORE_FREE_INTERFACE_SLOTS);\
			else\
				continue;\
		}\
	}
/**
 * Internal helper - returns maximal possible stack index for specified interface type
 */
static PRL_UINT32 GetMaxPossibleIndexValue( PRL_MASS_STORAGE_INTERFACE_TYPE nIfaceType )
{
	switch ( nIfaceType )
	{
		case PMS_IDE_DEVICE: return 4;
		case PMS_SCSI_DEVICE: return 16;
		case PMS_SATA_DEVICE: return 6;
		case PMS_VIRTIO_BLOCK_DEVICE:
			return 16;
		default:
			PRL_ASSERT(false);
			return 0;
	}
}

PRL_RESULT PrlHandleVmDevice::SetDefaultStackIndex()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	{
		GET_XML_MODEL_OBJECT
		CVmGenericPciDevice *pGenericPci = dynamic_cast<CVmGenericPciDevice *>(pVmDev);
		if (pGenericPci)
		{
			PRL_UINT32 nStackIndex = 0;
			if (m_pVm.getHandle())
			{
				const PRL_UINT32 nMaxPossibleIndexValue = 10; //MAX_GENERIC_PCI_DEVICES
				CVmGenericPciDevice *pVmDev = pGenericPci;
				while (true)
				{
					PROCESS_STACK_INDEX(m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericPciDevices)
					break;
				}
			}
			pGenericPci->setStackIndex(nStackIndex);
			return (PRL_ERR_SUCCESS);
		}
	}
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	PRL_UINT32 nStackIndex = 0;
	if (m_pVm.getHandle())
	{
		const PRL_UINT32 nMaxPossibleIndexValue = GetMaxPossibleIndexValue( pVmDev->getInterfaceType() );
		while (true)
		{
			PROCESS_STACK_INDEX(m_pVm->GetVmConfig().getVmHardwareList()->m_lstHardDisks)
			PROCESS_STACK_INDEX(m_pVm->GetVmConfig().getVmHardwareList()->m_lstOpticalDisks)
			PROCESS_STACK_INDEX(m_pVm->GetVmConfig().getVmHardwareList()->m_lstGenericScsiDevices)
			break;
		}
	}
	pVmDev->setStackIndex(nStackIndex);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::IsPassthrough(PRL_BOOL_PTR pbPassthrough)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	*pbPassthrough = PRL_BOOL(pVmDev->getPassthrough());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDevice::SetPassthrough(PRL_BOOL bPassthrough)
{
	SYNCHRO_VMDEV_DATA_ACCESS
	GET_MASS_STORAGE_DEVICE_XML_MODEL_OBJECT
	pVmDev->setPassthrough(PVE::DevicePassthroughMode(bPassthrough));
	return (PRL_ERR_SUCCESS);
}

CVmPort *PrlHandleVmDevice::GetVmPortDeviceElem(PRL_RESULT &nRetCode)
{
	nRetCode = PRL_ERR_INVALID_ARG;
	return (NULL);
}

PRL_RESULT PrlHandleVmDevice::Remove()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CVmDevice *pDevice = GetVmDeviceElem();
	if (!pDevice)//Trying to process on already cleaned VM device handle object
		return (PRL_ERR_OBJECT_WAS_REMOVED);
	//Cleanup corresponding (match to removing VM device) boot items
	if (m_pVm)
		((PrlHandleVmCfg* )m_pVm.getHandle())->CleanBootDevicesList(pDevice);

	PRL_RESULT nRes = ConcreteRemove();

	PrlControlValidity::MarkAsInvalid(pDevice);
	delete pDevice;	//Object deletion must be done here (not at ConcreteRemove method) after all unregistering actions because it's possible
					//of object usage from another instances of VM devices handles wrappers after it was physically destroyed at ConcreteRemove
	return (nRes);
}

PRL_RESULT PrlHandleVmDevice::CreateVmDevice(PrlHandleVmPtr pVm, PRL_DEVICE_TYPE device_type, PRL_HANDLE_PTR phVmDevice)
{
	if(pVm.getHandle())
		pVm->GetSynchroObject()->lock();

	PrlHandleVmDevice *pVmDevice = 0;
	try//FIXME: temporarily solution to prevent possibility of STD lib exceptions
	{
		switch (device_type)
		{
			case PDE_HARD_DISK:
				pVmDevice = new PrlHandleVmDeviceHardDrive(pVm);
				break;
			case PDE_FLOPPY_DISK:
				pVmDevice = new PrlHandleVmDeviceFloppy(pVm);
				break;
			case PDE_OPTICAL_DISK:
				pVmDevice = new PrlHandleVmDeviceOpticalDisk(pVm);
				break;
			case PDE_SERIAL_PORT:
				pVmDevice = new PrlHandleVmDeviceSerialPort(pVm);
				break;
			case PDE_PARALLEL_PORT:
				pVmDevice = new PrlHandleVmDeviceParallelPort(pVm);
				break;
			case PDE_USB_DEVICE:
				pVmDevice = new PrlHandleVmDeviceUsbPort(pVm);
				break;
			case PDE_GENERIC_NETWORK_ADAPTER:
				pVmDevice = new PrlHandleVmDeviceNetAdapter(pVm);
				break;
			case PDE_SOUND_DEVICE:
				pVmDevice = new PrlHandleVmDeviceSound(pVm);
				break;
			case PDE_PCI_VIDEO_ADAPTER:
				pVmDevice = new PrlHandleVmDeviceDisplay(pVm);
				break;
			case PDE_GENERIC_PCI_DEVICE:
				pVmDevice = new PrlHandleVmDeviceGenericPci(pVm);
				break;
			case PDE_GENERIC_SCSI_DEVICE:
				pVmDevice = new PrlHandleVmDeviceGenericScsi(pVm);
				break;
			default:
				if(pVm.getHandle())
					pVm->GetSynchroObject()->unlock();
				return (PRL_ERR_INVALID_ARG);
		}
	} catch (...)
	{}

	if(pVm.getHandle())
		pVm->GetSynchroObject()->unlock();
	if (!pVmDevice)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phVmDevice = pVmDevice->GetHandle();
	if (PDE_GENERIC_PCI_DEVICE == device_type)
		pVmDevice->SetDefaultStackIndex();
	return (PRL_ERR_SUCCESS);
}

quint32 PrlHandleVmDevice::ConcreteGenerateHashCode()
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CVmDevice* pVmDev = GetVmDeviceElem();
	if (!pVmDev)
		return 0;
	return (quint32 )pVmDev->getIndex();
}

PRL_RESULT PrlHandleVmDevice::fromStringCommon( PRL_CONST_STR sXml )
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CVmDevice* pVmDev = GetVmDeviceElem();
	if ( ! pVmDev )
		return PRL_ERR_UNEXPECTED;

	CVmHardware hw_list;
	if ( PRL_FAILED(hw_list.fromString(UTF8_2QSTR(sXml))) )
	{
		pVmDev->fromString(pVmDev->toString());

		if ( PRL_FAILED(hw_list.fromString(UTF8_2QSTR(sXml), pVmDev->getTagName())) )
			return PRL_ERR_UNEXPECTED;
	}

	QList<CVmDevice* >* pDevList = (QList<CVmDevice* >* )hw_list.m_aDeviceLists[pVmDev->getDeviceType()];
	if ( ! pDevList )
		return PRL_ERR_UNEXPECTED;

	if ( pDevList->isEmpty() )
		return fromString( sXml );

	return fromString( QSTR2UTF8(pDevList->at(0)->toString()) );
}

PRL_RESULT PrlHandleVmDevice::toStringCommon( PRL_VOID_PTR_PTR sXml )
{
	SYNCHRO_VMDEV_DATA_ACCESS
	CVmDevice* pVmDev = GetVmDeviceElem();
	if ( ! pVmDev )
		return PRL_ERR_UNEXPECTED;

	CVmHardware hw_list;
	hw_list.ClearLists();

	QList<CVmDevice* >* pDevList = (QList<CVmDevice* >* )hw_list.m_aDeviceLists[pVmDev->getDeviceType()];
	if ( ! pDevList )
		return PRL_ERR_UNEXPECTED;

	pDevList->append(pVmDev);
	*sXml = strdup( QSTR2UTF8(hw_list.toString(true)) );
	pDevList->clear();

	return (PRL_ERR_SUCCESS);
}

