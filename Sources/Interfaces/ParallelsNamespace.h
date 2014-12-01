/*
 * ParallelsNamespace.h: Parallels Namespace definition. This file
 * contains various definitions and macro, which are used throughout
 * the project modules.
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


#ifndef __PARALLELS_NAMESPACE__
#define __PARALLELS_NAMESPACE__

#include "SDK/Include/PrlErrors.h"
#include "SDK/Include/PrlEnums.h"
#include "SDK/Include/PrlPorts.h"
#include "Interfaces/ParallelsDefines.h"
#include "Interfaces/ParallelsTypes.h"

//Mouse and keyboard profiles extension
#define PROFILE_EXTENSION				"kbd"

// Disk tool utility finish timeout to get info about disk
#define GET_DISK_INFO_TIMEWOUT_SEC	120	// 2 min

#define VMDIR_DEFAULT_VM_CONFIG_FILE			"config.pvs"
#define VMDIR_DEFAULT_VM_BACKUP_SUFFIX			".backup"
#define VMDIR_DEFAULT_VM_MIGRATE_SUFFIX			".migrate"
#define VMDIR_DEFAULT_BUNDLE_SUFFIX				".pvm"
#define VM_PROBLEM_REPORT_PREFIX				"PrlProblemReport"
#define VM_PROBLEM_REPORT_SUFFIX				"xml"
#define VM_PROBLEM_REPORT_SCREEN_PREFIX			"PrlProblemReportScreen"
#define VM_PROBLEM_REPORT_SCREEN_SUFFIX			"png"
#define VM_PROBLEM_REPORT_DIR_NAME				"Problem Reports"
#define VM_PROBLEM_REPORT_SCREEN_HOST_PREFIX	"PrlProblemReportHostScreen"

#define VM_GENERATED_WINDOWS_APPLICATION_DIR	"Windows Applications"
#define VM_GENERATED_WINDOWS_WIN_DISKS_DIR		"Windows Disks"
#define VM_GENERATED_WINDOWS_SNAPSHOTS_DIR		"Snapshots"
#define VM_GENERATED_SNAPSHOTS_CONFIG_FILE		"Snapshots.xml"
#define VM_GENERATED_GUEST_CRASH_DUMPS_DIR		"GuestDumps"

#define VM_INFO_FILE_NAME						"VmInfo.pvi"
#define VM_INFO_FILE_SUFFIX						".pvi"

#define PRL_SOUND_DEFAULT_DEVICE_NAME			"Default"
#define PRL_SOUND_NULL_DEVICE_NAME				"None"
#define PRL_PRINTER_DEFAULT_DEVICE_NAME			"Default printer"
#define PRL_PRINTER_DEFAULT_LOCATION			"Mac"
#define PRL_DVD_DEFAULT_DEVICE_NAME				"Default CD/DVD-ROM"

#define PRL_VM_NVRAM_FILE_NAME						"NVRAM.dat"
#define PRL_VM_SUSPENDED_SCREEN_FILE_NAME			"suspended.png"
#define PRL_HOST_OS_64_BIT							"64"
#define PRL_HOST_OS_32_BIT							"32"

#define PRL_STATISTICS_KEY_POSTFIX						" Statistics"
#define PRL_KEY_CEP_STATISTICS_APP_SWITCHER_PACKAGES	"Packages"
#define PRL_KEY_CEP_STATISTICS_LEARN_VIDEO_TIMES		"Learn Video Times"
#define PRL_VMTIMING_LOGFILENAME						"statistic.log"
#define PRL_COMMON_LOGFILENAME							"parallels.log"
#define PRL_RECONFIGURATION_LOGFILENAME					"reconfiguration.log"

#define UNATTENDED_ISO			"Unattended.iso"
#define UNATTENDED_FDD			"unattended.fdd"

/* migrate client-dispatcher protocol versions */
/* RC version */
#define MIGRATE_CLIENT_PROTO_V1	1
/* current migrate protocol version */
#define MIGRATE_CLIENT_PROTO_VERSION MIGRATE_CLIENT_PROTO_V1

/* migrate dispatcher-dispatcher protocol versions */
/* RC version */
#define MIGRATE_DISP_PROTO_V1	1
/* VmMigrateCheckPreconditionsReply message was added */
#define MIGRATE_DISP_PROTO_V2	2
/* hot migration was added */
#define MIGRATE_DISP_PROTO_V3	3
/* migration of external disks was added */
#define MIGRATE_DISP_PROTO_V4	4
/* current migrate protocol version */
#define MIGRATE_DISP_PROTO_VERSION MIGRATE_DISP_PROTO_V4

/* backup client-dispatcher and dispatcher-dispatcher protocol versions */
/* RC version */
#define BACKUP_PROTO_V1	1
/* PSMB-5 : container's support was added */
#define BACKUP_PROTO_V2	2
/* PVS-6 : ploop container's support was added */
#define BACKUP_PROTO_V3	3
/* current backup protocol version */
#define BACKUP_PROTO_VERSION BACKUP_PROTO_V3

/* copy of CT template client-dispatcher and dispatcher-dispatcher protocol versions */
/* PSBM-5 version */
#define COPY_CT_TMPL_PROTO_V1	1
/* current backup protocol version */
#define COPY_CT_TMPL_PROTO_VERSION COPY_CT_TMPL_PROTO_V1

#define PRL_BACKUP_METADATA				".metadata"
#define PRL_BACKUP_FULL_TYPE				"f"
#define PRL_BACKUP_INCREMENTAL_TYPE			"i"
#define PRL_BACKUP_DIFFERENTIAL_TYPE			"d"
#define PRL_DISP_IO_BUFFER_SIZE				16*1024*1024

/**
 * Parallels return codes
 */
#define PRL_VM_CANT_CONNECT_TO_DISPATHER_RETCODE -2


/**
* SnapshotTree screenshot sizes
*/
#define DEFAULT_SCREENSHOT_WIDTH_FOR_SNAPSHOT	306
#define DEFAULT_SCREENSHOT_HEIGHT_FOR_SNAPSHOT	224

/**
* User mobile icon
*/
#define DEFAULT_MOBILE_ICON_WIDTH	256
#define DEFAULT_MOBILE_ICON_HEIGHT	256

/**
 * Parallels environment variables
 */


#define PVS_VMCATALOGUE_DIR_ENV	"PARALLELS_VMDIR"
#define PVS_DISPATCHER_CONFIG_DIR_ENV	"PARALLELS_CONFIG_DIR"

#define PVS_VM_EXECUTABLE_ENV	"PARALLELS_VM"
#define PVS_VM_SCRIPTS_ENV	"PARALLELS_SCRIPTS"
#define PVS_VM_ENVIRONMENT_ENV	"PARALLELS_VM_ENV"
#define PVS_VM_ENVIRONMENT_ENV_SEPARATOR	";"
#define PVS_APPLE_HARDWARE_MODEL_ENV "PARALLELS_HW_MDL_ENV"

#ifdef _WIN_
	#define VM_AUTH_EXECUTABLE	"/prlauth.exe"
#else
	#define VM_AUTH_EXECUTABLE	"/prlauth"
#endif

#define PRL_DEFAULT_SYNC_VMS_UPTIME_TIMEOUT_IN_MINS	15	/* minutes */

#define DISK_DESCRIPTOR_XML	"DiskDescriptor.xml"

#define UNDO_DISKS_UUID		"{AABBCCDD-0204-9357-5189-EFB0C034F5EA}"

// Client auth. session uuid is {AABBCCDD-1753-3A6C-EB20-89CFF97A408D}
#define SENTILLION_CLIENT_AUTH_SESSION_UUID_BASE64	"e0FBQkJDQ0RELTE3NTMtM0E2Qy1FQjIwLTg5Q0ZGOTdBNDA4RH0="

#define PROMPT_UNATTENDED_INSTALLATION_SERIAL_KEY	"#####-#####-#####-#####-#####"

// To start VM required free hdd space =
//		size_to_start = guest_ram_mem + guest_video_mem + VM_SIZE_OF_RESERVED_HDD_FREE_SPACE_TO_VM_START
// Note:
//		When host has free space on hdd more than ( guest_ram_mem + guest_video_mem )
//		but less than (guest_ram_mem + guest_video_mem + VM_SIZE_OF_RESERVED_HDD_FREE_SPACE_TO_VM_START)
//		sometimes host unable to alloc memory ( guest_ram + guest_video )
//	by anyav@
#define SIZE_OF_RESERVED_HDD_FREE_SPACE_TO_VM_START ( 150<<20 ) /* 150 mb */

// The password length is the limitation of VNC 4 protocol,
// allowed to use up to 8 symbols by design.
#define PRL_VM_REMOTE_DISPLAY_MAX_PASS_LEN 8

//GUI aplication name related defines set
#define PRL_PRODUCT_NAME_CONSOLE        "Parallels Management Console"
#define PRL_PRODUCT_NAME_DESKTOP        "Parallels Desktop"
#define PRL_PRODUCT_NAME_PLAYER			"Parallels Desktop Express"
#define PRL_PRODUCT_NAME_DESKTOP_WL		"Parallels Desktop"
#define PRL_PRODUCT_NAME_WORKSTATION_EXTREME "Parallels Workstation"
#define PRL_PRODUCT_NAME_MOBILE         "Parallels Access"
#define PRL_PRODUCT_NAME_UNKNOWN		"Unknown product"

#define PRL_VENDOR						"Parallels Software"
#define PRL_VENDOR_URL					"parallels.com"

// app switcher application name
#define PRL_APP_SWITCHER_NAME		"Parallels Application Switcher"
#define PRL_APP_LEARN_VIDEO_NAME	"Learn Windows 7"

#define PRL_TAG_VIRTUAL_MACHINE		"Parallels virtual machine"

/**
 * Various enumerations
 */

class PVE
{
public:

	/**
	 * Product activation status
	 */
	enum ProductActivationState
	{
		LicStatusUnknown = 0,
		LicNotActivated = 1,
		LicActivatedTrial = 2,
		LicTrialExpired = 3,
		LicActivatedPermanent = 4
	};

	/**
	 * PC speaker state (enabled/disabled)
	 */
	enum PcSpeakerState
	{
		PcSpeakerDisabled = 0,
		PcSpeakerEnabled = 1
	};

	/**
	 * VTx support flag
	 */
	enum VTxSupportFlag
	{
		VTxSupportDisabled = 0,
		VTxSupportEnabled = 1
	};

	/**
	 * Shared folder mode
	 */
    enum SharedFolderMode
    {
        ReadOnlySharedFolder = 0,
        ReadWriteSharedFolder = 1
    };

	/**
	 * Shared folder state
	 */
    enum SharedFolderState
    {
        SharedFolderDisabled = 0,
        SharedFolderEnabled = 1
    };

	/**
	 * VM Installed Software
	 */
	enum VmInstalledSoftware
	{
		// NOTE: Values should be BITMASK !
		VmInstalledSoftware_PIS				= ( 1 << 0 ),
		VmInstalledSoftware_NativeLook		= ( 1 << 1 ),
		VmInstalledSoftware_NortonAV		= ( 1 << 2 ),
		VmInstalledSoftware_DrWebAV			= ( 1 << 3 ),
		VmInstalledSoftware_Windows7Look	= ( 1 << 4 )
	};

	/**
	 * VM switch to fullscreen mode
	 */
	enum VmFullscreenOption
	{
		AutoFullscreenDisabled = 0,
		AutoFullscreenEnabled = 1
	};

	/**
	 * VM power-on actions
	 */
	enum VmPowerOnActions
	{
		VmOnPowerJustStart = 0,
		VmOnPowerStartAndSaveState = 1,
		VmOnPowerRevertToLastSavedState = 2
	};

	/**
	 * VM power-off actions
	 */
	enum VmPowerOffActions
	{
		VmOnDownJustFinish = 0,
		VmOnDownSaveStateAndFinish = 1,
		VmOnDownDoRestart = 2
	};

	/**
	 * Close console on VM shutdown (enabled/disabled)
	 */
	enum VmConsoleShutdownOption
	{
		ConsoleCloseOnShutdownDisabled = 0,
		ConsoleCloseOnShutdownEnabled = 1
	};

	/**
	 * On application quit actions
	 */
	enum VmOnAppQuit
	{
		OnAppQuitSuspend = 0,
		OnAppQuitStop = 1,
		OnAppQuitAskMe = 2,
	};

	/**
	 * On stop button actions
	 */
	enum VmOnStop
	{
		OnStopSuspend = 0,
		OnStopStop = 1
	};

	/**
	 * On pause button actions
	 */
	enum VmOnPause
	{
		OnPauseSuspend = 0,
		OnPausePause = 1
	};

	/**
	 * Switch animation mode
	 */
	enum SwitchAnimationMode
	{
		AnimationDisabled = 0,
		AnimationFade = 1,
		AnimationZoom = 2,
		AnimationReveal = 3,
		AnimationSlide = 4,
		AnimationWarp = 5,
		AnimationCube = 6,
		AnimationSwitch = 7,
		AnimationFlip = 8
	};

	/**
	 * Toolbar placement
	 */
	enum ToolbarPlacement
	{
		ToolbarDockedTop = 0,
		ToolbarDockedLeft = 1,
		ToolbarDockedRight = 2
	};

	/**
	* Toolbar placement
	*/
	enum SideBarPlacement
	{
		SideBarLeft = 0,
		SideBarRight
	};

	/**
	 * Shared web application mode
	 */
	enum SharedWebMode
	{
		OpenByDefault = 0,
		OpenByGuestOS = 1,
		OpenByHostOS = 2
	};

	/**
	 * Close console on VM shutdown (enabled/disabled)
	 */
	enum HierarchyTreeLastNode
	{
		LastNodeFalse = 0,
		LastNodeTrue = 1
	};

	/**
	 * Schedule basis
	 */
	enum VmScheduleBasis
	{
		VmNoSchedule = 0,
		// daily basis
		VmScheduleEveryDay = 1,
		VmScheduleWeekdays = 2,
		VmScheduleEveryNumDay = 3,
		// weekly basis
		VmScheduleWeekly = 4,
		// monthly basis
		VmScheduleNumDayOfMonth = 5,
		VmScheduleNumWeekdayOfMonth = 6,
		// one time only
		VmScheduleOneTimeOnly = 7,
		// when user connected
		VmScheduleWhenUserConnected = 8
	};

	/**
	 * Enable write back disk cache option
	 */
	enum DiskCacheWritebackOption
	{
		DiskCacheWritebackDisabled = 0,
		DiskCacheWritebackEnabled = 1
	};

	/**
	 * Method of power consumption optimization
	 */
	enum OptimizePowerConsumptionMode
	{
		OptimizeBatteryLife = 0,
		OptimizePerformance = 1
	};

	/**
	 * Enable acceleration option
	 */
	enum VmAccelerationOption
	{
		VmAccelerationDisabled = 0,
		VmAccelerationEnabled = 1
	};

	/**
	 * Virtual Appliance category
	 */
	enum VaCategory
	{
		VaCategoryApplications = 2,
		VaCategoryCommunications = 3,
		VaCategoryDataStorage = 4,
		VaCategoryOperatingSystems = 5,
		VaCategoryEntertainment = 6,
		VaCategoryNetworking = 7,
		VaCategoryDatabase = 8,
		VaCategoryDevelopment = 9,
		VaCategoryOther = 10
	};

	/**
	 * Virtual Appliance development status
	 */
	enum VaDevelopmentStatus
	{
		VaDevelopmentStatusPreRelease = 2,
		VaDevelopmentStatusStableProduction = 3,
		VaDevelopmentStatusDevelopment = 4
	};

	/**
	 * Device enabled state
	 */
	enum DeviceEnabledState
	{
		DeviceDisabled = 0,
		DeviceEnabled = 1
	};

	/**
	 * Device connected state
	 */
	enum DeviceConnectedState
	{
		DeviceDisconnected = 0,
		DeviceConnected = 1,
		DeviceConnecting = 2,
	};

	/**
	 * Host printer parameter changed
	 */
	enum HwDeviceParamChangedFlags
	{
		// NOTE: Values should be BITMASK !
		DevParamChangeNone = 0, // nothing
		DevParamChangeUnknown = 1, // undefined change
		DevParamChangeFriendlyName = 2,
		DevParamChangeIsDefaultSign = 4,
	};
	/**
	 * Generic device emulated type
	 */
	enum GenericDeviceEmulatedType
	{
		RealGenericDevice = PDT_USE_REAL_DEVICE
	};

	/**
	 * Generic device type
	 */
	enum GenericDeviceInterfaceType
	{
		GenericDeviceScsi = 0,
		GenericDevicePci = 1
	};

	/**
	 * Floppy emulated type
	 */
	enum FloppyEmulatedType
	{
		RealFloppyDisk = PDT_USE_REAL_DEVICE,
		FloppyDiskImage = PDT_USE_IMAGE_FILE
	};

	/**
	 * CD/DVD-ROM emulated type
	 */
	enum CdromEmulatedType
	{
		RealCdRom = PDT_USE_REAL_DEVICE,
		CdRomImage = PDT_USE_IMAGE_FILE
	};

	/**
	 * CD/DVD-ROM passthrough mode
	 */
	enum DevicePassthroughMode
	{
		PassthroughDisabled = 0,
		PassthroughEnabled
	};

	/**
	 * Hard disk emulated type
	 */
	enum HardDiskEmulatedType
	{
		RealHardDisk = PDT_USE_REAL_DEVICE,
		HardDiskImage = PDT_USE_IMAGE_FILE,
		BootCampHardDisk = PDT_USE_OTHER,
		// Virtuozzo Container vzfs file system
		ContainerHardDisk = PDT_USE_FILE_SYSTEM,
	};

	/**
	* Hard disk emulated type
	*/
	enum HardDiskVersion
	{
		HardDiskInvalidVersion = 0,
		HardDiskV1,
		HardDiskV2,
		HardDiskV2Suspended,
		HardDiskV2Changed,
	};

	/**
	 * Serial port emulated type
	 */
	enum SerialPortEmulatedType
	{
		RealSerialPort = PDT_USE_REAL_DEVICE,
		SerialOutputFile = PDT_USE_OUTPUT_FILE,
		SerialSocket = PDT_USE_OTHER
	};

	/**
	 * Parallel port emulated type
	 */
	enum ParallelPortEmulatedType
	{
		RealParallelPort = PDT_USE_REAL_DEVICE,
		ParallelOutputFile = PDT_USE_OUTPUT_FILE,
		ParallelPrinter = PDT_USE_OTHER,
		ParallelPDF = PDT_USE_IMAGE_FILE
	};

	/**
	 * Network adapter type
	 */
	enum NetworkAdapterType
	{
		Rtl8029AS = 0
	};

	/**
	 * Sound device emulated type
	 */
	enum SoundDeviceEmulatedType
	{
		AC97 = 0,
		SB16 = 1
	};

	/**
	 * USB controller emulated type
	 */
	enum UsbDeviceEmulatedType
	{
		RealUsbDevice = PDT_USE_REAL_DEVICE,
		SoftUsbDevice = PDT_USE_OTHER,

		UsbPrinterShared	= PDT_USE_OTHER,
		UsbPrinterFile		= PDT_USE_OUTPUT_FILE,
		UsbPrinterPDF		= PDT_USE_IMAGE_FILE
	};

	/**
	 * VM security shared option
	 */
	enum VmSharedOption
	{
		VmSharedDisabled = 0,
		VmSharedEnabled = 1
	};

	/**
	 * VM registered state (VM Directory)
	 */
	enum VmRegisteredState
	{
		VmUnregistered = 0,
		VmRegistered = 1
	};

	/**
	 * VM validation state (VM Directory)
	 */
	enum VmValidState
	{
		VmNotValid = 0,
		VmValid = 1
	};

	/**
	 * VM private flag (VM Directory)
	 */
	enum VmPrivateFlag
	{
		VmPublic = 0,
		VmPrivate = 1
	};

	/**
	 * User connection SSL option (Dispatcher)
	 */
	enum DispUserSslOption
	{
		UseNoSSL = 0,
		UseSSL = 1
	};

	/**
	 * User connection compression level (Dispatcher)
	 */
	enum DispUserCompressionLevel
	{
		UseNoCompression = 0,
		UseCompression = 1
	};

	/**
	 * User hotkey action codes
	 */
	enum HotkeyActionCode
	{
		SwitchFullScreenMode = 0,
		ReleaseKeyboardInput = 1,
		ToggleCoherence = 2,
		MouseRightClick = 3,
		Modality = 4,
		Crystal = 5,
		ShowHideApplication = 6,
		GrabInput = 7,

	};

	/**
	 * Command keys remap options
	 */
	enum CommandKeysRemapOption
	{
		RemapDoNothing = 0,
		RemapCmdZXCVAF = 1,
		RemapSwapCmdCtrl = 2
	};

	/**
	 * Hotkey use ALT key option
	 */
	enum HotkeyUseAltOption
	{
		HotkeyUseAltDisabled = 0,
		HotkeyUseAltEnabled = 1
	};

	/**
	 * Hotkey use CTRL key option
	 */
	enum HotkeyUseCtrlOption
	{
		HotkeyUseCtrlDisabled = 0,
		HotkeyUseCtrlEnabled = 1
	};

	/**
	 * Hotkey use CMD key option
	 */
	enum HotkeyUseCmdOption
	{
		HotkeyUseCmdDisabled = 0,
		HotkeyUseCmdEnabled = 1
	};

	/**
	 * Hotkey use SHIFT key option
	 */
	enum HotkeyUseShiftOption
	{
		HotkeyUseShiftDisabled = 0,
		HotkeyUseShiftEnabled = 1
	};

	/**
	 * Mouse buttons option
	 */
	enum MouseButtonsOption
	{
		MouseNoButton = -1,
		MousePrimaryButton = 0,
		MouseSecondaryButton = 1,
		MouseMiddleButton = 2
	};

	/**
	 * Hotkey use custom key option
	 */
	enum HotkeyUseCustomOption
	{
		HotkeyUseCustomDisabled = 0,
		HotkeyUseCustomEnabled = 1
	};

	/**
	 * Union condition operator
	 */
	enum VmQueryUnionConditionOperator
	{
		And = 0,
		Or = 1
	};

	/**
	 * Parameter field types
	 */
	enum ParamFieldDataType
	{
		UnsignedInt = 0,
		String = 1,
		Integer = 2,
		CData = 3,
		Text = 4,
		MessageShowNextTime = 5,
		Boolean = 6,
		VmConfiguration = 7,
		VmLicense = 8,
		Uuid = 9,
		VmReturnCode = 10,
		VmRuntimeState = 11,
		VmSmartKernelState = 12,
		VmGuestToolActiveState = 13,
		UserResponseOnQuestion = 14,
		VmDeviceStateData = 15,
		Int64 = 16,
		UInt64 = 17,
		DateTime = 18,
		Date = 19,
		Time = 20,
	};

	/**
	 * List of available object types inside dispatcher
	 */
	enum DspObjectType
	{
		DspObjectWS = 0,
		DspObjectVmMgr = 1,
		DspObjectVmItm = 2,
		DspObjectIO = 3,
		DspObjectUSER = 4,
		DspObjectCTL = 5,
		DspObjectLOGIC = 6,
		DspObjectFake = 7,
	};

	/**
	 * Condition operator
	 */
	enum VmQueryConditionOperator
	{
		Equal = 0,
		LessThan = 1,
		GreaterThan = 2,
		Contains = 3,
		In = 4,
		IsNull = 5
	};

	/**
	 * Condition NOT flag
	 */
	enum VmQueryConditionNotFlag
	{
		NotConditionFalse = 0,
		NotConditionTrue = 1
	};

	/**
	 * VM event types
	 */
	enum VmEventRespOption
	{
		EventRespNotRequired = 0,
		EventRespRequired = 1
	};

	/**
	 * VM event level
	 */
	enum VmEventLevel
	{
		EventLevel0 = 0,
		EventLevel1 = 1,
		EventLevel2 = 2,
		EventLevel3 = 3,
		EventLevel4 = 4
	};

	/**
	 * Result type
	 */
	enum ResultType
	{
		GenericResult = 0,
		ServerGeneratedEvent = 1
	};

	/**
	 * VM event type
	 *
	 * ATTENTION! using value from this enum in CVmEvent->m_uiEventType type
	 * makes an impact oh how event code is interpreted. See important notes below
	 * about how to use this enum.
	 */
	enum VmEventType
	{
		//
		// Enum items defined below are used for notification events, such as
		// information messages, warnings, questions and error messages.
		// Values from this group are to be set in CVmEvent->m_uiEventType field.
		// When one of these values is used there, use PRL_EVENT_TYPE enum to
		// identify specific value of CVmEvent->m_ulEventCode;
		//
		EventTypeError = 0,
		EventTypeComplexError = 1,
		EventTypeNotification = 2,
		EventTypeWarning = 3,
		EventTypeQuestion = 4,
		//
		// Enum items defined below are used for commands exchange, such as
		// request for command to VM, response from VM on command, previously issued
		// to VM. Values from this group are to be set in CVmEvent->m_uiEventType field.
		// When one of these values is used there, use PVE::IDispatcherCommands enum to
		// identify specific value of CVmEvent->m_ulEventCode;
		//
		EventTypeRequest = 5,
		EventTypeResponse = 6,
		EventTypeAnswer = 7,
	};

	/**
	 * VM event class types
	 */
	enum VmEventClassType
	{
		CResultType = 1100,
		CVmEventType,
		CWebServiceEventType
	};


	/**
	 * Symbolic names for all parameters
	 * to be used in setValue/getValue functions
	 */
	enum VmParameters
	{
		IdInvalid = 0,
		IdVmUuid = 1,
		IdInternalId = 2,
		IdVmName = 3,
		IdVmIcon = 4,
		IdVmSecurityShared = 5,
		IdVmHome = 6,

		IdVmRuntimeStatus = 7,
		IdVmRuntimeWorkload = 8,
		IdVmRuntimeConnections = 9,

		IdHwDeviceType = 10,
		IdHwDeviceAutostart = 11,
		IdHwDeviceStatus = 12,
		IdHwDeviceImagePath = 13,
		IdHwDeviceAlternativePath = 14,
		IdHwDeviceEmulationType = 15,
		IdHwDeviceInterface = 16,
		IdHwDeviceStackIndex = 17,
		IdHwDeviceStorageIndex = 18,

		IdUserId = 19,
		IdUserName = 20,
		IdUserHome = 21,
		IdUserCatalog = 22,

		IdUserAccessSMC = 23,

		IdSessionSsl = 24,
		IdSessionCompression = 25,

		IdAnimationFullScreen = 26,
		IdAnimationOsWindow = 27,
		IdAnimationSpeed = 28,
		IdAnimationCoherence = 29,
		IdAnimationSuspendAndResume = 30,

		IdMainWindowToolbarPlacement = 31,
		IdMainWindowShowToolbar = 32,

		IdSharedWebWebAppMode = 33,
		IdSharedWebMailAppMode = 34,

		IdAppearanceAppInDock = 35,
		IdAppearanceScreenShotInDock = 36,
		IdAppearanceOsResolutionInFullScreen = 37,
		IdAppearanceHiddenMessages = 38,
		IdAppearanceHiddenMessageId = 39,

		IdHwCpuQuantity = 40,
		IdHwRamSize = 41,
		IdHwVideoSize = 42,
		IdHwFloppy = 43,
		IdHwHardDisk = 44,
		IdHwNetworkAdapter = 45,
		IdHwCdDvd = 46,
		IdHwSound = 47,
		IdHwSerialPort = 48,
		IdHwParallelPort = 49,
		IdHwUsb = 50,

		IdCfgLastModified = 51,
		IdCfgLastModifiedBy = 52,
		IdCfgGuestOsType = 53,
		IdCfgGuestOsVersion = 54,
		IdCfgDescription = 55,
		IdCfgAccelerationLevel = 56,
		IdCfgBootSequence = 57,
		IdCfgCloseDesktopAuto = 58,
		IdCfgFullScreenAuto = 59,
		IdCfgSharedFolder = 60,
		IdCfgSchedule = 61,
		IdCfgSavedState = 62,
		IdCfgPowerOn = 63,
		IdCfgPowerOff = 64,
		IdCfgVmAutostart = 65,
		IdCfgWindowMode = 66,

		IdCfgOnAppQuit = 67,
		IdCfgOnStop = 68,
		IdCfgOnPause = 69,

		IdCfgShowTaskBar = 70,
		IdCfgRelocateTaskBar = 71,
		IdCfgExcludeDock = 72,
		IdCfgMultiDisplay = 73,

		IdCfgScreenResolution = 74,
		IdCfgScreenResolutionWidth = 75,
		IdCfgScreenResolutionHeight = 76,
		IdCfgScreenResolutionEnabled = 77,

		IdFgPriority = 78,
		IdBgPriority = 79,

		IdPcSpeaker = 80,

		IdHddType = 81,
		IdHddSize = 82,
		IdHddSizeOnDisk = 83,

		IdHddCylinders = 84,
		IdHddHeads = 85,
		IdHddSectors = 86,
		IdHddDiskCache = 87,
		IdHddPasstrough = 88,
		IdHddSplitted = 89,
		IdHddPersistent = 90,

		IdCdPasstrough = 91,

		IdNetworkType = 92,
		IdNetworkMAC = 93,

		IdChangeServerSettings = 94,
		IdSharedWebFtpAppMode = 95,

		IdUseHomeDirectory = 96,
		IdDispNetworkAdapterName = 97,

		IdVmCpuNumber = 98,
		IdVmCpuMode = 99,

		IdHwDevicePasstrough = 100,

		IdLicenseKey = 101,
		IdLicenseUser = 102,
		IdLicenseCompany = 103,
	};

	/**
	 * Symbolic name for virtual machine status
	 */
	enum VmStatus
	{
		VmStateUnknown = 0,
		VmIncorrect = 1,
		VmStartInProgress = 2,
		VmRunning = 3,
		VmStopped = 4,
		VmPaused = 5,
		VmSuspended = 6,
		VmLocked = 7
	};
	typedef VmStatus VmState;


	/**
	 * Symbolic name for possible snapshoted virtual machine status
	 */
	enum SnapshotedVmStatus
	{
		SnapshotedVmPoweredOff = 0,
		SnapshotedVmRunning = 1,
		SnapshotedVmPaused = 2,
		SnapshotedVmSuspended = 3
	};
	typedef SnapshotedVmStatus SnapshotedVmState;


	/**
	 * Runtime status
	 */
	enum RuntimeStatus
	{
		VmGuestToolsActivated = 0,
		VmGuestToolsNotActivated = 1,
		VmSmartKernelOff = 2,
		VmSmartKernelMode1 = 3,
		VmSmartKernelMode2 = 4,
		VmSmartKernelVtxMode = 5
	};

	/**
	 * Operation codes
	 */
	enum OperationCode
	{
		Nop = 0,
		PveLogin = 1
	};

	/**
	 * Button codes on question
	 */
	enum VmQuestionCode
	{
		VmDummyRetCode = 0,
		VmAnswerNo = 1,
		VmAnswerYes = 2,
		VmAnswerOk = 3,
		VmAnswerCancel = 4,
		VmAnswerAbort = 5,
		VmAnswerRetry = 6,
		VmAnswerIgnore = 7
	};
	/**
	 * Interactive user states
	 */
	enum DispUserState
	{
		UserDisconnected = 0,			// user currently disconnected
		UserConnectedNotLoggedOn = 1,	// user connected but not yet authenticated
		UserConnectedLoggedOn = 2,		// user connected and authenticated
		UserAuthFailed = 3				// user authentication failed
	};

	/**
	 * Update type
	 */
	enum UpdateType
	{
		UpdateType_Cumulative = 0,
		UpdateType_Incremental = 1,
		UpdateType_ForSmartInstaller = 2
	};

	/**
	 * Update period
	 */
	enum UpdatePeriod
	{
		UpdatePeriod_Never = 0,
		UpdatePeriod_Daily = 1,
		UpdatePeriod_Weekly = 2,
		UpdatePeriod_Monthly = 3
	};

	/**
	 * Update status
	 */
	enum UpdateStatus
	{
		UpdateStatus_ReadyToUpdate = 0,
		UpdateStatus_Installed = 1,
		UpdateStatus_Downloaded = 2,
		UpdateStatus_CanceledInstall = 3,
		UpdateStatus_CanceledDownload = 4,
	};

	/**
	 * Global dispatcher commands enum
	 *
	 * Every function, which receive commands, must check its number
	 * For example, it may look like this:
	 *
	 *   if (!(cmd = castDspCommand(networkReceivedBuffer, networkBufferSize)))
	 *      invalid command
	 *   else
	 *      valid command
	 */
	enum IDispatcherCommands
	{
		DspIllegalCommand = 0,

		// CLIENT -> VM
		DspClientToVmCommandRangeStart = 1000,

			DspCmdVmStart							= DspClientToVmCommandRangeStart + 1,
			DspCmdVmStop							= DspClientToVmCommandRangeStart + 2,
			DspCmdVmSetConfig						= DspClientToVmCommandRangeStart + 3,
			DspCmdVmGetConfig						= DspClientToVmCommandRangeStart + 4,
			DspCmdVmGetProblemReport				= DspClientToVmCommandRangeStart + 5,
			DspCmdVmReset							= DspClientToVmCommandRangeStart + 6,
			DspCmdVmPause							= DspClientToVmCommandRangeStart + 7,
			DspCmdVmSuspend							= DspClientToVmCommandRangeStart + 8,
			DspCmdVmResume							= DspClientToVmCommandRangeStart + 9,
			DspCmdVmDropSuspendedState				= DspClientToVmCommandRangeStart + 10,
			DspCmdVmCreateSnapshot					= DspClientToVmCommandRangeStart + 11,
			DspCmdVmSwitchToSnapshot				= DspClientToVmCommandRangeStart + 12,
			DspCmdVmGetState						= DspClientToVmCommandRangeStart + 13,
			DspCmdVmDevConnect						= DspClientToVmCommandRangeStart + 14,
			DspCmdVmDevGetState						= DspClientToVmCommandRangeStart + 15,
			DspCmdVmDevDisconnect					= DspClientToVmCommandRangeStart + 16,
			DspCmdVmDevChangeMedia					= DspClientToVmCommandRangeStart + 17,
			DspCmdVmTaskRun							= DspClientToVmCommandRangeStart + 18,
			DspCmdVmTaskGetState					= DspClientToVmCommandRangeStart + 19,
			DspCmdVmTaskCancel						= DspClientToVmCommandRangeStart + 20,
			DspCmdVmAnswer							= DspClientToVmCommandRangeStart + 21,
			DspCmdVmGetLogData						= DspClientToVmCommandRangeStart + 22,
			DspCmdVmGetMonitorState					= DspClientToVmCommandRangeStart + 23,
			DspCmdVmInitiateDevStateNotifications	= DspClientToVmCommandRangeStart + 24,
			DspCmdVmInstallTools					= DspClientToVmCommandRangeStart + 25,
			DspCmdVmCancelCompact					= DspClientToVmCommandRangeStart + 26,
			DspCmdVmGuestLogout						= DspClientToVmCommandRangeStart + 27,
			DspCmdVmGuestRunProgram					= DspClientToVmCommandRangeStart + 28,
			DspCmdVmGetSuspendedScreen				= DspClientToVmCommandRangeStart + 29,
			DspCmdVmMigrateCancel					= DspClientToVmCommandRangeStart + 30,
			DspCmdVmRunCompressor					= DspClientToVmCommandRangeStart + 31,
			DspCmdVmCancelCompressor				= DspClientToVmCommandRangeStart + 32,
			DspCmdVmFinishCompressorInternal		= DspClientToVmCommandRangeStart + 33,	// Not using
			DspCmdVmDeleteSnapshot					= DspClientToVmCommandRangeStart + 34,
			DspCmdVmGetSnapshotsTree				= DspClientToVmCommandRangeStart + 35,
			DspCmdVmStartEx							= DspClientToVmCommandRangeStart + 36,
			DspCmdVmLoginInGuest					= DspClientToVmCommandRangeStart + 37,
			DspCmdVmUpdateSnapshotData				= DspClientToVmCommandRangeStart + 38,
			DspCmdVmRestartGuest					= DspClientToVmCommandRangeStart + 39,
			DspCmdVmInstallUtility					= DspClientToVmCommandRangeStart + 40,
			DspCmdVmUpdateToolsSection				= DspClientToVmCommandRangeStart + 41,
			DspCmdVmStartVNCServer					= DspClientToVmCommandRangeStart + 42,
			DspCmdVmStopVNCServer					= DspClientToVmCommandRangeStart + 43,
			DspCmdVmGuestGetNetworkSettings			= DspClientToVmCommandRangeStart + 44,
			DspCmdVmAuthWithGuestSecurityDb			= DspClientToVmCommandRangeStart + 45,
			DspCmdVmGuestSetUserPasswd				= DspClientToVmCommandRangeStart + 46,
			DspCmdVmCreateDiskSnapshot				= DspClientToVmCommandRangeStart + 47, // Not using
			DspCmdVmGuestChangeSID					= DspClientToVmCommandRangeStart + 48,
			DspCmdVmCompact							= DspClientToVmCommandRangeStart + 49,
			DspCmdVmSuspendCancel					= DspClientToVmCommandRangeStart + 50,
			DspCmdVmChangeLogLevel					= DspClientToVmCommandRangeStart + 51,
			DspCmdVmGuestSuspendHardDisk			= DspClientToVmCommandRangeStart + 52,
			DspCmdVmGuestResumeHardDisk				= DspClientToVmCommandRangeStart + 53,
			DspCmdVmGetPackedProblemReport			= DspClientToVmCommandRangeStart + 54,
			DspCmdVmChangeSid						= DspClientToVmCommandRangeStart + 55,
			DspCmdVmResetUptime						= DspClientToVmCommandRangeStart + 56,
			DspCmdVmInternal						= DspClientToVmCommandRangeStart + 57,
			DspCmdVmConvertDisks					= DspClientToVmCommandRangeStart + 58,
			DspCmdVmMigrateFinish					= DspClientToVmCommandRangeStart + 59,


   		    DspClientToVmCommandRangeEnd = 1999,

		    // CLIENT -> DISPATCHER
			DspClientToDispatcherCommandRangeStart = 2000,


			DspCmdDirVmCreate						= DspClientToDispatcherCommandRangeStart + 23,
			DspCmdDirVmDelete						= DspClientToDispatcherCommandRangeStart + 24,
			DspCmdDirVmClone						= DspClientToDispatcherCommandRangeStart + 25,
			DspCmdDirVmEditBegin					= DspClientToDispatcherCommandRangeStart + 26,
			DspCmdDirVmEditCommit					= DspClientToDispatcherCommandRangeStart + 27,
			DspCmdDirVmMigrate						= DspClientToDispatcherCommandRangeStart + 28,
			DspCmdDirGetVmList						= DspClientToDispatcherCommandRangeStart + 29,
			DspCmdDirGetVmConfig					= DspClientToDispatcherCommandRangeStart + 30,
			DspCmdDirSetVmConfig					= DspClientToDispatcherCommandRangeStart + 31,
			DspCmdDirCreateImage					= DspClientToDispatcherCommandRangeStart + 32,
			DspCmdDirRegVm							= DspClientToDispatcherCommandRangeStart + 33,
			DspCmdDirUnregVm						= DspClientToDispatcherCommandRangeStart + 34,
			DspCmdDirLockVm							= DspClientToDispatcherCommandRangeStart + 35,
			DspCmdDirUnlockVm						= DspClientToDispatcherCommandRangeStart + 36,
			DspCmdDirVerifyVmConfig					= DspClientToDispatcherCommandRangeStart + 37,
			DspCmdStartSearchConfig					= DspClientToDispatcherCommandRangeStart + 38,
			DspCmdUserLogin							= DspClientToDispatcherCommandRangeStart + 39,
			DspCmdUserLoginLocal					= DspClientToDispatcherCommandRangeStart + 40,
			DspCmdUserLoginLocalStage2				= DspClientToDispatcherCommandRangeStart + 41,
			DspCmdUserLogoff						= DspClientToDispatcherCommandRangeStart + 42,
			DspCmdUserAttachEventNotification		= DspClientToDispatcherCommandRangeStart + 43,
			DspCmdUserGetEvent						= DspClientToDispatcherCommandRangeStart + 44,
			DspCmdUserGetProfile					= DspClientToDispatcherCommandRangeStart + 45,
			DspCmdUserProfileBeginEdit				= DspClientToDispatcherCommandRangeStart + 46,
			DspCmdUserProfileCommit					= DspClientToDispatcherCommandRangeStart + 47,
			DspCmdGetHostCommonInfo					= DspClientToDispatcherCommandRangeStart + 48,
			DspCmdHostCommonInfoBeginEdit			= DspClientToDispatcherCommandRangeStart + 49,
			DspCmdHostCommonInfoCommit				= DspClientToDispatcherCommandRangeStart + 50,
			DspCmdUserGetHostHwInfo					= DspClientToDispatcherCommandRangeStart + 51,
			DspCmdUserPing							= DspClientToDispatcherCommandRangeStart + 52,
			DspCmdUserCancelOperation				= DspClientToDispatcherCommandRangeStart + 53,
			DspCmdFsGetDiskList						= DspClientToDispatcherCommandRangeStart + 54,
			DspCmdFsGetCurrentDirectory				= DspClientToDispatcherCommandRangeStart + 55,
			DspCmdFsGetDirectoryEntries				= DspClientToDispatcherCommandRangeStart + 56,
			DspCmdFsGetFileList						= DspClientToDispatcherCommandRangeStart + 57,
			DspCmdFsCreateDirectory					= DspClientToDispatcherCommandRangeStart + 58,
			DspCmdFsRenameEntry						= DspClientToDispatcherCommandRangeStart + 59,
			DspCmdFsRemoveEntry						= DspClientToDispatcherCommandRangeStart + 60,
			DspCmdDirInstallGuestOS					= DspClientToDispatcherCommandRangeStart + 61,
			DspCmdGetVmInfo							= DspClientToDispatcherCommandRangeStart + 62,

			DspCmdSMCGetDispatcherRTInfo			= DspClientToDispatcherCommandRangeStart + 63,
			DspCmdSMCGetCommandHistoryByVm			= DspClientToDispatcherCommandRangeStart + 64, // obsolete
			DspCmdSMCGetCommandHistoryByUser		= DspClientToDispatcherCommandRangeStart + 65, // obsolete
			DspCmdSMCShutdownDispatcher				= DspClientToDispatcherCommandRangeStart + 66,
			DspCmdSMCRestartDispatcher				= DspClientToDispatcherCommandRangeStart + 68,
			DspCmdSMCDisconnectUser					= DspClientToDispatcherCommandRangeStart + 69,
			DspCmdSMCDisconnectAllUsers				= DspClientToDispatcherCommandRangeStart + 70,
			DspCmdSMCCancelUserCommand				= DspClientToDispatcherCommandRangeStart + 71,
			DspCmdSMCShutdownVm						= DspClientToDispatcherCommandRangeStart + 72,
			DspCmdSMCRestartVm						= DspClientToDispatcherCommandRangeStart + 73,
			DspCmdSMCShutdownDispatcherInternal		= DspClientToDispatcherCommandRangeStart + 74,

			#define DSP_CMD_IS_SMC_COMMAND( c )	( \
				( (c) == PVE::DspCmdSMCGetDispatcherRTInfo			) || \
				( (c) == PVE::DspCmdSMCGetCommandHistoryByVm		) || \
				( (c) == PVE::DspCmdSMCGetCommandHistoryByUser		) || \
				( (c) == PVE::DspCmdSMCShutdownDispatcher			) || \
				( (c) == PVE::DspCmdSMCRestartDispatcher			) || \
				( (c) == PVE::DspCmdSMCDisconnectUser				) || \
				( (c) == PVE::DspCmdSMCDisconnectAllUsers			) || \
				( (c) == PVE::DspCmdSMCCancelUserCommand			) || \
				( (c) == PVE::DspCmdSMCShutdownVm					) || \
				( (c) == PVE::DspCmdSMCRestartVm					) || \
				( (c) == PVE::DspCmdSMCShutdownDispatcherInternal	) || \
				0 \
			)
			DspCmdNetPrlNetworkServiceStart			= DspClientToDispatcherCommandRangeStart + 75,
			DspCmdNetPrlNetworkServiceStop			= DspClientToDispatcherCommandRangeStart + 76,
			DspCmdNetPrlNetworkServiceRestart		= DspClientToDispatcherCommandRangeStart + 77,
			DspCmdGetHostStatistics							= DspClientToDispatcherCommandRangeStart + 78,
			DspCmdVmGetStatistics								= DspClientToDispatcherCommandRangeStart + 79,
			DspCmdUserUpdateLicense							= DspClientToDispatcherCommandRangeStart + 80,
			DspCmdUserGetLicenseInfo						= DspClientToDispatcherCommandRangeStart + 81,
			DspCmdVmCreateUnattendedFloppy			= DspClientToDispatcherCommandRangeStart + 82,
			DspCmdFsCanCreateFile							= DspClientToDispatcherCommandRangeStart + 83,
			DspCmdSubscribeToHostStatistics				= DspClientToDispatcherCommandRangeStart + 84,
			DspCmdUnsubscribeFromHostStatistics			= DspClientToDispatcherCommandRangeStart + 85,
			DspCmdVmSubscribeToGuestStatistics				= DspClientToDispatcherCommandRangeStart + 86,
			DspCmdVmUnsubscribeFromGuestStatistics			= DspClientToDispatcherCommandRangeStart + 87,
			DspCmdConvertOldHdd								= DspClientToDispatcherCommandRangeStart + 88,
			DspCmdUpdateDeviceInfo							= DspClientToDispatcherCommandRangeStart + 89,
			DspCmdAttachToLostTask							= DspClientToDispatcherCommandRangeStart + 90,

			DspCmdGetNetServiceStatus							= DspClientToDispatcherCommandRangeStart + 91,
			DspCmdNetPrlNetworkServiceRestoreDefaults	= DspClientToDispatcherCommandRangeStart + 92,
			DspCmdAddNetAdapter		= DspClientToDispatcherCommandRangeStart + 93,
			DspCmdDeleteNetAdapter	= DspClientToDispatcherCommandRangeStart + 94,
			DspCmdUpdateNetAdapter	= DspClientToDispatcherCommandRangeStart + 95,

			DspCmdFileTransferLogin			= DspClientToDispatcherCommandRangeStart + 96,
			DspCmdFileTransferLoginResponse	= DspClientToDispatcherCommandRangeStart + 97,
			DspCmdFileTransferUpload		= DspClientToDispatcherCommandRangeStart + 98,
			DspCmdFileTransferUploadResponse	= DspClientToDispatcherCommandRangeStart + 99,
			DspCmdGetVmToolsInfo			= DspClientToDispatcherCommandRangeStart + 100,

			DspCmdFsGenerateEntryName					= DspClientToDispatcherCommandRangeStart + 101,
			DspCmdLookupParallelsServers					= DspClientToDispatcherCommandRangeStart + 102,
			DspCmdVmUpdateSecurity							= DspClientToDispatcherCommandRangeStart + 103,
			DspCmdVmSectionValidateConfig							= DspClientToDispatcherCommandRangeStart + 104,
			DspCmdUserInfoList			= DspClientToDispatcherCommandRangeStart + 105,
			DspCmdUserInfo				= DspClientToDispatcherCommandRangeStart + 106,

            DspCmdPerfomanceStatistics  = DspClientToDispatcherCommandRangeStart + 107,
			DspCmdAllHostUsers			= DspClientToDispatcherCommandRangeStart + 108,
			DspCmdDirRestoreVm			= DspClientToDispatcherCommandRangeStart + 109,
			DspCmdGetVirtualNetworkList		= DspClientToDispatcherCommandRangeStart + 110,
			DspCmdAddVirtualNetwork			= DspClientToDispatcherCommandRangeStart + 111,
			DspCmdUpdateVirtualNetwork		= DspClientToDispatcherCommandRangeStart + 112,
			DspCmdDeleteVirtualNetwork			= DspClientToDispatcherCommandRangeStart + 113,
			DspCmdConfigureGenericPci			= DspClientToDispatcherCommandRangeStart + 114,
			DspCmdPrepareForHibernate				= DspClientToVmCommandRangeStart + 115,
			DspCmdAfterHostResume				= DspClientToVmCommandRangeStart + 116,
			DspCmdGetBackupTree			= DspClientToDispatcherCommandRangeStart + 117,
			DspCmdCreateVmBackup			= DspClientToDispatcherCommandRangeStart + 118,
			DspCmdRestoreVmBackup			= DspClientToDispatcherCommandRangeStart + 119,
			DspCmdRemoveVmBackup			= DspClientToDispatcherCommandRangeStart + 120,
			DspCmdSendProblemReport					= DspClientToDispatcherCommandRangeStart + 122,
			DspCmdVmLock					= DspClientToDispatcherCommandRangeStart + 123,
			DspCmdVmUnlock					= DspClientToDispatcherCommandRangeStart + 124,
			DspCmdSetNonInteractiveSession	= DspClientToDispatcherCommandRangeStart + 125,
			DspCmdCreateUnattendedCd			= DspClientToDispatcherCommandRangeStart + 126,
			DspCmdCtlCreateVmBackup			= DspClientToDispatcherCommandRangeStart + 127,
			DspCmdVmResizeDisk				= DspClientToDispatcherCommandRangeStart + 128,
			DspCmdSetSessionConfirmationMode= DspClientToDispatcherCommandRangeStart + 129,
			DspCmdStorageSetValue			= DspClientToDispatcherCommandRangeStart + 130,
			DspCmdVmStorageSetValue			= DspClientToDispatcherCommandRangeStart + 131,
			DspCmdSendClientStatistics		= DspClientToDispatcherCommandRangeStart + 132,
			DspCmdUpdateUsbAssocList		= DspClientToDispatcherCommandRangeStart + 133,
			DspCmdDirReg3rdPartyVm			= DspClientToDispatcherCommandRangeStart + 134,
			DspCmdInstallAppliance			= DspClientToDispatcherCommandRangeStart + 135,
			DspCmdUpdateOfflineService		= DspClientToDispatcherCommandRangeStart + 136,
			DspCmdDeleteOfflineService		= DspClientToDispatcherCommandRangeStart + 137,
			DspCmdGetOfflineServicesList		= DspClientToDispatcherCommandRangeStart + 138,
			DspCmdStartClusterService		= DspClientToDispatcherCommandRangeStart + 139,
			DspCmdStopClusterService		= DspClientToDispatcherCommandRangeStart + 140,
			DspCmdGetVmVirtDevInfo			= DspClientToDispatcherCommandRangeStart + 141,
			DspCmdVmDevHdCheckPassword		= DspClientToDispatcherCommandRangeStart + 142,
			DspCmdVmAuthorise				= DspClientToDispatcherCommandRangeStart + 143,
			DspCmdVmChangePassword			= DspClientToDispatcherCommandRangeStart + 144,
			DspCmdVmEncrypt					= DspClientToDispatcherCommandRangeStart + 145,
			DspCmdVmDecrypt					= DspClientToDispatcherCommandRangeStart + 146,
			DspCmdUpdateNetworkClassesConfig                = DspClientToDispatcherCommandRangeStart + 147,
			DspCmdGetNetworkClassesConfig           = DspClientToDispatcherCommandRangeStart + 148,
			DspCmdUpdateNetworkShapingConfig         = DspClientToDispatcherCommandRangeStart + 149,
			DspCmdGetNetworkShapingConfig           = DspClientToDispatcherCommandRangeStart + 150,
			DspCmdRestartNetworkShaping		= DspClientToDispatcherCommandRangeStart + 151,
			DspCmdRegisterIscsiStorage		= DspClientToDispatcherCommandRangeStart + 152, // Not using
			DspCmdUnregisterIscsiStorage		= DspClientToDispatcherCommandRangeStart + 153, // Not using
			DspCmdExtendIscsiStorage		= DspClientToDispatcherCommandRangeStart + 154, // Not using
			DspCmdGetCtTemplateList			= DspClientToDispatcherCommandRangeStart + 155,
			DspCmdGetDefaultVmConfig		= DspClientToDispatcherCommandRangeStart + 156,


			DspCmdRemoveCtTemplate		= DspClientToDispatcherCommandRangeStart + 157,
			DspCmdCopyCtTemplate		= DspClientToDispatcherCommandRangeStart + 158,
			DspCmdCreateVmBackupLocal	= DspClientToDispatcherCommandRangeStart + 159,
			DspCmdAddIPPrivateNetwork		= DspClientToDispatcherCommandRangeStart + 160,
			DspCmdRemoveIPPrivateNetwork		= DspClientToDispatcherCommandRangeStart + 161,
			DspCmdUpdateIPPrivateNetwork		= DspClientToDispatcherCommandRangeStart + 162,
			DspCmdGetIPPrivateNetworksList		= DspClientToDispatcherCommandRangeStart + 163,
			DspCmdDirCopyImage					= DspClientToDispatcherCommandRangeStart + 164,
			DspCmdRefreshPlugins				= DspClientToDispatcherCommandRangeStart + 165,
			DspCmdVmMount				= DspClientToDispatcherCommandRangeStart + 166,
			DspCmdVmUmount				= DspClientToDispatcherCommandRangeStart + 167,
			DspCmdVmMountInfo			= DspClientToDispatcherCommandRangeStart + 168,
			DspCmdGetPluginsList			= DspClientToDispatcherCommandRangeStart + 169,
			DspCmdGetDiskFreeSpace		= DspClientToDispatcherCommandRangeStart + 170,
			DspCmdDirVmMove			= DspClientToDispatcherCommandRangeStart + 171,
			DspCmdDirVmMigrateClone		= DspClientToDispatcherCommandRangeStart + 172,
			DspCmdGetVmConfigById		= DspClientToDispatcherCommandRangeStart + 173,
			DspCmdDirVmCloneLinked		= DspClientToDispatcherCommandRangeStart + 174,
			DspCmdUpdateSessionInfo		= DspClientToDispatcherCommandRangeStart + 175,

			DspCmdVmSetProtection		= DspClientToDispatcherCommandRangeStart + 176,
			DspCmdVmRemoveProtection	= DspClientToDispatcherCommandRangeStart + 177,

			DspCmdGetCPUPoolsList		= DspClientToDispatcherCommandRangeStart + 178,
			DspCmdMoveToCPUPool		= DspClientToDispatcherCommandRangeStart + 179,
			DspCmdRecalculateCPUPool	= DspClientToDispatcherCommandRangeStart + 180,
			DspClientToDispatcherCommandRangeEnd = 2999,


		DspVmToClientCommandRangeStart = 3000,

			// DISPATCHER -> VM
			DspVmRequest							= DspVmToClientCommandRangeStart + 1,

			// VM -> DISPATCHER (in response to the DspVmRequest)
			DspVmResponse							= DspVmToClientCommandRangeStart + 2,

			// VM -> DISPATCHER
			DspVmEvent								= DspVmToClientCommandRangeStart + 3,
			DspVmEventStartVNCServer								= DspVmToClientCommandRangeStart + 4,
			DspVmEventStopVNCServer								= DspVmToClientCommandRangeStart + 5,
			DspVmBinaryEvent							= DspVmToClientCommandRangeStart + 8,
			DspVmBinaryResponse								= DspVmToClientCommandRangeStart + 9,


			// DISPATCHER -> VM (in response to the DspVmEvent that requested DspVmAnswer)
			DspVmAnswer								= DspVmToClientCommandRangeStart + 10,

			DspVmGetHostHwInfo						= DspVmToClientCommandRangeStart + 11,
			DspVmRetHostHwInfo						= DspVmToClientCommandRangeStart + 12,
			DspVmSendEchoEvent							= DspVmToClientCommandRangeStart + 13,

			// DISPATCHER -> VM (notification)
			DspEvtNotifyVm							= DspVmToClientCommandRangeStart + 14,

			// DISPATCHER -> VM (host hardware configuration changed)
			DspEvtHwChanged							= DspVmToClientCommandRangeStart + 20,
			DspVmAuth								= DspVmToClientCommandRangeStart + 21,

			DspVmGetHardDiskStates					= DspVmToClientCommandRangeStart + 22,	// unused
			DspVmRetHardDiskStates					= DspVmToClientCommandRangeStart + 23,	// unused

			// VM -> DISPATCHER
			DspVmDevConnect							= DspVmToClientCommandRangeStart + 24,
			DspVmDevDisconnect						= DspVmToClientCommandRangeStart + 25,
			DspEvtNetworkPrefChanged					= DspVmToClientCommandRangeStart + 26,
			DspVmRestoreState						= DspVmToClientCommandRangeStart + 27,
			DspVmStateInfo							= DspVmToClientCommandRangeStart + 28,

		DspVmToClientCommandRangeEnd = 3999,


		// DISPATCHER <-> DISPATCHER (internal commands)
		DspCtlCommandRangeStart = 4000,

			DspCmdCtlGetStatus						= DspCtlCommandRangeStart + 1,
			DspCmdCtlProcessDeferredTaskFinish		= DspCtlCommandRangeStart + 2,
			DspCmdCtlDeferredDeleteDspObject		= DspCtlCommandRangeStart + 3,
			DspCmdCtlApplyVmConfig					= DspCtlCommandRangeStart + 4,
			DspCmdCtlVmEditWithRename				= DspCtlCommandRangeStart + 5,
			DspCmdCtlVmEditWithHardwareChanged		= DspCtlCommandRangeStart + 6,
			DspCmdCtlVmEditBootcampReconfigure		= DspCtlCommandRangeStart + 7,
			DspCmdCtlVmEditAdapterIPChanged			= DspCtlCommandRangeStart + 8,
			DspCmdCtlVmStandByGuest					= DspCtlCommandRangeStart + 9,
			DspCmdCtlVmEditFirewall					= DspCtlCommandRangeStart + 10,
			DspCmdCtlUpdateShadowVm					= DspCtlCommandRangeStart + 11,
			DspCmdCtlLicenseChange					= DspCtlCommandRangeStart + 12,
			DspCmdCtlMigrateTarget				= DspCtlCommandRangeStart + 13,
			DspCmdCtlStartMigratedVm				= DspCtlCommandRangeStart + 14,
			DspCmdCtlVmCommitDiskUnfinished			= DspCtlCommandRangeStart + 15,
			DspCmdCtlVmCollectGuestUsage			= DspCtlCommandRangeStart + 16,

			DspCmdCtlDispatherFakeCommand			= DspCtlCommandRangeStart + 4998,
		DspCtlCommandRangeEnd = 4999,


		// DISPATCHER -> CLIENT
		DspWsCommandRangeStart = 5000,

			DspWsResponse							= DspWsCommandRangeStart + 1,
			DspReplyWsStatus						= DspWsCommandRangeStart + 2,
			DspWsBinaryResponse						= DspWsCommandRangeStart + 3,

		DspWsCommandRangeEnd = 5999,

		// DISPATCHER -> DISPATCHER (migration)
		DispToDispRangeStart = 6000,
			// All events are in apropriate DispToDisp header
		DispToDispRangeEnd = 7000
	};

	/**
	 * Webservice methods response args enum
	 *
	 * Every Webservice method returns response structure. For example,
	 * method "DspCmdUserLogin" returns _DspCmdUserLoginResponse structure.
	 * Here we must enumerate all fields of each response structure.
	 *
	 * If you need to add/modify/remove any response structure, please ask Ilya@
	 */
	enum IDspMethodsArgs
	{
		// DspCmdUserLogin_strAccessToken = 0,

		DspCmdUserGetHostHwInfo_strHostHwInfo = 1,
		DspCmdFsGetDiskList_strDiskList = 2,
		DspCmdFsGetCurrentDirectory_strDir = 3,
		DspCmdFsGetDirectoryEntries_strEntriesList = 4,
		DspCmdFsGetFileList_strFileList = 5,
		DspCmdFsCreateDirectory_strDirEntries = 6,
		DspCmdFsRenameEntry_strDirEntries = 7,
		DspCmdFsRemoveEntry_strDirEntries = 8,
		DspCmdUserGetProfile_strUserProfile = 9,
		DspCmdGetHostCommonInfo_strCommonInfo = 10, // don't forget of DspCmdGetHostCommonInfo_strNetworkInfo
		DspCmdVmGetProblemReport_strReport = 11,
		DspCmdDirValidateVmConfig_strEvent = 12,
		DspCmdVmGetConfig_strVmConfig = 13,
		DspCmdGetVmInfo_strContainer = 14,
		DspCmdSMCGetDispatcherRTInfo_strEventContainer = 15,
		DspCmdSMCGetCommandHistoryByVm_strContainer = 16,
		DspCmdSMCGetCommandHistoryByUser_strContainer = 17,
		DspCmdGetHostStatistics_strSystemStatistics = 18,
		DspCmdVmGetStatistics_strSystemStatistics = 19,
		DspCmdDirRegVm_strVmConfig = 20,
		DspCmdUserGetLicenseInfo_strLicenseInfo = 21,
		DspCmdUpdateDeviceInfo_strDeviceInfo = 22,
		DspCmdUserLogin_strLoginInfo	= 23,
		DspCmdUserLoginLocalStage2_strLoginInfo = 24,
		DspCmdGetNetServiceStatus_strNetServiceStatus = 25,
		DspCmdGetVmToolsInfo_strContainer = 26,
		DspCmdVmUpdateSecurity_strSecurityInfo = 27,
		DspCmdUserInfo_strUserInfo = 28,
		DspCmdGetHostCommonInfo_strNetworkInfo = 29,
		DspCmdPerfomanceStatistics_strPerfStats = 30,
		DspCmdVmGetSnapshotsTree_strSnapshotsTree = 31,
		DspCmdGetBackupTree_strBackupsTree = 32,
		DspCmdGetVmVirtDevInfo_strContainer = 33,
        DspCmdSendProblemReport_strReport = 34,

		// This element must be last!!!
		MultipleArgStart = 10000
	};

	// Defined in Libraries/PrlCommonUtilsBase/ParallelsNamespace.cpp
	// Please modify this function when DispatcherCommands are changed.
	static const char* DispatcherCommandToString( unsigned int cmd );

	/**
	* Vm Access Control Entries
	*/
	enum VmAccessControlEntries
	{
		VmAceStart						= 0x00000001,
		VmAceStop						= 0x00000002,
		VmAcePause						= 0x00000004,
		VmAceModify						= 0x00000008,
		VmAceDelete						= 0x00000010,
		VmAceRegister					= 0x00000020,
		VmAceUnregister				= 0x00000040,
		VmAceClone						= 0x00000080,
	};

	/**
	* Virtual machine compressor states enumeration
	*/
	enum VmCompressorState
	{
		PCP_UNKNOWN		= 0,

		PCP_STOPPED		= 0x1,
		PCP_RUNNING		= 0x2,
	};

	enum ConfirmationModeValue
	{
		CMV_DEFAULT = 0,
		CMV_DISABLED = 1,
		CMV_ENABLED = 2,
	};

	/* Execution mode of binaries */
	enum VmBinaryMode {
		VBMODE_DEFAULT	= 0,
		VBMODE_32BIT		=	1,
		VBMODE_64BIT		= 2,
	};

	enum LicenseActivationStatus
	{
		// INTERNAL STATES
		LAS_COMMON_SERVER_ERROR			= -4, // when server returns code unsupported in this version
		LAS_WRONG_CONFIRMATION_ID		= -3,
		LAS_UNKNOWN									= -2,
		LAS_SUCCESS_OFFLINE					= -1,
		// SERVER STATES
		LAS_SUCCESS									= 0,
		LAS_SERVER_ERROR						= 1,
		LAS_ACT_CODE_INVALID				= 2,
		LAS_LIC_KEY_INVALID					= 3,
		LAS_LIC_KEY_BLACKLISTED			= 4,
		LAS_MAX_HWIDS_AMOUNT_REACHED= 5,
		LAS_POSA_NOT_ACTIVATED			= 6,
		LAS_OLD_HWID_NOT_FOUND			= 7,

		LAS_LAST = LAS_OLD_HWID_NOT_FOUND // SHOULD BE EQUAL TO LAST ELEMENT

	};
#define IS_LAS_SUCCEEDED( s ) ( PVE::LAS_SUCCESS_OFFLINE == s || s == PVE::LAS_SUCCESS )
#define LAS_IN_SERVER_RANGE( s ) (  s >= PVE::LAS_SUCCESS &&  s <= PVE::LAS_LAST )
};



#undef VALUE
#undef SCOPE
#undef VM_GUEST_OS_TYPE_END
#undef VM_GUEST_OS_VERSION_END

#define PARALLELS_SERVER_BROADCAST_PORT 64000
#define PRL_GET_SERVER_INFO_BROADCAST_SERVER_MSG "GET_SERVER_INFO"

#define USB_LIST_DIVIDER "---sdjhfgsjhdfgs"

/**
 * constant for not assigned private network ID
 */
#define PRL_PRIVNET_GENERATE_ID 0xffff

#define NET_IFACE_NAME_FORMAT	"vme%08x.%d"

#define COMMON_NOT_INITIALIZED_INT_VALUE (-12345678)

#endif // __PARALLELS_NAMESPACE__
