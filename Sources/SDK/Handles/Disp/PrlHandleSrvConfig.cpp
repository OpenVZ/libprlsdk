/*
 * PrlHandleSrvConfig.cpp
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


#include "PrlHandleSrvConfig.h"
#include "XmlModel/HostHardwareInfo/CHwMemorySettings.h"
#include "PrlHandleHwGenericDevice.h"
#include "PrlHandleHwHardDisk.h"
#include "PrlHandleHwNetAdapter.h"
#include "PrlHandleHwGenericPciDevice.h"
#include "PrlHandleStringsList.h"
#include "PrlHandleCpuFeatures.h"

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


PrlHandleSrvConfig::PrlHandleSrvConfig(const QString& sSrvConfig)
: PrlHandleBase(PHT_SERVER_CONFIG), m_SrvConfig(sSrvConfig)
{
}

PrlHandleSrvConfig::PrlHandleSrvConfig()
: PrlHandleBase(PHT_SERVER_CONFIG)
{}

PrlHandleSrvConfig::~PrlHandleSrvConfig()
{
}


PRL_RESULT PrlHandleSrvConfig::GetHostRamSize(PRL_UINT32_PTR pnHostRamSize)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnHostRamSize = m_SrvConfig.getMemorySettings()->getHostRamSize();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetCpuModel(PRL_STR sCpuModelBuf, PRL_UINT32_PTR pnCpuModelBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_SrvConfig.getCpu()->getModel(), sCpuModelBuf, pnCpuModelBufLength);
}

PRL_RESULT PrlHandleSrvConfig::GetCpuCount(PRL_UINT32_PTR pnCpuCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCpuCount = m_SrvConfig.getCpu()->getNumber();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetCpuSpeed(PRL_UINT32_PTR pnCpuSpeed)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCpuSpeed = m_SrvConfig.getCpu()->getSpeed();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetCpuMode(PRL_CPU_MODE_PTR pnCpuMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnCpuMode = m_SrvConfig.getCpu()->getMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetCpuVtxMode(PRL_CPU_VTX_MODE_PTR pnVtxMode)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnVtxMode = m_SrvConfig.getCpu()->getVtxMode();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHostOsType(PRL_HOST_OS_TYPE_PTR pnHostOsType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnHostOsType = m_SrvConfig.getOsVersion()->getOsType();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHostOsMajor(PRL_UINT32_PTR pnHostOsMajor)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnHostOsMajor = m_SrvConfig.getOsVersion()->getMajor();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHostOsMinor(PRL_UINT32_PTR pnHostOsMinor)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnHostOsMinor = m_SrvConfig.getOsVersion()->getMinor();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHostOsSubMinor(PRL_UINT32_PTR pnHostOsSubMinor)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnHostOsSubMinor = m_SrvConfig.getOsVersion()->getSubMinor();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHostOsStrPresentation(PRL_STR sHostOsStrPresentationBuf, PRL_UINT32_PTR pnHostOsStrPresentationBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_SrvConfig.getOsVersion()->getStringPresentation(), sHostOsStrPresentationBuf,
													pnHostOsStrPresentationBufLength);
}

PRL_RESULT PrlHandleSrvConfig::IsSoundDefaultEnabled(PRL_BOOL_PTR pbSoundDefaultEnabled)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbSoundDefaultEnabled = m_SrvConfig.getSoundDefaultEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::IsUsbSupported(PRL_BOOL_PTR pbUsbSupported)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbUsbSupported = m_SrvConfig.getUsbSupported();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::IsVtdSupported(PRL_BOOL_PTR pbVtdSupported)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pbVtdSupported = PRL_BOOL(m_SrvConfig.isVtdSupported());
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetMaxHostNetAdapters(PRL_UINT32_PTR pnMaxNetAdaptersCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMaxNetAdaptersCount = m_SrvConfig.getNetworkSettings()->getMaxHostNetAdapters();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetMaxVmNetAdapters(PRL_UINT32_PTR pnMaxNetAdaptersCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnMaxNetAdaptersCount = m_SrvConfig.getNetworkSettings()->getMaxVmNetAdapters();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHostname(PRL_STR sHostname, PRL_UINT32_PTR pnHostnameBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_SrvConfig.getNetworkSettings()->getGlobalNetwork()->getHostName(), sHostname,
													pnHostnameBufLength);
}

PRL_RESULT PrlHandleSrvConfig::GetDefaultGateway(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_SrvConfig.getNetworkSettings()->getGlobalNetwork()->getDefaultGateway(), sDefaultGateway,
													pnDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleSrvConfig::GetDefaultGatewayIPv6(PRL_STR sDefaultGateway, PRL_UINT32_PTR pnDefaultGatewayBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return CopyStringValue(m_SrvConfig.getNetworkSettings()->getGlobalNetwork()->getDefaultGatewayIPv6(), sDefaultGateway,
													pnDefaultGatewayBufLength);
}

PRL_RESULT PrlHandleSrvConfig::GetDnsServers(PRL_HANDLE_PTR phDnsServersList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsList *pDnsServersList =
		new PrlHandleStringsList(m_SrvConfig.getNetworkSettings()->getGlobalNetwork()->getDnsIPAddresses());
	if (pDnsServersList)
	{
		*phDnsServersList = pDnsServersList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	else
		return (PRL_ERR_OUT_OF_MEMORY);
}

PRL_RESULT PrlHandleSrvConfig::GetSearchDomains(PRL_HANDLE_PTR phSearchDomainsList)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	PrlHandleStringsList *pSearchDomainsList =
		new PrlHandleStringsList(m_SrvConfig.getNetworkSettings()->getGlobalNetwork()->getSearchDomains());
	if (pSearchDomainsList)
	{
		*phSearchDomainsList = pSearchDomainsList->GetHandle();
		return (PRL_ERR_SUCCESS);
	}
	else
		return (PRL_ERR_OUT_OF_MEMORY);
}

#define CREATE_DEVICE(device_name)\
	if (nIndex >= PRL_UINT32(m_SrvConfig.m_lst##device_name##s.size()))\
		return (PRL_ERR_INVALID_ARG);\
	PrlHandleHwGenericDevice *pDevice = new PrlHandleHwGenericDevice(PrlHandleSrvConfigPtr(this), m_SrvConfig.m_lst##device_name##s.value(nIndex));\
	if (!pDevice)\
		return (PRL_ERR_OUT_OF_MEMORY);

PRL_RESULT PrlHandleSrvConfig::GetFloppyDisksCount(PRL_UINT32_PTR pnFloppyDisksCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnFloppyDisksCount = m_SrvConfig.m_lstFloppyDisks.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetFloppyDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phFloppyDisk)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(FloppyDisk)
	*phFloppyDisk = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetOpticalDisksCount(PRL_UINT32_PTR pnOpticalDisksCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnOpticalDisksCount = m_SrvConfig.m_lstOpticalDisks.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetOpticalDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phOpticalDisk)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(OpticalDisk)
	*phOpticalDisk = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetSerialPortsCount(PRL_UINT32_PTR pnSerialPortsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSerialPortsCount = m_SrvConfig.m_lstSerialPorts.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetSerialPort(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSerialPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(SerialPort)
	*phSerialPort = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetParallelPortsCount(PRL_UINT32_PTR pnParallelPortsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnParallelPortsCount = m_SrvConfig.m_lstParallelPorts.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetParallelPort(PRL_UINT32 nIndex, PRL_HANDLE_PTR phParallelPort)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(ParallelPort)
	*phParallelPort = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetSoundOutputDevsCount(PRL_UINT32_PTR pnSoundOutputDevsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSoundOutputDevsCount = m_SrvConfig.m_lstSoundOutputDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetSoundOutputDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSoundOutputDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(SoundOutputDevice)
	*phSoundOutputDev = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetSoundMixerDevsCount(PRL_UINT32_PTR pnSoundMixerDevsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnSoundMixerDevsCount = m_SrvConfig.m_lstSoundMixerDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetSoundMixerDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phSoundMixerDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(SoundMixerDevice)
	*phSoundMixerDev = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetPrintersCount(PRL_UINT32_PTR pnPrintersCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnPrintersCount = m_SrvConfig.m_lstPrinters.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetPrinterDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phPrinter)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(Printer)
	*phPrinter = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetUsbDevsCount(PRL_UINT32_PTR pnUsbDevsCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnUsbDevsCount = m_SrvConfig.m_lstUsbDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetUsbDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phUsbDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(UsbDevice)
	*phUsbDev = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHardDisksCount(PRL_UINT32_PTR pnHardDisksCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnHardDisksCount = m_SrvConfig.m_lstHardDisks.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetHardDisk(PRL_UINT32 nIndex, PRL_HANDLE_PTR phHardDisk)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SrvConfig.m_lstHardDisks.size()))
		return (PRL_ERR_INVALID_ARG);
	PrlHandleHwHardDisk *pDevice = new PrlHandleHwHardDisk(PrlHandleSrvConfigPtr(this), m_SrvConfig.m_lstHardDisks.value(nIndex));
	if (!pDevice)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phHardDisk = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetNetAdaptersCount(PRL_UINT32_PTR pnNetAdaptersCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnNetAdaptersCount = m_SrvConfig.m_lstNetworkAdapters.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetNetAdapter(PRL_UINT32 nIndex, PRL_HANDLE_PTR phNetAdapter)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SrvConfig.m_lstNetworkAdapters.size()))
		return (PRL_ERR_INVALID_ARG);
	PrlHandleHwNetAdapter *pDevice = new PrlHandleHwNetAdapter(PrlHandleSrvConfigPtr(this), m_SrvConfig.m_lstNetworkAdapters.value(nIndex));
	if (!pDevice)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phNetAdapter = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

CHostHardwareInfo &PrlHandleSrvConfig::GetSrvConfig()
{
	return (m_SrvConfig);
}

PRL_RESULT PrlHandleSrvConfig::GetGenericPciDevicesCount(PRL_UINT32_PTR pnGenericPciDevicesCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnGenericPciDevicesCount = m_SrvConfig.m_lstGenericPciDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetGenericPciDeviceDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericPciDevice)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (nIndex >= PRL_UINT32(m_SrvConfig.m_lstGenericPciDevices.size()))
		return (PRL_ERR_INVALID_ARG);
	PrlHandleHwGenericPciDevice *pDevice = new PrlHandleHwGenericPciDevice(PrlHandleSrvConfigPtr(this), m_SrvConfig.m_lstGenericPciDevices.value(nIndex));
	if (!pDevice)
		return (PRL_ERR_OUT_OF_MEMORY);
	*phGenericPciDevice = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetGenericScsiDevicesCount(PRL_UINT32_PTR pnGenericScsiDevicesCount)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	*pnGenericScsiDevicesCount = m_SrvConfig.m_lstGenericScsiDevices.size();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetGenericScsiDeviceDev(PRL_UINT32 nIndex, PRL_HANDLE_PTR phGenericScsiDevice)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CREATE_DEVICE(GenericScsiDevice)
	*phGenericScsiDevice = pDevice->GetHandle();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::fromString(PRL_CONST_STR sXml)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (PRL_SUCCEEDED(m_SrvConfig.fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;

	return PRL_ERR_INVALID_ARG;
}

QString PrlHandleSrvConfig::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return m_SrvConfig.toString();
}

PRL_RESULT PrlHandleSrvConfig::GetCpuFeatures(PRL_CPU_FEATURES_PTR pCpuFeatures)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	pCpuFeatures->nFEATURES = m_SrvConfig.getCpu()->getFEATURES();
	pCpuFeatures->nEXT_FEATURES = m_SrvConfig.getCpu()->getEXT_FEATURES();
	pCpuFeatures->nEXT_80000001_ECX = m_SrvConfig.getCpu()->getEXT_80000001_ECX();
	pCpuFeatures->nEXT_80000001_EDX = m_SrvConfig.getCpu()->getEXT_80000001_EDX();
	pCpuFeatures->nEXT_80000007_EDX = m_SrvConfig.getCpu()->getEXT_80000007_EDX();
	pCpuFeatures->nEXT_80000008_EAX = m_SrvConfig.getCpu()->getEXT_80000008_EAX();
	pCpuFeatures->nEXT_00000007_EBX = m_SrvConfig.getCpu()->getEXT_00000007_EBX();
	pCpuFeatures->nEXT_0000000D_EAX = m_SrvConfig.getCpu()->getEXT_0000000D_EAX();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetCpuFeaturesMaskingCapabilities(PRL_HANDLE_PTR phCpuFeatures)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PrlHandleCpuFeatures *pFeatures = new PrlHandleCpuFeatures();
	if (!pFeatures)
		return (PRL_ERR_OUT_OF_MEMORY);

	pFeatures->SetValue(PCFE_FEATURES,
			m_SrvConfig.getCpu()->getFEATURES_MASKING_CAP());
	pFeatures->SetValue(PCFE_EXT_FEATURES,
			m_SrvConfig.getCpu()->getEXT_FEATURES_MASKING_CAP());
	pFeatures->SetValue(PCFE_EXT_80000001_ECX,
			m_SrvConfig.getCpu()->getEXT_80000001_ECX_MASKING_CAP());
	pFeatures->SetValue(PCFE_EXT_80000001_EDX,
			m_SrvConfig.getCpu()->getEXT_80000001_EDX_MASKING_CAP());
	pFeatures->SetValue(PCFE_EXT_80000007_EDX,
			m_SrvConfig.getCpu()->getEXT_80000007_EDX_MASKING_CAP());
	pFeatures->SetValue(PCFE_EXT_80000008_EAX,
			m_SrvConfig.getCpu()->getEXT_80000008_EAX_MASKING_CAP());
	pFeatures->SetValue(PCFE_EXT_00000007_EBX,
			m_SrvConfig.getCpu()->getEXT_00000007_EBX_MASKING_CAP());

	*phCpuFeatures = pFeatures->GetHandle();

	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleSrvConfig::GetCpuFeaturesEx(PRL_HANDLE_PTR phCpuFeatures)
{
	SYNCHRO_INTERNAL_DATA_ACCESS

	PrlHandleCpuFeatures *pFeatures = new PrlHandleCpuFeatures();
	if (!pFeatures)
		return (PRL_ERR_OUT_OF_MEMORY);

	pFeatures->SetValue(PCFE_FEATURES, m_SrvConfig.getCpu()->getFEATURES());
	pFeatures->SetValue(PCFE_EXT_FEATURES, m_SrvConfig.getCpu()->getEXT_FEATURES());
	pFeatures->SetValue(PCFE_EXT_80000001_ECX, m_SrvConfig.getCpu()->getEXT_80000001_ECX());
	pFeatures->SetValue(PCFE_EXT_80000001_EDX, m_SrvConfig.getCpu()->getEXT_80000001_EDX());
	pFeatures->SetValue(PCFE_EXT_80000007_EDX, m_SrvConfig.getCpu()->getEXT_80000007_EDX());
	pFeatures->SetValue(PCFE_EXT_80000008_EAX, m_SrvConfig.getCpu()->getEXT_80000008_EAX());
	pFeatures->SetValue(PCFE_EXT_00000007_EBX, m_SrvConfig.getCpu()->getEXT_00000007_EBX());

	*phCpuFeatures = pFeatures->GetHandle();

	return (PRL_ERR_SUCCESS);
}

