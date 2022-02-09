#
# SDK.pro
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

TEMPLATE = lib
CONFIG += shared
CONFIG -= static

QT = core network xml

include(SDK.deps)
include(SDKConfig.pri)

RESOURCES     = sdk.qrc
TRANSLATIONS += sdk_err_strings_cs.ts\
	sdk_err_strings_en.ts\
	sdk_err_strings_uk.ts\
	sdk_err_strings_fr.ts\
	sdk_err_strings_de.ts\
	sdk_err_strings_it.ts\
	sdk_err_strings_pl.ts\
	sdk_err_strings_es.ts\
	sdk_err_strings_ja.ts\
	sdk_err_strings_ru.ts

# Hide all exported symbols on Unix (Windows like behaviour)
unix:QMAKE_CXXFLAGS += -fvisibility=hidden

# Distribution specific boost dependency - we want to link with
# multithreaded version where possible
BOOST_FILESYSTEM = -lboost_filesystem
exists(/usr/lib64/libboost_filesystem-mt*) : BOOST_FILESYSTEM = -lboost_filesystem-mt
BOOST_SYSTEM = -lboost_system
exists(/usr/lib64/libboost_system-mt*) : BOOST_SYSTEM = -lboost_system-mt

LIBS += -lprl_xml_model -lprlcommon $${BOOST_FILESYSTEM} $${BOOST_SYSTEM}

include(SDKSources.pri)

