#
# staticlib.pri
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

#message(static::: $$SUBDIRS)
CONFIG += _staticlib
include(commonlib.pri)
CONFIG -= _staticlib
! isEqual(TEMPLATE, subdirs) {
	isEqual(TEMPLATE, app) | contains(CONFIG, shared) | contains(CONFIG, dll) {
		# will add pre_targetdeps only to apps and shared libs
		# no pre_targetdeps for static libs!
		unix: LIBTARGET_NAME = lib$${LIBTARGET}.a
		win32: LIBTARGET_NAME = $${LIBTARGET}.lib
		PRE_TARGETDEPS += $$LIB_BUILD_DIR/$$LIBTARGET_NAME
	}
}
