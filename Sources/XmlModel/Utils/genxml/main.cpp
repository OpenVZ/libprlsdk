/*
 * main.cpp: That is to generate xml files from config classes.
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


#include <stdio.h>

#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QList>

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/Interfaces/ParallelsQt.h>

#include <CVmConfiguration.h>
#include <CVmDirectories.h>
#include <CDispatcherConfig.h>
#include <CDispNetAdapter.h>
#include <CProblemReport.h>
#include <CVmEvent.h>
#include <CVmEventParameter.h>
#include <CHostHardwareInfo.h>
#include <CHwFileSystemInfo.h>
#include <CDispDhcpPreferences.h>
#include <CPresentationPreferences.h>
#include <CMainWindowPreferences.h>
#include <CSharedWebPreferences.h>
#include <CKeyboardPreferences.h>
#include <CUpdatePreferences.h>
#include <CVmHostSharing.h>
#include <CVmGuestSharing.h>
#include <CClientPreferences.h>
#include <CVmGenericDevice.h>
#include <CVmGenericPciDevice.h>
#include <CVmGenericScsiDevice.h>
#include <CUpdaterConfig.h>
#include <CAutomaticUpdate.h>
#include <CUpdates.h>
#include <CUpdProduct.h>
#include <CUpdProductVersion.h>
#include <CUpdate.h>
#include <CHwNetAdapter.h>
#include <CDispLicense.h>
#include <CVaConfig.h>
#include <CDiskXML.h>

static QTextStream qerr(stderr, QIODevice::WriteOnly);

/**
 * Shows how to use this test.
 */
static void usage()
{
    qerr << "Usage: genxml -VmConfig <outfile.xml>\n";
    qerr << "Usage: genxml -DispConfig <outfile.xml>\n";
    qerr << "Usage: genxml -ProbRep <outfile.xml>\n";
    qerr << "Usage: genxml -VmDirs <outfile.xml>\n";
    qerr << "Usage: genxml -VmEvent <outfile.xml>\n";
    qerr << "Usage: genxml -HostHwInfo <outfile.xml>\n";
    qerr << "Usage: genxml -HostFsInfo <outfile.xml>\n";
    qerr << "Usage: genxml -ClientPrefs <outfile.xml>\n";
    qerr << "Usage: genxml -UpdConfig <outfile.xml>\n";
    qerr << "Usage: genxml -Updates <outfile.xml>\n";
    qerr << "Usage: genxml -DiskDescriptor <outfile.xml>\n";
   exit(1);
}


/**
 * Generates a XML file from a CVmConfiguration class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenVmConfig(const QString& qsFileName)
{
	CVmConfiguration* pVmCfg = new CVmConfiguration();

	CVmIdentification* pVmId = pVmCfg->getVmIdentification();
	if (pVmId)
	{
		pVmId->setVmUuid("VM_UUID");
		pVmId->setVmName("VM_NAME");
		pVmId->setServerUuid("SERVER_NAME");
		pVmId->setServerHost("SERVER_HOST");
		pVmId->setLastModifDate(QDateTime::currentDateTime());
		pVmId->setModifierName("MODIFIER_NAME");
		pVmId->setHomePath("HOME_PATH");
	}

	CVmSecurity* pVmSec = pVmCfg->getVmSecurity();
	if (pVmSec)
	{
		pVmSec->getAccessControlList()->setAccessControl( QList<PRL_ALLOWED_VM_COMMAND>()  );
	}

	CVmHardware* pVmHwLst =  pVmCfg->getVmHardwareList();
	if (pVmHwLst)
	{
		CVmCpu* pCpu = pVmHwLst->getCpu();
		if ( pCpu )
		{
			pCpu->setNumber( 2 );
			pCpu->setMode( PCM_CPU_MODE_64 );
			pCpu->setAccelerationLevel( PVA_ACCELERATION_NORMAL );
			pCpu->setEnableVTxSupport( PVE::VTxSupportDisabled );
		}

		CVmVideo* pVideo = pVmHwLst->getVideo();
		if ( pVideo )
		{
			pVideo->setMemorySize( 32 );
			pVideo->setEnableDirectXShaders( false );
			CVmScreenResolutions* pVmSResols = pVideo->getVmScreenResolutions();
			if (pVmSResols)
			{
				CVmScreenResolution* pVmSResol = new CVmScreenResolution();
				pVmSResols->addScreenResolution( pVmSResol);
				pVmSResols->addScreenResolution( pVmSResol);
			}
		}

		CVmGenericDevice* pVmGenDev = new CVmGenericDevice();
		pVmGenDev->setSystemName("ACTUAL_DEVICE_PATH");
		pVmGenDev->setUserFriendlyName("DEVICE_NAME_ASSIGNED_BY_USER");
		pVmGenDev->setInterfaceType(PVE::GenericDeviceScsi);
		pVmGenDev->setStackIndex(1);
		pVmHwLst->addGenericDevice(pVmGenDev);
		CVmGenericDevice* pVmGenDev_copy = new CVmGenericDevice();
		*pVmGenDev_copy = *pVmGenDev;
		pVmGenDev_copy->setIndex(1);
		pVmHwLst->addGenericDevice(pVmGenDev_copy);

		CVmGenericPciDevice* pVmGenPciDev = new CVmGenericPciDevice();
		pVmGenPciDev->setSystemName("ACTUAL_GENERIC_PCI_DEVICE_PATH");
		pVmGenPciDev->setUserFriendlyName("GENERIC_PCI_DEVICE_NAME_ASSIGNED_BY_USER");
		pVmGenPciDev->setDescription("GENERIC_PCI_DEVICE_DESCRIPTION");
		pVmGenPciDev->setStackIndex(1);
		pVmHwLst->addGenericPciDevice(pVmGenPciDev);
		CVmGenericPciDevice* pVmGenPciDev_copy = new CVmGenericPciDevice();
		*pVmGenPciDev_copy = *pVmGenPciDev;
		pVmGenPciDev_copy->setIndex(1);
		pVmHwLst->addGenericPciDevice(pVmGenPciDev_copy);

		CVmGenericScsiDevice* pVmGenScsiDev = new CVmGenericScsiDevice();
		pVmGenScsiDev->setSystemName("ACTUAL_GENERIC_SCSI_DEVICE_PATH");
		pVmGenScsiDev->setUserFriendlyName("GENERIC_SCSI_DEVICE_NAME_ASSIGNED_BY_USER");
		pVmGenScsiDev->setDescription("GENERIC_SCSI_DEVICE_DESCRIPTION");
		pVmGenScsiDev->setStackIndex(1);
		pVmGenScsiDev->setPassthrough(PVE::PassthroughEnabled);
		pVmHwLst->addGenericScsiDevice(pVmGenScsiDev);
		CVmGenericScsiDevice* pVmGenScsiDev_copy = new CVmGenericScsiDevice();
		*pVmGenScsiDev_copy = *pVmGenScsiDev;
		pVmGenScsiDev_copy->setIndex(1);
		pVmHwLst->addGenericScsiDevice(pVmGenScsiDev_copy);

		CVmFloppyDisk* pVmFlop = new CVmFloppyDisk();
		pVmFlop->setEmulatedType((PVE::FloppyEmulatedType)1);
		pVmHwLst->addFloppyDisk(pVmFlop);
		CVmFloppyDisk* pVmFlop_copy = new CVmFloppyDisk();
		*pVmFlop_copy = *pVmFlop;
		pVmHwLst->addFloppyDisk(pVmFlop_copy);

		CVmMassStorageDevice* pVmMSDev = new CVmMassStorageDevice();
		pVmMSDev->setInterfaceType((PRL_MASS_STORAGE_INTERFACE_TYPE)1);
		pVmMSDev->setStackIndex(1);
		pVmHwLst->addIdeDevice(pVmMSDev);
		CVmMassStorageDevice* pVmMSDev_copy = new CVmMassStorageDevice();
		*pVmMSDev_copy = *pVmMSDev;
		pVmHwLst->addIdeDevice(pVmMSDev_copy);

		CVmOpticalDisk* pVmOptDisk = new CVmOpticalDisk();
		pVmOptDisk->setEmulatedType((PVE::CdromEmulatedType)1);
		pVmOptDisk->setPassthrough((PVE::DevicePassthroughMode)1);
		pVmHwLst->addOpticalDisk(pVmOptDisk);
		CVmOpticalDisk* pVmOptDisk_copy = new CVmOpticalDisk();
		*pVmOptDisk_copy = *pVmOptDisk;
		pVmHwLst->addOpticalDisk(pVmOptDisk_copy);

		CVmHardDisk* pVmHardDisk = new CVmHardDisk();
		pVmHardDisk->setEmulatedType((PVE::HardDiskEmulatedType)1);
		pVmHardDisk->setPassthrough((PVE::DevicePassthroughMode)1);
		pVmHardDisk->setSplitted( true );
		pVmHwLst->addHardDisk(pVmHardDisk);
		CVmHardDisk* pVmHardDisk_copy = new CVmHardDisk();
		*pVmHardDisk_copy = *pVmHardDisk;
		pVmHardDisk_copy->setIndex(1);
		pVmHwLst->addHardDisk(pVmHardDisk_copy);

		CVmSerialPort* pVmSerPort = new CVmSerialPort();
		pVmSerPort->setSocketMode((PRL_SERIAL_PORT_SOCKET_OPERATION_MODE)1);
		pVmSerPort->setEmulatedType((PVE::SerialPortEmulatedType)1);
		pVmHwLst->addSerialPort(pVmSerPort);
		CVmSerialPort* pVmSerPort_copy = new CVmSerialPort();
		*pVmSerPort_copy = *pVmSerPort;
		pVmHwLst->addSerialPort(pVmSerPort_copy);

		CVmParallelPort* pVmParPort = new CVmParallelPort();
		pVmParPort->setEmulatedType((PVE::ParallelPortEmulatedType)1);
		pVmHwLst->addParallelPort(pVmParPort);
		CVmParallelPort* pVmParPort_copy = new CVmParallelPort();
		*pVmParPort_copy = *pVmParPort;
		pVmHwLst->addParallelPort(pVmParPort_copy);

		CVmGenericNetworkAdapter* pVmNet = new CVmGenericNetworkAdapter();
		pVmNet->setBoundAdapterIndex(1);
		pVmNet->setBoundAdapterName("BOUND_ADAPTER_NAME");
		pVmNet->setMacAddress("MAC_ADDRESS");
		pVmNet->setEmulatedType((PRL_NET_ADAPTER_EMULATED_TYPE)1);
		pVmHwLst->addNetworkAdapter(pVmNet);
		CVmGenericNetworkAdapter* pVmNet_copy = new CVmGenericNetworkAdapter();
		*pVmNet_copy = *pVmNet;
		pVmHwLst->addNetworkAdapter(pVmNet_copy);

		CVmSoundDevice* pVmSound = new CVmSoundDevice();
		pVmSound->setEmulatedType((PVE::SoundDeviceEmulatedType)1);
		pVmSound->setOutputDevice("OUTPUT_DEVICE");
		pVmSound->setMixerDevice("MIXER_DEVICE");
		pVmHwLst->addSoundDevice(pVmSound);
		CVmSoundDevice* pVmSound_copy = new CVmSoundDevice();
		*pVmSound_copy = *pVmSound;
		pVmHwLst->addSoundDevice(pVmSound_copy);

		CVmUsbDevice* pVmUsb = new CVmUsbDevice();
		pVmUsb->setEmulatedType((PVE::UsbDeviceEmulatedType)1);
		pVmUsb->setAutoconnectDevices((PRL_USB_DEVICE_AUTO_CONNECT_OPTION)1);
		pVmHwLst->addUsbDevice(pVmUsb);
		CVmUsbDevice* pVmUsb_copy = new CVmUsbDevice();
		*pVmUsb_copy = *pVmUsb;
		pVmHwLst->addUsbDevice(pVmUsb_copy);
	}

	CVmSettings* pVmSet = pVmCfg->getVmSettings();
	if ( pVmSet )
	{
			CVmCommonOptions* pVmComOpts = pVmSet->getVmCommonOptions();
			if ( pVmComOpts )
			{
				pVmComOpts->setOsType( 1 );
				pVmComOpts->setOsVersion( 311 );
				pVmComOpts->setIcon( "VM_ICON" );
				pVmComOpts->setVmDescription( "VM_DESCRIPTION" );
			}
			CVmRunTimeOptions* pVmRTOpts = pVmSet->getVmRuntimeOptions();
			if ( pVmRTOpts )
			{
				pVmRTOpts->setForegroundPriority( (PRL_VM_PRIORITY)1 );
				pVmRTOpts->setBackgroundPriority( (PRL_VM_PRIORITY)1 );
				pVmRTOpts->setDockIconType( PIT_DOCK_ICON_START_MENU );
				pVmRTOpts->setOsResolutionInFullScreen( true );
				pVmRTOpts->setCloseAppOnShutdown( false );
				pVmRTOpts->setUndoDisksMode( PUD_COMMIT_CHANGES );
			}
			CVmStartupOptions* pVmStOpts = pVmSet->getVmStartupOptions();
			if ( pVmStOpts )
			{
				pVmStOpts->setAutoStart( (PRL_VM_AUTOSTART_OPTION)1 );
				QList<CVmStartupOptions::CVmBootDevice*> bootDeviceList;

				CVmStartupOptions::CVmBootDevice* bootDevice1 =
					new CVmStartupOptions::CVmBootDevice( PDE_FLOPPY_DISK, 1, 1, 1 );
				CVmStartupOptions::CVmBootDevice* bootDevice2 =
					new CVmStartupOptions::CVmBootDevice( PDE_OPTICAL_DISK, 2, 2, 0 );
				bootDeviceList.append( bootDevice1 );
				bootDeviceList.append( bootDevice2 );

				pVmStOpts->setBootDeviceList( bootDeviceList );
			}
			CVmSharing* pVmSFOpts = pVmSet->getVmTools()->getVmSharing();
			if ( pVmSFOpts )
			{
				CVmHostSharing* pHost = pVmSFOpts->getHostSharing();
				if ( pHost )
				{
					pHost->setEnabled( false );
					pHost->setShareAllMacDisks( false );
					pHost->setShareUserHomeDir( false );
					pHost->setMapSharedFoldersOnLetters( false );
					CVmSharedFolder* pVmSF = new CVmSharedFolder();
					pVmSF->setName("NAME");
					pVmSF->setPath("PATH");
					pHost->addSharedFolder(pVmSF);
					pHost->addSharedFolder(pVmSF);
				}
				CVmGuestSharing* pGuest = pVmSFOpts->getGuestSharing();
				if ( pGuest )
				{
					pGuest->setEnabled( false );
					pGuest->setAutoMount( false );
					pGuest->setEnableSpotlight( false );
				}
			}
			CVmCoherence* pVmChrn = pVmSet->getVmTools()->getVmCoherence();
			if ( pVmChrn )
			{
				pVmChrn->setShowTaskBar( false );
				pVmChrn->setRelocateTaskBar( false );
				pVmChrn->setExcludeDock( false );
				pVmChrn->setMultiDisplay( false );
				pVmChrn->setGroupAllWindows( true );
				pVmChrn->setDisableDropShadow( true );
				pVmChrn->setDoNotMinimizeToDock( true );
				pVmChrn->setBringToFront( true );
			}
			CVmScheduleOptions* pVmShedOpts = pVmSet->getVmScheduleOptions();
			if (pVmShedOpts)
			{
				pVmShedOpts->setScheduleBasis((PVE::VmScheduleBasis)1);
				pVmShedOpts->setScheduleGranularity(1);
				pVmShedOpts->setDayOfWeek(1);
				pVmShedOpts->setDayOfMonth(1);
				pVmShedOpts->setDay(1);
				pVmShedOpts->setWeek(1);
				pVmShedOpts->setMonth(1);
				pVmShedOpts->setStartDate(QDate::currentDate());
				pVmShedOpts->setStartTime(QTime::currentTime());
				pVmShedOpts->setStopDate(QDate::currentDate().addDays(1));
				pVmShedOpts->setStopTime(QTime::currentTime().addSecs(3600));
			}
			CVmRemoteDisplay* pVmRD = pVmSet->getVmRemoteDisplay();
			if (pVmRD)
			{
				pVmRD->setMode( PRD_DISABLED );
				pVmRD->setHostName( "HOST_NAME" );
				pVmRD->setPassword( "PASSWORD" );
				pVmRD->setPortNumber( 5900 );
			}
			CVmSharedProfile* pVmSP = pVmSet->getVmTools()->getVmSharedProfile();
			if (pVmSP)
			{
				pVmSP->setEnabled( false );
				pVmSP->setUseDesktop(false);
				pVmSP->setUseDocuments(false);
				pVmSP->setUsePictures(false);
				pVmSP->setUseMusic(false);
			}
			CVmSharedApplications* pVmSA = pVmSet->getVmTools()->getVmSharedApplications();
			if (pVmSA)
			{
				pVmSA->setWinToMac(false);
				pVmSA->setMacToWin(false);
				pVmSA->setSmartSelect(false);
				pVmSA->setApplicationInDock( PDM_APP_IN_DOCK_COHERENCE_ONLY );
			}
			CVmTools* pVmTools = pVmSet->getVmTools();
			if (pVmTools)
			{
				pVmTools->getClipboardSync()->setEnabled(false);
				pVmTools->getMouseSync()->setEnabled(false);
				CVmToolsTimeSync* pVmToolsTS = pVmTools->getTimeSync();
				pVmToolsTS->setEnabled( false );
				pVmToolsTS->setKeepTimeDiff( false );
				pVmToolsTS->setSyncInterval( 100 );
			}
	}

	// CVaConfig* pVaCfg = pVmCfg->getVaConfig();

	QFile file(qsFileName);
	int iResult = pVmCfg->saveToFile(&file);
	delete pVmCfg;

	return iResult;
}

/**
 * Generates a XML file from a CDispatcherConfig class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenDispConfig(const QString& qsFileName)
{
	CDispatcherConfig* pDispCfg = new CDispatcherConfig();

	CVmServerIdentification* pDispId = pDispCfg->getVmServerIdentification();
	if (pDispId)
	{
		pDispId->setServerUuid("SERVER_UUID");
	}

	CDispLicense* pLic = pDispCfg->getLicense();
	if (pLic)
	{
		pLic->setKey("810000-002A3F-0600F0");
		pLic->setUser("Ivan Pronin");
		pLic->setCompany("Gazprom");
	}

	CDispatcherSettings* pDispSets = pDispCfg->getDispatcherSettings();
	if (pDispSets)
	{
		CDispCommonPreferences* pComPrefs = pDispSets->getCommonPreferences();
		if (pComPrefs)
		{
			CDispWorkspacePreferences* pWSPrefs = pComPrefs->getWorkspacePreferences();
			if (pWSPrefs)
			{
				pWSPrefs->setDefaultVmDirectory("DEFAULT_VM_DIRECTORY");
				pWSPrefs->setDistributedDirectory( true );
				pWSPrefs->setDefaultUseConsole( false );
				pWSPrefs->setDefaultChangeSettings( false );
			}
			CDispRemoteDisplayPreferences* pRDPrefs = pComPrefs->getRemoteDisplayPreferences();
			if (pRDPrefs)
			{
				pRDPrefs->setBaseHostName( "0.0.0.0" );
				pRDPrefs->setBasePort( 5900 );
			}
			CDispMemoryPreferences* pMemPrefs = pComPrefs->getMemoryPreferences();
			if (pMemPrefs)
			{
				pMemPrefs->setReservedMemoryLimit(1);
				pMemPrefs->setMinVmMemory(1);
				pMemPrefs->setMaxVmMemory(1);
				pMemPrefs->setRecommendedMaxVmMemory(1);
				pMemPrefs->setHostRamSize(1);
				pMemPrefs->setMinReservedMemoryLimit(1);
				pMemPrefs->setMaxReservedMemoryLimit(1);
			}
			CDispNetworkPreferences* pNetPrefs = pComPrefs->getNetworkPreferences();
			if (pNetPrefs)
			{
				CDispNetAdapter* pAdapter1 = new CDispNetAdapter();
				pAdapter1->setUuid( "UUID_0" );
				pAdapter1->setSysName( "en0" );
				pAdapter1->setName( "NetAdapter1" );
				pAdapter1->setNetworkType( PNA_SHARED );
				pAdapter1->setEnabled( true );
				pAdapter1->setNatEnabled( true );
				pAdapter1->setIndex( 0 );
				CDispDhcpPreferences* pDhcpPrefs1 = pAdapter1->getDhcpPreferences();
				if (pDhcpPrefs1)
				{
					pDhcpPrefs1->setEnabled( false );
					pDhcpPrefs1->setDhcpScopeStartIp(QHostAddress("127.0.0.0"));
					pDhcpPrefs1->setDhcpScopeEndIp(QHostAddress("127.0.0.0"));
					pDhcpPrefs1->setDhcpScopeMask(QHostAddress("127.0.0.0"));
				}
				CDispNetAdapter* pAdapter2 = new CDispNetAdapter();
				pAdapter2->setUuid( "UUID_1" );
				pAdapter2->setSysName( "en1" );
				pAdapter2->setName( "NetAdapter2" );
				pAdapter2->setNetworkType( PNA_HOST_ONLY );
				pAdapter2->setEnabled( false );
				pAdapter2->setNatEnabled( false );
				pAdapter1->setIndex( 1 );
				CDispDhcpPreferences* pDhcpPrefs2 = pAdapter2->getDhcpPreferences();
				if (pDhcpPrefs2)
				{
					pDhcpPrefs2->setEnabled( true );
					pDhcpPrefs2->setDhcpScopeStartIp(QHostAddress("127.0.0.1"));
					pDhcpPrefs2->setDhcpScopeEndIp(QHostAddress("127.0.0.1"));
					pDhcpPrefs2->setDhcpScopeMask(QHostAddress("127.0.0.1"));
				}
				pNetPrefs->addNetAdapter( pAdapter1 );
				pNetPrefs->addNetAdapter( pAdapter2 );
			}
		}
		CDispUsersPreferences* pUserPrefs = pDispSets->getUsersPreferences();
		if (pUserPrefs)
		{
			CDispUser* pDispUser = new CDispUser();
			pDispUser->setUserName("USER_NAME");
			pDispUser->setUserId("USER_ID");
			CDispUserWorkspace* pUserWS = pDispUser->getUserWorkspace();
			if ( pUserWS )
			{
				pUserWS->setVmDirectory( "VM_DIRECTORY_UUID" );
				pUserWS->setDefaultVmFolder( "DEFAULT_VM_FOLDER" );
				pUserWS->setUserHomeFolder( "USER_HOME_FOLDER" );
			}
		   CDispUserAccess* pAcs = pDispUser->getUserAccess();
		   if ( pAcs )
		   {
			   pAcs->setUseServerManagementConsoleFlag( true );
			   pAcs->setChangeServerSettingsFlag( true );
		   }
			pUserPrefs->addUser( pDispUser );
			pUserPrefs->addUser( pDispUser );
		}
	}

	int iResult = pDispCfg->saveToFile( qsFileName );
	delete pDispCfg;

	return iResult;
}

/**
 * Generates a XML file from a CClientPreferences class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenClientPreferences(const QString& qsFileName)
{
	CClientPreferences* pClientPrefs = new CClientPreferences();
   CPresentationPreferences* pPresent = pClientPrefs->getPresentationPreferences();
   if ( pPresent )
   {
	   pPresent->setFullScreenTransition( PVE::AnimationFlip );
	   pPresent->setOsWindowTransition( PVE::AnimationCube );
	   pPresent->setAnimationSpeed( 60 );
	   pPresent->setAnimateCoherence( true );
	   pPresent->setAnimateSuspendAndResume( true );
   }
   CMainWindowPreferences* pMWPrefs = pClientPrefs->getMainWindowPreferences();
   if ( pMWPrefs )
   {
	   pMWPrefs->setToolbarPlacement( PVE::ToolbarDockedRight );
	   pMWPrefs->setShowToolbar( false );
   }
   CSharedWebPreferences* pWeb = pClientPrefs->getSharedWebPreferences();
   if ( pWeb )
   {
	   pWeb->setWebAppMode( PVE::OpenByGuestOS );
	   pWeb->setMailAppMode( PVE::OpenByHostOS );
	   pWeb->setFtpAppMode( PVE::OpenByHostOS );
   }
   CKeyboardPreferences* pKeyboard = pClientPrefs->getKeyboardPreferences();
   if ( pKeyboard )
   {
	   pKeyboard->setMouseRightClickDelayed( false );
	   pKeyboard->setMouseRightClickDelayRate( 60 );
	   pKeyboard->setRemapOption( PVE::RemapSwapCmdCtrl );
	   pKeyboard->setNumEnterToAltGr( true );

	   CHotkeyCombination* pHKComb = pKeyboard->getHotkeySwitchFullscreen();
		if (pHKComb)
			pHKComb->setCustomKey(0);
		pHKComb = pKeyboard->getHotkeyReleaseInput();
		if (pHKComb)
			pHKComb->setCustomKey(1);
		pHKComb = pKeyboard->getHotkeyToggleCoherence();
		if (pHKComb)
			pHKComb->setCustomKey(2);
		pHKComb = pKeyboard->getHotkeyMouseRightClick();
		if (pHKComb)
			pHKComb->setCustomKey(3);
   }

   CUpdatePreferences* pUpd = pClientPrefs->getUpdatePreferences();
   if ( pUpd )
   {
		pUpd->setUpdatePeriod( 10 );
	}

	int iResult = pClientPrefs->saveToFile( qsFileName );
	delete pClientPrefs;

	return iResult;
}

/**
 * Generates a XML file from a CProblemReport class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenProblemReport(const QString& qsFileName)
{
	CProblemReport* pProbRep = new CProblemReport();

#ifdef ALL_FIXED
	CRepUserDefinedData* pUserDefData = pProbRep->getRepUserDefinedData();
	if (pUserDefData)
	{
		CRepScreenShot* pScreenShot = new CRepScreenShot("SHOT_NAME", "SHOT_DATA");
		pUserDefData->addScrShotData(pScreenShot);
		pUserDefData->addScrShotData(pScreenShot);
	}
#endif

	int iResult = pProbRep->saveToFile(qsFileName);
	delete pProbRep;

	return iResult;
}

/**
 * Generates a XML file from a CVmDirectories class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenVmDirs(const QString& qsFileName)
{
	CVmDirectory* pVmDir = new CVmDirectory();
	pVmDir->setUuid("VM_DIR_UUID");
	pVmDir->setDefaultVmFolder("DEFAULT_VM_FOLDER");
	pVmDir->setUserFriendlyName("USER_FRIENDLY_NAME");

	CVmDirectoryItem* pDirItem = new CVmDirectoryItem();
	pDirItem->setVmUuid("VM_UUID");
	pDirItem->setRegistered((PVE::VmRegisteredState)1);
	pDirItem->setValid((PVE::VmValidState)1);
	pDirItem->setVmHome("VM_HOME");
	pDirItem->setIsPrivate((PVE::VmPrivateFlag)1);
	pDirItem->setRegisteredBy("REGISTERED_BY");
	pDirItem->setRegDateTime(QDateTime::currentDateTime());
	pDirItem->setChangedBy("CHANGED_BY");
	pDirItem->setChangeDateTime(QDateTime::currentDateTime());

	pVmDir->addVmDirectoryItem(pDirItem);
	pVmDir->addVmDirectoryItem(pDirItem);

	CVmDirectories* pVmDirs = new CVmDirectories();
	pVmDirs->addVmDirectory(pVmDir);
	pVmDirs->addVmDirectory(pVmDir);

	CVmServerIdentification* pServId = pVmDirs->getVmServerIdentification();
	if (pServId)
	{
		pServId->setServerUuid("SERVER_UUID");
	}

	QFile file(qsFileName);
	int iResult = pVmDirs->saveToFile(&file);

	delete pVmDirs;

	return iResult;
}

/**
 * Generates a XML file from a CVmEvent class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenVmEvent(const QString& qsFileName)
{
	CVmEvent* pEvn = new CVmEvent();
	pEvn->setEventType((PRL_EVENT_TYPE)1);
	pEvn->setEventLevel((PVE::VmEventLevel)1);
	pEvn->setEventCode(PRL_ERR_UNEXPECTED);
	pEvn->setRespRequired((PVE::VmEventRespOption)1);
	pEvn->setEventSource("EVENT_SOURCE");
	pEvn->setEventIssuerType((PRL_EVENT_ISSUER_TYPE)1);
	pEvn->setEventIssuerId("EVENT_ISSUER_ID");
	pEvn->setInitRequestId("INIT_REQUEST_ID");

	CVmEventParameter* pParam = new CVmEventParameter();
	pParam->setParamType(PVE::CData);
//	pParam->setParamType((PVE::ParamFieldDataType)1);
	pParam->setParamName("PARAM_NAME");
	pParam->setParamValue("PARAM_VALUE");
	pParam->setCdata("CDATA");
	pEvn->addEventParameter(pParam);
	pEvn->addEventParameter(pParam);

	int iResult = 0;
	QString qsXml = pEvn->toString();

	QFile file(qsFileName);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(qsXml.toUtf8(), qsXml.size());
		file.close();
	}
	else
		qerr << "Failed to open file: " << qsFileName << "\n";

	delete pEvn;

	return iResult;
}

/**
 * Generates a XML file from a CHwFileSystemInfo class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenHostFileSystemInfo(const QString& qsFileName)
{
	CHwFileSystemInfo* pFsInfo = new CHwFileSystemInfo();
	CHwFsItem* pFsInfoItem1 = new CHwFsItem("FS_ITEM_1");
	CHwFsItem* pFsInfoItem2 = new CHwFsItem("FS_ITEM_2");

	pFsInfo->addFileSystemItem(pFsInfoItem1);
	pFsInfo->addFileSystemItem(pFsInfoItem2);

	int iResult = 0;
	QString qsXml = pFsInfo->toString();

	QFile file(qsFileName);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(qsXml.toUtf8(), qsXml.size());
		file.close();
	}
	else
		qerr << "Failed to open file: " << qsFileName << "\n";

	delete pFsInfo;

	return iResult;
}

/**
 * Generates a XML file from a CHostHardwareInfo class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenHostHwInfo(const QString& qsFileName)
{
	CHostHardwareInfo* pHostHwInfo = new CHostHardwareInfo();

	CHwOsVersion* pOsVersion = pHostHwInfo->getOsVersion();
	if ( pOsVersion )
	{
		pOsVersion->setOsType( PHO_LIN );
		pOsVersion->setMajor( 10 );
		pOsVersion->setMinor( 1 );
		pOsVersion->setSubMinor( 2 );
		pOsVersion->setStringPresentation( "Version 10.1.2" );
	}

	CHwCpu* pCpu = pHostHwInfo->getCpu();
	if ( pCpu )
	{
		pCpu->setNumber( 2 );
		pCpu->setMode( PCM_CPU_MODE_64 );
		pCpu->setModel( "QQQ Model" );
		pCpu->setSpeed( 3000 );
	}

	CHwMemorySettings* pMemSets = pHostHwInfo->getMemorySettings();
	if (pMemSets)
	{
		pMemSets->setReservedMemoryLimit(1);
		pMemSets->setMinVmMemory(1);
		pMemSets->setMaxVmMemory(1);
		pMemSets->setRecommendedMaxVmMemory(1);
		pMemSets->setHostRamSize(1);
		pMemSets->setMaxReservedMemoryLimit(1);
	}

	CHwGenericDevice* pFlop =
		new CHwGenericDevice(PDE_FLOPPY_DISK, "FLOPPY", "FLOP_UUID");
	pHostHwInfo->addFloppyDisk(pFlop);
	pHostHwInfo->addFloppyDisk(pFlop);

	CHwGenericDevice* pOptical =
		new CHwGenericDevice(PDE_OPTICAL_DISK, "OPTICAL","OPTICAL_DEV");
	pHostHwInfo->addOpticalDisk(pOptical);
	pHostHwInfo->addOpticalDisk(pOptical);

	CHwGenericDevice* pSerial =
		new CHwGenericDevice(PDE_SERIAL_PORT, "SERIAL", "SER_UUID");
	pHostHwInfo->addSerialPort(pSerial);
	pHostHwInfo->addSerialPort(pSerial);

	CHwGenericDevice* pParallel =
		new CHwGenericDevice(PDE_PARALLEL_PORT, "PARALLEL", "PRL_UUID");
	pHostHwInfo->addParallelPort(pParallel);
	pHostHwInfo->addParallelPort(pParallel);

	CHwNetAdapter* pNet =
		new CHwNetAdapter(PDE_GENERIC_NETWORK_ADAPTER, "NETWORK",
		"SYS_NAME",
		0,
		"00:1c:00:00:00:00",
		PRL_INVALID_VLAN_TAG,
		1,
		"NET_UUID");


	pHostHwInfo->addNetworkAdapter(pNet);
	pHostHwInfo->addNetworkAdapter(pNet);

	CHwGenericDevice* pSoundOut =
		new CHwGenericDevice(PDE_SOUND_DEVICE, "SOUND_OUT", "SND_OUT_UUID");
	pHostHwInfo->addSoundOutputDevice(pSoundOut);
	pHostHwInfo->addSoundOutputDevice(pSoundOut);

	CHwGenericDevice* pSoundMix =
		new CHwGenericDevice(PDE_MIXER_DEVICE, "SOUND_MIX", "SND_MIX_UUID");
	pHostHwInfo->addSoundMixerDevice(pSoundMix);
	pHostHwInfo->addSoundMixerDevice(pSoundMix);

#ifdef ALL_FIXED
	CHwGenericDevice* pUsbDev =
		new CHwGenericDevice(PDE_USB_DEVICE, "USB_DEVICE", "USB_DEV_UUID");
	pHostHwInfo->addUsbDevice(pUsbDev);
	pHostHwInfo->addUsbDevice(pUsbDev);
#endif

	CHwGenericDevice* pPrinter =
		new CHwGenericDevice(PDE_PRINTER, "PRINTER", "PRINTER_UUID");
	pHostHwInfo->addPrinter(pPrinter);
	pHostHwInfo->addPrinter(pPrinter);

#ifdef ALL_FIXED
	CHwGenericDevice* pPciDev =
		new CHwGenericDevice(PDE_GENERIC_PCI_DEVICE, "GENERIC_PCI_DEVICE", "GENERIC_PCI_DEVICE_UUID");
	pHostHwInfo->addGenericPciDevice(pPciDev);
	pHostHwInfo->addGenericPciDevice(pPciDev);
#endif

	CHwGenericDevice* pScsi =
		new CHwGenericDevice(PDE_GENERIC_SCSI_DEVICE, "GENERIC_SCSI_DEVICE", "GENERIC_SCSI_DEVICE_UUID");
	pHostHwInfo->addGenericScsiDevice(pScsi);
	pHostHwInfo->addGenericScsiDevice(pScsi);

	pHostHwInfo->setSoundDefaultEnabled(1);
	pHostHwInfo->setUsbSupported(1);

	CHwHardDisk* pHard = new CHwHardDisk("HD_NAME", 1000, PDE_HARD_DISK,"HD_DEV");

	CHwHddPartition* pHDPart = new CHwHddPartition("PART_NAME", 1000, 1, 1, 1, 1, 1);
	pHard->addPartition(pHDPart);
	pHard->addPartition(pHDPart);

	pHostHwInfo->addHardDisk(pHard);
	pHostHwInfo->addHardDisk(pHard);

	int iResult = 0;
	QString qsXml = pHostHwInfo->toString();

	QFile file(qsFileName);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(qsXml.toUtf8(), qsXml.size());
		file.close();
	}
	else
		qerr << "Failed to open file: " << qsFileName << "\n";

	delete pHostHwInfo;

	return iResult;
}

/**
 * Generates a XML file from a CUpdaterConfig class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenUpdaterConfig(const QString& qsFileName)
{
	CUpdaterConfig* pUpdConfig = new CUpdaterConfig();

	pUpdConfig->setUpdateUrl( "UPDATE_URL" );
	pUpdConfig->setRemindPeriod( 1000 );
	pUpdConfig->setBuyLink( "BUY_LINK" );
	pUpdConfig->setDownloadPath( "DOWNLOAD_PATH" );

	CAutomaticUpdate* pAutoUpd = pUpdConfig->getAutomaticUpdate();
	if ( pAutoUpd )
	{
		pAutoUpd->setUpdateEnabled( false );
		pAutoUpd->setUpdatePeriod( PVE::UpdatePeriod_Monthly );
		pAutoUpd->setNotifyUser( false );
		pAutoUpd->setDownloadEnabled( false );
		pAutoUpd->setInstallEnabled( false );
	}
	CUpdates* pUpds = pUpdConfig->getUpdates();
	if ( pUpds )
	{
		for ( int i = 1; i < 3; ++i )
 		{
			CUpdProduct* pProduct = new CUpdProduct();
			pProduct->setName( QString("PRODUCT_%1").arg(i) );
			for ( int j = 1; j < 3; ++j )
			{
				CUpdProductVersion* pVer = new CUpdProductVersion();
				pVer->setMajor( j );
				pVer->setMinor( j );
				pVer->setSubMinor( j );
				pVer->setSubSubMinor( j );
				pVer->setStringRepresentation( QString("Version %1.%2.%3.%4").arg(j).arg(j).arg(j).arg(j) );

				for ( int k = 1; k < 3; ++k )
				{
					CUpdate* pUpdate = new CUpdate();
					pUpdate->setUuid( QString("UUID_%1").arg(k) );
					pUpdate->setName( QString("UPDATE_%1").arg(k) );
					pUpdate->setLocaleName( QString("LOCALE_%1").arg(k) );
					pUpdate->setDistributorName( QString("DISTRIBUTOR_%1").arg(k) );
					pUpdate->addAncestor( "1.1.1.1.en.prl.mac" );
					pUpdate->addAncestor( "1.1.1.1.en.prl.win" );
					pUpdate->addAncestor( "1.1.1.1.en.prl.lin" );
					pVer->addUpdate( pUpdate );
				}

				pProduct->addVersion( pVer );
			}
			pUpds->addProduct( pProduct );
		}
	}

	int iResult = pUpdConfig->saveToFile( qsFileName );
	delete pUpdConfig;

	return iResult;
}

/**
 * Generates a XML file from a CUpdates class.
 * @param qsFileName A name of output XML file.
 * @return Error code.
 */
int GenUpdates(const QString& qsFileName)
{
	CUpdates* pUpds = new CUpdates();
	if ( pUpds )
	{
		for ( int i = 1; i < 3; ++i )
 		{
			CUpdProduct* pProduct = new CUpdProduct();
			pProduct->setName( QString("PRODUCT_%1").arg(i) );
			for ( int j = 1; j < 3; ++j )
			{
				CUpdProductVersion* pVer = new CUpdProductVersion();
				pVer->setMajor( j );
				pVer->setMinor( j );
				pVer->setSubMinor( j );
				pVer->setSubSubMinor( j );
				pVer->setStringRepresentation( QString("Version %1.%2.%3.%4").arg(j).arg(j).arg(j).arg(j) );

				for ( int k = 1; k < 3; ++k )
				{
					CUpdate* pUpdate = new CUpdate();
					pUpdate->setUuid( QString("UUID_%1").arg(k) );
					pUpdate->setName( QString("UPDATE_%1").arg(k) );
					pUpdate->setLocaleName( QString("LOCALE_%1").arg(k) );
					pUpdate->setDistributorName( QString("DISTRIBUTOR_%1").arg(k) );
					pUpdate->addAncestor( "1.1.1.1.en.prl.mac" );
					pUpdate->addAncestor( "1.1.1.1.en.prl.win" );
					pUpdate->addAncestor( "1.1.1.1.en.prl.lin" );
					pVer->addUpdate( pUpdate );
				}

				pProduct->addVersion( pVer );
			}
			pUpds->addProduct( pProduct );
		}
	}

	int iResult = pUpds->saveToFile( qsFileName );
	delete pUpds;

	return iResult;
}

/*
 * Generate XML for disk descriptor
 */
int GenDiskDesc(const QString& File)
{
	CDiskXML* Xml = new (std::nothrow) CDiskXML();

	if (!Xml)
		return -1;

	ParametersNode* Params = Xml->getParameters();

	// 1024/16/63 predefined
	Params->setSize(1032192);
	Params->setCylinders(1024);
	Params->setHeads(16);
	Params->setSectors(63);
	Params->setPadding(0);

	StorageNode* Stor = Xml->getStorageData()->m_lstStorage.at(0);

	ImageNode* Im = Stor->getImage();
	Im->setGUID("Something like the UID");
	Im->setType("The gorgeous parallels image");
	Im->setFile("/the/path/to/the/file.itself");

	Stor->setStart(0);
	Stor->setEnd(1032192);
	Stor->setBlockSize(512);

	ShotNode* Snap = Xml->getSnapshots()->m_lstShot.at(0);

	Snap->setGUID("My own UID");
	Snap->setParentGUID("My parent UID");

	// Add operation somehow.
	int iResult = Xml->saveToFile( File );
	delete Xml;

	return iResult;
}

/**
 * Main function.
 */
int main(int argc, const char **argv)
{
	if (argc < 2)
		usage();

	QString qsFileName;
	bool bVmConfig = false;
	bool bDispConfig = false;
	bool bProbRep = false;
	bool bVmDirs = false;
	bool bVmEvent = false;
	bool bHostHwInfo = false;
	bool bHostFsInfo = false;
	bool bClientPrefs = false;
	bool bUpdConfig = false;
	bool bUpdates = false;
	bool bDiskDesc = false;

 	for (int i = 1; i < argc; ++i)
	{
        QString arg = argv[i];
        if (arg == "-VmConfig")
            bVmConfig = true;
        else if (arg == "-DispConfig")
            bDispConfig = true;
        else if (arg == "-ProbRep")
            bProbRep = true;
        else if (arg == "-VmDirs")
            bVmDirs = true;
        else if (arg == "-VmEvent")
            bVmEvent = true;
        else if (arg == "-HostHwInfo")
            bHostHwInfo = true;
        else if (arg == "-HostFsInfo")
            bHostFsInfo = true;
        else if (arg == "-ClientPrefs")
            bClientPrefs = true;
        else if (arg == "-UpdConfig")
            bUpdConfig = true;
        else if (arg == "-Updates")
            bUpdates = true;
        else if (arg == "-DiskDescriptor")
            bDiskDesc = true;
        else if (qsFileName.isNull())
            qsFileName = arg;
        else
            usage();
    }

	if (qsFileName.isEmpty())
        usage();

    int iResult = 0;

	if (bVmConfig)
		iResult = GenVmConfig(qsFileName);
	else if (bDispConfig)
		iResult = GenDispConfig(qsFileName);
	else if (bProbRep)
		iResult = GenProblemReport(qsFileName);
	else if (bVmDirs)
		iResult = GenVmDirs(qsFileName);
	else if (bVmEvent)
		iResult = GenVmEvent(qsFileName);
	else if (bHostHwInfo)
		iResult = GenHostHwInfo(qsFileName);
	else if (bHostFsInfo)
		iResult = GenHostFileSystemInfo(qsFileName);
	else if (bClientPrefs)
		iResult = GenClientPreferences(qsFileName);
	else if (bUpdConfig)
		iResult = GenUpdaterConfig(qsFileName);
	else if (bUpdates)
		iResult = GenUpdates(qsFileName);
	else if (bDiskDesc)
		iResult = GenDiskDesc(qsFileName);
	else
		usage();

	return iResult;
};
