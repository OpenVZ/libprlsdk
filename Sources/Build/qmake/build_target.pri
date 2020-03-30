#
# build_target.pri
#
# Copyright (c) 1999-2017, Parallels International GmbH
# Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
#
# This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
# Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
# Schaffhausen, Switzerland; http://www.virtuozzo.com/.
#

mkfile = Makefile_$$TARGET

isEmpty(NON_SUBDIRS) {
	# Will overwrite TEMPLATE in project file 
	# if it is included via build.target
	TEMPLATE = subdirs

	!contains(SUBDIRS, $$TARGET) {
		message(---> $$TARGET)
		SUBDIRS += $$TARGET
		eval($${TARGET}.file = $$PROJ_PATH/build.target)
		eval($${TARGET}.makefile = $$mkfile)
	}
}

_TARGET_IS_SET = 1

!include(../../../Sources/Virtuozzo.pri): error(Cannot include Virtuozzo.pri)

equals(TEMPLATE, subdirs) {
	win32-msvc2013: wdkruntime: isEmpty(_msvcrt_compat_internal) {
		include($$LIBS_LEVEL/msvcrt_compat/msvcrt_compat.pri)
	}
} else {
	!isEmpty(NON_SUBDIRS): MAKEFILE = $$mkfile
}
