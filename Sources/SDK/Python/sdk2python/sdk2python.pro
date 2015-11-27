#
# sdk2python.pro
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

LEVEL = ../../../..
include($$LEVEL/Sources/Parallels.pri)

TEMPLATE	= app
TARGET		= sdk2python

QT			+= xml

CONFIG		+= console exceptions
CONFIG      -= app_bundle

SOURCES		+= \
			./main.cpp \
			./SDKParser.cpp \
			./Composer.cpp \
			./ModuleComposer.cpp \
			./PythonComposer.cpp \

HEADERS		+= \
			./SDKParser.h \
			./Composer.h \
			./ModuleTemplates.h \
			./ModuleComposer.h \
			./PythonComposer.h \
			./PythonTemplates.h \
