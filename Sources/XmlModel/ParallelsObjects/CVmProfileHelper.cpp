/*
 * CVmProfileHelper.cpp: Set of simple methods which help to work
 * with VM config profile.
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


#include "CVmProfileHelper.h"
#include "SDK/Include/PrlOses.h"


namespace CVmProfileHelper
{

typedef QMap<QString, QVariantList> mapValues;

static mapValues init_profiles_table(const CHostHardwareInfo& host_info,
									 const CVmConfiguration& vm_config)
{
	mapValues t;

#define VL QVariantList()
#define V(val) << val
#define VE(val) V(qlonglong(val))

	bool bHostRetinaEnabled = vm_config.getVmSettings()->getVmRuntimeOptions()->isHostRetinaEnabled();
	unsigned int nOsVersion = vm_config.getVmSettings()->getVmCommonOptions()->getOsVersion();

	unsigned int nCpu = host_info.getCpu()->getNumber() / 2;
	nCpu = (nCpu < 2 ? 2 : nCpu);
	unsigned int nMemSize = host_info.getMemorySettings()->getRecommendedMaxVmMemory();
	unsigned int nVFixMemSize = (nOsVersion >= PVS_GUEST_VER_WIN_VISTA ? 512 : 256);
	unsigned int nVMemSize = (bHostRetinaEnabled ? nVFixMemSize : 256);
	bool bVirtualizedHV = (nOsVersion >= PVS_GUEST_VER_WIN_WINDOWS8 && nOsVersion <= PVS_GUEST_VER_WIN_LAST)
						|| IS_LINUX(nOsVersion);

	//	| Productivity			| Games					| Software developer	| Designer				|
	t.insert( "Hardware.Cpu.Number", // CPU
	VL	V(2)					V(nCpu)					V(nCpu)					V(nCpu)					);
	t.insert( "Hardware.Memory.RAM", // RAM
	VL	V(1024)					V(nMemSize)				V(nMemSize)				V(nMemSize)				);
	t.insert( "Settings.Startup.WindowMode", // Startup View
	VL	VE(PWM_DEFAULT_WINDOW_MODE)
								VE(PWM_FULL_SCREEN_WINDOW_MODE)
														VE(PWM_DEFAULT_WINDOW_MODE)
																				VE(PWM_DEFAULT_WINDOW_MODE)
																										);
	t.insert( "Settings.Runtime.DiskCachePolicy", // Performance
	VL	VE(PVE::DiskCacheWritebackEnabled)
								VE(PVE::DiskCacheWritebackEnabled)
														VE(PVE::DiskCacheWritebackEnabled)
																				VE(PVE::DiskCacheWritebackEnabled)
																										);
	t.insert( "Settings.Runtime.EnableAdaptiveHypervisor", // Enable Adaptive Hypervisor
	VL	V(true)					V(true)					V(true)					V(true)					);
	t.insert( "Settings.Runtime.DisableWin7Logo", // Tune Windows for speed
	VL	V(true)					V(true)					V(true)					V(true)					);
	t.insert( "Settings.Runtime.OptimizePowerConsumptionMode", // Power: (Longer battery life, Better performance)
	VL	VE(PVE::OptimizePerformance)
								VE(PVE::OptimizePerformance)
														VE(PVE::OptimizePerformance)
																				VE(PVE::OptimizePerformance)
																										);
	t.insert( "Hardware.Cpu.VirtualizedHV", // Nested Virtualization
	VL	V(false)				V(false)				V(bVirtualizedHV)		V(false));
	t.insert( "Hardware.Cpu.VirtualizePMU", // PMU Virtualization
	VL	V(false)				V(false)				V(true)					V(false)				);
	t.insert( "Settings.Tools.SharedFolders.HostSharing.Enabled", // Share Folders
	VL	V(true)					V(true)					V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedFolders.HostSharing.ShareAllMacDisks", // Share Folders: All disks
	VL	V(false)				V(false)				V(true)					V(false)				);
	t.insert( "Settings.Tools.SharedFolders.HostSharing.ShareUserHomeDir", // Share Folders: Home folder only
	VL	V(true)					V(true)					V(false)				V(true)					);
	t.insert( "Settings.Tools.SharedProfile.Enabled", // Shared Profile
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedProfile.UseDesktop", // Shared Profile: Desktop
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedProfile.UseDocuments", // Shared Profile: Documents
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedProfile.UsePictures", // Shared Profile: Pictures
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedProfile.UseMusic", // Shared Profile: Music
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedProfile.UseMovies", // Shared Profile: Movies
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedProfile.UseDownloads", // Shared Profile: Downloads
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedProfile.UseTrashBin", // Shared Profile: Merge Recycle Bin with Trash
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedFolders.GuestSharing.Enabled", // Access Windows folder from Mac
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedFolders.GuestSharing.AutoMount", // Mount virtual disks to Mac desktop
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SharedApplications.FromWinToMac", // Share Windows applications with Mac
	VL	V(true)					V(false)				V(false)				V(true)					);
	t.insert( "Settings.Tools.SharedApplications.ShowWindowsAppInDock", // Show Windows applications folder in Dock
	VL	V(true)					V(false)				V(false)				V(true)					);
	t.insert( "Settings.Tools.Coherence.PauseIdleVM", // Pause Windows when no applications are open
	VL	V(true)					V(false)				V(false)				V(false)				);
	t.insert( "Settings.Tools.SharedApplications.FromMacToWin", // Share Mac applications with Windows
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.Coherence.SwitchToFullscreenOnDemand", // Allow application to switch to fullscreen
	VL	V(true)					V(false)				V(false)				V(false)				);
	t.insert( "Settings.Runtime.FullScreen.UseNativeFullScreen", // Full Screen: Use Mac OS X Full Screen
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Runtime.FullScreen.UseActiveCorners", // Activate Screen Corners
	VL	V(false)				V(false)				V(false)				V(false)				);
	t.insert( "Settings.Runtime.FullScreen.ScaleViewMode", // Scale to fit screen
	VL	VE(PSVM_AUTO)			VE(PSVM_KEEP_ASPECT_RATIO)
														VE(PSVM_AUTO)			VE(PSVM_AUTO)			);
	t.insert( "Settings.Runtime.FullScreen.UseAllDisplays", // Use all displays in full screen
	VL	V(false)				V(false)				V(false)				V(false)				);
	t.insert( "Settings.Tools.ClipboardSync.Enabled", // Shared clipboard
	VL	V(true)					V(true)					V(true)					V(true)					);
	t.insert( "Settings.Tools.ClipboardSync.PreserveTextFormatting", // Preserve text formatting
	VL	V(true)					V(true)					V(true)					V(true)					);
	t.insert( "Hardware.Video.VideoMemorySize", // Video memory
	VL	V(nVMemSize)			V(nVFixMemSize)			V(nVMemSize)			V(nVFixMemSize)         );
	t.insert( "Hardware.Video.Enable3DAcceleration", // 3D acceleration
	VL	VE(P3D_ENABLED_HIGHEST)	VE(P3D_ENABLED_HIGHEST)	VE(P3D_ENABLED_HIGHEST)	VE(P3D_ENABLED_HIGHEST)	);
	t.insert( "Hardware.Video.EnableVSync", // Enable vertical synchronization
	VL	V(true)					V(true)					V(false)				V(true)					);
	t.insert( "Hardware.Video.EnableHiResDrawing", // Resolution
	VL	V(true)					V(false)				V(false)				V(false)				);
	t.insert( "Hardware.Video.UseHiResInGuest", // Resolution
	VL	V(true)					V(false)				V(false)				V(false)				);
	t.insert( "Settings.Tools.SmartMouse.Enabled", // Smart Mouse
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.MouseSync.Enabled", // Smart Mouse
	VL	V(true)					V(false)				V(true)					V(true)					);
	t.insert( "Settings.Tools.SmoothScrolling.Enabled", // Enable smooth scrolling
	VL	V(true)					V(true)					V(true)					V(true)					);
	t.insert( "Settings.Runtime.OptimizeModifiers", // Optimize modifiers for games
	VL	VE(POD_STANDARD)		VE(POD_OPTIMIZE_FOR_GAMES)
														VE(POD_STANDARD)		VE(POD_STANDARD)		);

#undef VE
#undef V
#undef VL

	return t;
}

PRL_RESULT set_vm_profile(PRL_VIRTUAL_MACHINE_PROFILE profile,
			  const CHostHardwareInfo& host_info,
			  CVmConfiguration& vm_config)
{
	if (!are_profiles_supported(vm_config.getVmSettings()->getVmCommonOptions()->getOsVersion()))
		return PRL_ERR_INVALID_PARAM;

	bool bProfileIsValid = false;
	switch (profile)
	{
		case VMP_PRODUCTIVITY:
		case VMP_GAMES:
		case VMP_SOFTWARE_DEVELOPER:
		case VMP_DESIGNER:
			bProfileIsValid = true;
			break;
		// Do not add default section to keep this code and enum synchronised.
	}
	if (!bProfileIsValid)
		return PRL_ERR_INVALID_ARG;

	vm_config.getVmSettings()->getVmCommonOptions()->getProfile()->setType(profile);
	vm_config.getVmSettings()->getVmCommonOptions()->getProfile()->setCustom(false);

	mapValues t = init_profiles_table(host_info, vm_config);

	mapValues::const_iterator it;
	for(it = t.begin(); it != t.end(); ++it)
	{
		QString path = it.key();
		QVariant value = it.value().at(profile);

		vm_config.setPropertyValue(path, value);
	}

	return PRL_ERR_SUCCESS;
}

int check_vm_custom_profile(const CHostHardwareInfo& host_info,
							const CVmConfiguration& cfg1,
							const CVmConfiguration& cfg2,
							bool check_diff)
{
	if (!are_profiles_supported(cfg1.getVmSettings()->getVmCommonOptions()->getOsVersion()))
		return 0;

	const mapValues t = init_profiles_table(host_info, cfg1);
	QStringList lstPaths = t.keys();

	if (check_diff)
	{
		QStringList diffList;
		cfg1.diffDocuments( &cfg2, diffList );
		if ( diffList.isEmpty() )
			return -1;

		lstPaths = lstPaths.toSet().intersect( diffList.toSet() ).toList();
		if ( lstPaths.isEmpty() )
			return -1;
	}

	CVmConfiguration cfg = cfg1;

	int nProfile = cfg.getVmSettings()->getVmCommonOptions()->getProfile()->getType();
	foreach(QString path, lstPaths)
	{
		QVariant value = t[path].at(nProfile);

		bool bValueChanged = false;
		cfg.setPropertyValue(path, value, &bValueChanged);
		if (bValueChanged)
			return 1;
	}

	return 0;
}

bool are_profiles_supported(PRL_UINT32 osVersion)
{
	return IS_VALID_WINDOWS_VERSION(osVersion) &&
			osVersion >= PVS_GUEST_VER_WIN_2K &&
			osVersion <= PVS_GUEST_VER_WIN_LAST;
}

}
