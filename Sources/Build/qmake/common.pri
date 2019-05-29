#
# common.pri
#
# Copyright (c) 1999-2017, Parallels International GmbH
# Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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

isEmpty(__COMMON_PRI__) {
	__COMMON_PRI__ = 1
	equals(TARGET, bin) | equals(TARGET, lib): error(TARGET has unacceptable name $$TARGET)

	include(../../../Sources/Parallels.pri)

	# ROOT = $$PWD
	# BUILD_DIR = $$ROOT/build
	# LIB_BUILD_DIR = $$BUILD_DIR/lib
	# PROJ_BUILD_DIR = $$BUILD_DIR/$$TARGET
	BIN_BUILD_DIR = $$PRL_LIBS_PATH
	LIB_BUILD_DIR = $$PRL_LIBS_PATH

	# equals(TEMPLATE, lib): DESTDIR = $$LIB_BUILD_DIR
	# equals(TEMPLATE, app): DESTDIR = $$BIN_BUILD_DIR
	# MOC_DIR = $$PROJ_BUILD_DIR/moc
	# OBJECTS_DIR = $$PROJ_BUILD_DIR/obj
	# RCC_DIR = $$PROJ_BUILD_DIR/rcc
	QMAKE_LIBDIR += $$LIB_BUILD_DIR

	static | staticlib {
		CONFIG -= dll shared sharedlib plugin dylib
	}

	equals(TEMPLATE, subdirs) | equals(TEMPLATE, vcsubdirs): CONFIG += no_fixpath
}
