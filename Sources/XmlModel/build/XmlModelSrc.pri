#
# XmlModelSrc.pri
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

include(../GuestOsInformation/GuestOsInformation.pri)
include(../Messaging/Messaging.pri)
include(../ParallelsObjects/ParallelsObjects.pri)
include(../UserInformation/UserInformation.pri)
include(../VmInfo/VmInfo.pri)
include(../VmConfig/VmConfigBase.pri)
include(../KeyboardMouse/KeyboardMouse.pri)
include(../DispConfig/DispConfig.pri)
include(../ProblemReport/ProblemReport.pri)
include(../Appliance/Appliance.pri)
include(../VmDirectory/VmDirectory.pri)

!mobilePlatform {
	include(../AppLists/AppLists.pri)
	include(../BackupTree/BackupTree.pri)
	include(../ClientPreferences/ClientPreferences.pri)
	include(../DiskImageInfo/DiskImageInfo.pri)
	include(../HostHardwareInfo/HostHardwareInfoBase.pri)
	include(../NetworkConfig/NetworkConfig.pri)
	include(../Reports/Reports.pri)
	include(../Updater/UpdaterConfig.pri)
	include(../Licenses/Licenses.pri)
	include(../Updater/Updates.pri)
	include(../XKeyMaps/XKeyMaps.pri)
	include(../IscsiStorage/IscsiStorage.pri)
	include(../CtTemplate/CtTemplate.pri)
	include(../DiskDescriptor/DiskDescriptor.pri)
	include(../VmDataStatistic/VmDataStatistic.pri)
	include(../InterfaceInfo/InterfaceInfo.pri)
}

HEADERS += $$GEN_HEADERS
SOURCES += $$GEN_SOURCES
