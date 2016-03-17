#
# SDKSources.pri
#
# Copyright (C) 1999-2014 Parallels IP Holdings GmbH
#
# This file is part of Parallels SDK. Parallels SDK is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License,
# or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library.  If not, see
# <http://www.gnu.org/licenses/>.
#
# Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
# Schaffhausen, Switzerland; http://www.parallels.com/.
#

# FIXME: Remove this additional include paths
#        after splitting on different libraries.
#        It's need only for success compilation.


INCLUDEPATH +=	$$SRC_LEVEL/SDK/Handles \
		$$PWD/Core \
		$$PWD/Stat \
		$$PWD/Net \
		$$PWD/Disp \
		$$PWD/Vm \
		$$PWD/Deprecated \
		$$PWD/Disk

INSTALL_HEADERS = $$SRC_LEVEL/SDK/Include/Parallels.h \
	$$SRC_LEVEL/SDK/Include/PrlApi.h \
	$$SRC_LEVEL/SDK/Include/PrlApiMacro.h \
	$$SRC_LEVEL/SDK/Include/PrlApiVersion.h \
	$$SRC_LEVEL/SDK/Include/PrlCommandsFlags.h \
	$$SRC_LEVEL/SDK/Include/PrlDisk.h \
	$$SRC_LEVEL/SDK/Include/PrlEnums.h \
	$$SRC_LEVEL/SDK/Include/PrlErrors.h \
	$$SRC_LEVEL/SDK/Include/PrlErrorsValues.h \
	$$SRC_LEVEL/SDK/Include/PrlEventsValues.h \
	$$SRC_LEVEL/SDK/Include/PrlFs.h \
	$$SRC_LEVEL/SDK/Include/PrlIOStructs.h \
	$$SRC_LEVEL/SDK/Include/PrlKeys.h \
	$$SRC_LEVEL/SDK/Include/PrlOses.h \
	$$SRC_LEVEL/SDK/Include/PrlPerfCounters.h \
	$$SRC_LEVEL/SDK/Include/PrlPluginClasses.h \
	$$SRC_LEVEL/SDK/Include/PrlPorts.h \
	$$SRC_LEVEL/SDK/Include/PrlTypes.h \
	$$SRC_LEVEL/SDK/Include/PrlApiCore.h \
	$$SRC_LEVEL/SDK/Include/PrlApiDisp.h \
	$$SRC_LEVEL/SDK/Include/PrlApiVm.h \
	$$SRC_LEVEL/SDK/Include/PrlApiNet.h \
	$$SRC_LEVEL/SDK/Include/PrlApiStat.h \
	$$SRC_LEVEL/SDK/Include/PrlApiDeprecated.h

headers.files = $${INSTALL_HEADERS}
headers.path = $${PREFIX}/include/prlsdk
INSTALLS += headers

HEADERS += $${INSTALL_HEADERS} \
	\
	$$SRC_LEVEL/SDK/Handles/HandleInterface/IEventSourceHandle.h \
	$$SRC_LEVEL/SDK/Handles/HandleInterface/IHandleVm.h \
	$$SRC_LEVEL/SDK/Handles/HandleInterface/GetHandleInterface.h \
	\
	$$SRC_LEVEL/SDK/Handles/Core/EventDispatcher.h \
	$$SRC_LEVEL/SDK/Handles/Core/BuiltinEventSource.h \
	$$SRC_LEVEL/SDK/Handles/Core/ExternEventSource.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlNew.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlCommon.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlContextSwitcher.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleBase.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleJob.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleLocalJob.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleResult.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleEvent.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleEventParam.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleIOEvent.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleIOEventPackage.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleIOJob.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleStringsList.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleHandlesList.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleOpTypeList.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleProblemReportBase.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleProblemReport.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlProblemReportAssembler.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlProblemReportSender.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleVmEvent.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlErrStringsStorage.h \
	$$SRC_LEVEL/SDK/Handles/Core/PrlControlValidity.h \
	\
	$$SRC_LEVEL/SDK/Handles/Disp/PveControl.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlCheckServerHelper.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlEventsHandler.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlQuestionsList.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServer.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServerDisp.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServerJob.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLoginResponse.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLoginHelperJob.h \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLoginLocalHelperJob.h \
	\
	$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVm.h \
	\
	$$SRC_LEVEL/SDK/Handles/Disk/PrlHandleDiskMap.h \
	$$SRC_LEVEL/SDK/Handles/Disk/PrlHandleDisk.h


SOURCES += \
	$$SRC_LEVEL/SDK/Handles/HandleInterface/GetHandleInterface.cpp \
	\
	$$SRC_LEVEL/SDK/Handles/Core/EventDispatcher.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/BuiltinEventSource.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/ExternEventSource.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlCommon.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlApiCore.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleBase.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleJob.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleLocalJob.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleResult.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleEvent.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleEventParam.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleIOEvent.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleIOEventPackage.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleIOJob.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleStringsList.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleHandlesList.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleOpTypeList.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleProblemReportBase.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleProblemReport.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlProblemReportAssembler.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlProblemReportSender.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlHandleVmEvent.cpp \
	$$SRC_LEVEL/SDK/Handles/Core/PrlErrStringsStorage.cpp\
	$$SRC_LEVEL/SDK/Handles/Core/PrlErrStringsStorage_PS.cpp\
	$$SRC_LEVEL/SDK/Handles/Core/PrlControlValidity.cpp\
	\
	$$SRC_LEVEL/SDK/Handles/Disp/PveControl.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlApiDisp.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlCheckServerHelper.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlEventsHandler.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlQuestionsList.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServer.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServerDisp.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServerJob.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLoginResponse.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLoginHelperJob.cpp \
	$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLoginLocalHelperJob.cpp \
	\
	$$SRC_LEVEL/SDK/Handles/Vm/PrlApiVm.cpp \
	$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVm.cpp \
	\
	$$SRC_LEVEL/SDK/Handles/Disk/PrlHandleDiskSDK.cpp \
	$$SRC_LEVEL/SDK/Handles/Disk/PrlHandleDiskMap.cpp \
	$$SRC_LEVEL/SDK/Handles/Disk/PrlHandleDisk.cpp

	HEADERS += \
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleServerStat.h \
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatistics.h\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsCpu.h\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsIface.h\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsUser.h\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsDisk.h\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsDiskPart.h\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsProc.h\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsVmData.h\
		\
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleServerNet.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleVirtNet.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandlePortFwd.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetServiceStatus.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleOfflineService.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkClass.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkShaping.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkShapingConfig.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkShapingBandwidth.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkRate.h \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleIPPrivNet.h \
		\
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServerInfo.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleRemoteFsInfo.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleSrvConfig.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwGenericDevice.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwHardDisk.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwHddPart.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwNetAdapter.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwGenericPciDevice.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleUserProfile.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleUserInfo.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleDispConfig.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleCpuPool.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLicense.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleRunningTask.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleUsbIdentity.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandlePluginInfo.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleBackupResult.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleRemoteFsEntry.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleDispNet.h \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleCpuFeatures.h \
		\
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleServerVm.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmSrv.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmCfg.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleAccessRights.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmInfo.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmGuest.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDevice.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceHardDrive.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceHdPart.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceFloppy.h\
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceOpticalDisk.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceSerialPort.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceParallelPort.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceUsbPort.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceNetAdapter.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceSound.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceDisplay.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceGenericPci.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceGenericScsi.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleFirewallRule.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmShare.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmBootDev.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleFoundVmInfo.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmToolsInfo.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleGuestOsesMatrix.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleCtTemplate.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDefaultConfig.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleRemoteCommand.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleIORemoteCommand.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlRunProgramInGuestJob.h \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlFileDescriptorsMech.h \

	SOURCES += \
		$$SRC_LEVEL/SDK/Handles/Stat/PrlApiStat.cpp \
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleServerStat.cpp \
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatistics.cpp\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsCpu.cpp\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsIface.cpp\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsUser.cpp\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsDisk.cpp\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsDiskPart.cpp\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsProc.cpp\
		$$SRC_LEVEL/SDK/Handles/Stat/PrlHandleSystemStatisticsVmData.cpp\
		\
		$$SRC_LEVEL/SDK/Handles/Net/PrlApiNet.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleServerNet.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleVirtNet.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandlePortFwd.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetServiceStatus.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleOfflineService.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkClass.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkShaping.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkShapingConfig.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkShapingBandwidth.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleNetworkRate.cpp \
		$$SRC_LEVEL/SDK/Handles/Net/PrlHandleIPPrivNet.cpp \
		\
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleServerInfo.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleRemoteFsInfo.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleSrvConfig.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwGenericDevice.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwHardDisk.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwHddPart.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwNetAdapter.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleHwGenericPciDevice.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleUserProfile.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleUserInfo.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleDispConfig.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleCpuPool.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleLicense.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleRunningTask.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleUsbIdentity.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandlePluginInfo.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleRemoteFsEntry.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleDispNet.cpp \
		$$SRC_LEVEL/SDK/Handles/Disp/PrlHandleCpuFeatures.cpp \
		\
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleServerVm.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmSrv.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmCfg.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleAccessRights.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmInfo.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmGuest.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDevice.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceHardDrive.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceHdPart.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceFloppy.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceOpticalDisk.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceSerialPort.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceParallelPort.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceUsbPort.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceNetAdapter.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceSound.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceDisplay.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceGenericPci.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDeviceGenericScsi.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleFirewallRule.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmShare.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmBootDev.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleFoundVmInfo.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmToolsInfo.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleGuestOsesMatrix.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleCtTemplate.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlHandleVmDefaultConfig.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlRunProgramInGuestJob.cpp \
		$$SRC_LEVEL/SDK/Handles/Vm/PrlFileDescriptorsMech.cpp \


        HEADERS += \
                $$SRC_LEVEL/SDK/Handles/Deprecated/PrlHandleServerDeprecated.h


        SOURCES += \
                $$SRC_LEVEL/SDK/Handles/Deprecated/PrlApiDeprecated.cpp \
                $$SRC_LEVEL/SDK/Handles/Deprecated/PrlHandleServerDeprecated.cpp \

pkgconfig.files = parallels-sdk.pc
#FIXME: where should we install this file?
pkgconfig.path = /usr/lib64/pkgconfig
INSTALLS += pkgconfig

