#
# PrlCommonUtilsBase.pro
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

TARGET = prl_common_utils_base
TEMPLATE = lib
CONFIG += staticlib

DEFINES += PRINTABLE_TARGET=cmn_utils_base

include(PrlCommonUtilsBase.pri)

# We cannot include XmlModel.pri into PrlCommonUtils.pri
# because the second includes the first one already
# But XmlModel headers are used here. So include source
# generating config directly.
include($$SRC_LEVEL/XmlModel/build/gen_xmlmodel_src.pri)

HEADERS = ParallelsDirs.h \
		Common.h \
		SysError.h \
		PrlStringifyConsts.h \
            ParallelsDirsDefs.h \
            ParallelsDirsBase.h \
            CommandLine.h \
            OsInfo.h \
            CSimpleFileHelper.h \
            StringUtils.h \
            EnumToString.h \
            NetworkUtils.h \
            CFeaturesMatrix.h \
            CGuestOsesHelper.h \
            CommandConvHelper.h \
            countof.h \
            CHardDiskHelper.h \
            CUrlParser.h \
            netutils.h


SOURCES = ParallelsDirs.cpp \
		ParallelsNamespace.cpp \
		Common.cpp \
		SysError.cpp \
            ParallelsDirsBase.cpp \
            CommandLine.cpp \
            OsInfo.c \
            CSimpleFileHelper.cpp \
            StringUtils.cpp \
            EnumToString.cpp \
            NetworkUtils.cpp \
            CFeaturesMatrix.cpp \
            CGuestOsesHelper.cpp \
            CommandConvHelper.cpp \
            CHardDiskHelper.cpp \
            CUrlParser.cpp \
            netutils.cpp

SDK_VALUES = \
    $$SRC_LEVEL/SDK/Include/PrlErrorsValues.txt \
    $$SRC_LEVEL/SDK/Include/PrlEventsValues.txt \
    $$SRC_LEVEL/SDK/Include/PrlEnums.h

QMAKE_EXTRA_COMPILERS += gen_sdk
gen_sdk.input = SDK_VALUES
gen_sdk.commands = $$SRC_LEVEL/SDK/GenSdk.py header string-consts
gen_sdk.output = PrlStringifyConsts.cpp
gen_sdk.variable_out = SOURCES
gen_sdk.CONFIG += combine
gen_sdk.depends += $$SRC_LEVEL/SDK/GenSdk.py

