#
# PrlPythonSDK.pro
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

MODULE_NAME = prlsdk
TEMPLATE = lib

CONFIG += plugin no_plugin_name_prefix dll thread shared
CONFIG -= qt static

include(PrlPythonSDK.deps)

TARGET = $$MODULE_NAME

# Python modules should be linked with undefined python symbols.
# So make exception for these shared objects.
linux-*: QMAKE_LFLAGS -= -Wl,--no-undefined

win32 {
	# Disable declaration of the standard C function round() in Python 2.7.6
	# header pymath.h because it is incompatible with Microsoft's one
	DEFINES += HAVE_ROUND
}

DEFINES += PYTHON_MODULE_NAME=$$MODULE_NAME

SOURCES += \
	PrlPythonSDK_entry.c

SDK2PYTHON_INPUT = \
	$$SRC_LEVEL/SDK/Include/PrlApiMacro.h \
	$$SRC_LEVEL/SDK/Include/PrlApiCore.h \
	$$SRC_LEVEL/SDK/Include/PrlApiDisp.h \
	$$SRC_LEVEL/SDK/Include/PrlApiVm.h \
	$$SRC_LEVEL/SDK/Include/PrlApiCt.h \
	$$SRC_LEVEL/SDK/Include/PrlApiNet.h \
	$$SRC_LEVEL/SDK/Include/PrlApiStat.h \
	$$SRC_LEVEL/SDK/Include/PrlDisk.h \
	$$SRC_LEVEL/SDK/Include/PrlEnums.h \
	$$SRC_LEVEL/SDK/Include/PrlCommandsFlags.h \
	$$SRC_LEVEL/SDK/Include/PrlOses.h \
	$$SRC_LEVEL/SDK/Include/PrlKeys.h \
	$$SRC_LEVEL/SDK/Include/PrlErrorsValues.h \

parse_log = $$PWD/parse_log.txt
win32: norm_path(parse_log)

equals(TEMPLATE, lib) {
	win32: _sdk2python_generator = $$DESTDIR/sdk2python.exe
	else:  _sdk2python_generator = $$DESTDIR/sdk2python

	QMAKE_EXTRA_COMPILERS += sdk2python
	sdk2python.input = SDK2PYTHON_INPUT
	sdk2python.commands = $$_sdk2python_generator $$SRC_LEVEL/SDK/Include $$PWD $$SRC_LEVEL/SDK/Docs/Python/prlpythonapidoc.xml $$parse_log
	sdk2python.output = PrlPythonSDK.cpp
	sdk2python.variable_out = SOURCES
	sdk2python.depends += $$_sdk2python_generator
	sdk2python.CONFIG += combine
}

QMAKE_CLEAN += $$parse_log prlsdkapi.py

# Including automatically generated include file with system-dependant paths
# Its generated by Gen.py (developer build) or BuildPyPrlSdk.py (production build)
include(PrlPython.pri)

!isEmpty(PYTHON_VER) {
	DESTDIR = $$join(DESTDIR, "", "", /$$PYTHON_VER)
}

# For developers need both dll and pyd
win32 {
	SRC_FILE = $$DESTDIR"/"$$MODULE_NAME".dll"
	SRC_FILE = $$replace(SRC_FILE,"/","\\\\")
	DST_FILE = $$DESTDIR"/"$$MODULE_NAME".pyd"
	DST_FILE = $$replace(DST_FILE,"/","\\\\")

	QMAKE_POST_LINK=$(COPY_FILE) $$SRC_FILE $$DST_FILE
}

