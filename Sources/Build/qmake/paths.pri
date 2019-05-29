#
# paths.pri
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

defineTest(norm_path) {
	_PWD = $$eval($$1)
	unix {
		_NORM_PATH = $$system(_OLDPWD=$PWD; cd $$_PWD; echo $PWD; cd $_OLDPWD)
	}
	win32 {
		_NORM_PATH = $$replace(_PWD, /, \\\\)
	}
	eval($$1 = $$_NORM_PATH)
	export($$1)
}

ROOT_LEVEL = $$PWD/../../..
norm_path(ROOT_LEVEL)

SRC_LEVEL = $$ROOT_LEVEL/Sources
EXT_LEVEL = $$ROOT_LEVEL/External

ZBUILD_LEVEL = $$ROOT_LEVEL/z-Build

LIBS_LEVEL = $$SRC_LEVEL/Libraries

