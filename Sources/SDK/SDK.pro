#
# SDK.pro
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

TEMPLATE = subdirs

LEVEL = ../..
include($$LEVEL/Sources/Build/Options.pri)
include($$LEVEL/Sources/Parallels.pri)
include($$SRC_LEVEL/SDK/Handles/SDK.deps)

CHECK_INC_PATH='/usr/include'
!isEmpty(LOCAL_DEPS_INCLUDE) {
    CHECK_INC_PATH=$$LOCAL_DEPS_INCLUDE
}

!exists($$CHECK_INC_PATH/prlcommon) {
	error(Please install libprlcommon-devel package)
}

