/*
 * ParallelsDomModel.h: Parallels XML data model. This file contains
 * various definitions and macroses, which are used for maintaining
 * of the VM XML data model.
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


#ifndef PARALLELSDOMMODEL_H
#define PARALLELSDOMMODEL_H

/**
 * XML parsing return codes
 */

/**
 * VM Configuration XML parser return codes
 */
class ConfigurationParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		BadIdentification = 5,
		BadSettings = 6,
		BadSecurity = 7,
		BadVmUuid = 8,
		BadVmName = 9,
		BadServerUuid = 10,
		BadServerHost = 11,
		BadVmSharedFlag = 12,
		BadGeneralOptions = 13,
		BadVmFlags = 14,
		BadHardware = 15,
		BadOvReservedMemoryLimit = 16,
		BadOvMaxReservedMemoryLimit = 17,
		BadOvMinVmMemory = 18,
		BadOvMaxVmMemory = 19,
		BadOvRecommendedMaxVmMemory = 20,
		BadOvHostRamSize = 21,
		BadMemoryBlockSize = 22,
		BadEnableWriteDiskCache = 23,
		BadEnableAcceleration = 24,
		BadVTxSupportOption = 25,
		BadCommonOptions = 26,
		BadGuestOsType = 27,
		BadGuestOsNumber = 28,
		BadRuntimeOptions = 29,
		BadAccelerationLevel = 30,
		BadPcSpeakerEnabled = 31,
		BadForegroundPriority = 32,
		BadBackgroundPriority = 33,
		BadStartupOptions = 34,
		BadAutostartFlag = 35,
		BadWindowMode = 36,
		BadActions = 37,
		BadOnAppQuit = 38,
		BadOnStop = 39,
		BadOnPause = 40,
		BadAutofullscreenFlag = 41,
		BadPowerOnActions = 42,
		BadBootDevicesList = 43,
		BadBootDevType = 44,
		BadBootDevNumber = 45,
		BadBootDevInUse = 46,
		BadBootSequence = 47,
		BadFinalizationOptions = 48,
		BadCloseConsoleOnShutdownFlag = 49,
		BadPowerOffActions = 50,
		BadSharedFoldersOptions = 51,
		BadSharedFolder = 52,
		BadSharedFolderName = 53,
		BadSharedFolderPath = 54,
        BadSharedFolderState = 55,
        BadSharedFolderMode = 56,
		BadScheduleOptions = 57,
		BadScheduleBasis = 58,
		BadMemory = 59,
		BadRamSize = 60,
		BadVideoSize = 61,
		BadVmCpuNumber = 62,
		BadFloppyDisk = 63,
		BadOpticalDisk = 64,
		BadHardDisk = 65,
		BadSerialPort = 66,
		BadParallelPort = 67,
		BadNetworkAdapter = 68,
		BadSoundDevice = 69,
		BadUsbDevice = 70,
		BadFloppyIndex = 71,
		BadFloppyEnabled = 72,
		BadFloppyConnected = 73,
		BadFloppyEmulatedType = 74,
		BadFloppyImagePath = 75,
		BadOpticalIndex = 76,
		BadOpticalEnabled = 77,
		BadOpticalConnected = 78,
		BadOpticalEmulatedType = 79,
		BadOpticalImagePath = 80,
		BadOpticalIfaceType = 81,
		BadOpticalStackIndex = 82,
		BadOpticalPassthrough = 83,
		BadHardDiskIndex = 84,
		BadHardDiskEnabled = 85,
		BadHardDiskConnected = 86,
		BadHardDiskEmulatedType = 87,
		BadHardDiskImagePath = 88,
		BadHardDiskIfaceType = 89,
		BadHardDiskStackIndex = 90,
		BadHardDiskPassthrough = 91,
		BadHardDiskSplitted = 92,
		BadHardDiskPersistent = 93,
		BadHardDiskType = 94,
		BadHardDiskSize = 95,
		BadHardDiskSizeOnDisk = 96,
		BadHardDiskCylinders = 97,
		BadHardDiskHeads = 98,
		BadHardDiskSectors = 99,
		BadSerialIndex = 100,
		BadSerialEnabled = 101,
		BadSerialConnected = 102,
		BadSerialEmulatedType = 103,
		BadSerialOutput = 104,
		BadSerialSocketMode = 105,
		BadParallelIndex = 106,
		BadParallelEnabled = 107,
		BadParallelConnected = 108,
		BadParallelEmulatedType = 109,
		BadParallelOutput = 110,
		BadNetworkAdapterIndex = 111,
		BadNetworkAdapterName = 112,
		BadNetworkAdapterEnabled = 113,
		BadNetworkAdapterConnected = 114,
		BadNetworkAdapterEmulatedType = 115,
		BadNetworkAdapterBoundIndex = 116,
		BadNetworkAdapterType = 117,
		BadNetworkAdapterPort = 118,
		BadNetworkAdapterIrq = 119,
		BadNetworkAdapterMacAddress = 120,
		BadSoundEnabled = 121,
		BadSoundConnected = 122,
		BadSoundEmulatedType = 123,
		BadSoundOutput = 124,
		BadSoundMixer = 125,
		BadUsbDeviceEnabled = 126,
		BadUsbDeviceConnected = 127,
		BadUsbDeviceEmulatedType = 128,
		BadUsbDeviceAutoconnect = 129,
		BadElementType = 130,
		BadCoherence = 131,
		BadShowTaskBar = 132,
		BadRelocateTaskBar = 133,
		BadExcludeDock = 134,
		BadMultiDisplay = 135,
		BadScreenResolutions = 136,
		BadScreenResolution = 137,
		BadScreenResolutionWidth = 138,
		BadScreenResolutionHeight = 139,
		BadSharing = 140,
		BadGuestSharing = 141,
		BadGuestSharingEnabled = 142,
		BadGuestSharingAutoMount = 143,
		BadHostSharing = 144,
		BadHostSharingEnabled = 145,
		BadHostSharingLocal = 146,
		BadHostSharingGlobal = 147,
		BadVmCpu = 148,
		BadVmCpuMode = 149,
		BadGenericDevice = 150,
		BadGenericDeviceIndex = 151,
		BadGenericDeviceEnabled = 152,
		BadGenericDeviceConnected = 153,
		BadGenericDeviceEmulatedType = 154,
		BadGenericDevicePath = 155,
		BadGenericDeviceName = 156,
		BadGenericDeviceIfaceType = 157,
		BadGenericDeviceStackIndex = 158,
		BadUndoDisksMode = 159,
		BadUndoDisksModeEnabled = 160,
		BadRemoteDisplay = 161,
		BadRemoteDisplayEnabled = 162,
		BadRemoteDisplayHostName = 163,
		BadRemoteDisplayPassword = 164,
		BadRemoteDisplayPortNumber = 165,
		BadDevSystemName = 166,
		BadDevUserFriendlyName = 167,
		BadDevRemote = 168,
		BadIdeRelative = 169,
		BadFddRelative = 170,
		BadSerialRelative = 171,
		BadParallelRelative = 172,
		BadConfigEditorPreferences = 173,
		NoAppVersion = 174,
		BadSharedProfile = 175,
		BadSharedApplications = 176,
		BadGuestSharingEnableSpotlight = 177,
		BadVideo = 178,
		BadEnableDirectXShaders = 179,
		BadTools = 180,
		BadEnableClipboardSync = 181,
		BadEnableMouseSync = 182,
		BadToolsTimeSync = 183,
		BadToolsTimeSyncEnabled = 184,
		BadToolsTimeSyncKeepTimeDiff = 185,
		BadToolsTimeSyncSyncInterval = 186,
		BadHddVersion = 187,
		BadUsbConnectReason =188,

		BadSecurityAcl = 189
	};
};

/**
 * VM Directory XML parser return codes
 */
class VmDirectoryParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		BadServerIdentification = 5,
		BadVmCatalogue = 6,
		BadServerUuid = 7,
		BadVmDirectoryItem = 8,
		BadVmUuid = 9,
		BadIsRegisteredFlag = 10,
		BadIsValidFlag = 11,
		BadVmHome = 12,
		BadIsPrivateFlag = 13,
		BadRegisteredBy = 14,
		BadRegistrationDateTime = 15,
		BadChangedBy = 16,
		BadChangedDateTime = 17,
		BadSavedStatesOptions = 18,
		BadSavedStatesStoreDirectory = 19,
		BadSavedStatesCollection = 20,
		BadSavedState = 21,
		BadSavedStateSerial = 22,
		BadSavedStateTimeStamp = 23,
		BadSavedStateFile = 24,
		BadSavedStateName = 25,
		BadSavedStateLastNodeFlag = 26,
		BadSavedStateParent = 27,
		BadVmDirectories = 28,
		BadVmDirectory = 29,
		BadVmDirectoryUuid = 30,
	};
};

/**
 * Dispatcher Configuration XML parser return codes
 */
class DispConfigParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		BadServerIdentification = 5,
		BadServerSettings = 6,
		BadCommonPreferences = 7,
		BadUsersPreferences = 8,
		BadWorkspacePreferences = 9,
		BadMemoryPreferences = 10,
		BadNetworkPreferences = 11,
		BadDefaultVmDir = 12,
		BadUserVmDir = 13,
		BadVmCatalogueFile = 14,
		BadUserCatalogueFile = 15,
		BadDispatcherPort = 16,
		BadAdjustMemAuto = 19,
		BadReservedMemoryLimit = 20,
		BadMaxReservedMemoryLimit = 21,
		BadMinVmMemory = 22,
		BadMaxVmMemory = 23,
		BadRecommendedMaxVmMemory = 24,
		BadHostRamSize = 25,
		BadDhcpScopeStartAddress = 26,
		BadDhcpScopeEndAddress = 27,
		BadDhcpScopeMask = 28,
		BadDispUserItem = 29,
		BadUserName = 30,
		BadUserId = 31,
		BadUserConnectionProperties = 32,
		BadUserWorkspace = 33,
		BadUserPersonalVmGroups = 34,
		BadUserPersonalVmQueries = 35,
		BadUseSslFlag = 36,
		BadCompressionLevel = 37,
		BadVmGroupItem = 38,
		BadVmQueryItem = 39,
		BadVmGroupId = 40,
		BadVmGroupName = 41,
		BadVmGroupParentId = 42,
		BadVmGroupLastNodeFlag = 43,
		BadVmGroupMemberId = 44,
		BadQuerySearchFieldName = 45,
		BadQuerySearchFieldType = 46,
		BadQueryOperator = 47,
		BadQueryConditionNotFlag = 48,
		BadQueryComparedValue = 49,
		BadQueryPredicate = 50,
		BadQuerySubgroup = 51,
		BadQuerySearchCriteria = 52,
		BadQueryUnionCondition = 53,
		BadQueryName = 54,
		BadQueryId = 55,
		BadUserAccess = 56,
		BadAllowUseSMC = 57,
		BadDefaultCommandHistorySize = 58,
		BadDistributedDirectory = 59,
		BadDefaultUseConsole = 60,
		BadDefaultChangeSettings = 61,
		BadChangeServerSettings = 62,
		BadAppearanceAppInDock = 63,
		BadAppearanceScreenShotInDock = 64,
		BadAppearanceOsResolutionInFullScreen = 65,
		BadUseHomeDirectory = 66,
		BadMinReservedMemoryLimit = 67,
		BadDispNetworkAdapterName = 68,
		BadDispNetworkAdapterSysName = 69,
		BadDispNetworkAdapterUuid = 70,
		BadDispNetworkAdapterIndex = 71,
		BadDispNetworkAdapterNatEnabled = 72,
		BadUserDefaultVmFolder = 73,
		BadLicense = 74,
	};
};

/**
 * VM Event XML parser return codes
 */
class VmEventParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		BadEventParameter = 5,
		BadEventType = 6,
		BadEventLevel = 7,
		BadEventCode = 8,
		BadEventRespOption = 9,
		BadEventSource = 10,
		BadEventIssuerType = 11,
		BadEventIssuerId = 12,
		BadParamType = 13,
		BadParamName = 14,
		BadParamValue = 15,
		BadParamCData = 16,
	};
};


/**
 * Host hardware info XML parser return codes
 */
class HostHwInfoParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema =3,
		BadDeviceData = 4,
		BadSourceData = 5,
		BadDeviceName = 6,
		BadDeviceId = 7,
		BadDeviceSize = 8,
		BadMemorySettings = 9,
		BadReservedMemLimit =10,
		BadReservedMemMax = 11,
		BadMinVmMem = 12,
		BadMaxVmMem = 13,
		BadRecomMaxMem = 14,
		BadHostRamSize = 15,
		BadFloppyDiskData = 16,
		BadHardDiskData = 17,
		BadCdRomData = 18,
		BadSerialPortData = 19,
		BadParallelPortData = 20,
		BadSoundDeviceData = 21,
		BadSoundOutputDeviceData = 22,
		BadSoundMixerDeviceData = 23,
		BadNetworkAdapterData = 24,
		BadUsbDeviceData = 25,
		BadPrinterData = 26,
		BadPartitionName = 27,
		BadPartitionSize = 28,
		BadPartitionIndex = 29,
		BadPartitionInUseFlag = 30,
		BadPartitionType = 31,
		BadPartitionExtendedFlag = 32,
		BadPartitionLogicalFlag = 33,
		BadPartitionIsActiveFlag = 34,
		BadSoundDefaultEnabledFlag = 35,
		BadVirtualNicGuid = 36,
		BadUsbDeviceSupportedFalg = 37,
		BadDiskType = 38,
		BadDiskIndex = 39,
		BadFileSystemData = 40,
		BadFsItemName = 41,
		BadFsItemSize = 42,
		BadFsItemType = 43,
		BadFsItemModifiedDateTime = 44,
		BadFsGenre = 45,
		BadFsItemPermissions = 46,
		BadHwCpu = 47,
		BadHwCpuMode = 48,
		BadHwCpuNumber = 49,
		BadHwCpuModel = 50,
		BadHwCpuSpeed = 51,
		BadHwOsVersion = 52,
		BadHwOsVersionType = 53,
		BadHwOsVersionMajor = 54,
		BadHwOsVersionMinor = 55,
		BadHwOsVersionSubMinor = 56,
		BadHwOsVersionStringPresentation = 57,
		BadHwInfoNetAdapterType = 58,
		BadHwInfoNetAdapterSysName = 59,
		BadHwInfoNetAdapterSysIndex = 60,
		BadGenericPciDeviceData = 61,
		BadGenericScsiDeviceData = 62,
		BadHwCpuVtxMode = 63,
		BadFsType = 64,
	};
};


/**
 * System statistics XML parser return codes
 */
class SystemStatisticsParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadStatisticsElementData = 4,
		BadSourceData = 5,
		BadCpuPercentsUsage = 6,
		BadCpuTotalTime = 7,
		BadCpuUserTime = 8,
		BadCpuSystemTime = 9,
		AbsentCpusStatGroupElement = 10,
		BadDiskSystemName = 11,
		BadDiskUsageDiskSpace = 12,
		BadDiskFreeDiskSpace = 13,
		AbsentDisksStatGroupElement = 14,
		BadMemTotalSize = 15,
		BadMemUsageSize = 16,
		BadMemFreeSize = 17,
		AbsentMemoryStatElement = 18,
		AbsentSwapStatElement = 19,
		BadUptimeOsUptime = 20,
		BadUptimeDispatcherUptime = 21,
		AbsentUptimeStatElement = 22,
		BadProcInfoCommandName = 23,
		BadProcInfoProcId = 24,
		BadProcInfoOwnerUser = 25,
		BadProcInfoTotalMemUsage = 26,
		BadProcInfoRealMemUsage = 27,
		BadProcInfoVirtualMemUsage = 28,
		BadProcInfoStartTime = 29,
		BadProcInfoTotalTime = 30,
		BadProcInfoUserTime = 31,
		BadProcInfoSystemTime = 32,
		BadProcInfoState = 33,
		AbsentProcInfoStatGroupElement = 34,
		BadNetIfaceSystemName = 35,
		BadNetIfaceInDataSize = 36,
		BadNetIfaceOutDataSize = 37,
		BadNetIfaceInPkgsCount = 38,
		BadNetIfaceOutPkgsCount = 39,
		AbsentNetIfacesStatGroupElement = 40,
		BadSessionUserName = 41,
		BadSessionServiceName = 42,
		BadSessionHostName = 43,
		BadSessionDurationTime = 44,
		AbsentUsersStatGroupElement = 45,
		AbsentPartitionsStatGroupElement = 46,
	};
};


/**
 * Problem report XML parser return codes
 */
class ProblemReportParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		InconsistentLicenseInfoObject = 5,
		InconsistentHostInfoObject = 6,
		InconsistentAppConfigObject = 7,
		InconsistentMemInfoObject = 8,
		InconsistentNetPresObject = 9,
		InconsistentAutoupdateInfoObject = 10,
		InconsistentUiPrefsObject = 11,
		InconsistentGuestInfoObject = 12,
		InconsistentBootInfoObject = 13,
		InconsistentGuestMemInfoObject = 14,
		InconsistentGuestScreenDataObject = 15,
		InconsistentVmConfigObject = 16,
		InconsistentMonitorDataObject = 17,
		InconsistentPreambleObject = 18,
		InconsistentUserDefinedDataObject = 19,
		BadReportData = 20,
		BadDateTimeStamp = 21,
		BadLicenseInfo = 22,
		BadHostInfo = 23,
		BadSysLogs = 24,
		InconsistentContactInfoObject = 25,
		InconsistentPerfCountersObject = 26,
		InconsistentMoreHostInfoObject = 27,
		InconsistentAllProcessesObject = 28,
		BadAutoStatisticInfoObject = 29,
		InconsistentCrashDumpObject = 30,
		InconsistentKMProfilesObject = 31,
		InconsistentNetConfigObject = 32,
	};
};

/**
 * Client preferences XML parser return codes
 */
class ClientPreferencesParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		BadPresentationPreferences = 5,
		BadAnimationPreferences = 6,
		BadAnimationFullScreen = 7,
		BadAnimationOsWindow = 8,
		BadAnimationSpeed = 9,
		BadAnimationCoherence = 10,
		BadAnimationSuspendAndResume = 11,
		BadMainWindowPreferences = 12,
		BadMainWindowToolbarPlacement = 13,
		BadMainWindowShowToolbar = 14,
		BadSharedWebPreferences = 15,
		BadSharedWebWebApp = 16,
		BadSharedWebMailApp = 17,
		BadAppearancePreferences = 18,
		BadAppearanceHiddenMessages = 19,
		BadAppearanceHiddenMessageId = 20,
		BadKeyboardPreferences = 21,
		BadKeyboardMouseRightClickDelayed = 22,
		BadKeyboardMouseRightClickDelayRate = 23,
		BadKeyboardCommandKeysRemap = 24,
		BadKeyboardNumEnterToAltGr = 25,
		BadUserHotkeys = 26,
		BadHotkey = 27,
		BadHotkeyAction = 28,
		BadHotkeySwitchFullscreen = 29,
		BadHotkeyReleaseInput = 30,
		BadHotkeyToggleCoherence = 31,
		BadHotkeyMouseRightClick = 32,
		BadHotkeyUseAlt = 33,
		BadHotkeyUseCtrl = 34,
		BadHotkeyUseShift = 35,
		BadHotkeyUseCustom = 36,
		BadHotkeyCustomKey = 37,
		BadSharedWebFtpApp = 38,
	};
};

/**
 * Updater Configuration XML parser return codes
 */
class UpdaterConfigParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		BadUpdaterConfig = 5,
		BadUpdRemindPeriod = 6,
		BadUpdAutomatic = 7,
		BadUpdAutomaticUpdateEnabled = 8,
		BadUpdAutomaticUpdatePeriod = 9,
		BadUpdAutomaticNotifyUser = 10,
		BadUpdAutomaticDownloadEnabled = 11,
		BadUpdAutomaticInstallEnabled = 12,
		BadUpdates = 13,
		BadUpdProduct = 14,
		BadUpdProductName = 15,
		BadUpdProductUpdateEnabled = 16,
		BadUpdProductVersion = 17,
		BadUpdProductVersionMajor = 18,
		BadUpdProductVersionMinor = 19,
		BadUpdProductVersionSubMinor = 20,
		BadUpdProductVersionStr = 21,
		BadUpdLocale = 22,
		BadUpdateLocaleName = 23,
		BadUpdate = 24,
		BadUpdateUuid = 25,
		BadUpdateName = 26,
		BadUpdateDescription = 27,
		BadUpdateFilePath = 28,
		BadUpdateFileSize = 29,
		BadUpdateStatus = 30,
		BadUpdateDateTime = 31,
		BadUpdateChargeable = 32,
		BadUpdDistributor = 33,
		BadUpdateDistributorName = 34,
		BadUpdProductVersionSubSubMinor = 35,
		BadUpdateOsType = 36,
	};
};

/**
 * Virtual Appliance Configuration XML parser return codes
 */
class VaConfigParser
{
public:
	enum XmlParserReturnCode
	{
		RcSuccess = 0,
		BadRoot = 1,
		NoSchemaVersion = 2,
		IncompatibleSchema = 3,
		BadSourceData = 4,
		BadVaConfig = 5,
		BadVaLicences = 6,
		BadVaRequirements = 7,
		BadVaAdminInterface = 8,
	};
};

/**
 * Common XML constants
 */

#define XML_VM_CONFIG_SCHEMA_VERSION				"1.0"
#define XML_UUID_FORMAT								"{00000000-0000-0000-0000-000000000000}"
#define XML_DATETIME_FORMAT							"yyyy-MM-dd hh:mm:ss"
#define XML_DATETIME_FORMAT_LONG					"yyyy-MM-dd hh:mm:ss.zzz"
#define XML_DATE_FORMAT								"yyyy-MM-dd"
#define XML_TIME_FORMAT								"hh:mm:ss"
#define XML_DEFAULT_DATE							"1752-01-01"
#define XML_DEFAULT_DATE_FORMAT						"yyyy-MM-dd"
#define XML_DEFAULT_TIME							"99:99:99"
#define XML_DEFAULT_TIME_FORMAT						"hh:mm:ss"
#define XML_DEFAULT_DATE_TIME						"1752-01-01 00:00:00"
#define XML_DEFAULT_PARAM_UINT						(unsigned int)-1
#define XML_DEFAULT_PARAM_STR						"FFFFFFFF"
#define XML_DEFAULT_RAM_SIZE						256
#define XML_DEFAULT_MAXRAM_SIZE						8192
#define XML_DEFAULT_MAXNUMARAM_SIZE					4096
#define XML_DEFAULT_MAXRAM_SLOTS					256
#define XML_DEFAULT_QUOTA_MIN_SIZE					512
#define XML_DEFAULT_QUOTA_PRIORITY					50
#define XML_DEFAULT_VIDEO_SIZE						16
#define XML_DEFAULT_VM_CPU_NUMBER			1
#define XML_DEFAULT_VM_CPU_MODE				0
#define XML_DEFAULT_HW_CPU_NUMBER			1
#define XML_DEFAULT_HW_CPU_MODE				1
#define XML_DEFAULT_MIN_RESERVED_MEMORY_LIMIT 96 // MB
#define XML_DEFAULT_IP_ADDRESS						"127.0.0.1"
#define XML_DEFAULT_IP_MASK						"255.255.255.255"
#define XML_DEFAULT_SUBNET_MASK					"255.255.255.0"
// Network Address Range In IPv6, the first 48 bits are for Internet routing.
// Subnetting Range:  The 16 bits from the 49th to the 54th are for defining subnets.
// Device (Interface) Range: The last 64 bits are for device (interface) ID's.
// So full 128-bit hex mask would be FFFF:FFFF:FFFF:FFFF:0:0:0:0
// http://www.ietf.org/rfc/rfc2373.txt
#define XML_DEFAULT_IP6_MASK						"FFFF:FFFF:FFFF:FFFF:0:0:0:0"
#define XML_DEFAULT_IP6_ADDRESS						"::1" // loopback

#define XML_DEFAULT_REMOTE_DISPLAY_PORT			6500
#define XML_DEFAULT_VMDIR_SUFFIX_WIN				"Parallels Virtual Machines"
#define XML_DEFAULT_VMDIR_SUFFIX_LIN				"parallels-vm"
#define XML_DEFAULT_VMDIR_PREFIX_WIN				"C:\\Documents and Settings"
#define XML_DEFAULT_VMDIR_PREFIX_LIN				"/home"
#define XML_DEFAULT_VMDIR_PREFIX_MAC				"/Users"
// #define XML_DEFAULR_VMDIR_CATALOGUE_FILE			"vmdirectory.xml"
#define XML_DEFAULT_DHCP_SCOPE_START				"10.37.129.1"
#define XML_DEFAULT_DHCP_SCOPE_END					"10.37.129.254"
#define XML_DEFAULT_DHCP_SCOPE_MASK					"255.255.255.0"
#define XML_DEFAULT_STRING_DATA						"no data"
#define XML_DEFAULT_GUEST_MODE						"Unknown mode"
#define XML_DEFAULT_NUMERIC_DATA					0

#define XML_DEFAULT_CHRN_SHOWTASKBAR	true
#define XML_DEFAULT_CHRN_RELOCATETASKBAR	true
#define XML_DEFAULT_CHRN_EXCLUDEDOCK	true
#define XML_DEFAULT_CHRN_MULTIDISPLAY	true

#define XML_DEFAULT_SCREEN_WIDTH	800
#define XML_DEFAULT_SCREEN_HEIGHT	600

#define XML_DEFAULT_ANIMATION_SPEED_MIN	10
#define XML_DEFAULT_ANIMATION_SPEED_MAX	300

#define XML_DEFAULT_MOUSE_RIGHTCLICK_DELAY_MIN	500
#define XML_DEFAULT_MOUSE_RIGHTCLICK_DELAY_MAX	3000

#define XML_DOC_INCLUDE_XML_NODE					true
#define XML_DOC_OMIT_XML_NODE						false

#define XML_DOC_VERSION_TAG							"xml"
//#define XML_DOC_VERSION_DATA						"version=\"1.0\" encoding=\"ISO-8859-1\""
#define XML_DOC_VERSION_DATA						"version=\"1.0\" encoding=\"UTF-8\""

#define XML_DEFAULT_PROBLEM_REP_PREAMBLE			QT_TRANSLATE_NOOP("QObject", "This is a Parallels problem report. Please send this file to Parallels IP Holdings GmbH. mailto:reports@parallels.com")

/**
 * Virtual Machine configuration XML model
 */

//
// XML tree basic elements
//
#define XML_VM_CONFIG_EL_ROOT						"ParallelsVirtualMachine"
#define XML_VM_CONFIG_EL_HARDWARE					"Hardware"
#define XML_VM_CONFIG_EL_SETTINGS					"Settings"
#define XML_VM_CONFIG_EL_SECURITY					"Security"
#define XML_VM_CONFIG_EL_VMIDENT					"Identification"
#define XML_VM_CONFIG_EL_VMGENERAL					"General"
#define XML_VM_CONFIG_EL_COMMONOPTS					"General"
#define XML_VM_CONFIG_EL_RUNTIMEOPTS				"Runtime"
#define XML_VM_CONFIG_EL_STARTUPOPTS				"Startup"
#define XML_VM_CONFIG_EL_FINALOPTS					"Finalization"
#define XML_VM_CONFIG_EL_SAVEDSTATES_OPTS			"SavedStates"
#define XML_VM_CONFIG_EL_SAVEDSTATE					"SavedState"
#define XML_VM_CONFIG_EL_SAVEDSTATES_LIST			"SavedStatesList"
#define XML_VM_CONFIG_EL_SHARFOPTS					"SharedFolders"
#define XML_VM_CONFIG_EL_SHAREDFOLDER				"SharedFolder"
#define XML_VM_CONFIG_EL_SHAREDPROFILE				"SharedProfile"
#define XML_VM_CONFIG_EL_SHAREDAPPLICATIONS	"SharedApplications"
#define XML_VM_CONFIG_EL_VMFLAGS					"System"
#define XML_VM_CONFIG_EL_SCHEDULE					"Schedule"
#define XML_VM_CONFIG_EL_DEVICE						"Device"
#define XML_VM_CONFIG_EL_CLUSTERED_DEVICE			"ClusteredDevice"
#define XML_VM_CONFIG_EL_STORAGE_DEVICE				"StorageDevice"
#define XML_VM_CONFIG_EL_MASS_STORAGE_DEVICE		"MassStorageDevice"
#define XML_VM_CONFIG_EL_NETWORK_INTERFACE			"NetworkInterfaceCard"
#define XML_VM_CONFIG_EL_SOUND_DEVICE				"Sound"
#define XML_VM_CONFIG_EL_USB_DEVICE					"USB"
#define XML_VM_CONFIG_EL_PORT						"Port"
#define XML_VM_CONFIG_EL_GENERIC_DEVICE					"GenericDevice"
#define XML_VM_CONFIG_EL_GENERIC_PCI_DEVICE				"GenericPciDevice"
#define XML_VM_CONFIG_EL_GENERIC_SCSI_DEVICE				"GenericScsiDevice"
#define XML_VM_CONFIG_EL_HARD_DISK					"Hdd"
#define XML_VM_CONFIG_EL_OPTICAL_DISK				"CdRom"
#define XML_VM_CONFIG_EL_FLOPPY_DISK				"Fdd"
#define XML_VM_CONFIG_EL_SERIAL_PORT				"Serial"
#define XML_VM_CONFIG_EL_PARALLEL_PORT				"Parallel"
#define XML_VM_CONFIG_EL_PRINTER					"Printer"
#define XML_VM_CONFIG_EL_GEN_NET_ADAPTER			"NetworkAdapter"
#define XML_VM_CONFIG_EL_ACTIONS			"Actions"
#define XML_VM_CONFIG_EL_MEMORY				"Memory"
#define XML_VM_CONFIG_EL_COHERENCE				"Coherence"
#define XML_VM_CONFIG_EL_SCREEN_RESOLUTION		"ScreenResolution"
#define XML_VM_CONFIG_EL_SCREEN_RESOLUTIONS	"ScreenResolutions"
#define XML_VM_CONFIG_EL_GUEST_SHARING	"GuestSharing"
#define XML_VM_CONFIG_EL_HOST_SHARING	"HostSharing"
#define XML_VM_CONFIG_EL_CPU	"Cpu"
#define XML_VM_CONFIG_EL_REMOTE_DISPLAY	"RemoteDisplay"
#define XML_VM_CONFIG_EL_VIDEO				"Video"
#define XML_VM_CONFIG_EL_TOOLS				"Tools"
#define XML_VM_CONFIG_EL_TOOLS_TIME_SYNC	"TimeSync"
#define XML_CLIENT_USER_CONFIG_EL_PATH_RELATIVE		"RelativePathes"

//
// XML tree subsequent nodes
//
#define XML_VM_CONFIG_ND_DEV_ENABLED				"Enabled"
#define XML_VM_CONFIG_ND_DEV_CONNECTED				"Connected"
#define XML_VM_CONFIG_ND_DEV_EMTYPE					"EmulatedType"
#define XML_VM_CONFIG_ND_DEV_REMOTE				"Remote"
#define XML_VM_CONFIG_ND_DEV_SYSTEM_NAME		"SystemName"
#define XML_VM_CONFIG_ND_DEV_USER_FRIENDLY_NAME		"UserFriendlyName"
#define XML_VM_CONFIG_ND_DEV_RELATIVE			"Relative"
#define XML_VM_CONFIG_ND_IMAGE_PATH					"Path"
#define XML_VM_CONFIG_ND_IFACE_TYPE					"InterfaceType"
#define XML_VM_CONFIG_ND_STACK_INDEX				"StackIndex"
#define XML_VM_CONFIG_ND_DEV_DESCRIPTION			"DeviceDescription"
#define XML_VM_CONFIG_ND_DISK_TYPE					"DiskType"
#define XML_VM_CONFIG_ND_CYLINDERS					"Cylinders"
#define XML_VM_CONFIG_ND_HEADS						"Heads"
#define XML_VM_CONFIG_ND_SECTORS					"Sectors"
#define XML_VM_CONFIG_ND_PASSTHROUGH				"Passthrough"
#define XML_VM_CONFIG_ND_HDD_SPLITTED				"Splitted"
#define XML_VM_CONFIG_ND_HDD_PERSISTENT				"Persistent"
#define XML_VM_CONFIG_ND_HDD_VERSION				"DiskVersion"
#define XML_VM_CONFIG_ND_SOCKETMODE					"SocketMode"
#define XML_VM_CONFIG_ND_OUTFILE					"Path"
#define XML_VM_CONFIG_ND_VMSHARED					"Shared"
#define XML_VM_CONFIG_ND_VMID						"VmUuid"
#define XML_VM_CONFIG_ND_VMNAME						"VmName"
#define XML_VM_CONFIG_ND_SERVERID					"ServerUuid"
#define XML_VM_CONFIG_ND_SERVERHOST					"ServerHost"
#define XML_VM_CONFIG_ND_RAM_SIZE					"RAM"
#define XML_VM_CONFIG_ND_VIDEO_SIZE					"VideoMemorySize"
#define XML_VM_CONFIG_ND_ENABLE_DIRECTX_SHADERS	"EnableDirectXShaders"
#define XML_VM_CONFIG_ND_CPU_NUMBER					"Number"
#define XML_VM_CONFIG_ND_CPU_MODE					"Mode"
#define XML_VM_CONFIG_ND_OSTYPE						"OsType"
#define XML_VM_CONFIG_ND_OSNUMBER						"OsNumber"
#define XML_VM_CONFIG_ND_ICON						"Icon"
#define XML_VM_CONFIG_ND_VMDESCRIPTION				"VmDescription"
#define XML_VM_CONFIG_ND_IS_TEMPLATE				"IsTemplate"
#define XML_VM_CONFIG_ND_ACCELLEVEL					"AccelerationLevel"
#define XML_VM_CONFIG_ND_PCSPEAKER					"PCSpeakerEnabled"
#define XML_VM_CONFIG_ND_FGPRIORITY					"ForegroundPriority"
#define XML_VM_CONFIG_ND_BGPRIORITY					"BackgroundPriority"
#define XML_VM_CONFIG_ND_AUTOSTART					"AutoStart"
#define XML_VM_CONFIG_ND_AUTOFULLSCREEN				"AutoFullscreen"
#define XML_VM_CONFIG_ND_POWERON_ACTS				"PowerOnActions"
#define XML_VM_CONFIG_ND_BOOTDEVICES				"BootDevices"
#define XML_VM_CONFIG_ND_BOOTSEQUENCE				"BootSequence"
#define XML_VM_CONFIG_ND_BOOTORDER				    "BootingOrder"
#define XML_VM_CONFIG_ND_BOOTDEVICE				    "BootDevice"
#define XML_VM_CONFIG_ND_CLOSECONSOLE				"CloseConsole"
#define XML_VM_CONFIG_ND_POWEROFF_ACTS				"PowerOffActions"
#define XML_VM_CONFIG_ND_SVDSTAT_STORE				"SavedStateStore"
#define XML_VM_CONFIG_ND_SVDSTAT_DATETIME			"SavedStateTimeStamp"
#define XML_VM_CONFIG_ND_SVDSTAT_FILE				"SavedStateFile"
#define XML_VM_CONFIG_ND_SVDSTAT_NAME				"SavedStateName"
#define XML_VM_CONFIG_ND_SVDSTAT_DESCR				"SavedStateDescription"
#define XML_VM_CONFIG_ND_SVDSTAT_LASTNODE			"SavedStateLastNode"
#define XML_VM_CONFIG_ND_SVDSTAT_PARENT				"SavedStateParent"
#define XML_VM_CONFIG_ND_SHARF_PATH					"Path"
#define XML_VM_CONFIG_ND_SHARF_STATE				"State"
#define XML_VM_CONFIG_ND_SHARF_MODE					"Mode"
#define XML_VM_CONFIG_ND_SHARF_DESC						"FolderDescription"
#define XML_VM_CONFIG_ND_SHARF_READONLY					"ReadOnly"
#define XML_VM_CONFIG_ND_SHARP_USE_DESKTOP		"UseDesktop"
#define XML_VM_CONFIG_ND_SHARP_USE_DOCUMENTS	"UseDocuments"
#define XML_VM_CONFIG_ND_SHARP_USE_PICTURES		"UsePictures"
#define XML_VM_CONFIG_ND_SHARP_USE_MUSIC			"UseMusic"
#define XML_VM_CONFIG_ND_SHARA_WINTOMAC			"FromWinToMac"
#define XML_VM_CONFIG_ND_SHARA_MACTOWIN			"FromMacToWin"
#define XML_VM_CONFIG_ND_SHARA_SMARTSELECT		"SmartSelect"
#define XML_VM_CONFIG_ND_LOCAL_SHARING					"LocalSharing"
#define XML_VM_CONFIG_ND_GLOBAL_SHARING					"GlobalSharing"
#define XML_VM_CONFIG_ND_SHARE_ALL_MAC_DISKS			"ShareAllMacDisks"
#define XML_VM_CONFIG_ND_SHARE_USER_HOME_DIR		"ShareUserHomeDir"
#define XML_VM_CONFIG_ND_MAP_SHARED_FOLDERS_ON_LETTERS		"MapSharedFoldersOnLetters"
#define XML_VM_CONFIG_ND_SCHED_BASIS				"SchedBasis"
#define XML_VM_CONFIG_ND_SCHED_GRAN					"SchedGranularity"
#define XML_VM_CONFIG_ND_SCHED_DAYWEEK				"SchedDayOfWeek"
#define XML_VM_CONFIG_ND_SCHED_DAYMONTH				"SchedDayOfMonth"
#define XML_VM_CONFIG_ND_SCHED_MONTH				"SchedMonth"
#define XML_VM_CONFIG_ND_SCHED_DAY					"SchedDay"
#define XML_VM_CONFIG_ND_SCHED_WEEK					"SchedWeek"
#define XML_VM_CONFIG_ND_SCHED_STARTDATE			"SchedStartDate"
#define XML_VM_CONFIG_ND_SCHED_STARTTIME			"SchedStartTime"
#define XML_VM_CONFIG_ND_SCHED_STOPDATE				"SchedStopDate"
#define XML_VM_CONFIG_ND_SCHED_STOPTIME				"SchedStopTime"
#define XML_VM_CONFIG_ND_BOUND_ADAPTER_INDEX		"AdapterNumber"
#define XML_VM_CONFIG_ND_BOUND_ADAPTER_NAME			"AdapterName"
#define XML_VM_CONFIG_ND_ADAPTER_TYPE				"AdapterType"
#define XML_VM_CONFIG_ND_ADAPTER_PORT				"PortAddress"
#define XML_VM_CONFIG_ND_ADAPTER_IRQ				"IRQ"
#define XML_VM_CONFIG_ND_ADAPTER_MAC_ADDRESS		"MAC"
#define XML_VM_CONFIG_ND_SOUND_OUTPUT				"Output"
#define XML_VM_CONFIG_ND_SOUND_MIXER				"Mixer"
#define XML_VM_CONFIG_ND_USB_AUTOCONNECT			"AutoConnect"
#define XML_VM_CONFIG_ND_USB_CONNECT_REASON			"ConnectReason"
#define XML_VM_CONFIG_ND_VM_STATE                   "CurrentVmState"
#define XML_VM_CONFIG_ND_SESSION_COUNT              "ActiveSessions"
// VM flags (i.e. system parameters):
#define XML_VM_CONFIG_ND_PARAM1						"MemoryBlockSize"
#define XML_VM_CONFIG_ND_PARAM2						"DiskCachePolicy"
#define XML_VM_CONFIG_ND_PARAM3						"EnableAcceleration"
#define XML_VM_CONFIG_ND_PARAM4						"MultiFrame"
#define XML_VM_CONFIG_ND_PARAM5						"MonitorBase"
#define XML_VM_CONFIG_ND_PARAM6						"AnalysisUser"
#define XML_VM_CONFIG_ND_PARAM7						"BitDRemap"
#define XML_VM_CONFIG_ND_RESERVED_PARAM1			"ReservedParam1"
#define XML_VM_CONFIG_ND_PARAM9						"SmartKernelMode2"
#define XML_VM_CONFIG_ND_PARAM10					"StrongChecks"
#define XML_VM_CONFIG_ND_PARAM11					"FastStubs"
#define XML_VM_CONFIG_ND_PARAM12					"FastSysCall"
#define XML_VM_CONFIG_ND_PARAM13					"FastSysCall2"
#define XML_VM_CONFIG_ND_PARAM14					"IgnoreIdeInterrupts"
#define XML_VM_CONFIG_ND_PARAM15					"MinAsyncTime"
#define XML_VM_CONFIG_ND_PARAM16					"FullLdtMap"
#define XML_VM_CONFIG_ND_PARAM17					"OsDependentOptimization"
#define XML_VM_CONFIG_ND_ENABLE_VTX					"EnableVTxSupport"
#define XML_VM_CONFIG_ND_SYS_FLAGS					"SystemFlags"
#define XML_VM_CONFIG_ND_WND_MODE				"WindowMode"
#define XML_VM_CONFIG_ND_ON_APP_QUIT				"OnAppQuit"
#define XML_VM_CONFIG_ND_ON_STOP				"OnStop"
#define XML_VM_CONFIG_ND_ON_PAUSE				"OnPause"
#define XML_VM_CONFIG_ND_CLOSE_APP_ON_SHUTDOWN		"CloseAppOnShutdown"
#define XML_VM_CONFIG_ND_CHRN_SHOWTASKBAR				"ShowTaskBar"
#define XML_VM_CONFIG_ND_CHRN_RELOCATETASKBAR			"RelocateTaskBar"
#define XML_VM_CONFIG_ND_CHRN_EXCLUDEDOCK					"ExcludeDock"
#define XML_VM_CONFIG_ND_CHRN_MULTIDISPLAY					"MultiDisplay"
#define XML_VM_CONFIG_ND_CHRN_GROUPALLWINDOWS		"GroupAllWindows"
#define XML_VM_CONFIG_ND_CHRN_DISABLEDROPSHADOW		"DisableDropShadow"
#define XML_VM_CONFIG_ND_CHRN_DONOTMINIMIZETODOCK	"DoNotMinimizeToDock"
#define XML_VM_CONFIG_ND_CHRN_BRINGTOFRONT				"BringToFront"
#define XML_VM_CONFIG_ND_SCREEN_WIDTH				"Width"
#define XML_VM_CONFIG_ND_SCREEN_HEIGHT			"Height"
#define XML_VM_CONFIG_ND_BOOTDEV_TYPE			"Type"
#define XML_VM_CONFIG_ND_BOOTDEV_NUMBER			"BootingNumber"
#define XML_VM_CONFIG_ND_BOOTDEV_INUSE			"InUse"
#define XML_VM_CONFIG_ND_UNDO_DISKS			"UndoDisks"
#define XML_VM_CONFIG_ND_HOST_NAME			"HostName"
#define XML_VM_CONFIG_ND_PASSWORD			"Password"
#define XML_VM_CONFIG_ND_PORT_NUMBER			"PortNumber"
#define XML_VM_CONFIG_ND_ENABLE_CLIPBOARD_SYNC "EnableClipboardSync"
#define XML_VM_CONFIG_ND_ENABLE_MOUSE_SYNC "EnableMouseSync"
#define XML_VM_CONFIG_ND_TIME_SYNC_KEEP_TIME_DIFF	"KeepTimeDiff"
#define XML_VM_CONFIG_ND_TIME_SYNC_SYNC_INTERVAL	"SyncInterval"
#define XML_VM_CONFIG_ND_SECURITY_ACL	"AccessControlList"

#define XML_CLIENT_USER_CONFIG_HDD_RELATIVE	"HddRelative"
#define XML_CLIENT_USER_CONFIG_FDD_RELATIVE	"FddRelative"
#define XML_CLIENT_USER_CONFIG_SERIAL_RELATIVE	"SerialRelative"
#define XML_CLIENT_USER_CONFIG_PARALLEL_RELATIVE	"ParallelRelative"



/**
 * Virtual Machine Directory XML model
 */

//
// XML tree basic elements
//
#define XML_VM_DIR_EL_ROOT					"ParallelsVmDirectories"
#define XML_VM_DIR_EL_SERVER_ID			"ServerIdentification"
#define XML_VM_DIR_EL_CATALOGUE			"Catalogue"
#define XML_VM_DIR_EL_VMDIR					"VmDirectory"
#define XML_VM_DIR_EL_VMDIR_ITEM		"VirtualMachine"

//
// XML tree subsequent nodes
//
#define XML_VM_DIR_ND_SERVER_UUID					"ServerUuid"
#define XML_VM_DIR_ND_VM_HOME						"VmHome"
#define XML_VM_DIR_ND_IS_REGISTERED				"IsRegistered"
#define XML_VM_DIR_ND_IS_VALID							"IsValid"
#define XML_VM_DIR_ND_IS_PRIVATE					"IsPrivate"
#define XML_VM_DIR_ND_REGISTERED_BY					"RegisteredBy"
#define XML_VM_DIR_ND_REGISTERED_DATETIME			"RegistrationDateTime"
#define XML_VM_DIR_ND_CHANGED_BY					"ChangedBy"
#define XML_VM_DIR_ND_CHANGED_DATETIME				"ChangeDateTime"
#define XML_VM_DIR_ND_DEFAULT_VM_FOLDER		"DefaultVmFolder"
#define XML_VM_DIR_ND_USER_FRIENDLY_NAME		"UserFriendlyName"

/**
 * Dispatcher Configuration XML model
 */

//
// XML tree basic elements
//
#define XML_VM_DISP_EL_ROOT							"ParallelsPreferences"
#define XML_VM_DISP_EL_SETTINGS						"ServerSettings"
#define XML_VM_DISP_EL_SERVER_PREFERS				"CommonPreferences"
#define XML_VM_DISP_EL_USERS_PREFERS				"UsersPreferences"
#define XML_VM_DISP_EL_WORKSPACE_PREFERS			"WorkspacePreferences"
#define XML_VM_DISP_EL_MEMORY_PREFERS				"MemoryPreferences"
#define XML_VM_DISP_EL_NETWORK_PREFERS				"NetworkPreferences"
#define XML_VM_DISP_EL_USER_ITEM					"ParallelsUser"
#define XML_VM_DISP_EL_USER_CONNECTION				"ConnectionProperties"
#define XML_VM_DISP_EL_USER_WORKSPACE				"UserWorkspace"
#define XML_VM_DISP_EL_USER_HOTKEYS					"Hotkeys"
#define XML_VM_DISP_EL_USER_VMGROUPS				"PersonalGroups"
#define XML_VM_DISP_EL_USER_VMQUERIES				"PersonalQueries"
#define XML_VM_DISP_EL_USER_ACCESS				   "AccessPreferences"
#define XML_VM_DISP_EL_PERSONAL_VMGROUP				"PersonalVmGroup"
#define XML_VM_DISP_EL_QUERY_PREDICATE				"QueryPredicate"
#define XML_VM_DISP_EL_QUERY_GROUP					"QueryGroup"
#define XML_VM_DISP_EL_PERSONAL_QUERY				"PersonalQuery"
#define XML_VM_DISP_EL_LICENSE								"License"

//
// XML tree subsequent nodes
//
#define XML_VM_DISP_ND_DEFAULT_VM_DIR				"DefaultVmDirectory"
#define XML_VM_DISP_ND_USER_VM_DIR					"UserVmDirectory"
#define XML_VM_DISP_ND_USER_HOME_FOLDER			"UserHomeFolder"
#define XML_VM_DISP_ND_USER_VM_CATALOGUE			"UserDefaultVmFolder"
#define XML_VM_DISP_ND_DISPATCHER_PORT			"DispatcherPort"
#define XML_VM_DISP_ND_ADJUST_MEM_AUTO				"AdjustMemAuto"
#define XML_VM_DISP_ND_RESERVED_MEM_LIMIT			"ReservedMemoryLimit"
#define XML_VM_DISP_ND_RESERVED_MEMORY_MAX			"ReservedMemoryMax"
#define XML_VM_DISP_ND_RESERVED_MEMORY_MIN			"ReservedMemoryMin"
#define XML_VM_DISP_ND_MIN_VM_MEMORY				"MinVmMemory"
#define XML_VM_DISP_ND_MAX_VM_MEMORY				"MaxVmMemory"
#define XML_VM_DISP_ND_RECOM_MAX_VM_MEMORY			"RecommendedMaxVmMemory"
#define XML_VM_DISP_ND_HOST_RAM_SIZE				"HostRamSize"
#define XML_VM_DISP_ND_DHCP_SCOPE					"DhcpScope"
#define XML_VM_DISP_ND_DHCP_START					"DhcpScopeStartAddress"
#define XML_VM_DISP_ND_DHCP_END						"DhcpScopeEndAddress"
#define XML_VM_DISP_ND_DHCP_MASK					"DhcpScopeMask"
#define XML_VM_DISP_ND_USE_SSL_FLAG					"UseSSL"
#define XML_VM_DISP_ND_COMPRESS_LEVEL				"CompressionLevel"
#define XML_VM_DISP_ND_HKEY_USE_ALT					"HkUseAlt"
#define XML_VM_DISP_ND_HKEY_USE_CTRL				"HkUseCtrl"
#define XML_VM_DISP_ND_HKEY_USE_SHIFT				"HkUseShift"
#define XML_VM_DISP_ND_HKEY_USE_CUSTOM				"HkUseCustom"
#define XML_VM_DISP_ND_HKEY_CUSTOM_KEY				"HkCustomKey"
#define XML_VM_DISP_ND_PARENT_GROUP_ID				"ParentGroupId"
#define XML_VM_DISP_ND_LAST_GROUP_NODE				"LastNode"
#define XML_VM_DISP_ND_MEMBER_VMID					"MemberVmId"
#define XML_VM_DISP_ND_QRY_FIELD_NAME				"FieldName"
#define XML_VM_DISP_ND_QRY_FIELD_TYPE				"FieldType"
#define XML_VM_DISP_ND_QRY_OPERATOR					"Operator"
#define XML_VM_DISP_ND_QRY_NOTCONDITION				"ConditionNot"
#define XML_VM_DISP_ND_QRY_COMP_VALUE				"ComparedValue"
#define XML_VM_DISP_ND_QRY_UNION_CONDITION			"UnionCondition"
#define XML_VM_DISP_ND_ALLOW_USE_SMC_FLAG			      "UseManagementConsole"
#define XML_VM_DISP_ND_DEFAULT_COMMAND_HISTORY_SIZE   "DefaultCommandHistorySize"
#define XML_VM_DISP_ND_DEFAULT_USE_CONSOLE   "DefaultUseConsole"
#define XML_VM_DISP_ND_DEFAULT_CHANGE_SETTINGS   "DefaultChangeSettings"
#define XML_VM_DISP_ND_CHANGE_SERVER_SETTINGS  "ChangeServerSettings"
#define XML_VM_DISP_ND_DISTRIBUTED_DIRECTORY		"DistributedDirectory"
#define XML_VM_DISP_ND_NET_ADAPTER						"NetworkAdapter"
#define XML_VM_DISP_ND_NET_ADAPTER_SYSNAME		"SysName"
#define XML_VM_DISP_ND_NET_ADAPTER_INDEX				"Index"
#define XML_VM_DISP_ND_NET_ADAPTER_NAT_ENABLED	"NatEnabled"
#define XML_VM_DISP_ND_LICENSE_KEY							"Key"
#define XML_VM_DISP_ND_LICENSE_USER						"User"
#define XML_VM_DISP_ND_LICENSE_COMPANY				"Company"

/**
 * VM Event XML model
 */

//
// XML tree basic elements
//
#define XML_VM_EVENT_EL_ROOT						"ParallelsEvent"
#define XML_VM_EVENT_EL_PARAMETERS					"EventParameters"
#define XML_VM_EVENT_EL_PARAM						"EventParameter"

//
// XML tree subsequent nodes
//
#define XML_VM_EVENT_ND_TYPE						"EventType"
#define XML_VM_EVENT_ND_LEVEL						"EventLevel"
#define XML_VM_EVENT_ND_CODE						"EventCode"
#define XML_VM_EVENT_ND_RESP_OPTION					"EventNeedResponse"
#define XML_VM_EVENT_ND_SOURCE						"EventSource"
#define XML_VM_EVENT_ND_ISSUER_TYPE					"EventIssuerType"
#define XML_VM_EVENT_ND_ISSUER_ID					"EventIssuerId"
#define XML_VM_EVENT_ND_INIT_REQ_ID                 "EventInitialRequestId"

/**
 * Host Hardware Info XML model
 */

//
// XML tree basic elements
//
#define XML_HOST_INFO_EL_ROOT						"ParallelsHostInfo"
#define XML_HOST_INFO_EL_PRINTER_GROUP				"Printers"
#define XML_HOST_INFO_EL_PRINTER					"Printer"
#define XML_HOST_INFO_EL_USBDEVICE_GROUP			"UsbDevices"
#define XML_HOST_INFO_EL_USBDEVICE					"UsbDevice"
#define XML_HOST_INFO_EL_NETADAPTER_GROUP			"NetworkAdapters"
#define XML_HOST_INFO_EL_NETADAPTER					"NetworkAdapter"
#define XML_HOST_INFO_EL_SOUND_OUTPUT_GROUP			"OutputDevices"
#define XML_HOST_INFO_EL_SOUND_OUTPUT				"OutputDevice"
#define XML_HOST_INFO_EL_SOUND_MIXER_GROUP			"MixerDevices"
#define XML_HOST_INFO_EL_SOUND_MIXER				"MixerDevice"
#define XML_HOST_INFO_EL_SOUND_DEVICE_GROUP			"SoundDevices"
#define XML_HOST_INFO_EL_PARALLEL_PORT_GROUP		"ParallelPorts"
#define XML_HOST_INFO_EL_PARALLEL_PORT				"ParallelPort"
#define XML_HOST_INFO_EL_SERIAL_PORT_GROUP			"SerialPorts"
#define XML_HOST_INFO_EL_SERIAL_PORT				"SerialPort"
#define XML_HOST_INFO_EL_CDROM_GROUP				"CdROMs"
#define XML_HOST_INFO_EL_CDROM						"CdROM"
#define XML_HOST_INFO_EL_HARDDISK_GROUP				"HardDisks"
#define XML_HOST_INFO_EL_HARDDISK					"HardDisk"
#define XML_HOST_INFO_EL_FLOPPY_DISK_GROUP			"FloppyDisks"
#define XML_HOST_INFO_EL_FLOPPY_DISK				"FloppyDisk"
#define XML_HOST_INFO_EL_MEM_SETTINGS				"MemorySettings"
#define XML_HOST_INFO_EL_GENERIC_DEVICE				"GenericDevice"
#define XML_HOST_INFO_EL_FILESYSTEM_INFO			"FileSystemInfo"
#define XML_HOST_INFO_EL_CPU								"Cpu"
#define XML_HOST_INFO_EL_OSVERSION					"OsVersion"
#define XML_HOST_INFO_EL_GENERIC_PCI_DEVICE_GROUP		"GenericPciDevices"
#define XML_HOST_INFO_EL_GENERIC_PCI_DEVICE					"GenericPciDevice"
#define XML_HOST_INFO_EL_GENERIC_SCSI_DEVICE_GROUP	"GenericScsiDevices"
#define XML_HOST_INFO_EL_GENERIC_SCSI_DEVICE				"GenericScsiDevice"

//
// XML tree subsequent nodes
//
#define XML_HOST_INFO_ND_RESERVED_MEM_LIMIT			"ReservedMemoryLimit"
#define XML_HOST_INFO_ND_RESERVED_MEM_MAX			"ReservedMemoryMax"
#define XML_HOST_INFO_ND_MIN_VM_MEM					"MinVmMemory"
#define XML_HOST_INFO_ND_MAX_VM_MEM					"MaxVmMemory"
#define XML_HOST_INFO_ND_RECOM_MAX_MEM				"RecommendedMaxMemory"
#define XML_HOST_INFO_ND_HOST_RAM_SIZE				"HostRamSize"
#define XML_HOST_INFO_ND_HDD_PARTITION				"Partition"
#define XML_HOST_INFO_ND_SOUNDDEFAULT				"SoundDefaultEnabled"
#define XML_HOST_INFO_ND_VIRTUAL_NIC_GUID			"VirtualNicGuid"
#define XML_HOST_INFO_ND_USB_SUPPORTED				"UsbSupported"
#define XML_HOST_INFO_ND_FILESYSTEM_ITEM			"FsElement"
#define XML_HOST_INFO_ND_FSITEM_PARENT				"ParentElement"
#define XML_HOST_INFO_ND_CPU_NUMBER					"Number"
#define XML_HOST_INFO_ND_CPU_MODE					"Mode"
#define XML_HOST_INFO_ND_CPU_VTX_MODE				"VtxMode"
#define XML_HOST_INFO_ND_CPU_VTD_MODE				"VtxEnabled"
#define XML_HOST_INFO_ND_CPU_MODEL					"Model"
#define XML_HOST_INFO_ND_CPU_SPEED					"Speed"
#define XML_HOST_INFO_ND_OSVERSION_TYPE			"Type"
#define XML_HOST_INFO_ND_OSVERSION_MAJOR		"Major"
#define XML_HOST_INFO_ND_OSVERSION_MINOR		"Minor"
#define XML_HOST_INFO_ND_OSVERSION_SUBMINOR	"SubMinor"
#define XML_HOST_INFO_ND_OSVERSION_STRING		"StringPresentation"

/**
 * System statistics XML model
 */

//
// XML tree basic elements
//
#define XML_STATISTICS_EL_ROOT						"ParallelsSystemStatistics"
#define XML_STATISTICS_EL_CPUS_GROUP			"Cpus"
#define XML_STATISTICS_EL_CPU							"Cpu"
#define XML_STATISTICS_EL_DISKS_GROUP			"Disks"
#define XML_STATISTICS_EL_DISK						"Disk"
#define XML_STATISTICS_EL_MEMORY					"Memory"
#define XML_STATISTICS_EL_SWAP						"Swap"
#define XML_STATISTICS_EL_UPTIME					"Uptime"
#define XML_STATISTICS_EL_PROCESSES_GROUP	"Processes"
#define XML_STATISTICS_EL_PROCESS					"Process"
#define XML_STATISTICS_EL_NET_IFACES_GROUP	"NetIfaces"
#define XML_STATISTICS_EL_NET_IFACE				"NetIface"
#define XML_STATISTICS_EL_USER_SESSIONS_GROUP	"UsersSessions"
#define XML_STATISTICS_EL_USER_SESSION				"UserSession"
#define XML_STATISTICS_EL_DISK_PARTITIONS_GROUP			"Partitions"
#define XML_STATISTICS_EL_DISK_PARTITION	"Partition"

//
// XML tree subsequent nodes
//
#define XML_STATISTICS_CPU_PERCENTS_USAGE				"PercentsUsage"
#define XML_STATISTICS_CPU_TOTAL_TIME						"TotalTime"
#define XML_STATISTICS_CPU_USER_TIME						"UserTime"
#define XML_STATISTICS_CPU_SYSTEM_TIME					"SystemTime"
#define XML_STATISTICS_DISK_SYSTEM_NAME					"SystemName"
#define XML_STATISTICS_DISK_USAGE_DISK_SPACE		"UsageDiskSpace"
#define XML_STATISTICS_DISK_FREE_DISK_SPACE			"FreeDiskSpace"
#define XML_STATISTICS_MEM_TOTAL_SIZE						"TotalSize"
#define XML_STATISTICS_MEM_USAGE_SIZE						"UsageSize"
#define XML_STATISTICS_MEM_FREE_SIZE						"FreeSize"
#define XML_STATISTICS_MEM_REAL_SIZE						"RealSize"
#define XML_STATISTICS_UPTIME_OS_UPTIME					"OsUptime"
#define XML_STATISTICS_UPTIME_DISPATCHER_UPTIME	"DispatcherUptime"
#define XML_STATISTICS_PROCESS_COMMAND_NAME			"CommandName"
#define XML_STATISTICS_PROCESS_ID								"Id"
#define XML_STATISTICS_PROCESS_OWNER_USER				"OwnerUser"
#define XML_STATISTICS_PROCESS_TOTAL_MEM_USAGE	"TotalMemUsage"
#define XML_STATISTICS_PROCESS_REAL_MEM_USAGE		"RealMemUsage"
#define XML_STATISTICS_PROCESS_VIRTUAL_MEM_USAGE	"VirtualMemUsage"
#define XML_STATISTICS_PROCESS_START_TIME				"StartTime"
#define XML_STATISTICS_PROCESS_TOTAL_TIME				"TotalTime"
#define XML_STATISTICS_PROCESS_USER_TIME				"UserTime"
#define XML_STATISTICS_PROCESS_SYSTEM_TIME			"SystemTime"
#define XML_STATISTICS_PROCESS_STATE						"State"
#define XML_STATISTICS_NET_IFACE_SYSTEM_NAME		"SystemName"
#define XML_STATISTICS_NET_IFACE_IN_SIZE				"InDataSize"
#define XML_STATISTICS_NET_IFACE_OUT_SIZE				"OutDataSize"
#define XML_STATISTICS_NET_IFACE_IN_PKGS_COUNT	"InPkgsCount"
#define XML_STATISTICS_NET_IFACE_OUT_PKGS_COUNT	"OutPkgsCount"
#define XML_STATISTICS_SESSION_USER_NAME				"UserName"
#define XML_STATISTICS_SESSION_SERVICE_NAME			"ServiceName"
#define XML_STATISTICS_SESSION_HOST_NAME				"HostName"
#define XML_STATISTICS_SESSION_DURATION_TIME		"DurationTime"

/**
 * Problem Report XML model
 */

//
// XML tree basic elements
//
#define XML_BUGREP_EL_ROOT							"ParallelsProblemReport"
#define XML_BUGREP_EL_PREAMBLE						"Preamble"
#define XML_BUGREP_EL_TYPE							"Type"
#define XML_BUGREP_EL_SERVER_VER					"ServerVersion"
#define XML_BUGREP_EL_CLIENT_VER					"ClientVersion"
#define XML_BUGREP_EL_HOST_STAT						"HostStatistic"
#define XML_BUGREP_EL_PROBLEM_TYPE_DESCR			"ProblemTypeDescr"
#define XML_BUGREP_EL_COMPUTER_MODEL				"ComputerModel"
#define XML_BUGREP_EL_LIC_INFO						"LicenseInfo"
#define XML_BUGREP_EL_CONTACT_INFO					"ContactInfo"
#define XML_BUGREP_EL_HOST_INFO						"HostInfo"
#define XML_BUGREP_EL_MORE_HOST_INFO				"MoreHostInfo"
#define XML_BUGREP_EL_ALL_PROCESSES					"AllProcesses"
#define XML_BUGREP_EL_PRIMARY_OS					"HostPrimaryOs"
#define XML_BUGREP_EL_HOST_CPU						"HostCPU"
#define XML_BUGREP_EL_HOST_MEM						"HostMemory"
#define XML_BUGREP_EL_HOST_DISPLAY					"HostDisplay"
#define XML_BUGREP_EL_HOST_ENVIRON					"HostEnvironment"
#define XML_BUGREP_EL_APP_CONFIG					"AppConfig"
#define XML_BUGREP_EL_APP_CURR_VM_CFG				"CurrentVm"
#define XML_BUGREP_EL_APP_OVERALL_MEM				"OverallMemory"
#define XML_BUGREP_EL_NET_PREFS						"NetworkPrefs"
#define XML_BUGREP_EL_NET_CONFIG					"NetConfig"
#define XML_BUGREP_EL_AUTOUPDATE					"Autoupdate"
#define XML_BUGREP_EL_UI_PREFS						"UserInterface"
#define XML_BUGREP_EL_BOOTING						"GuestBooting"
#define XML_BUGREP_EL_GUEST_MEMORY					"GuestMemory"
#define XML_BUGREP_EL_GUEST_SCREEN_DATA				"GuestScreenData"
#define XML_BUGREP_EL_GUEST_DEVICES					"GuestDevices"
#define XML_BUGREP_EL_GUEST_SERIAL_PORTS			"GuestSerialPorts"
#define XML_BUGREP_EL_GUEST_PARALLEL_PORTS			"GuestParallelPorts"
#define XML_BUGREP_EL_GUEST_FLOPPY_DISKS			"GuestFloppyDisks"
#define XML_BUGREP_EL_GUEST_HARD_DISKS				"GuestHardDisks"
#define XML_BUGREP_EL_GUEST_CDROMS					"GuestCdRoms"
#define XML_BUGREP_EL_GUEST_NET_ADAPTERS			"GuestNetAdapters"
#define XML_BUGREP_EL_MONITOR_FLAGS					"MonitorFlags"
#define XML_BUGREP_EL_GUEST_CPU_STATE				"GuestCpuState"
#define XML_BUGREP_EL_MONITOR_DATA					"MonitorData"
#define XML_BUGREP_EL_PERF_COUNTERS					"PerformanceCounters"
#define XML_BUGREP_EL_USER_DEFINED_DATA				"UserDefined"
#define XML_BUGREP_EL_PROBLEM_DESCR					"ProblemDescription"
#define XML_BUGREP_EL_SCREEN_SHOTS					"ScreenShots"
#define XML_BUGREP_EL_SYSTEM_LOGS					"SystemLogs"
#define XML_BUGREP_EL_GUEST_OS_INFORMATION			"GuestOs"
#define XML_BUGREP_EL_ADVANCED_VM_INFO				"AdvancedVmInfo"
#define XML_BUGREP_EL_DISK_INFO						"DiskInfo"
#define XML_BUGREP_EL_KM_PROFILES					"KeyboardMouseProfiles"
#define XML_BUGREP_EL_AUTO_STATISTIC_INFO			"AutoStatisticInfo"
#define XML_BUGREP_EL_CRASH_DUMP				"CrashDump"
#define XML_BUGREP_EL_AUTOSTAT_DISP_CONFIG			"AutoStatDispConfig"
#define XML_BUGREP_EL_AUTOSTAT_VMCATALOGUE			"AutoStatVmCatalogue"
#define XML_BUGREP_EL_AUTOSTAT_VMLIST				"AutoStatVmList"
#define XML_BUGREP_EL_AUTOSTAT_VMCONFIG				"AutoStatVmConfig"


//
// XML tree subsequent nodes
//
#define XML_BUGREP_ND_PRODUCT_NAME					"ProductName"
#define XML_BUGREP_ND_PRODUCT_VERSION				"ProductVersion"
#define XML_BUGREP_ND_PRODUCT_ID					"ProductId"
#define XML_BUGREP_ND_PRODUCT_VALID_START			"ProductValidFrom"
#define XML_BUGREP_ND_PRODUCT_VALID_END				"ProductValidTo"
#define XML_BUGREP_ND_PRODUCT_ACTIVE_STATUS			"ActivationStatus"
#define XML_BUGREP_ND_OVERALL_MAX_LIM				"MaxLimit"
#define XML_BUGREP_ND_OVERALL_ASSIGNED_LIM			"AssignedLimit"
#define XML_BUGREP_ND_MEM_USED						"MemoryUsed"
#define XML_BUGREP_ND_VMS_RUNNING					"VMsRunning"
#define XML_BUGREP_ND_DHCP_SCOPE_START				"ScopeStartIp"
#define XML_BUGREP_ND_DHCP_SCOPE_END				"ScopeEndIp"
#define XML_BUGREP_ND_DHCP_SCOPE_MASK				"ScopeMask"
#define XML_BUGREP_ND_LAST_UPDATE					"LastUpdate"
#define XML_BUGREP_ND_RELEASE_KEY					"ReleaseKeySeq"
#define XML_BUGREP_ND_FSM_KEY						"FsmKeySeq"
#define XML_BUGREP_ND_BOOT_DEVICES					"GuestBootDevices"
#define XML_BUGREP_ND_BOOT_SEQUENCE					"GuestBootSequence"
#define XML_BUGREP_ND_GUEST_MEM_SIZE				"GuestMemSize"
#define XML_BUGREP_ND_GUEST_MEM_BLK_SIZE			"GuestMemBlockSize"
#define XML_BUGREP_ND_GUEST_WORKSET_CUR				"GuestWorkingSetCur"
#define XML_BUGREP_ND_GUEST_WORKSET_MAX				"GuestWorkingSetMax"
#define XML_BUGREP_ND_GUEST_RESOLUTION				"Resolution"
#define XML_BUGREP_ND_GUEST_VESA_MODE				"VesaMode"
#define XML_BUGREP_ND_GUEST_SERIAL_PORT				"GuestSerialPort"
#define XML_BUGREP_ND_GUEST_PARALLEL_PORT			"GuestParallelPort"
#define XML_BUGREP_ND_GUEST_FLOPPY_DISK				"GuestFloppyDisk"
#define XML_BUGREP_ND_GUEST_HARD_DISK				"GuestHardDisk"
#define XML_BUGREP_ND_GUEST_CDROM					"GuestCdRom"
#define XML_BUGREP_ND_GUEST_NET_ADAPTER				"GuestNetAdapter"
#define XML_BUGREP_ND_GUEST_SOUND					"GuestSound"
#define XML_BUGREP_ND_GUEST_USB						"GuestUsb"
#define XML_BUGREP_ND_GUEST_PIC_DEVICES				"GuestPciDevices"
#define XML_BUGREP_ND_SCREEN_SHOT					"ScreenShot"
#define XML_BUGREP_ND_SYSTEM_LOG					"SystemLog"
#define XML_BUGREP_ND_CONTACT_INFO_NAME				"Name"
#define XML_BUGREP_ND_CONTACT_INFO_E_MAIL			"EMail"
#define XML_BUGREP_ND_CRASH_DUMP_PATH				"Path"
#define XML_BUGREP_ND_CRASH_DUMP_DATE_TIME				"CreationDateTime"
#define XML_BUGREP_ND_CRASH_DUMP_PID				"ApplicationPid"
#define XML_BUGREP_ND_CRASH_DUMP_BASE64				"Dump"

/**
 * Client Preferences XML model
 */

#define XML_PREFS_EL_ROOT		"ParallelsClientPreferences"
#define XML_VM_DISP_EL_HOTKEY_GENERIC				"HotkeyCombination"
#define XML_VM_DISP_EL_ANIMATION						"Animation"
#define XML_VM_DISP_EL_PRESENTATION					"Presentation"
#define XML_VM_DISP_EL_MAIN_WINDOW					"MainWindow"
#define XML_VM_DISP_EL_SHARED_WEB						"SharedWeb"
#define XML_VM_DISP_EL_APPEARANCE						"Appearance"
#define XML_VM_DISP_EL_KEYBOARD							"Keyboard"

//
// XML tree subsequent nodes
//
#define XML_VM_DISP_ND_ANIMATION_FULL_SCREEN   "FullScreenTransition"
#define XML_VM_DISP_ND_ANIMATION_OS_WINDOW   "OsWindowTransition"
#define XML_VM_DISP_ND_ANIMATION_SPEED   "AnimationSpeed"
#define XML_VM_DISP_ND_ANIMATION_COHERENCE   "AnimateCoherence"
#define XML_VM_DISP_ND_ANIMATION_SUSPEND_AND_RESUME   "AnimateSuspendAndResume"
#define XML_VM_DISP_ND_MAIN_WINDOW_TOOLBAR_PLACEMENT   "ToolbarPlacement"
#define XML_VM_DISP_ND_MAIN_WINDOW_SIDEBAR_PLACEMENT   "SideBarPlacement"
#define XML_VM_DISP_ND_MAIN_WINDOW_SHOW_TOOLBAR   "ShowToolbar"
#define XML_VM_DISP_ND_MAIN_WINDOW_SHOW_SIDEBAR   "ShowSideBar"
#define XML_VM_DISP_ND_SHARED_WEB_WEBAPP				"Web"
#define XML_VM_DISP_ND_SHARED_WEB_MAILAPP				"Mail"
#define XML_VM_DISP_ND_SHARED_WEB_FTPAPP				"Ftp"
#define XML_VM_DISP_ND_APPEARANCE_APPINDOCK			"AppInDock"
#define XML_VM_DISP_ND_APPEARANCE_SCREENSHOTINDOCK			"DockIcon"
#define XML_VM_DISP_ND_APPEARANCE_OSRESOLUTION			"OsResolutionInFullScreen"
#define XML_VM_DISP_ND_APPEARANCE_HIDDEN_MESSAGES			"HiddenMessages"
#define XML_VM_DISP_ND_APPEARANCE_HIDDEN_MESSAGE_ID			"MessageId"
#define XML_VM_DISP_ND_MOUSE_RIGHTCLICK_DELAYED			"MouseRightClickDelayed"
#define XML_VM_DISP_ND_MOUSE_RIGHTCLICK_DELAY_RATE			"MouseRightClickDelayRate"
#define XML_VM_DISP_ND_CMD_KEYS_REMAP			"CommandKeysRemap"
#define XML_VM_DISP_ND_NUMENTER_TO_ALTGR			"TranslateNumEnterToAltGr"

/**
 * Parallels Updater XML model
 */

//
// XML tree basic elements
//
#define XML_UPD_UPDATER_CONFIG_EL_ROOT		"ParallelsUpdaterConfig"
#define XML_UPD_UPDATER_CONFIG_EL_AUTO		"AutomaticUpdate"
#define XML_UPD_UPDATES_EL_ROOT					"ParallelsUpdates"
#define XML_UPD_UPDATES_EL_PRODUCT				"Product"
#define XML_UPD_UPDATES_EL_PRODUCT_VER		"Version"
#define XML_UPD_UPDATES_EL_LOCALE					"Locale"
#define XML_UPD_UPDATES_EL_DISTRIBUTOR			"Distributor"
#define XML_UPD_UPDATES_EL_UPDATE					"Update"

//
// XML tree subsequent nodes
//
#define XML_UPD_UPDATER_CONFIG_ND_UPDATE_URL					"UpdateUrl"
#define XML_UPD_UPDATER_CONFIG_ND_REMIND_PERIOD			"RemindPeriod"
#define XML_UPD_UPDATER_CONFIG_ND_BUY_LINK						"BuyLink"
#define XML_UPD_UPDATER_CONFIG_ND_DOWNLOAD_PATH			"DownloadPath"
#define XML_UPD_UPDATER_CONFIG_ND_UPDATE_ENABLED			"UpdateEnabled"
#define XML_UPD_UPDATER_CONFIG_ND_UPDATE_PERIOD			"UpdatePeriod"
#define XML_UPD_UPDATER_CONFIG_ND_DOWNLOAD_ENABLED	"DownloadEnabled"
#define XML_UPD_UPDATER_CONFIG_ND_INSTALL_ENABLED			"InstallEnabled"
#define XML_UPD_UPDATER_CONFIG_ND_NOTIFY_USER				"NotifyUser"

#define XML_UPD_UPDATES_ND_PRODUCT_NAME			"ProductName"
#define XML_UPD_UPDATES_ND_UPDATE_ENABLED			"UpdateEnabled"
#define XML_UPD_UPDATES_ND_VERSION_MAJOR			"Major"
#define XML_UPD_UPDATES_ND_VERSION_MINOR			"Minor"
#define XML_UPD_UPDATES_ND_VERSION_SUBMINOR		"SubMinor"
#define XML_UPD_UPDATES_ND_VERSION_SUBSUBMINOR		"SubSubMinor"
#define XML_UPD_UPDATES_ND_VERSION_STRING_REP	"StringRepresentation"
#define XML_UPD_UPDATES_ND_FILE_PATH						"FilePath"
#define XML_UPD_UPDATES_ND_FILE_SIZE						"FileSize"
#define XML_UPD_UPDATES_ND_NAME							"UpdateName"
#define XML_UPD_UPDATES_ND_DESCRIPTION				"UpdateDescription"
#define XML_UPD_UPDATES_ND_STATUS						"Status"
#define XML_UPD_UPDATES_ND_DATE_TIME					"DateTime"
#define XML_UPD_UPDATES_ND_CHARGEABLE					"Chargeable"
#define XML_UPD_UPDATES_ND_LOCALE_NAME				"LocaleName"
#define XML_UPD_UPDATES_ND_DISTRIBUTOR_NAME		"DistributorName"
#define XML_UPD_UPDATES_ND_OSTYPE							"OsType"
#define XML_UPD_UPDATES_ND_ANCESTRY						"Ancestry"
#define XML_UPD_UPDATES_ND_ANCESTOR					"Ancestor"

/**
 * Common XML tree nodes
 */

#define XML_VM_ND_PATH		"Path"
#define XML_VM_ND_NAME	"Name"
#define XML_VM_ND_TYPE		"Type"
#define XML_VM_ND_SIZE		"Size"
#define XML_VM_ND_VALUE	"Value"
#define XML_VM_ND_IS_LIST	"IsList"
#define XML_VM_ND_LIST_ITEM_VALUE	"ListItem"
#define XML_VM_ND_UUID		"Uuid"
#define XML_VM_ND_CDATA	"Data"
#define XML_VM_ND_INDEX	"Index"
#define XML_VM_ND_INUSE	"InUse"
#define XML_VM_ND_ENABLED	"Enabled"
#define XML_VM_ND_LOGICAL	"Logical"
#define XML_VM_ND_ACTIVE	"Active"
#define XML_VM_ND_MODIFIED	"Modified"
#define XML_VM_ND_PERMISSIONS	"Permissions"
#define XML_VM_ND_SIZE_ON_DISK	"SizeOnDisk"
#define XML_VM_ND_AUTO_MOUNT	"AutoMount"
#define XML_VM_ND_ENABLE_SPOTLIGHT	"EnableSpotlight"
#define XML_VM_ND_FS_TYPE		"FsType"

/**
 * Common XML tree attributes
 */

#define XML_VM_ATTR_SCHEMA_VERSION					"schemaVersion"
#define XML_VM_ATTR_APP_VERSION					"appVersion"
#define XML_VM_ATTR_TIMESTAMP						"created"
#define XML_VM_ATTR_ENABLED							"enabled"
#define XML_VM_ATTR_VALUE							"value"
#define XML_VM_ATTR_INDEX							"index"
#define XML_VM_ATTR_SERIAL							"serial"
#define XML_VM_ATTR_NAME							"name"
#define XML_VM_ATTR_SYS_NAME							"sys_name"
#define XML_VM_ATTR_UUID							"uuid"
#define XML_VM_ATTR_IS_REGISTERED					"isRegistered"
#define XML_VM_ATTR_IS_VALID						"isValid"
#define XML_VM_ATTR_ACTION							"action"
#define XML_VM_ATTR_TYPE							"type"
#define XML_VM_ATTR_SIZE							"size"
#define XML_VM_ATTR_EXTENDED						"extended"
#define XML_VM_ATTR_LOGICAL							"logical"
#define XML_VM_ATTR_INUSE							"inuse"
#define XML_VM_ATTR_ACTIVE							"active"
#define XML_VM_ATTR_MODIFIED						"modified"
#define XML_VM_ATTR_PERMISSIONS						"permissions"
#define XML_VM_ATTR_STATE							"state"
#define XML_VM_ATTR_PATH							"path"
#define XML_VM_ATTR_CYLINDERS						"cyl"
#define XML_VM_ATTR_HEADS							"hds"
#define XML_VM_ATTR_SECTORS							"sct"
#define XML_VM_ATTR_MACADDR							"macAddr"
#define XML_VM_ATTR_BOOTNUMBER						"bootingNumber"
#define XML_VM_ATTR_TOBE_REMOVED                    "toBeRemoved"

/**
 * Macro-definitions used in XML procedures
 */

#define IS_OPERATION_SUCCEEDED(x)					(x?0:1)
#define IS_DEFAULT_PARAM_UINT(x)					((x==(unsigned int)-1)?1:0)
#define IS_DEFAULT_PARAM_STR(x)						((QString::compare(x,"FFFFFFFF")==0)?1:0)
#define IS_DEFAULT_DATE(x)							((x==QDate::fromString( XML_DEFAULT_DATE,XML_DEFAULT_DATE_FORMAT ))?1:0)
#define IS_DEFAULT_TIME(x)							((x==QTime::fromString( XML_DEFAULT_TIME,XML_DEFAULT_TIME_FORMAT ))?1:0)

/**
 * Event parameters names
 */

#define EVT_PARAM_OP_CODE							"op_code"
#define EVT_PARAM_VM_UUID							"vm_uuid"
#define EVT_PARAM_VM_NAME							"vm_name"
#define EVT_PARAM_VM_CONFIG_PATH					"vm_config_path"
#define EVT_PARAM_VM_DIR_UUID						"vm_dir_uuid"
#define EVT_PARAM_VM_CONFIG							"vm_cfg"
#define EVT_PARAM_VM_CONFIG_OLD							"vm_cfg_old"
#define EVT_PARAM_VM_CONFIG_SECTION					"vm_cfg_section"
#define EVT_PARAM_OP_RC								"op_rc"
#define EVT_PARAM_IOFLOW_KEY						"ioflow_key"
#define EVT_PARAM_RETURN_PARAM_TOKEN				"param_token"
#define EVT_PARAM_VM_MESSAGE_TYPE					"message_type"
#define EVT_PARAM_VM_LICENSE						"vm_license"
#define EVT_PARAM_VM_RUNTIME_STATUS					"vm_runtime_status"
#define EVT_PARAM_VM_GUEST_TOOL_STATUS				"vm_guest_tool_status"
#define EVT_PARAM_VM_KERNEL_STATUS					"vm_kernel_status"
#define EVT_PARAM_VM_CONFIG_ACTION					"vm_config_action"
#define EVT_PARAM_VM_CONFIG_DEV_STATE				"vm_config_dev_state"
#define EVT_PARAM_VM_CONFIG_DEV_IMAGE				"vm_config_dev_image"
#define EVT_PARAM_VM_CONFIG_DEV_NAME				"vm_config_dev_name"
#define EVT_PARAM_VM_CONFIG_DEV_INDEX				"vm_config_dev_index"
#define EVT_PARAM_VM_CONFIG_DEV_ITEM_ID			"vm_config_dev_item_id"
#define EVT_PARAM_VM_CONFIG_DEV_CONNECT_STATE		"vm_config_dev_connect_state"
#define EVT_PARAM_VM_ANSWER_CODE					"VM_ANSWER_CODE"
#define EVT_PARAM_VM_PROBLEM_REPORT					"vm_problem_report"
#define EVT_PARAM_VM_PROBLEM_REPORT_DIR_PATH		"vm_problem_report_dir_path"
#define EVT_PARAM_VM_PROBLEM_REPORT_VERSION			"vm_problem_report_version"
#define EVT_PARAM_DISP_COMMON_PREFERENSES			"disp_common_prefs"
#define EVT_PARAM_USE_ACPI							"use_acpi"
#define EVT_PARAM_DSP_SESSION_ID					"dsp_session_id"
#define EVT_PARAM_STATISTICS						"statistics"
#define EVT_PARAM_VM_SECURITY_INFO					"vm_security_info"
#define EVT_PARAM_BUFF_TYPE							"buff_type"
#define EVT_PARAM_BUFF_SIZE							"buff_size"
#define EVT_PARAM_DETAIL_DESCRIPTION				"Details"
#define EVT_PARAM_VM_OS_NUMBER						"vm_os_number"
#define EVT_PARAM_VM_SEARCH_INFO					"vm_search_info"
#define EVT_PARAM_VM_OLD_CONFIG						"vm_old_config"
#define EVT_PARAM_VM_IS_TEMPLATE					"vm_is_template"
#define EVT_PARAM_VM_VALIDATE_CONFIG_ERROR			"vm_validate_config_error"
#define EVT_PARAM_GENERATED_VM_NAME					"vm_generated_name"
#define EVT_PARAM_NETWORK_PREFERENSES				"vm_network_config_prefs"
#define EVT_PARAM_HOST_HARDWARE_INFO				"host_hardware_info"
#define EVT_PARAM_HOST_HARDWARE_INFO_GEN			"host_hardware_info_gen"
#define EVT_PARAM_HOST_USER_SYS_NAME				"host_user_sys_name"
#define EVT_PARAM_HOST_USER_FRIENDLY_NAME			"host_user_friendly_name"
#define EVT_PARAM_HOST_USER_ICON					"host_user_icon"
#define EVT_PARAM_DEV_GENERIC_PCI_TYPE				"dev_generic_pci_type"
#define EVT_PARAM_HDD_HAS_STATES					"hdd_has_states"
#define EVT_PARAM_DOWNLOAD_PERCENT					"download_percent"
#define EVT_PARAM_DOWNLOAD_ESTIMATED_TIME			"download_estimated_time"
#define EVT_PARAM_DOWNLOADED_BYTES					"downloaded_bytes"
#define EVT_PARAM_DOWNLOAD_FILE_SIZE				"download_file_size"
#define EVT_PARAM_DOWNLOAD_RATE						"download_rate"
#define EVT_PARAM_APPLIANCE_ID						"appliance_id"
#define EVT_PARAM_SNAPSHOT_UUID						"snapshot_uuid"
#define EVT_PARAM_WRITING_FILE_STRING				"writing_file_string"
#define EVT_PARAM_USB_DEVICE_TYPE					"usb_device_type"
#define EVT_PARAM_HOST_HW_INFO						"host_hw_info"
#define EVT_PARAM_LICENSE_VOLUME_INFO				"license_info"
#define EVT_PARAM_LICENSE_TRIAL_INFO				"license_trial_info"
#define EVT_PARAM_LICENSE_LANG						"license_lang"
#define EVT_PARAM_DISPLAY_ENCODINGS					"display_encodings"
#define EVT_PARAM_AUDIO_ENCODINGS					"audio_encodings"
#define EVT_PARAM_NETWORK_ADAPTER_TYPE				"network_adapter_type"
#define EVT_PARAM_VM_INFO							"vm_tis_backup"
#define EVT_PARAM_HW_DEVICE_CHANGED_FLAGS			"hw_dev_changed_flags"
#define EVT_PARAM_HW_DEVICE_CONFIG					"hw_dev_config"
#define EVT_PARAM_SEND_RECEIVE_TIMEOUT				"send_receive_timeout"
#define EVT_PARAM_CONNECTION_TIMEOUT				"connection_timeout"
#define EVT_PARAM_CFSN_BUNDLE_ID                    "coherence_fake_stub_id"
#define EVT_PARAM_CFSN_BUNDLE_NAME                  "coherence_fake_stub_name"
#define EVT_PARAM_CFSN_BUNDLE_PATH                  "coherence_fake_stub_path"
#define EVT_PARAM_SESSION_INFO						"session_info"
#define EVT_PARAM_VM_DEVICES_ACTIVITY				"vm_devices_activity"
#define EVT_PARAM_SRV_SEND_PROBLEM_REPORT_DATA      "problemreport_data"

/**
 * it is for complex events param name
 *
*/
#define EVT_PARAM_COMPLEX_EVENT						"internal_event"
/**
 * This field will hold an identifier that will be translated to the message,
 * but this message may have some additional parameters left like "Hello %0 world %1"
 */
#define EVT_PARAM_MESSAGE							"vm_message"

/**
 * These fields contain strings, that will be parsed as parameters to the EVT_PARAM_MESSAGE
 */
#define EVT_PARAM_MESSAGE_PARAM_PREFIX				"vm_message_param_"
#define EVT_PARAM_MESSAGE_PARAM_0					EVT_PARAM_MESSAGE_PARAM_PREFIX "0"
#define EVT_PARAM_MESSAGE_PARAM_1					EVT_PARAM_MESSAGE_PARAM_PREFIX "1"
#define EVT_PARAM_MESSAGE_PARAM_2					EVT_PARAM_MESSAGE_PARAM_PREFIX "2"
#define EVT_PARAM_MESSAGE_PARAM_3					EVT_PARAM_MESSAGE_PARAM_PREFIX "3"
#define EVT_PARAM_MESSAGE_PARAM_4					EVT_PARAM_MESSAGE_PARAM_PREFIX "4"
#define EVT_PARAM_MESSAGE_PARAM_n					EVT_PARAM_MESSAGE_PARAM_PREFIX "%1"
/**
 * These fields contain identifiers, - some messages may be treated as questions.
 * In this case they will have identifiers that will be translated to the buttons.
 */
#define EVT_PARAM_MESSAGE_CHOICE_PREFIX				"vm_message_choice_"
#define EVT_PARAM_MESSAGE_CHOICE_0					EVT_PARAM_MESSAGE_CHOICE_PREFIX "0"
#define EVT_PARAM_MESSAGE_CHOICE_1					EVT_PARAM_MESSAGE_CHOICE_PREFIX "1"
#define EVT_PARAM_MESSAGE_CHOICE_2					EVT_PARAM_MESSAGE_CHOICE_PREFIX "2"
#define EVT_PARAM_MESSAGE_CHOICE_n					EVT_PARAM_MESSAGE_CHOICE_PREFIX "%1"

// Dispatcher dump nodes
//
////////////////////////////////////////////////////////////////////////////////

#define XML_VM_DISPATCHER_DUMP_ROOT			"VmDispatcherDump"
#define XML_VM_DISPATCHER_DUMP_SET			"DumpSet"
#define XML_VM_DISPATCHER_DUMP_VM_UUID		"vmUuid"
#define XML_VM_DISPATCHER_DUMP_OBJ_UUID		"objectUuid"
#define XML_VM_DISPATCHER_DUMP_VM_PID		"vmPid"
#define XML_VM_DISPATCHER_DUMP_DISP_PID		"dispPid"
#define XML_VM_DISPATCHER_DUMP_STARTED		"started"
#define XML_VM_DISPATCHER_DUMP_CHANGED		"changed"

////////////////////////////////////////////////////////////////////////////////
//
#define EVT_PARAM_INITIAL_REQ_UUID					"initial_req_uuid"
#define EVT_PARAM_PROGRESS_CHANGED					"progress_changed"
#define EVT_PARAM_PROGRESS_STAGE					"progress_stage"
#define EVT_PARAM_DEVICE_TYPE						"device_type"
#define EVT_PARAM_DEVICE_INDEX						"device_index"
#define EVT_PARAM_EMULATED_TYPE						"device_emulated_type"
#define EVT_PARAM_EXECUTIVE_SERVER					"executive_server"
#define EVT_PARAM_COMPACTED_DISK_PATH               "compacted_disk_path"
#define EVT_PARAM_COMPACTED_DISK_ERR_CODE           "compacted_disk_err_code"
#define EVT_PARAM_USB_CONNECTION_TYPE	            "usb_connection_type"
#define EVT_PARAM_USB_ASSOC_LIST		            "usb_assoc_list"
#define EVT_PARAM_USB_ASSOC_LIST_VERSION		    "usb_assoc_list_ver"

#define EVT_PARAM_PROGRESS_CHANGED_TOTAL			"progress_changed_total"
#define EVT_PARAM_PROGRESS_CHANGED_DONE				"progress_changed_done"

////////////////////////////////////////////////////////////////////////////////
//
#define EVT_PARAM_HARD_DISKS_SIZE					"hard_disks_size"
#define EVT_PARAM_AFTER_COMPACT_HARD_DISKS_SIZE		"after_compact_hard_disks_size"

////////////////////////////////////////////////////////////////////////////////
//
#define EVT_PARAM_SESSION_UUID					"session_uuid"
#define EVT_PARAM_SESSION_USERNAME				"session_username"
#define EVT_PARAM_SESSION_HOSTNAME           "session_hostname"
#define EVT_PARAM_SESSION_ACTIVITY_TIME      "session_activity_time"
#define EVT_PARAM_SESSION_ALLOWED_VM    	   "session_allowed_vm"
#define EVT_PARAM_SESSION_LICENSE_STATUS			"session_license_status"
#define EVT_PARAM_SESSION_LICENSE_IS_CONFIRMED "session_license_is_confirmed"
#define EVT_PARAM_SESSION_LICENSE_INFO				 "session_license_info"
#define EVT_PARAM_SESSION_RESTRICT_EVENT			"session_rectrict_event"
#define EVT_PARAM_SESSION_RESTRICT_KEY_PREFIX		"session_rectrict_key_"
#define MAKE_EVT_PARAM_SESSION_RESTRICT_KEY(key)	(EVT_PARAM_SESSION_RESTRICT_KEY_PREFIX + QString("%1").arg(key))

#define EVT_PARAM_CMDHISTORY_TIMESTAMP       "cmdhistory_timestamp"
#define EVT_PARAM_CMDHISTORY_COMMAND_NUM     "cmdhistory_command_num"
#define EVT_PARAM_CMDHISTORY_PARAMS_AS_XML   "cmdhistory_paramlist_as_xml"
#define EVT_PARAM_CMDHISTORY_RESPONSED       "cmdhistory_responsed"
#define EVT_PARAM_CMDHISTORY_RETURNCODE      "cmdhistory_returncode"

#define EVT_PARAM_VMINFO_VM_STATE					"vminfo_vm_state"
#define EVT_PARAM_VMINFO_VM_ADDITION_STATE			"vminfo_vm_addition_state"
#define EVT_PARAM_VMINFO_VM_SECURITY				"vminfo_vm_security"
#define EVT_PARAM_VMINFO_VM_IS_INVALID				"vminfo_vm_is_invalid"
#define EVT_PARAM_VMINFO_VM_VALID_RC				"vminfo_vm_valid_rc"
#define EVT_PARAM_VMINFO_IS_VNC_SERVER_STARTED		"vminfo_vm_is_vnc_server_started"
#define EVT_PARAM_VMINFO_IS_VM_WAITING_FOR_ANSWER	"vminfo_is_vm_waiting_for_answer"
#define EVT_PARAM_VMINFO_VM_PROCESS_ID				"vminfo_vm_process_id"
#define EVT_PARAM_VMINFO_VM_TYPE				"vminfo_vm_type"

#define EVT_PARAM_VM_TOOLS_STATE	     "vm_tools_state"
#define EVT_PARAM_VM_TOOLS_VERSION	     "vm_tools_version"
#define EVT_PARAM_VM_TOOLS_LANGUAGE	     "vm_tools_language"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_PRL_LICENSE_IS_VALID				"license_is_valid"
#define EVT_PARAM_PRL_LICENSE_STATUS				"license_status"
#define EVT_PARAM_PRL_LICENSE_KEY					"license_key"
#define EVT_PARAM_PRL_LICENSE_USER					"license_user"
#define EVT_PARAM_PRL_LICENSE_COMPANY				"license_company"
#define EVT_PARAM_PRL_LICENSE_HAS_RESTRICTIONS		"license_has_restrictions"
#define EVT_PARAM_PRL_LICENSE_NODE_HARDWARE_ID		"license_node_hardware_id"
#define EVT_PARAM_PRL_LICENSE_OFFLINE_EXPIRATION_DATE "license_offline_expiration_date"
#define EVT_PARAM_PRL_LICENSE_ACTIVATION_ID "license_activation_id"
#define EVT_PARAM_PRL_LICENSE_DEACTIVATION_ID "license_deactivation_id"
#define EVT_PARAM_PRL_LICENSE_IS_CONFIRMED 	"license_is_confirmed"
#define EVT_PARAM_PRL_LICENSE_DEFERRED_ACTIVATION	"license_deferred_activation"
#define EVT_PARAM_PRL_LICENSE_ALLOW_TO_SKIP_CONFIRMATION "license_allow_to_skip_confirmation"
#define EVT_PARAM_PRL_LICENSE_REGISTRATION_STATUS "license_registration_status"
#define EVT_PARAM_PRL_PRODUCT_STORED_USER_CREDS_NAME "product_curr_cd"
#define EVT_PARAM_PRL_LICENSE_IS_UPGRADABLE			"license_is_upgradable"

// VZ license
#define	EVT_PARAM_PRL_VZLICENSE_GRACE_PERIOD		"vzlicense_graceperiod"
#define EVT_PARAM_PRL_VZLICENSE_FILE				"vzlicense_file"
#define EVT_PARAM_PRL_VZLICENSE_IS_EXPIRATION_UNLIM	"vzlicense_is_expiration_unlimited"
#define EVT_PARAM_PRL_VZLICENSE_EXPIRATION_DATE		"vzlicense_expiration_date"
#define EVT_PARAM_PRL_VZLICENSE_START_DATE			"vzlicense_start_date"
#define EVT_PARAM_PRL_VZLICENSE_UPDATE_DATE			"vzlicense_update_date"
#define EVT_PARAM_PRL_VZLICENSE_CPU_TOTAL			"vzlicense_cpu_total"
#define EVT_PARAM_PRL_VZLICENSE_MAX_MEMORY			"vzlicense_max_memory"
#define EVT_PARAM_PRL_VZLICENSE_PRODUCT				"vzlicense_product"
#define EVT_PARAM_PRL_VZLICENSE_VMS_TOTAL			"vzlicense_vms_total"
#define EVT_PARAM_PRL_VZLICENSE_KEY_NUMBER			"vzlicense_key_number"
#define EVT_PARAM_PRL_VZLICENSE_KEY_NUMBER_VALUE	"vzlicense_key_number_value"
#define EVT_PARAM_PRL_VZLICENSE_VZCC_USERS			"vzlicense_max_vzcc_users"
#define EVT_PARAM_PRL_VZLICENSE_VTD_AVAILABLE		"vzlicense_vtd_available"
#define EVT_PARAM_PRL_VZLICENSE_VERSION				"vzlicense_version"
#define EVT_PARAM_PRL_VZLICENSE_PLATFORM			"vzlicense_platform"
#define EVT_PARAM_PRL_VZLICENSE_HWID					"vzlicense_hardware_id"
#define EVT_PARAM_PRL_VZLICENSE_ORIGINAL_LICENSE_KEY	"vzlicense_original_license_key"
#define EVT_PARAM_PRL_VZLICENSE_SERIAL_NUMBER	"vzlicense_serial_number"
#define EVT_PARAM_PRL_VZLICENSE_IS_VOLUME			"vzlicense_is_volume"
#define EVT_PARAM_PRL_VZLICENSE_RKU_ALLOWED			"vzlicense_rku_allowed"
#define EVT_PARAM_PRL_VZLICENSE_HA_ALLOWED			"vzlicense_ha_allowed"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_PRL_SERVER_INFO_OS_VERSION			"server_info_os_version"
#define EVT_PARAM_PRL_SERVER_INFO_COMMANDS_PORT			"server_info_cmd_port"
#define EVT_PARAM_PRL_SERVER_INFO_SERVER_UUID			"server_info_server_uuid"
#define EVT_PARAM_PRL_SERVER_INFO_HOSTNAME				"server_info_hostname"
#define EVT_PARAM_PRL_SERVER_INFO_PRODUCT_VERSION		"server_info_product_version"
#define EVT_PARAM_PRL_SERVER_INFO_CONFIRMATION_MODE		"server_info_confirmation_mode"
#define EVT_PARAM_PRL_SERVER_INFO_APP_EXECUTE_MODE		"server_info_app_execute_mode"
#define EVT_PARAM_PRL_SERVER_INFO_START_TIME			"server_info_start_time"
#define EVT_PARAM_PRL_SERVER_INFO_START_TIME_MONOTONIC	"server_info_start_time_monotonic"

#define EVT_PARAM_PRL_SERVER_INFO_IS_LAUNCHD_MODE "server_info_is_launchd_mode"

#define EVT_PARAM_PRL_SUPPORTED_OSES_LIST				"prl_supported_oses_list"
#define EVT_PARAM_PRL_FEATURES_MATRIX					"prl_features_matrix"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_PROTO_REQUEST_OP_CODE			"proto_request_op_code"
#define EVT_PARAM_PROTO_ACCESS_TOKEN			"proto_access_token"
#define EVT_PARAM_PROTO_FIRST_STR_PARAM			"proto_first_str_param"
#define EVT_PARAM_PROTO_SECOND_STR_PARAM		"proto_second_str_param"
#define EVT_PARAM_PROTO_FORCE_QUESTIONS_SIGN	"proto_force_questions_sign"
#define EVT_PARAM_PROTO_CMD_FLAGS				"proto_command_flags"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_WS_RESPONSE_CMD_HOST_HARDWARE_INFO			"ws_response_cmd_host_hardware_info"
#define EVT_PARAM_WS_RESPONSE_CMD_HW_FILE_SYSTEM_INFO			"ws_response_cmd_hw_file_system_info"
#define EVT_PARAM_WS_RESPONSE_CMD_USER_PROFILE						"ws_response_cmd_user_profile"
#define EVT_PARAM_WS_RESPONSE_CMD_USER_INFO							"ws_response_cmd_user_info"
#define EVT_PARAM_WS_RESPONSE_CMD_HOST_COMMON_INFO				"ws_response_cmd_host_common_info"
#define EVT_PARAM_WS_RESPONSE_CMD_HOST_COMMON_INFO_NETWORK_CONFIG	"ws_response_cmd_host_common_info_network_config"
#define EVT_PARAM_WS_RESPONSE_CMD_PROBLEM_REPORT					"ws_response_cmd_problem_report"
#define EVT_PARAM_WS_RESPONSE_CMD_VM_EVENT								"ws_response_cmd_vm_event"
#define EVT_PARAM_WS_RESPONSE_CMD_VM_CONFIG								"ws_response_cmd_vm_config"
#define EVT_PARAM_WS_RESPONSE_CMD_SYSTEM_STATISTICS				"ws_response_cmd_system_statistics"
#define EVT_PARAM_WS_RESPONSE_CMD_ERROR										"ws_response_cmd_error"
#define EVT_PARAM_WS_RESPONSE_CMD_STANDARD_PARAM					"ws_response_cmd_standard_param"
#define EVT_PARAM_WS_RESPONSE_CMD_VM_DEVICE						"ws_response_cmd_vm_device"
#define EVT_PARAM_WS_RESPONSE_CMD_NET_SERVICE_STATUS		"ws_response_cmd_net_service_status"
#define EVT_PARAM_WS_RESPONSE_CMD_LOGIN_EVENT						"ws_response_cmd_login_event"
#define EVT_PARAM_WS_RESPONSE_CMD_SECURITY_INFO						"ws_response_cmd_security_info"
#define EVT_PARAM_WS_RESPONSE_CMD_USER_INFO_LIST				"ws_response_cmd_user_info_list"
#define EVT_PARAM_WS_RESPONSE_CMD_GET_PERFSTATS             "ws_response_cmd_get_perf_stats"
#define EVT_PARAM_WS_RESPONSE_CMD_GET_SNAPSHOTS_TREE				"ws_response_cmd_get_snapshots_tree"
#define EVT_PARAM_WS_RESPONSE_CMD_GET_BACKUPS_TREE				"ws_response_cmd_get_backups_tree"


////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_RESPONSE_LOGIN_CMD_SESSION				"response_login_cmd_session_uuid"
#define EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_INFO				"response_login_cmd_task_info"
#define EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_UUID				"response_login_cmd_task_id"
#define EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_TYPE				"response_login_cmd_task_type"
#define EVT_PARAM_RESPONSE_LOGIN_CMD_TASK_PARAMS			"response_login_cmd_task_params"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_LOGIN_CMD_USERNAME			"login_cmd_username"
#define EVT_PARAM_LOGIN_CMD_PASSWORD			"login_cmd_password"
#define EVT_PARAM_LOGIN_CMD_SOURCE_HOST			"login_cmd_source_host"
#define EVT_PARAM_LOGIN_CMD_SESSION_TO_RESTORE	"login_cmd_session_uuid_to_restore"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_LOGIN_LOCAL_CMD_USER_ID			"login_local_cmd_user_id"
#define EVT_PARAM_LOGIN_LOCAL_CMD_OPERATION_UUID	"login_local_cmd_operation_uuid"
#define EVT_PARAM_LOGIN_LOCAL_CMD_APPLICATION_MODE	"login_local_cmd_application_mode"
#define EVT_PARAM_LOGIN_LOCAL_CMD_PROCESS_ID		"login_local_cmd_process_id"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_LOGIN_LOCAL_STAGE2_CMD_OPERATION_UUID	"login_local_stage2_cmd_operation_uuid"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_BASIC_VM_CMD_VM_UUID			"basic_vm_cmd_vm_uuid"
#define EVT_PARAM_VM_CMD_SECOND_STR_PARAM		"vm_cmd_second_str_param"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_CMD_WITH_ACPI_SIGN_IS_USE_ACPI			"vm_cmd_with_acpi_sign_is_use_acpi"
#define EVT_PARAM_VM_CMD_STOP_MODE			"vm_cmd_vm_stop_mode"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_DEV_CMD_DEVICE_TYPE			"vm_dev_cmd_device_type"
#define EVT_PARAM_VM_DEV_CMD_DEVICE_INDEX			"vm_dev_cmd_device_index"
#define EVT_PARAM_VM_DEV_CMD_DEVICE_CONFIG		"vm_dev_cmd_device_config"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_CREATE_CMD_VM_CONFIG			"vm_create_cmd_vm_config"
#define EVT_PARAM_VM_CREATE_CMD_VM_HOME_PATH		"vm_create_cmd_vm_home_path"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_CLONE_CMD_VM_NAME			"vm_clone_cmd_vm_name"
#define EVT_PARAM_VM_CLONE_CMD_CREATE_TEMPLATE	"vm_clone_cmd_create_template"
#define EVT_PARAM_VM_CLONE_CMD_CHANGE_SID	"vm_clone_cmd_change_sid"
#define EVT_PARAM_VM_CLONE_CMD_VM_UUID		"vm_clone_cmd_vm_uuid"
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_START_EX_CMD_START_MODE				"vm_start_ex_cmd_start_mode"
#define EVT_PARAM_VM_START_EX_CMD_RESERVED					"vm_start_ex_cmd_reserved_parameter"
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_LOGIN			"vm_login_in_guest_cmd_user_login"
#define EVT_PARAM_VM_LOGIN_IN_GUEST_CMD_USER_PASSWORD		"vm_login_in_guest_cmd_user_password"
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_GUEST_CMD_SESSION_ID					"vm_guest_cmd_session_id"
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_GUEST_RUN_APP_CMD_PROGRAM_NAME			"vm_guest_run_app_cmd_program_name"
#define EVT_PARAM_VM_GUEST_RUN_APP_CMD_ARGUMENTS			"vm_guest_run_app_cmd_arguments"
#define EVT_PARAM_VM_GUEST_RUN_APP_CMD_ENV_VARS				"vm_guest_run_app_cmd_env_vars"
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_LOGIN_NAME	"vm_guest_set_user_passwd_cmd_user_login_name"
#define EVT_PARAM_VM_GUEST_SET_USER_PASSWD_CMD_USER_PASSWD	"vm_guest_set_user_passwd_cmd_user_passwd"
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_DELETE_CMD_VM_DEVICES_LIST	"vm_delete_cmd_vm_devices_list"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_CREATE_IMAGE_CMD_IMAGE_CONFIG			"create_image_cmd_image_config"
#define EVT_PARAM_CREATE_IMAGE_CMD_RECREATE_SIGN		"create_image_cmd_recreate_sign"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_COPY_IMAGE_CMD_IMAGE_CONFIG			"copy_image_cmd_image_config"
#define EVT_PARAM_COPY_IMAGE_CMD_NEW_IMAGE_NAME			"copy_image_cmd_new_image_name"
#define EVT_PARAM_COPY_IMAGE_CMD_TARGET_PATH			"copy_image_cmd_target_path"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_START_SEARCH_CONFIG_CMD_SEARCH_DIRS			"start_search_config_cmd_search_dirs"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_FS_RENAME_ENTRY_CMD_OLD_ENTRY_NAME			"fs_rename_entry_cmd_old_entry_name"
#define EVT_PARAM_FS_RENAME_ENTRY_CMD_NEW_ENTRY_NAME			"fs_rename_entry_cmd_new_entry_name"

////////////////////////////////////////////////////////////////////////////////

#define EVT_PARAM_CREATE_SNAPSHOT_NAME				"vm_create_snapshot_name"
#define EVT_PARAM_CREATE_SNAPSHOT_DESCRIPTION			"vm_create_snapshot_description"
#define EVT_PARAM_CREATE_SNAPSHOT_UUID				"vm_create_snapshot_uuid"
#define EVT_PARAM_CREATE_SNAPSHOT_CREATOR			"vm_create_snapshot_creator"
#define EVT_PARAM_CREATE_SNAPSHOT_PATH				"vm_create_snapshot_path"

////////////////////////////////////////////////////////////////////////////////

#define EVT_PARAM_SWITCH_TO_SNAPSHOT_UUID				"vm_switch_to_snapshot_uuid"
#define EVT_PARAM_SWITCH_TO_SNAPSHOT_VM_STATE			"vm_switch_to_snapshot_vm_state"

#define EVT_PARAM_INTERNAL_CMD_NAME					"vm_internal_cmd_name"
#define EVT_PARAM_INTERNAL_CMD_ARGUMENTS			"vm_internal_cmd_arglist"

////////////////////////////////////////////////////////////////////////////////

#define EVT_PARAM_DELETE_SNAPSHOT_UUID				"vm_delete_snapshot_uuid"
#define EVT_PARAM_DELETE_SNAPSHOT_CHILD_SIGN			"vm_delete_snapshot_child_sign"
#define EVT_PARAM_DELETE_SNAPSHOT_MERGE_SIGN			"vm_delete_snapshot_merge_sign"
#define EVT_PARAM_DELETE_SNAPSHOT_WAS_STARTED_SIGN		"vm_delete_snapshot_was_started_sign"

////////////////////////////////////////////////////////////////////////////////

#define EVT_PARAM_UPDATE_SNAPSHOT_DATA_UUID				"vm_update_snapshot_data_uuid"
#define EVT_PARAM_UPDATE_SNAPSHOT_DATA_NAME				"vm_update_snapshot_data_name"
#define EVT_PARAM_UPDATE_SNAPSHOT_DATA_DESCRIPTION		"vm_update_snapshot_data_description"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_RESIZE_DISK_CMD_IMAGE				"vm_resize_disk_image"
#define EVT_PARAM_RESIZE_DISK_CMD_SIZE				"vm_resize_disk_size"
#define EVT_PARAM_RESIZE_DISK_CMD_RESERVED_FLAGS		"vm_resize_disk_reserved_flags"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_COMFIRMATION_MODE_CMD_USERNAME			"confirmation_mode_cmd_username"
#define EVT_PARAM_COMFIRMATION_MODE_CMD_PASSWORD			"confirmation_mode_cmd_password"
#define EVT_PARAM_COMFIRMATION_MODE_CMD_ENABLE_FLAG			"confirmation_mode_cmd_enable_sign"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_STORAGE_CMD_KEY								"storage_cmd_key"
#define EVT_PARAM_STORAGE_CMD_VALUE								"storage_cmd_value"

////////////////////////////////////////////////////////////////////////////////////

#define EVT_PARAM_BACKUP_PROTO_VERSION			"backup_proto_version"
#define EVT_PARAM_BACKUP_CMD_VM_UUID			"backup_cmd_vm_uuid"
#define EVT_PARAM_BACKUP_CMD_VM_NAME			"backup_cmd_vm_name"
#define EVT_PARAM_BACKUP_CMD_SERVER_HOSTNAME		"backup_cmd_server_hostname"
#define EVT_PARAM_BACKUP_CMD_SERVER_PORT		"backup_cmd_server_port"
#define EVT_PARAM_BACKUP_CMD_SERVER_SESSION_UUID	"backup_cmd_server_session_uuid"
#define EVT_PARAM_BACKUP_CMD_TARGET_VM_HOME_PATH	"backup_cmd_target_vm_home_path"
#define EVT_PARAM_BACKUP_CMD_TARGET_VM_NAME		"backup_cmd_target_vm_name"
#define EVT_PARAM_BACKUP_CMD_RESERVED_FLAGS		"backup_cmd_reserved_flags"
#define EVT_PARAM_BACKUP_CMD_BACKUP_UUID		"backup_cmd_backup_uuid"
#define EVT_PARAM_BACKUP_CMD_BACKUP_NUMBER		"backup_cmd_backup_number"
#define EVT_PARAM_BACKUP_CMD_HOST			"backup_cmd_host"
#define EVT_PARAM_BACKUP_CMD_SERVER_UUID		"backup_cmd_server_uuid"
#define EVT_PARAM_BACKUP_CMD_VM_CONFIG			"backup_cmd_vm_config"
#define EVT_PARAM_BACKUP_CMD_TREE			"backup_cmd_tree"
#define EVT_PARAM_BACKUP_TREE				"backup_tree"
#define EVT_PARAM_BACKUP_CMD_DESCRIPTION		"backup_cmd_backup_description"
#define EVT_PARAM_BACKUP_CMD_BACKUP_ROOT_PATH		"backup_cmd_backup_root_path"
#define EVT_PARAM_BACKUP_CMD_ORIGINAL_SIZE		"backup_cmd_backup_original_size"
#define EVT_PARAM_BACKUP_CMD_FREE_DISK_SPACE		"backup_cmd_free_disk_space"
#define EVT_PARAM_BACKUP_CMD_HDD_LIST			"backup_cmd_hdd_list"
#define EVT_PARAM_BACKUP_CMD_INTERNAL_FLAGS		"backup_cmd_internal_flags"
#define EVT_PARAM_BACKUP_CMD_BUNDLE_PERMISSIONS		"backup_cmd_backup_bundle_permissions"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_SERIAL_NUM_CMD_USER_NAME			"serial_num_cmd_user_name"
#define EVT_PARAM_SERIAL_NUM_CMD_COMPANY_NAME		"serial_num_cmd_company_name"
#define EVT_PARAM_SERIAL_NUM_CMD_SERIAL_NUMBER	"serial_num_cmd_serial_number"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_DIRPATH			"fs_generate_entry_name_cmd_dirpath"
#define EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_PREFIX	"fs_generate_entry_name_cmd_filename_prefix"
#define EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_FILENAME_SUFFIX	"fs_generate_entry_name_cmd_filename_suffix"
#define EVT_PARAM_FS_GENERATE_ENTRY_NAME_CMD_INDEX_DELIMITER	"fs_generate_entry_name_cmd_index_delimiter"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_CREATE_UNATTENDED_FLOPPY_CMD_GUEST_DISTRO_TYPE			"create_unattended_cmd_guest_distro_type"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_CREATE_UNATTENDED_CD_USERNAME		"create_unattended_cd_cmd_username"
#define EVT_PARAM_CREATE_UNATTENDED_CD_PASSWORD		"create_unattended_cd_cmd_password"
#define EVT_PARAM_CREATE_UNATTENDED_CD_FULL_USERNAME	"create_unattended_cd_cmd_full_username"
#define EVT_PARAM_CREATE_UNATTENDED_CD_OS_DISTROE_PATH	"create_unattended_cd_os_distro_path"
#define EVT_PARAM_CREATE_UNATTENDED_CD_OUT_IMAGE_PATH	"create_unattended_cd_out_image_path"
#define EVT_PARAM_CREATE_UNATTENDED_CD_GUEST_TYPE	"create_unattended_cd_guest_type"

////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_UPDATE_SECURITY_CMD_SECURITY_INFO			"vm_update_security_cmd_security_info"

#define EVT_PARAM_CONVERT_OLD_HDD_LIST						"convert_old_hdd_paths_list"

#define EVT_PARAM_OTHERS_VM_PERMISSIONS						"others_vm_permissions"
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_OLD_VIRTUAL_NETWORK_ID			"old_virtual_network_id"

////////////////////////////////////////////////////////////////////////////////
//  Task parameters
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_DISP_TASK_VM_UUID						"task_vm_uuid"
#define EVT_PARAM_DISP_TASK_UUID						"task_uuid"

#define EVT_PARAM_DISP_TASK_CLONE_VM_UUID				EVT_PARAM_DISP_TASK_VM_UUID
#define EVT_PARAM_DISP_TASK_CLONE_NEW_VM_NAME			"task_clone_new_vm_name"
#define EVT_PARAM_DISP_TASK_CLONE_NEW_VM_ROOT_DIR		"task_clone_new_vm_root_dir"
#define EVT_PARAM_DISP_TASK_CLONE_AS_TEMPLATE			"task_clone_as_template"

#define EVT_PARAM_DISP_TASK_CREATE_IMAGE_VM_UUID		EVT_PARAM_DISP_TASK_VM_UUID
#define EVT_PARAM_DISP_TASK_CREATE_IMAGE_DEV_CONFIG		"task_create_image_dev_config"
#define EVT_PARAM_DISP_TASK_CREATE_IMAGE_RECREATE_FLAG	"task_create_image_recreate_allowed"

#define EVT_PARAM_DISP_TASK_VM_DELETE_VM_UUID			EVT_PARAM_DISP_TASK_VM_UUID

#define EVT_PARAM_DISP_TASK_VM_CREATE_CONFIG		"task_vm_create_config"
#define EVT_PARAM_DISP_TASK_VM_CREATE_ROOT_DIR		"task_vm_create_root_dir"

#define EVT_PARAM_DISP_TASK_VM_REGISTER_PATH		"task_vm_register_path"

#define EVT_PARAM_DISP_TASK_MOVE_NEW_HOME_PATH		"task_move_new_home_path"

////////////////////////////////////////////////////////////////////////////////
//  Dispatcher-dispatcher protocols family defines
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_DISP_TO_DISP_AUTHORIZE_CMD_USER_SESSION_UUID		"disp_to_disp_authorize_cmd_user_session_uuid"
#define EVT_PARAM_DISP_TO_DISP_AUTHORIZE_CMD_USER_NAME			"disp_to_disp_authorize_cmd_user_name"
#define EVT_PARAM_DISP_TO_DISP_AUTHORIZE_CMD_PASSWORD			"disp_to_disp_authorize_cmd_user_password"

#define EVT_PARAM_DISP_TO_DISP_RESPONSE_CMD_REQUEST_ID				"disp_to_disp_response_cmd_request_id"
#define EVT_PARAM_DISP_TO_DISP_RESPONSE_CMD_ERROR_INFO				"disp_to_disp_response_cmd_error_info"
#define EVT_PARAM_DISP_TO_DISP_RESPONSE_CMD_PARAMS_LIST				"disp_to_disp_response_cmd_params_list"

#define EVT_PARAM_MIGRATE_PROTO_VERSION				"migrate_proto_version"
#define EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_HOSTNAME		"migrate_cmd_target_server_hostname"
#define EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_PORT		"migrate_cmd_target_server_port"
#define EVT_PARAM_MIGRATE_CMD_TARGET_SERVER_SESSION_UUID	"migrate_cmd_target_server_session_uuid"
#define EVT_PARAM_MIGRATE_CMD_VM_CONFIG				"migrate_cmd_vm_config"
#define EVT_PARAM_MIGRATE_CMD_VM_RUNTIME_CONFIG				"migrate_cmd_vm_runtime_config"
#define EVT_PARAM_MIGRATE_CMD_HOST_HARDWARE_INFO		"migrate_cmd_host_hardware_info"
#define EVT_PARAM_MIGRATE_CMD_TARGET_VM_NAME			"migrate_cmd_target_vm_name"
#define EVT_PARAM_MIGRATE_CMD_TARGET_VM_HOME_PATH		"migrate_cmd_target_vm_home_path"
#define EVT_PARAM_MIGRATE_CMD_SHARED_FILE_NAME			"migrate_cmd_shared_file_name"
#define EVT_PARAM_MIGRATE_CMD_SHARED_FILE_NAMES_EXTRA		"migrate_cmd_shared_file_names_extra"
#define EVT_PARAM_MIGRATE_CMD_RESERVED_FLAGS 			"migrate_cmd_reserved_flags"
#define EVT_PARAM_MIGRATE_CMD_VM_PREV_STATE			"migrate_cmd_vm_prev_state"
#define EVT_PARAM_MIGRATE_CMD_NETWORK_CONFIG			"migrate_cmd_network_config"
#define EVT_PARAM_MIGRATE_CMD_DISPATCHER_CONFIG			"migrate_cmd_dispatcher_config"
#define EVT_PARAM_MIGRATE_IS_SOURCE				"migrate_is_source"
#define EVT_PARAM_MIGRATE_CMD_SNAPSHOT_UUID			"migrate_cmd_snapshot_uuid"
#define EVT_PARAM_MIGRATE_CHECK_PRECONDITIONS_RESULT		"migrate_check_preconditions_result"
#define EVT_PARAM_MIGRATE_CHECK_PRECONDITIONS_NON_SHARED	"migrate_check_preconditions_non_shared"
#define EVT_PARAM_MIGRATE_MEMORY_FILE_PATH			"migrate_memory_file_path"
#define EVT_PARAM_MIGRATE_CMD_STORAGE_INFO			"migrate_cmd_storage_info"
#define EVT_PARAM_MIGRATE_CMD_BUNDLE_PERMISSIONS		"migrate_cmd_bundle_permissions"
#define EVT_PARAM_MIGRATE_CMD_CONFIG_PERMISSIONS		"migrate_cmd_config_permissions"
#define EVT_PARAM_MIGRATE_REQUIRES_DISK_SPACE			"migrate_requires_disk_space"

////////////////////////////////////////////////////////////////////////////////
// Iscsi storage management command params
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_ISCSI_STORAGE					"iscsi_storage"

////////////////////////////////////////////////////////////////////////////////
// Copy of CT template
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_COPY_CT_TMPL_PROTO_VERSION			"copy_ct_tmpl_proto_version"
#define EVT_PARAM_COPY_CT_TMPL_TMPL_NAME			"copy_ct_tmpl_tmpl_name"
#define EVT_PARAM_COPY_CT_TMPL_OS_TMPL_NAME			"copy_ct_tmpl_os_tmpl_name"
#define EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_HOSTNAME		"copy_ct_tmpl_target_server_hostname"
#define EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_PORT		"copy_ct_tmpl_target_server_port"
#define EVT_PARAM_COPY_CT_TMPL_TARGET_SERVER_SESSION_UUID 	"copy_ct_tmpl_target_server_session_uuid"
#define EVT_PARAM_COPY_CT_TMPL_RESERVED_FLAGS 			"copy_ct_tmpl_reserved_flags"
#define EVT_PARAM_COPY_CT_TMPL_CHECK_RESULT			"copy_ct_tmpl_check_result"

////////////////////////////////////////////////////////////////////////////////
// PerfStats command params
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_PERFSTATS_CMD_ACTION     "perfstats_action"
#define EVT_PARAM_PERFSTATS_CMD_FILTER     "perfstats_filter"

////////////////////////////////////////////////////////////////////////////////
//  Event parameters for PET_VM_EVT_VM_CONFIG_CHANGED event
//        Used to provide mech to store any config value from
// 		  VM to the config.
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VMCFG_TIMESHIFT						"vmcfg_time_shift"
#define EVT_PARAM_VMCFG_DEVICE_CONFIG_WITH_NEW_STATE	"vmcfg_vm_device_config_with_new_state"
#define EVT_PARAM_VMCFG_TOOLS_SHARED_FOLDERS			"vmcfg_tools_shared_folders"
#define EVT_PARAM_VMCFG_TOOLS_SHARED_PROFILES			"vmcfg_tools_shared_profiles"
#define EVT_PARAM_VMCFG_TOOLS_COHERENCE					"vmcfg_tools_coherence"
#define EVT_PARAM_VMCFG_TOOLS_SHARED_APPLICATIONS		"vmcfg_tools_shared_applications"
#define EVT_PARAM_VMCFG_TOOLS_NATIVE_LOOK				"vmcfg_tools_native_look"
#define EVT_PARAM_VMCFG_TOOLS_WIN7_LOOK					"vmcfg_tools_win7look"
#define EVT_PARAM_VMCFG_TOOLS_APPS_FOLDER_IN_DOCK		"vmcfg_tools_apps_folder_in_dock"
#define EVT_PARAM_VMCFG_SAFE_MODE						"vmcfg_safe_mode"
#define EVT_PARAM_VMCFG_SERVER_UUID						"vmcfg_server_uuid"
#define EVT_PARAM_VMCFG_INSTALLED_SOFTWARE_ID			"vmcfg_installed_software_id"
#define EVT_PARAM_VMCFG_REMOTE_DEVICES					"vmcfg_remote_devices"
#define EVT_PARAM_VMCFG_COMPACT_HDD_MASK				"vmcfg_compact_hdd_mask"
#define EVT_PARAM_VMCFG_NEW_VM_COLOR					"vmcfg_new_vm_color"
#define EVT_PARAM_VMCFG_SEAMLESS_MODE					"vmcfg_new_seamless_mode"
#define EVT_PARAM_VMCFG_AUTO_COMPRESS_ENABLED			"vmcfg_auto_compress_enabled"
#define EVT_PARAM_VMCFG_USE_ALL_DISPLAYS_IN_FULLSCREEN	"vmcfg_use_all_displays_in_fullscreen"
#define EVT_PARAM_VMCFG_NEW_VNETWORK_ID					"vmcfg_new_vnetwork_id"
#define EVT_PARAM_VMCFG_OLD_VNETWORK_ID					"vmcfg_old_vnetwork_id"
#define EVT_PARAM_VMCFG_REMOTE_DISPLAY_PORT				"vmcfg_remote_display_port"
#define EVT_PARAM_VMCFG_HDD_NEW_SIZE					"vmcfg_hdd_new_size"
#define EVT_PARAM_VMCFG_HDD_IS_SPLITTED				"vmcfg_hdd_splitted"
#define EVT_PARAM_VM_UPTIME_DELTA						"vmcfg_vm_uptime_delta"
#define EVT_PARAM_VMCFG_FLAGS							"vmcfg_system_flags"
#define EVT_PARAM_VMCFG_FLAGS_VALUE						"vmcfg_system_flags_value"
#define EVT_PARAM_VMCFG_GUEST_OS_TYPE					"vmcfg_guest_os_type"
#define EVT_PARAM_VMCFG_GUEST_OS_VERSION				"vmcfg_guest_os_version"
#define EVT_PARAM_VMCFG_DISABLE_SYNC_DEFAULT_PRINTER	"vmcfg_disable_sync_default_printer"
#define EVT_PARAM_VMCFG_EXPIRATION_OFFLINE_START_DATE	"vmcfg_expiration_offline_start_date"
#define EVT_PARAM_VM_DUMP_META_FILE				"vmcfg_vm_dump_meta_file"
#define EVT_PARAM_VM_FAST_REBOOT					"vmcfg_vm_fast_reboot"
#define EVT_PARAM_VM_FAST_REBOOT_USER					"vmcfg_vm_fast_reboot_user"

////////////////////////////////////////////////////////////////////////////////
//  Event parameters for PET_VM_EVT_REQUEST_FILE_OPEN event
//        Used to open file as superuser
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_IPCFO_SOCKET_NAME					"ipcfo_socket_name"
#define EVT_PARAM_IPCFO_FILE_NAME					"ipcfo_file_name"
#define EVT_PARAM_IPCFO_FLAGS						"ipcfo_open_file_flags"

////////////////////////////////////////////////////////////////////////////////
//  Event parameters for PET_VM_EVT_UNMOUNT/MOUNT event
//        Used to mount/unmount disk as superuser
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_DISK_NAME							"mouumou_disk_name"
#define EVT_PARAM_UNMOUNT_CONTEXT					"unmount_context"

////////////////////////////////////////////////////////////////////////////////
//  Event parameters for PRL_QUESTION_CONVERT_VM_SPECIFY_DISTRO_PATH event
//        Used to add new search path
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_CONVERT_VM_SEARCH_PATH			"convert_vm_search_path"

////////////////////////////////////////////////////////////////////////////////
//  Event parameters for PRL_QUESTION_ASK_ENCRYPTED_VM_PASSWORD_ON_REGISTRATION event
//        Used to send password
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_PASSWORD			"encrypted_vm_password"

//  Event parameter event to send hashed password to vm
#define EVT_PARAM_VM_PASSWORD_HASH					"encrypted_vm_password_hash"
#define EVT_PARAM_VM_TEST_PLUGINS_DIR_PATH			"encrypted_vm_test_plugins_path"

////////////////////////////////////////////////////////////////////////////////
//  Event parameters for PET_DSP_EVT_VM_PIS_NOTIFICATION_ASK_TO_INSTALL event
//        Used to select type of Antivirus
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_GUEST_ANTIVURUS_TYPE				"guest_antivirus_type"

/**
 * Virtual Appliance configuration XML model
 */

//
// XML tree basic elements
//
#define XML_VA_CONFIG_EL_ROOT						"VirtualAppliance"
#define XML_VA_CONFIG_EL_LICENCES				"Licences"
#define XML_VA_CONFIG_EL_REQUIREMENTS		"Requirements"
#define XML_VA_CONFIG_EL_ADMIN_INTERFACE	"AdminInterface"

//
// XML tree subsequent nodes
//
#define XML_VA_CONFIG_ND_CATEGORY				"Category"
#define XML_VA_CONFIG_ND_TITLE					"Title"
#define XML_VA_CONFIG_ND_ABSTRACT				"Abstract"
#define XML_VA_CONFIG_ND_DEVEL_STATUS		"DevelopmentStatus"
#define XML_VA_CONFIG_ND_VERSION				"Version"
#define XML_VA_CONFIG_ND_LAST_UPDATE		"LastUpdate"
#define XML_VA_CONFIG_ND_OS_UTILIZED			"OsUtilized"
#define XML_VA_CONFIG_ND_PACKAGE_SIZE		"PackageSize"
#define XML_VA_CONFIG_ND_DESCRIPTION			"Description"
#define XML_VA_CONFIG_ND_TOOLS_VERSION	"ToolsVersion"
#define XML_VA_CONFIG_ND_AUTHOR_NAME		"AuthorName"
#define XML_VA_CONFIG_ND_AUTHOR_EMAIL		"AuthorEmail"
#define XML_VA_CONFIG_ND_DOWNLOAD_URL	"DownloadURL"
#define XML_VA_CONFIG_ND_IS_COMMERCIAL	"IsCommercial"
#define XML_VA_CONFIG_ND_LICENCES_LIST_ITEM		"List"
#define XML_VA_CONFIG_ND_CPU_TYPE				"CPU"
#define XML_VA_CONFIG_ND_MEMORY_SIZE		"Memory"
#define XML_VA_CONFIG_ND_DISK_SIZE				"Disk"
#define XML_VA_CONFIG_ND_OTHER_REQ			"Other"
#define XML_VA_CONFIG_ND_ADMIN_URL			"URL"
#define XML_VA_CONFIG_ND_ADMIN_LOGIN		"Login"
#define XML_VA_CONFIG_ND_ADMIN_PASSWORD	"Password"

////////////////////////////////////////////////////////////////////////////////
// VM exec result parameters
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_EXEC_APP_RET_CODE		"vm_exec_app_ret_code"
#define EVT_PARAM_VM_EXEC_STDOUT			"vm_exec_stdout"
#define EVT_PARAM_VM_EXEC_STDERR			"vm_exec_stderr"

////////////////////////////////////////////////////////////////////////////////
// Register 3rd party VM command parameters
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_CONFIG		"srv_reg_3rd_party_vm_cmd_path_to_config"
#define EVT_PARAM_SRV_REG_3RD_PARTY_VM_CMD_PATH_TO_ROOT_DIR		"srv_reg_3rd_party_vm_cmd_path_to_root_dir"

////////////////////////////////////////////////////////////////////////////////
// Plugin info parameters
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_PLUGIN_INFO_VENDOR					"plugin_info_vendor"
#define EVT_PARAM_PLUGIN_INFO_COPYRIGHT					"plugin_info_copyright"
#define EVT_PARAM_PLUGIN_INFO_DESC_SHORT				"plugin_info_desc_short"
#define EVT_PARAM_PLUGIN_INFO_DESC_LONG					"plugin_info_desc_long"
#define EVT_PARAM_PLUGIN_INFO_VERSION					"plugin_info_version"
#define EVT_PARAM_PLUGIN_INFO_ID						"plugin_info_id"

////////////////////////////////////////////////////////////////////////////////
// Connection Statistics parameters
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_CONN_STATS_CONNECTION_INFO			"conn_stats_connection_info"
#define EVT_PARAM_CONN_STATS_CONNECTION_ID				"conn_stats_connection_id"
#define EVT_PARAM_CONN_STATS_IS_CONNECTED				"conn_stats_is_connected"
#define EVT_PARAM_CONN_STATS_BYTES_RECEIVED				"conn_stats_bytes_received"
#define EVT_PARAM_CONN_STATS_BYTES_SENT					"conn_stats_bytes_sent"
#define EVT_PARAM_CONN_STATS_PACKAGES_RECEIVED			"conn_stats_packages_received"
#define EVT_PARAM_CONN_STATS_PACKAGES_SENT				"conn_stats_packages_sent"

////////////////////////////////////////////////////////////////////////////////
// Two-way time synchronization
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_VM_SETTIME_SECONDS					"vm_settime_seconds"
#define EVT_PARAM_VM_SETTIME_TIMEZONE					"vm_settime_timezone"

// SSL private key to encrypt the VNC traffic for a VE remove desktop.
#define EVT_PARAM_VNC_SSL_PRIVATE_KEY					"vnc_ssl_private_key"
// SSL certificate to encrypt the VNC traffic for a VE remove desktop.
#define EVT_PARAM_VNC_SSL_CERTIFICATE					"vnc_ssl_sertificate"

////////////////////////////////////////////////////////////////////////////////
// attach VM backup parameters
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_ATTACH_VM_BACKUP_DISK_CONFIG			"attach_vm_backup_disk_config"
#define EVT_PARAM_ATTACH_VM_BACKUP_DISK_DIR				"attach_vm_backup_disk_dir"

////////////////////////////////////////////////////////////////////////////////
// connect VM backup source task parameters
////////////////////////////////////////////////////////////////////////////////
#define EVT_PARAM_CONNECT_VM_BACKUP_SOURCE_DISK_CONFIG	"connect_vm_backup_source_disk_config"

#endif // PARALLELSDOMMODEL_H
