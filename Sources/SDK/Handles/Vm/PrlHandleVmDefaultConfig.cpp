/*
 * PrlHandleVmDefaultConfig.cpp: Contains class, which implements
 * generation of Virtual Machine default settings and devices.
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


#include <algorithm>
#include "PrlHandleVmDefaultConfig.h"

#include "PrlHandleSrvConfig.h"
#include "PrlHandleHwGenericDevice.h"

#include "PrlHandleServerNet.h"
#include "PrlHandleVirtNet.h"
#include "PrlHandleResult.h"

#include "PrlHandleVmDeviceFloppy.h"
#include "PrlHandleVmDeviceOpticalDisk.h"
#include "PrlHandleVmDeviceHardDrive.h"
#include "PrlHandleVmDeviceNetAdapter.h"
#include "PrlHandleVmDeviceSound.h"
#include "PrlHandleVmDeviceParallelPort.h"
#include "PrlHandleVmDeviceSerialPort.h"
#include "PrlHandleVmDeviceUsbPort.h"
#include "PrlHandleVmDeviceGenericPci.h"
#include "PrlHandleVmDeviceDisplay.h"
#include "ConfigDefaults.h"

#include <prlxmlmodel/ParallelsObjects/CXmlModelHelper.h>
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmSoundInputs.h>
#include <prlxmlmodel/VmConfig/CVmSoundOutputs.h>
#include <prlxmlmodel/HostHardwareInfo/CHostHardwareInfo.h>
#include <prlcommon/Std/PrlTime.h>
#include <prlcommon/Interfaces/ParallelsSdk.h>
#include <prlcommon/PrlCommonUtilsBase/ParallelsDirs.h>
#include <prlcommon/PrlCommonUtilsBase/CHardDiskHelper.h>
#include <prlcommon/HostUtils/HostUtils.h>


#define STR_BUF_LENGTH 1024
enum {JOB_WAIT_TIMEOUT = 5 * 1000};

using namespace CXmlModelHelper;

static const QString strImagePatternFdd = UTF8_2QSTR( "floppy@.fdd" );
static const QString strImagePatternHdd = UTF8_2QSTR( "harddisk@.hdd" );
static const QString strImagePatternEth = UTF8_2QSTR( "eth@" );
static const QString strImagePatternSerial = UTF8_2QSTR( "serial@.txt" );
static const QString strImagePatternParallel = UTF8_2QSTR( "parallel@.txt" );

const char* DEFAULT_NETWORK_ADAPTER_NAME  = QT_TRANSLATE_NOOP( "PrlErrStringsStorage", "Default Adapter" );

PrlHandleVmDefaultConfig::PrlHandleVmDefaultConfig ( PrlHandleSrvConfigPtr pSrvConfig, PrlHandleVmPtr pVm ) :
	m_pSrvConfig( pSrvConfig ), m_pVm( pVm )
{
}


PrlHandleVmDefaultConfig::~PrlHandleVmDefaultConfig()
{
}


QString PrlHandleVmDefaultConfig::CreateDefaultVmConfig(
		uint osVersion,
		bool needCreateDevices )
{
	CVmConfiguration cfg;

	/**
	 *	Store VM OS type, version and UUID
	 */

	cfg.getVmSettings()->getVmCommonOptions()->setOsType( PVS_GET_GUEST_TYPE(osVersion) );
	cfg.getVmSettings()->getVmCommonOptions()->setOsVersion( osVersion );

	QString vmUuid = Uuid::createUuid().toString();
	cfg.getVmIdentification()->setVmUuid( vmUuid );

	/**
	 *	Setup common VM defaults
	 */

	SetupRuntimeOptionsDefaults( cfg );
	SetupBootingOptionsDefaults( cfg );
	SetupCpuDefaults( cfg );
	SetupVideoDefaults( cfg );
	SetupMemoryDefaults( cfg );
	SetupToolsDefaults( cfg );
	SetupVirtualPrintersDefaults( cfg );
	SetupUsbControllerDefaults( cfg );

	if (PrlGetApiAppMode() == PAM_SERVER)
		cfg.getVmSettings()->getShutdown()->setAutoStop(PAO_VM_SHUTDOWN);

	/**
	 *	Setup devices
	 */

	if ( needCreateDevices )
	{
		DeviceList devList;
		SetupDeviceDefaults( cfg, devList );
		foreach( PRL_DEVICE_TYPE devType, devList )
			AddDefaultDevice( cfg, devType );
	}

	return cfg.toString();
}


uint PrlHandleVmDefaultConfig::GetDefaultRamSize (uint osVersion, uint hostRam )
{
	Q_UNUSED( hostRam );

	switch( osVersion )
	{
	case PVS_GUEST_VER_WIN_311:
		return 64;

	case PVS_GUEST_VER_WIN_95:
		return 128;

	case PVS_GUEST_VER_WIN_NT:
	case PVS_GUEST_VER_WIN_98:
	case PVS_GUEST_VER_WIN_ME:
	case PVS_GUEST_VER_WIN_2K:
		return 256;

	case PVS_GUEST_VER_WIN_XP:
	case PVS_GUEST_VER_WIN_2003:
	case PVS_GUEST_VER_WIN_VISTA:
	case PVS_GUEST_VER_WIN_2008:
	case PVS_GUEST_VER_WIN_WINDOWS7:
	case PVS_GUEST_VER_WIN_WINDOWS8:
	case PVS_GUEST_VER_WIN_WINDOWS8_1:
	case PVS_GUEST_VER_WIN_OTHER:
		return CalculateDefaultWindowsRamSize( hostRam );

	case PVS_GUEST_VER_WIN_WINDOWS10:
	case PVS_GUEST_VER_WIN_2016:
		return 2048;

	case PVS_GUEST_VER_OS2_ECS11:
	case PVS_GUEST_VER_OS2_ECS12:
	case PVS_GUEST_VER_OS2_WARP45:
		return 256;

	case PVS_GUEST_VER_SOL_OTHER:
	case PVS_GUEST_VER_SOL_9:
	case PVS_GUEST_VER_WIN_2012:
	case PVS_GUEST_VER_LIN_FEDORA:
		return 1024;
	case PVS_GUEST_VER_SOL_10:
		return 1536;
	case PVS_GUEST_VER_SOL_11:
		return 768;

	case PVS_GUEST_VER_LIN_FEDORA_5:
		return 512;

	case PVS_GUEST_VER_LIN_REDHAT_7:
	case PVS_GUEST_VER_LIN_CENTOS_7:
	case PVS_GUEST_VER_LIN_VZLINUX_7:
	case PVS_GUEST_VER_LIN_CLOUDLINUX_7:
	case PVS_GUEST_VER_LIN_PSBM:
		return 2048;

	default:
		break ;
	}

	switch(PVS_GET_GUEST_TYPE(osVersion)) {
	case PVS_GUEST_TYPE_WINDOWS:
		return 1024;

	case PVS_GUEST_TYPE_OS2:
		return 128;

	case PVS_GUEST_TYPE_MSDOS:
		return 32;

	case PVS_GUEST_TYPE_MACOS:
	{
		if ( PVS_GUEST_VER_MACOS_TIGER == osVersion )
			return CalculateDefaultGuestRamSize( hostRam );
		else
			return CalculateDefaultMacRamSize( hostRam );
	}

	case PVS_GUEST_TYPE_LINUX:
	case PVS_GUEST_TYPE_CHROMEOS:
	case PVS_GUEST_TYPE_ANDROID:
		return CalculateDefaultGuestRamSize( hostRam );

	default:
		break;
	}

	return 256;
}


uint PrlHandleVmDefaultConfig::GetDefaultHddSize ( uint osVersion )
{
    switch( osVersion )
    {
        case PVS_GUEST_VER_WIN_311:
        case PVS_GUEST_VER_WIN_95:
            return 2048;

        case PVS_GUEST_VER_WIN_98:
        case PVS_GUEST_VER_WIN_ME:
            return 8192;

        case PVS_GUEST_VER_WIN_NT:
			return 4096;

        default:
            break ;
    }

	switch( PVS_GET_GUEST_TYPE(osVersion) )
	{
		case PVS_GUEST_TYPE_OS2:
		case PVS_GUEST_TYPE_MSDOS:
			return 2048;

		case PVS_GUEST_TYPE_NETWARE:
			return 8192;

		case PVS_GUEST_TYPE_FREEBSD:
		case PVS_GUEST_TYPE_SOLARIS:
		case PVS_GUEST_TYPE_MACOS:
		case PVS_GUEST_TYPE_LINUX:
		case PVS_GUEST_TYPE_CHROMEOS:
		case PVS_GUEST_TYPE_ANDROID:
		case PVS_GUEST_TYPE_WINDOWS:
			return 65536;

		case PVS_GUEST_TYPE_OTHER:
		default:
			break;
	}

    return 8192;
}

PRL_UINT32 PrlHandleVmDefaultConfig::GetDefaultVideoRamSize ( PRL_UINT32 osVersion )
{
	Q_UNUSED(osVersion);
	return 32;
}

PRL_UINT32 PrlHandleVmDefaultConfig::GetDefaultVideoRamSize ( PRL_UINT32 osVersion,
										  PRL_HOST_OS_TYPE nHostOsType,
										  PRL_UINT32 hostRamSize,
										  bool bIs3DSupportEnabled )
{
	//Special case for Windows 2003 guest OS
	//https://bugzilla.sw.ru/show_bug.cgi?id=460001
	if ( PVS_GUEST_VER_WIN_2003 == osVersion )
		return ( 64 );

	// To fix #119783
	if ( (nHostOsType == PHO_MAC || nHostOsType == PHO_LIN) &&
		bIs3DSupportEnabled &&
		PVS_GET_GUEST_TYPE(osVersion) == PVS_GUEST_TYPE_WINDOWS &&
		hostRamSize > 1024 )
	{
		// for win server versions (w2k3 / win2008) set 128Mb anycase
		if( (osVersion == PVS_GUEST_VER_WIN_2K ||
			 osVersion == PVS_GUEST_VER_WIN_XP ||
			 osVersion == PVS_GUEST_VER_WIN_VISTA ||
			 osVersion == PVS_GUEST_VER_WIN_WINDOWS7 ||
			 osVersion == PVS_GUEST_VER_WIN_WINDOWS8 ||
			 osVersion == PVS_GUEST_VER_WIN_WINDOWS8_1) &&
			 hostRamSize > 2048 )
			return ( 256 );
		else
			return ( 128 );
	}
	else // if host ram size is small, this is very dangerous to create 128mb video ram
		return ( GetDefaultVideoRamSize( osVersion ) );
}

PRL_UINT32 PrlHandleVmDefaultConfig::GetDefaultVideoRamSize ( PRL_UINT32 osVersion, const PrlHandleSrvConfigPtr &pSrvConfig, bool bIs3DSupportEnabled )
{
	PRL_HOST_OS_TYPE nHostOsType = PHO_MAC;
	PRL_UINT32 hostRamSize = 0;
	PRL_RESULT res = PRL_ERR_SUCCESS;
	res = pSrvConfig.getHandle() ? pSrvConfig->GetHostOsType( &nHostOsType ) : PRL_ERR_INVALID_ARG;
	if( PRL_SUCCEEDED(res) )
		res = pSrvConfig.getHandle() ? pSrvConfig->GetHostRamSize( &hostRamSize ): PRL_ERR_INVALID_ARG;

	if ( PRL_SUCCEEDED(res) )
		return GetDefaultVideoRamSize( osVersion, nHostOsType,hostRamSize, bIs3DSupportEnabled );
	else
		return GetDefaultVideoRamSize( osVersion );
}


uint PrlHandleVmDefaultConfig::GetOsType( const CVmConfiguration& cfg )
{
	return cfg.getVmSettings()->getVmCommonOptions()->getOsType();
}


uint PrlHandleVmDefaultConfig::GetOsVersion ( const CVmConfiguration& cfg )
{
	return cfg.getVmSettings()->getVmCommonOptions()->getOsVersion();
}


void PrlHandleVmDefaultConfig::SetupRuntimeOptionsDefaults( CVmConfiguration& cfg )
{
	CVmRunTimeOptions* runtimeOpt = cfg.getVmSettings()->getVmRuntimeOptions();

	runtimeOpt->setBackgroundPriority( (PRL_VM_PRIORITY)DEFAULT_BG_PRIORITY );
	runtimeOpt->setForegroundPriority( (PRL_VM_PRIORITY)DEFAULT_FG_PRIORITY );
	runtimeOpt->setDiskCachePolicy( (PVE::DiskCacheWritebackOption)DEFAULT_CACHE_ENABLE );
	runtimeOpt->setCloseAppOnShutdown( DEFAULT_AUTOSHUTDOWN );
	runtimeOpt->setOsResolutionInFullScreen( DEFAULT_FSM );
	runtimeOpt->setCloseAppOnShutdown( DEFAULT_AUTOSHUTDOWN );
	// disable speaker by default for server mode #429304
	if ( PrlGetApiAppMode() == PAM_SERVER )
		runtimeOpt->setDisableSpeaker( true );
}


void PrlHandleVmDefaultConfig::SetupBootingOptionsDefaults( CVmConfiguration& cfg )
{
	int i = -1;

	CVmStartupOptions* startupOpts = cfg.getVmSettings()->getVmStartupOptions();

	startupOpts->setAutoStart( (PRL_VM_AUTOSTART_OPTION)DEFAULT_AUTOSTART );
	startupOpts->setWindowMode( PWM_DEFAULT_WINDOW_MODE );

	// FIXME enable it when #PSBM-42171 will be fixed
	//if ( GetOsVersion( cfg ) == PVS_GUEST_VER_WIN_2012 )
	//{
	//	// Set EFI BIOS as default for Windows-2012 (#PSBM-16282)
	//	startupOpts->getBios()->setType(PBT_EFI);
	//}

	// Setup booting order
	QList<CVmStartupOptions::CVmBootDevice*> bootDeviceList;
	CVmStartupOptions::CVmBootDevice* bootDevice1 =
		new CVmStartupOptions::CVmBootDevice( PDE_HARD_DISK, 0, 0, true );
	CVmStartupOptions::CVmBootDevice* bootDevice2 =
		new CVmStartupOptions::CVmBootDevice( PDE_OPTICAL_DISK, 0, 1, true );
	CVmStartupOptions::CVmBootDevice* bootDevice4 =
		new CVmStartupOptions::CVmBootDevice( PDE_GENERIC_NETWORK_ADAPTER, 0, 3, true );
	CVmStartupOptions::CVmBootDevice* bootDevice5 =
		new CVmStartupOptions::CVmBootDevice( PDE_USB_DEVICE, 0, 4, true );

	bootDeviceList.append( bootDevice1 );
	bootDeviceList.append( bootDevice2 );

	// Do not add network on Netware
	if ( GetOsType( cfg ) != PVS_GUEST_TYPE_NETWARE )
	{
		bootDeviceList.append( bootDevice4 );
		bootDevice4 = NULL;
	}

	// for warp os/2 systems setup floppy first
	if ( GetOsType( cfg ) == PVS_GUEST_TYPE_OS2 &&
			GetOsVersion( cfg ) <= PVS_GUEST_VER_OS2_WARP45  )
	{
		bootDeviceList.prepend( bootDeviceList.takeAt( 2 ) );
	}

	if ( GetOsType( cfg ) != PVS_GUEST_TYPE_MACOS )
	{
		// Insert USB boot device after optical boot device
		for( i = 0 ; i < bootDeviceList.size() ; i++ )
		{
			if ( bootDeviceList[i]->getType() != PDE_OPTICAL_DISK )
				continue;
			bootDeviceList.insert(i + 1, bootDevice5);
			bootDevice5->setInUse( startupOpts->getBios()->isEfiEnabled() );
			bootDevice5 = NULL;
			break;
		}
	}

	for( i = 0 ; i < bootDeviceList.size() ; i++ )
		bootDeviceList[i]->setBootingNumber( i );

	if (bootDevice4)
		delete bootDevice4;
	if (bootDevice5)
		delete bootDevice5;

	startupOpts->setBootDeviceList( bootDeviceList );
}

void PrlHandleVmDefaultConfig::SetupToolsDefaults( CVmConfiguration& )
{
}


void PrlHandleVmDefaultConfig::SetupCpuDefaults( CVmConfiguration& cfg )
{
	CVmHardware* vmHardware = cfg.getVmHardwareList();

	/**
	 * Setup the VT-x support
	 */

	PRL_CPU_VTX_MODE vtxMode = PCM_CPU_NONE_HV;
	PRL_UINT32 defaultCpuCount  = 1;
	PRL_UINT32 nOsVersion = GetOsVersion(cfg);
	bool bMoreOneCpuOs = ! (   (nOsVersion >= PVS_GUEST_VER_WIN_311 && nOsVersion < PVS_GUEST_VER_WIN_2K)
							|| nOsVersion == PVS_GUEST_VER_LIN_KRNL_24
							|| IS_VALID_OS2_VERSION(nOsVersion)
							|| IS_VALID_MSDOS_VERSION(nOsVersion)
							|| IS_VALID_SOLARIS_VERSION(nOsVersion)
							);

	if (IsServerPresent() && bMoreOneCpuOs)
	{
		PRL_UINT32 serverCpuCount = 1;
		m_pSrvConfig->GetCpuCount( &serverCpuCount );

		if (PrlGetApiAppMode() == PAM_SERVER)
		{
			//Apply follow logic just for server application mode
			m_pSrvConfig->GetCpuVtxMode( &vtxMode );

			// set default number of proccessors:
			// 1 cpu on srv => 1 cpu in config
			// 2 cpu on srv => 1 cpu in config
			// 4 cpu on srv => 2 cpu in config (if HV presented)
			if ( serverCpuCount >= 4 && vtxMode != PCM_CPU_NONE_HV )
				defaultCpuCount = 2;
		}
	}

	vmHardware->getCpu()->setNumber( defaultCpuCount );

	vmHardware->getCpu()->setEnableVTxSupport( PVE::VTxSupportEnabled );

	/**
	 * Setup acceleration level
	 */
	vmHardware->getCpu()->setAccelerationLevel( PVA_ACCELERATION_HIGH );
}


void PrlHandleVmDefaultConfig::SetupVideoDefaults ( CVmConfiguration& cfg )
{
	//Disable 3D acceleration for server mode under Mac OS X by default
	//See https://bugzilla.sw.ru/show_bug.cgi?id=272272 for more details
	if ( IsServerPresent() )
	{
		PRL_HOST_OS_TYPE hostOsType = m_pSrvConfig->GetSrvConfig().getOsVersion()->getOsType();
		if( PHO_MAC == hostOsType || PHO_LIN == hostOsType )
		{
			if ( PAM_SERVER == PrlGetApiAppMode() )
				cfg.getVmHardwareList()->getVideo()->setEnable3DAcceleration(P3D_DISABLED);

			if( cfg.getVmSettings()->getVmCommonOptions()->getOsType() == PVS_GUEST_TYPE_WINDOWS &&
				cfg.getVmSettings()->getVmCommonOptions()->getOsVersion() < PVS_GUEST_VER_WIN_2K
				)
				cfg.getVmHardwareList()->getVideo()->setEnable3DAcceleration( P3D_DISABLED );
		}
	}

	// Set video memory
	uint videoSize =
		GetDefaultVideoRamSize(
			cfg.getVmSettings()->getVmCommonOptions()->getOsVersion(),
			m_pSrvConfig,
			cfg.getVmHardwareList()->getVideo()->getEnable3DAcceleration() != P3D_DISABLED
		);

	cfg.getVmHardwareList()->getVideo()->setMemorySize( videoSize );

	// Set high resolution drawing
	cfg.getVmHardwareList()->getVideo()->setEnableHiResDrawing(
		GetOsVersion(cfg) >= PVS_GUEST_VER_WIN_WINDOWS7 && GetOsVersion(cfg) < PVS_GUEST_VER_WIN_OTHER );
}


void PrlHandleVmDefaultConfig::SetupMemoryDefaults( CVmConfiguration& cfg )
{
	/**
	 * Obtain the host RAM size required to calculate default memory
	 * size for VM. Use default value if creating configuration without
	 * available server.
	 */
	uint hostRam = 512;
	if ( IsServerPresent() )
		m_pSrvConfig->GetHostRamSize( &hostRam );

	CVmHardware* vmHardware = cfg.getVmHardwareList();

	// Set ram size
	uint memorySize = GetDefaultRamSize( cfg, hostRam );
	vmHardware->getMemory()->setRamSize( memorySize );
}

void PrlHandleVmDefaultConfig::SetupVirtualPrintersDefaults ( CVmConfiguration& cfg )
{
	cfg.getVmSettings()->getVirtualPrintersInfo()->setUseHostPrinters(false);
	cfg.getVmSettings()->getVirtualPrintersInfo()->setSyncDefaultPrinter(false);
}

void PrlHandleVmDefaultConfig::SetupUsbControllerDefaults ( CVmConfiguration& cfg )
{
	cfg.getVmSettings()->getUsbController()->setUhcEnabled();
	cfg.getVmSettings()->getUsbController()->setEhcEnabled();
	cfg.getVmSettings()->getUsbController()->setXhcEnabled();

	uint osVersion = GetOsVersion( cfg );

	if ( osVersion == PVS_GUEST_VER_MACOS_UNIVERSAL
		|| IS_LINUX(osVersion)
		|| (osVersion >= PVS_GUEST_VER_WIN_WINDOWS8 && osVersion < PVS_GUEST_VER_WIN_OTHER))
	{
		cfg.getVmSettings()->getUsbController()->setUhcEnabled(false);
		cfg.getVmSettings()->getUsbController()->setEhcEnabled(false);
		cfg.getVmSettings()->getUsbController()->setXhcEnabled(true);
	}

	if (IS_OS2(osVersion))
	{
		cfg.getVmSettings()->getUsbController()->setEhcEnabled(false);
	}
}

void PrlHandleVmDefaultConfig::SetupDeviceDefaults ( CVmConfiguration& cfg, DeviceList& devList )
{
	uint osVersion = GetOsVersion( cfg );

	SetupHardDiskDefaults( osVersion, devList );
	SetupFloppyDefaults( osVersion, devList );
	SetupCdRomDefaults( osVersion, devList );
	SetupSerialPortDefaults( osVersion, devList );
	SetupSoundDefaults( osVersion, devList );
	SetupNetworkDefaults( osVersion, devList );
	SetupUsbDefaults( osVersion, devList );
	// Setup ParallelPortDefaults after UsbDefaults,
	// because we have USB printer by default that depends on USB controller.
	// Firstly we must add usb controller and after try to add printers.
	SetupParallelPortDefaults( osVersion, devList );
}


uint PrlHandleVmDefaultConfig::GetDefaultRamSize( CVmConfiguration& cfg, uint hostRam )
{
	return GetDefaultRamSize( GetOsVersion( cfg ), hostRam );
}


uint PrlHandleVmDefaultConfig::GetDefaultHddSize ( CVmConfiguration& cfg )
{
	return GetDefaultHddSize( GetOsVersion( cfg ) );
}


void PrlHandleVmDefaultConfig::SetupHardDiskDefaults( uint, DeviceList& devList )
{
	devList.append( PDE_HARD_DISK );
}


void PrlHandleVmDefaultConfig::SetupFloppyDefaults( uint osVersion, DeviceList& devList )
{
	switch( PVS_GET_GUEST_TYPE(osVersion) )
	{
		case PVS_GUEST_TYPE_SOLARIS:
			break;

		case PVS_GUEST_TYPE_LINUX:
			break;

		// XXX: disable floppy in default MacOS configuration
		case PVS_GUEST_TYPE_MACOS:
			break;

		default:
			devList.append( PDE_FLOPPY_DISK );
			break;
	}
}


void PrlHandleVmDefaultConfig::SetupCdRomDefaults( uint, DeviceList& devList )
{
	devList.append( PDE_OPTICAL_DISK );
}


void PrlHandleVmDefaultConfig::SetupSoundDefaults( uint osVersion, DeviceList& devList )
{
	if ( PrlGetApiAppMode() == PAM_SERVER )
		return;

	switch( PVS_GET_GUEST_TYPE(osVersion) )
	{
		case PVS_GUEST_TYPE_MSDOS:
		case PVS_GUEST_TYPE_NETWARE:
		case PVS_GUEST_TYPE_SOLARIS:
			break;

		default:
			devList.append( PDE_SOUND_DEVICE );
			break;
	}
}

void PrlHandleVmDefaultConfig::SetupParallelPortDefaults( uint osVersion, DeviceList& devList )
{
	if ( PrlGetApiAppMode() == PAM_SERVER )
		return;

	switch( PVS_GET_GUEST_TYPE(osVersion) )
	{
		case PVS_GUEST_TYPE_LINUX:
		case PVS_GUEST_TYPE_CHROMEOS:
			if ( osVersion == PVS_GUEST_VER_LIN_OTHER )
				devList.append( PDE_PARALLEL_PORT );
			break;
		case PVS_GUEST_TYPE_FREEBSD:
			devList.append( PDE_PARALLEL_PORT );
			break;
		case PVS_GUEST_TYPE_WINDOWS:
			if (osVersion != PVS_GUEST_VER_WIN_311)
				devList.append( PDE_PARALLEL_PORT );
			break;

		default:
			break;
	}
}

void PrlHandleVmDefaultConfig::SetupSerialPortDefaults( uint osVersion, DeviceList& devList )
{
	switch( PVS_GET_GUEST_TYPE(osVersion) )
	{
		case PVS_GUEST_TYPE_OS2:
			devList.append( PDE_SERIAL_PORT );
			break;

		default:
			break;
	}
}

void PrlHandleVmDefaultConfig::SetupNetworkDefaults( uint osVersion, DeviceList& devList )
{
	switch( PVS_GET_GUEST_TYPE(osVersion) )
	{
		case PVS_GUEST_TYPE_NETWARE:
			break;

		default:
			devList.append( PDE_GENERIC_NETWORK_ADAPTER );
			break;
	}
}


void PrlHandleVmDefaultConfig::SetupUsbDefaults( uint osVersion, DeviceList& devList )
{
    switch( osVersion )
    {
        case PVS_GUEST_VER_LIN_KRNL_24:
            return ;

        case PVS_GUEST_VER_WIN_311:
        case PVS_GUEST_VER_WIN_95:
        case PVS_GUEST_VER_WIN_NT:
        case PVS_GUEST_VER_WIN_OTHER:
            return ;

        case PVS_GUEST_VER_OS2_ECS11:
        case PVS_GUEST_VER_OS2_ECS12:
            devList.append( PDE_USB_DEVICE );
            return;

        default:
            break ;
    }

	switch( PVS_GET_GUEST_TYPE(osVersion) )
	{
		case PVS_GUEST_TYPE_LINUX:
		case PVS_GUEST_TYPE_CHROMEOS:
		case PVS_GUEST_TYPE_ANDROID:
            devList.append( PDE_USB_DEVICE );
            break;

		case PVS_GUEST_TYPE_WINDOWS:
            devList.append( PDE_USB_DEVICE );
            break;

		case PVS_GUEST_TYPE_MACOS:
			devList.append( PDE_USB_DEVICE );
			break;
		case PVS_GUEST_TYPE_FREEBSD:
			devList.append( PDE_USB_DEVICE );
			break;

		default:
			break;
	}
}

bool PrlHandleVmDefaultConfig::AddDefaultFloppy( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice )
{
	CVmFloppyDisk* floppy = new CVmFloppyDisk();

	PVE::FloppyEmulatedType emulType = PVE::FloppyDiskImage;

	uint devIdx = GetUnusedDeviceIndex<CVmFloppyDisk >(cfg.getVmHardwareList()->m_lstFloppyDisks);

	unsigned int t = GetOsType(cfg);
	unsigned int v = GetOsVersion(cfg);
	// Set default floppy image path
	QString systemName;
	if (t == PVS_GUEST_TYPE_OS2)
	{
		systemName = ParallelsDirs::getFddToolsImageBaseName(t);
	}
	else if (IS_WINDOWS(v))
	{
		systemName = ParallelsDirs::getWindowsUnattendedFloppy(v);
		QFileInfo f(systemName);
		if (!f.exists())
		{
			WRITE_TRACE(DBG_FATAL, "Unable to find necessary file "
					"for Windows guest installation: '%s'",
					QSTR2UTF8(systemName));
			return false;
		}
	}
	else
	{
		systemName = GetDefaultImageName(strImagePatternFdd, devIdx);
	}

	QString friendlyName = systemName;

	PVE::DeviceConnectedState connectState = PVE::DeviceDisconnected;
	if ((t == PVS_GUEST_TYPE_MSDOS) ||
		(t == PVS_GUEST_TYPE_OS2) ||
		IS_WINDOWS(v))
	{
		connectState = PVE::DeviceConnected;
	}

	if (IsServerPresent() && (t != PVS_GUEST_TYPE_OS2) && !IS_WINDOWS(v))
	{
		if (m_pSrvConfig->GetSrvConfig().m_lstFloppyDisks.size())
		{
			friendlyName = m_pSrvConfig->GetSrvConfig().m_lstFloppyDisks.value(0)->getDeviceName();
			systemName = m_pSrvConfig->GetSrvConfig().m_lstFloppyDisks.value(0)->getDeviceId();
			emulType = PVE::RealFloppyDisk;
		}
	}

	floppy->setIndex(devIdx);
	floppy->setEmulatedType(emulType);
	floppy->setEnabled(PVE::DeviceEnabled);
	floppy->setConnected(connectState);
	floppy->setUserFriendlyName(friendlyName);
	floppy->setSystemName(systemName);

	cfg.getVmHardwareList()->addFloppyDisk(floppy);

	SortDeviceListByIndex<CVmFloppyDisk >(cfg.getVmHardwareList()->m_lstFloppyDisks);

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceFloppy(m_pVm, floppy);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}

/**
 * Limits placed upon IDA/SATA/SCSI interfaces and positions for attached backup
 *
 * There could be cases when we would need to limit the amount of available
 * interfaces and indices. One case is attaching a backup as virtual disk
 * to a Windows VM. There we need to take into account the positions of already
 * existing bootable HDDs and place the newly attached backup after the "last"
 * bootable HDD, according to the Windows boot preference order (IDE->SATA->SCSI).
 */
struct StackIndexLimit
{
	StackIndexLimit(const CVmConfiguration& vm, PRL_DEVICE_TYPE devType)
		: m_iface(PMS_IDE_DEVICE), m_stackIndex(0),
		  m_active(false)
	{
		/* boot preference order for Windows VMs: IDE->SATA->SCSI */
		m_windowsBootPreference[PMS_UNKNOWN_DEVICE] = -1;
		m_windowsBootPreference[PMS_IDE_DEVICE] = 0;
		m_windowsBootPreference[PMS_SATA_DEVICE] = 1;
		m_windowsBootPreference[PMS_SCSI_DEVICE] = 2;
		/* we're only interested in backups, attached to a Windows VM */
		if (devType != PDE_ATTACHED_BACKUP_DISK)
			return;
		if (vm.getVmType() != PVT_VM
			|| vm.getVmSettings()->getVmCommonOptions()->getOsType() != PVS_GUEST_TYPE_WINDOWS)
			return;
		/* find the last bootable HDD, according to Windows boot preference
		 * order: IDE->SATA->SCSI, i.e. if Windows sees several clones of the same
		 * disk during the boot (e.g. backup of the Windows system disk was
		 * attached to the same VM), then it first tries to boot from the disk,
		 * connected to IDE interface, then tries SATA, and in the end SCSI. */
		foreach(const CVmStartupOptions::CVmBootDevice *dev,
			vm.getVmSettings()->getVmStartupOptions()->getBootDeviceList())
		{
			if (dev->deviceType != PDE_HARD_DISK || !dev->inUseStatus)
				continue;
			const CVmHardDisk *hdd = findDiskByIndex(vm.getVmHardwareList()->m_lstHardDisks, dev->deviceIndex);
			if (!hdd)
				continue;
			PRL_MASS_STORAGE_INTERFACE_TYPE iface = hdd->getInterfaceType();
			if (m_windowsBootPreference.value(iface) < m_windowsBootPreference.value(m_iface))
				continue;
			unsigned stackIndex = hdd->getStackIndex();
			if (iface == m_iface)
			{
				m_stackIndex = qMax(stackIndex, m_stackIndex);
			}
			else if (m_windowsBootPreference.value(iface) > m_windowsBootPreference.value(m_iface))
			{
				m_iface = iface;
				m_stackIndex = stackIndex;
			}
			m_active = true;
		}
	}

	bool hasFreeStackIndices(uint iface) const
	{
		if (!m_active)
			return true;
		return m_windowsBootPreference.value(static_cast<PRL_MASS_STORAGE_INTERFACE_TYPE>(iface))
			>= m_windowsBootPreference.value(m_iface);
	}

	int getStackIndex(uint iface, QList<uint>& indices) const
	{
		if (!m_active || static_cast<PRL_MASS_STORAGE_INTERFACE_TYPE>(iface) != m_iface)
			return indices.first();
		QList<uint>::const_iterator p = std::find_if(indices.begin(), indices.end(),
			std::bind2nd(std::greater<uint>(), m_stackIndex));
		return p == indices.end() ? -1 : *p;
	}

private:
	const CVmHardDisk *findDiskByIndex(QList<CVmHardDisk* >& list, unsigned int index) const
	{
		foreach(const CVmHardDisk *hdd, list)
			if (hdd->getIndex() == index)
				return hdd;
		return NULL;
	}

private:
	PRL_MASS_STORAGE_INTERFACE_TYPE m_iface;
	unsigned int m_stackIndex;
	bool m_active;
	QMap<PRL_MASS_STORAGE_INTERFACE_TYPE, int> m_windowsBootPreference;
};

bool PrlHandleVmDefaultConfig::calculateInterfaceParamsForHddCdrom( const CVmConfiguration& cfg,
																	PRL_DEVICE_TYPE devType,
																	int & interfaceSlot,
																	PRL_MASS_STORAGE_INTERFACE_TYPE & interfaceType )
{
	bool bTryAddToIde = false;
	interfaceType = PMS_IDE_DEVICE;
	unsigned int v = cfg.getVmSettings()->getVmCommonOptions()->getOsVersion();

	// For Windows guest CD drive must be connected to IDE bus during
	// guest installation (#PSBM-40949)
	// The same for Debian (#PSBM-41679)
	if (PVS_GUEST_VIRTIO_SUPPORTED(v)
		&& !(IS_WINDOWS(v) && devType == PDE_OPTICAL_DISK)
		&& !(v == PVS_GUEST_VER_LIN_DEBIAN && devType == PDE_OPTICAL_DISK))
	{
		interfaceType = PMS_SCSI_DEVICE;
		bTryAddToIde = true;
	}

	StackIndexLimit limit(cfg, devType);
	interfaceSlot = GetFreeStackIndex( cfg, interfaceType, &limit );
	if ( interfaceSlot < 0 )
	{
		interfaceType = PMS_SCSI_DEVICE;
		interfaceSlot = GetFreeStackIndex( cfg, interfaceType, &limit );
		if ( interfaceSlot < 0 )
		{
			if( bTryAddToIde )
			{
				if ( IS_VALID_MACOS_VERSION(v) )
					return false;

				interfaceType = PMS_IDE_DEVICE;
				interfaceSlot = GetFreeStackIndex( cfg, interfaceType, &limit );
				if ( interfaceSlot < 0 )
					return false;
			}
			else
				return false;
		}
	}

	return true;
}

bool PrlHandleVmDefaultConfig::AddDefaultCdRom( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice )
{
	CVmOpticalDisk* cdrom = new CVmOpticalDisk();

	PVE::CdromEmulatedType emulType = PVE::CdRomImage;

	// Set interface slot
	PRL_MASS_STORAGE_INTERFACE_TYPE interfaceType = PMS_IDE_DEVICE;
	int interfaceSlot = -1;
	if( !calculateInterfaceParamsForHddCdrom( cfg, PDE_OPTICAL_DISK, interfaceSlot, interfaceType ) )
	{
		delete cdrom;
		return false;
	}

	cdrom->setIndex( GetUnusedDeviceIndex<CVmOpticalDisk >(cfg.getVmHardwareList()->m_lstOpticalDisks) );
	cdrom->setEmulatedType( emulType );
	cdrom->setConnected( PVE::DeviceConnected );
	cdrom->setEnabled( PVE::DeviceEnabled );
	cdrom->setInterfaceType( interfaceType );
	cdrom->setStackIndex( interfaceSlot );
	cdrom->setRemote( false );
	cdrom->setPassthrough(PVE::PassthroughDisabled);

	if (cdrom->getInterfaceType() == PMS_SCSI_DEVICE)
	{
		unsigned int v = cfg.getVmSettings()->getVmCommonOptions()->getOsVersion();
		if (PVS_GUEST_SCSI_LSI_SPI_SUPPORTED(v))
			cdrom->setSubType(PCD_LSI_SPI);
		if (PVS_GUEST_SCSI_LSI_SAS_SUPPORTED(v))
			cdrom->setSubType(PCD_LSI_SAS);
		if (PVS_GUEST_VIRTIO_SUPPORTED(v))
			cdrom->setSubType(PCD_VIRTIO_SCSI);
	}

	cfg.getVmHardwareList()->addOpticalDisk( cdrom );

	SortDeviceListByIndex<CVmOpticalDisk >(cfg.getVmHardwareList()->m_lstOpticalDisks);

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceOpticalDisk(m_pVm, cdrom);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}


bool PrlHandleVmDefaultConfig::AddDefaultHardDisk ( CVmConfiguration& cfg, PRL_DEVICE_TYPE devType,
	PRL_HANDLE_PTR phDevice )
{
	CVmHardDisk* hdd = new CVmHardDisk();

	uint devIdx = GetUnusedDeviceIndex<CVmHardDisk >(cfg.getVmHardwareList()->m_lstHardDisks);

	QString friendlyName = GetDefaultImageName( strImagePatternHdd, devIdx );
	QString systemName = friendlyName;

	PRL_MASS_STORAGE_INTERFACE_TYPE interfaceType = PMS_IDE_DEVICE;
	int interfaceSlot = -1;
	if( !calculateInterfaceParamsForHddCdrom( cfg, devType, interfaceSlot, interfaceType ) )
	{
		delete hdd;
		return false;
	}

	int hddSize = GetDefaultHddSize( cfg );

	hdd->setIndex( devIdx );
	hdd->setEmulatedType( PVE::HardDiskImage );
	hdd->setConnected( PVE::DeviceConnected );
	hdd->setEnabled( PVE::DeviceEnabled );
	hdd->setInterfaceType( interfaceType );
	hdd->setSplitted( false );
	hdd->setPassthrough( PVE::PassthroughDisabled );

	PRL_BOOL bPlainDisk = PRL_FALSE;

	(void) m_pVm->GetServerDisp()->IsFeatureSupported( PFSM_DEFAULT_PLAINDISK_ALLOWED, &bPlainDisk );

	hdd->setDiskType( bPlainDisk ? PHD_PLAIN_HARD_DISK: PHD_EXPANDING_HARD_DISK );
	hdd->setStackIndex( interfaceSlot );
	hdd->setSize( hddSize );
	hdd->setUserFriendlyName( friendlyName );
	hdd->setSystemName( systemName );

	if (hdd->getInterfaceType() == PMS_SCSI_DEVICE)
	{
		unsigned int v = cfg.getVmSettings()->getVmCommonOptions()->getOsVersion();
		if (PVS_GUEST_SCSI_LSI_SPI_SUPPORTED(v))
			hdd->setSubType(PCD_LSI_SPI);
		if (PVS_GUEST_SCSI_LSI_SAS_SUPPORTED(v))
			hdd->setSubType(PCD_LSI_SAS);
		if (PVS_GUEST_VIRTIO_SUPPORTED(v))
			hdd->setSubType(PCD_VIRTIO_SCSI);
	}

	cfg.getVmHardwareList()->addHardDisk( hdd );

	SortDeviceListByIndex<CVmHardDisk >(cfg.getVmHardwareList()->m_lstHardDisks);

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceHardDrive(m_pVm, hdd);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}


bool PrlHandleVmDefaultConfig::AddDefaultSound ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice )
{
	CVmSoundDevice* sound = new CVmSoundDevice();

	// Setup emulated type
	uint osType = GetOsType( cfg );
	uint osVersion = GetOsVersion( cfg );
	PVE::SoundDeviceEmulatedType emulatedType = PVE::AC97;
	if ( osVersion == PVS_GUEST_VER_WIN_311 || osType == PVS_GUEST_TYPE_MSDOS )
		emulatedType = PVE::SB16;

	if ( IsServerPresent() )
	{
		if ( m_pSrvConfig->GetSrvConfig().m_lstSoundOutputDevices.size() )
		{
			CVmSoundDevice * pOutputSound = NULL;
			if( sound->getSoundOutputs()->m_lstSoundDevices.size() )
				pOutputSound = sound->getSoundOutputs()->m_lstSoundDevices[0];
			else
			{
				pOutputSound = new CVmSoundDevice();
				sound->getSoundOutputs()->m_lstSoundDevices.append( pOutputSound );
			}

			pOutputSound->setSystemName(
				m_pSrvConfig->GetSrvConfig().m_lstSoundOutputDevices.value(0)->getDeviceId() );

			pOutputSound->setUserFriendlyName(
				m_pSrvConfig->GetSrvConfig().m_lstSoundOutputDevices.value(0)->getDeviceName() );
		}

		if ( m_pSrvConfig->GetSrvConfig().m_lstSoundMixerDevices.size() )
		{
			CVmSoundDevice * pMixedSound = NULL;
			if( sound->getSoundInputs()->m_lstSoundDevices.size() )
				pMixedSound = sound->getSoundInputs()->m_lstSoundDevices[0];
			else
			{
				pMixedSound = new CVmSoundDevice();
				sound->getSoundInputs()->m_lstSoundDevices.append( pMixedSound );
			}

			pMixedSound->setSystemName(
				m_pSrvConfig->GetSrvConfig().m_lstSoundMixerDevices.value(0)->getDeviceId() );

			pMixedSound->setUserFriendlyName(
				m_pSrvConfig->GetSrvConfig().m_lstSoundMixerDevices.value(0)->getDeviceName() );

		}
	}

	sound->setConnected( PVE::DeviceConnected );
	sound->setEnabled( PVE::DeviceEnabled );
	sound->setEmulatedType( emulatedType );

	cfg.getVmHardwareList()->addSoundDevice( sound );

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceSound(m_pVm, sound);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}


bool PrlHandleVmDefaultConfig::AddDefaultUsb ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice )
{
	CVmUsbDevice* usb = new CVmUsbDevice();

	usb->setAutoconnectDevices( PUD_CONNECT_TO_PRIMARY_OS );
	usb->setEnabled( PVE::DeviceEnabled );

	cfg.getVmHardwareList()->addUsbDevice( usb );

	// NOTE: In order to USB devices became working
	//       it needs to reset USB controller
	//       if it was switched off.
	if ( ! (cfg.getVmSettings()->getUsbController()->isUhcEnabled()
		 || cfg.getVmSettings()->getUsbController()->isEhcEnabled()
		 || cfg.getVmSettings()->getUsbController()->isXhcEnabled()
		))
	{
		SetupUsbControllerDefaults( cfg );
	}

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceUsbPort(m_pVm, usb);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}

QString PrlHandleVmDefaultConfig::getDefaultBridgedNetwork()
{
	PrlHandleServerNetPtr pServer =
		PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>(m_pVm->GetServer()->GetHandle());
	PrlHandleJobPtr pJob = pServer->GetVirtualNetworkList(0);
	PRL_RESULT res = pJob->Wait(JOB_WAIT_TIMEOUT);
	if (PRL_FAILED(res))
		return QString();

	PRL_RESULT retcode;
	res = pJob->GetRetCode(&retcode);
	if (PRL_FAILED(res) || PRL_FAILED(retcode))
		return QString();

	PRL_HANDLE hResult;
	res = pJob->GetResult(&hResult);
	if (PRL_FAILED(res))
		return QString();

	PrlHandleResultPtr pResult = PRL_OBJECT_BY_HANDLE<PrlHandleResult>(hResult);
	PRL_UINT32 count;
   	res = pResult->GetParamsCount(&count);
	if (PRL_FAILED(res))
		return QString();

	for (PRL_UINT32 i = 0; i < count; ++i) {
		PRL_HANDLE pParam;
		res = pResult->GetParamByIndex(i, &pParam);
		if (PRL_FAILED(res))
			return QString();

		PrlHandleVirtNetPtr pVirtNet = PRL_OBJECT_BY_HANDLE<PrlHandleVirtNet>(pParam);

		PRL_NET_VIRTUAL_NETWORK_TYPE type;
		res = pVirtNet->GetNetworkType(&type);
		if (PRL_FAILED(res))
			return QString();

		if (type != PVN_BRIDGED_ETHERNET)
			continue;

		PRL_BOOL bEnabled;
		res = pVirtNet->IsEnabled(&bEnabled);
		if (PRL_FAILED(res))
			return QString();

		if (!bEnabled)
			continue;

		PRL_UINT32 bufLength = 0;
		res = pVirtNet->GetNetworkId(NULL, &bufLength);
		if (PRL_FAILED(res))
			return QString();

		char buf[bufLength];
		res = pVirtNet->GetNetworkId(buf, &bufLength);
		if (PRL_FAILED(res))
			return QString();

		return buf;
	}
	return QString();
}

bool PrlHandleVmDefaultConfig::AddDefaultNetwork ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice )
{
	CVmGenericNetworkAdapter* network = new CVmGenericNetworkAdapter();

	QString macAddress = HostUtils::generateMacAddress();

	bool isServerMode = ( PAM_SERVER == PrlGetApiAppMode() );

	uint devIdx = GetUnusedDeviceIndex<CVmGenericNetworkAdapter >(cfg.getVmHardwareList()->m_lstNetworkAdapters);
	QString systemName = GetDefaultNetworkName( strImagePatternEth, devIdx );
	QString friendlyName = systemName;

	network->setIndex( devIdx );
	network->setEmulatedType( isServerMode ? PNA_BRIDGED_NETWORK : PNA_SHARED );
	network->setConnected( PVE::DeviceConnected );
	network->setEnabled( PVE::DeviceEnabled );
	network->setMacAddress( macAddress );
	network->setStaticAddress(false);
	network->setBoundAdapterIndex( -1 );
	network->setBoundAdapterName( QObject::tr( DEFAULT_NETWORK_ADAPTER_NAME ) );
	network->setSystemName( systemName );
	network->setUserFriendlyName( friendlyName );
	if (network->getEmulatedType() == PNA_BRIDGED_NETWORK)
		network->setVirtualNetworkID(getDefaultBridgedNetwork());

	cfg.getVmHardwareList()->addNetworkAdapter( network );

	SortDeviceListByIndex<CVmGenericNetworkAdapter >(cfg.getVmHardwareList()->m_lstNetworkAdapters);

	unsigned int nVersion = cfg.getVmSettings()->getVmCommonOptions()->getOsVersion();
	if (nVersion == 0 || PVS_GUEST_VIRTIO_SUPPORTED(nVersion))
	{
		network->setAdapterType(PNT_VIRTIO);
	}
	else if (PVS_GUEST_E1000_SUPPORTED(nVersion))
	{
		network->setAdapterType(PNT_E1000);
	}

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceNetAdapter(m_pVm, network);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}


bool PrlHandleVmDefaultConfig::AddDefaultParallel ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice )
{
	PRL_PRINTER_INTERFACE_TYPE printerInterfaceType = PRN_LPT_DEVICE;

	PRL_BOOL bSupportUsbPrinters = PRL_FALSE;
	(void) m_pVm->GetServerDisp()->IsFeatureSupported( PFSM_USB_PRINTER_SUPPORT, &bSupportUsbPrinters );

	if ( !cfg.getVmHardwareList()->m_lstUsbDevices.isEmpty() &&  bSupportUsbPrinters )
	{
		int numUsbPorts = 0;
		int numLptPorts = 0;
		foreach( CVmParallelPort* p, cfg.getVmHardwareList()->m_lstParallelPorts )
		{
			if ( p->getPrinterInterfaceType() == PRN_USB_DEVICE )
				numUsbPorts++;
			if ( p->getPrinterInterfaceType() == PRN_LPT_DEVICE )
				numLptPorts++;
		}
		// Check that maximum USB and LPT printers not exhausted.
		// By default at first we add USB printers and only when it exhausted we add LPT printers.
		if ( numUsbPorts >= MAX_USB_PRINTER_COUNT )
		{
			printerInterfaceType = PRN_LPT_DEVICE;
			if ( numLptPorts >= MAX_PARALLEL_COUNT )
				return false;
		}
		else
			printerInterfaceType = PRN_USB_DEVICE;
	}

	CVmParallelPort* parallel = new CVmParallelPort();

	uint devIdx = GetUnusedDeviceIndex<CVmParallelPort >(cfg.getVmHardwareList()->m_lstParallelPorts);

	QString friendlyName = GetDefaultImageName( strImagePatternParallel, devIdx );
	QString systemName = friendlyName;

	PVE::ParallelPortEmulatedType emulatedType = PVE::ParallelOutputFile;

	uint osType = GetOsType( cfg );
	uint osVersion = GetOsVersion( cfg );

	// For FreeBSD and Other Linux we emulate using output file by default in any case
	bool forceEmulateOutputFile = ( osType == PVS_GUEST_TYPE_FREEBSD ||
									osVersion == PVS_GUEST_VER_LIN_OTHER );

	if ( !forceEmulateOutputFile && IsServerPresent() )
	{
		if ( osType == PVS_GUEST_TYPE_WINDOWS && osVersion != PVS_GUEST_VER_WIN_311
			&& !m_pSrvConfig->GetSrvConfig().m_lstPrinters.isEmpty() )
		{
			// set printer[0] as default!
			friendlyName = m_pSrvConfig->GetSrvConfig().m_lstPrinters.value(0)->getDeviceName();
			systemName = m_pSrvConfig->GetSrvConfig().m_lstPrinters.value(0)->getDeviceId();
			emulatedType = PVE::ParallelPrinter;
		}
		else if ( !m_pSrvConfig->GetSrvConfig().m_lstParallelPorts.isEmpty() )
		{
			friendlyName = m_pSrvConfig->GetSrvConfig().m_lstParallelPorts.value(0)->getDeviceName();
			systemName = m_pSrvConfig->GetSrvConfig().m_lstParallelPorts.value(0)->getDeviceId();
			emulatedType = PVE::RealParallelPort;
		}
	}

	parallel->setIndex( devIdx );
	parallel->setEmulatedType( emulatedType );
	parallel->setConnected( PVE::DeviceConnected );
	parallel->setEnabled( PVE::DeviceEnabled );
	parallel->setUserFriendlyName( friendlyName );
	parallel->setSystemName( systemName );
	parallel->setPrinterInterfaceType( printerInterfaceType );

	cfg.getVmHardwareList()->addParallelPort( parallel );

	SortDeviceListByIndex<CVmParallelPort >(cfg.getVmHardwareList()->m_lstParallelPorts);

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceParallelPort(m_pVm, parallel);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}


bool PrlHandleVmDefaultConfig::AddDefaultSerial ( CVmConfiguration& cfg, PRL_HANDLE_PTR phDevice )
{
	CVmSerialPort* serial = new CVmSerialPort();

	uint devIdx = GetUnusedDeviceIndex<CVmSerialPort >(cfg.getVmHardwareList()->m_lstSerialPorts);

	QString friendlyName = GetDefaultImageName( strImagePatternSerial, devIdx );
	QString systemName = friendlyName;

	PVE::SerialPortEmulatedType emulatedType = PVE::SerialOutputFile;

	if ( IsServerPresent() && PrlGetApiAppMode() != PAM_SERVER )
	{
		if ( m_pSrvConfig->GetSrvConfig().m_lstSerialPorts.size() )
		{
			friendlyName = m_pSrvConfig->GetSrvConfig().m_lstSerialPorts.value(0)->getDeviceName();
			systemName = m_pSrvConfig->GetSrvConfig().m_lstSerialPorts.value(0)->getDeviceId();
			emulatedType = PVE::RealSerialPort;
		}
	}

	serial->setIndex( devIdx );
	serial->setEmulatedType( emulatedType );
	serial->setConnected( PVE::DeviceConnected );
	serial->setSocketMode( PSP_SERIAL_SOCKET_SERVER );
	serial->setEnabled( PVE::DeviceEnabled  );
	serial->setUserFriendlyName( friendlyName );
	serial->setSystemName( systemName );

	cfg.getVmHardwareList()->addSerialPort( serial );

	SortDeviceListByIndex<CVmSerialPort >(cfg.getVmHardwareList()->m_lstSerialPorts);

	if (phDevice)
	{
		*phDevice = PRL_INVALID_HANDLE;
		PrlHandleVmDevice* pVmDevice = new PrlHandleVmDeviceSerialPort(m_pVm, serial);
		if (pVmDevice)
			*phDevice = pVmDevice->GetHandle();
	}

	return true;
}

bool PrlHandleVmDefaultConfig::AddDefaultDevice( CVmConfiguration& cfg,
												 PRL_DEVICE_TYPE devType,
												 PRL_HANDLE_PTR phDevice)
{
	switch ( devType )
	{
		case PDE_FLOPPY_DISK:
			return AddDefaultFloppy( cfg, phDevice );

		case PDE_OPTICAL_DISK:
			return AddDefaultCdRom( cfg, phDevice );

		case PDE_ATTACHED_BACKUP_DISK:
		case PDE_HARD_DISK:
			return AddDefaultHardDisk( cfg, devType, phDevice );

		case PDE_GENERIC_NETWORK_ADAPTER:
			return AddDefaultNetwork( cfg, phDevice );

		case PDE_SERIAL_PORT:
			return AddDefaultSerial( cfg, phDevice );

		case PDE_PARALLEL_PORT:
		case PDE_PRINTER:
			return AddDefaultParallel( cfg, phDevice );

		case PDE_SOUND_DEVICE:
			return AddDefaultSound( cfg, phDevice );

		case PDE_USB_DEVICE:
			return AddDefaultUsb( cfg, phDevice );

		default:
			return false;
	}
}


bool PrlHandleVmDefaultConfig::IsDefaultDeviceNeeded( PRL_UINT32 osVersion,
													  PRL_DEVICE_TYPE deviceType )
{
	DeviceList devList;

	switch ( deviceType )
	{
		case PDE_FLOPPY_DISK:
			SetupFloppyDefaults( osVersion, devList );
			break;

		case PDE_OPTICAL_DISK:
			SetupCdRomDefaults( osVersion, devList );
			break;

		case PDE_HARD_DISK:
			SetupHardDiskDefaults( osVersion, devList );
			break;

		case PDE_GENERIC_NETWORK_ADAPTER:
			SetupNetworkDefaults( osVersion, devList );
			break;

		case PDE_SOUND_DEVICE:
			SetupSoundDefaults( osVersion, devList );
			break;

		case PDE_PARALLEL_PORT:
			SetupParallelPortDefaults( osVersion, devList );
			break;

		case PDE_USB_DEVICE:
			SetupUsbDefaults( osVersion, devList );
			break;

		default:
			break;
	}

	return devList.contains( deviceType );
}


bool PrlHandleVmDefaultConfig::IsServerPresent() const
{
	return m_pSrvConfig.isValid();
}


QString PrlHandleVmDefaultConfig::GetDeviceName( PRL_HANDLE hDevice )
{
	if ( hDevice == PRL_INVALID_HANDLE )
		return QString();

	PrlHandleHwGenericDevicePtr pGenDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hDevice );

	PRL_CHAR sDevName[STR_BUF_LENGTH];
	PRL_UINT32 nDevNameBufLength = STR_BUF_LENGTH;

	pGenDevice->GetName( sDevName, &nDevNameBufLength );
	return UTF8_2QSTR( sDevName );
}


QString PrlHandleVmDefaultConfig::GetDeviceId( PRL_HANDLE hDevice )
{
	if ( hDevice == PRL_INVALID_HANDLE )
		return QString();

	PrlHandleHwGenericDevicePtr pGenDevice = PRL_OBJECT_BY_HANDLE<PrlHandleHwGenericDevice>( hDevice );

	PRL_CHAR sDevName[STR_BUF_LENGTH];
	PRL_UINT32 nDevNameBufLength = STR_BUF_LENGTH;

	pGenDevice->GetId( sDevName, &nDevNameBufLength );
	return UTF8_2QSTR( sDevName );
}

int PrlHandleVmDefaultConfig::GetFreeStackIndex( const CVmConfiguration& cfg,
												 uint interfaceType,
												 StackIndexLimit *limit) const
{
	if (limit && !limit->hasFreeStackIndices(interfaceType))
		return -1;

	QList<uint> lstFreeSlots;

	// Fill free stack indexes list
	CVmHardware* hwList = cfg.getVmHardwareList();

	CHwDeviceInterfaceSettings devIfaceSet;
	if (IsServerPresent())
		devIfaceSet.fromString(m_pSrvConfig->GetSrvConfig().getDeviceInterfaceSettings()->toString());

	uint slotNumb = devIfaceSet.getMaxIdeDevices();
	switch(interfaceType)
	{
	case PMS_SCSI_DEVICE:
		slotNumb = devIfaceSet.getMaxScsiDevices();
		break;
	case PMS_SATA_DEVICE:
		slotNumb = devIfaceSet.getMaxSataDevices();
		break;
	default:
		;
	}

	for ( uint i = 0; i < slotNumb; ++i )
		lstFreeSlots.append( i );

	foreach ( CVmHardDisk* hdd, hwList->m_lstHardDisks )
		if ( (uint)hdd->getInterfaceType() == interfaceType )
			lstFreeSlots.removeAll( hdd->getStackIndex() );

	foreach ( CVmOpticalDisk* cdRom, hwList->m_lstOpticalDisks )
		if ( (uint)cdRom->getInterfaceType() == interfaceType )
			lstFreeSlots.removeAll( cdRom->getStackIndex() );

	foreach ( CVmGenericScsiDevice* scsi, hwList->m_lstGenericScsiDevices )
		if ( (uint)scsi->getInterfaceType() == interfaceType )
			lstFreeSlots.removeAll( scsi->getStackIndex() );

	if ( lstFreeSlots.isEmpty() )
		return -1;

	return limit ? limit->getStackIndex(interfaceType, lstFreeSlots) : lstFreeSlots.first();
}


QString PrlHandleVmDefaultConfig::GetDefaultImageName( const QString& patternName,
													   uint devIdx )
{
	QString imageName( patternName ), seqNumber;

	if ( devIdx == 0 )
		// Remove placeholder from image name for device with zero index
		imageName.remove( '@' );
	else
		// Substitute device index to placeholder
		imageName.replace( QString("@"), seqNumber.setNum(devIdx + 1) );

	return imageName;
}

QString PrlHandleVmDefaultConfig::GetDefaultNetworkName( const QString& patternName,
													   uint devIdx )
{
	QString imageName( patternName ), seqNumber;

	// Substitute device index to placeholder
	imageName.replace( QString("@"), seqNumber.setNum(devIdx) );

	return imageName;
}

uint PrlHandleVmDefaultConfig::CalculateDefaultGuestRamSize( uint uiHostRam )
{
	if ( uiHostRam <= 1024 )
		return 512;
	else
	if ( uiHostRam > 2048 )
		return 1024;
	else
		return 768;
}

uint PrlHandleVmDefaultConfig::CalculateDefaultWindowsRamSize( uint uiHostRam )
{
	if ( uiHostRam > 3072 )
		return 1024;
	if ( uiHostRam > 2048 )
		return 768;
	return 512;
}

uint PrlHandleVmDefaultConfig::CalculateDefaultMacRamSize( uint uiHostRam )
{
	if ( uiHostRam <= 2048 )
		return 512;
	if ( uiHostRam < 4096 )
		return 1024;
	return 2048;
}
