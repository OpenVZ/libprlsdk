#
# PrlCommonUtilsBase.pri
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

LIBTARGET = prl_common_utils_base
PROJ_FILE = $$PWD/PrlCommonUtilsBase.pro
QTCONFIG = core network
!include(../../../Sources/Build/qmake/staticlib.pri): error(include error)

INCLUDEPATH *= $$PWD

include($$LIBS_LEVEL/Logging/Logging.pri)
include($$LIBS_LEVEL/PrlUuid/PrlUuid.pri)
win32 {
	include($$LIBS_LEVEL/WmiWrap/WmiWrap.pri)
	# TODO Check if these to libs are needed really
	LIBS *= -lNetapi32 -lUserenv -lAdvapi32
}

