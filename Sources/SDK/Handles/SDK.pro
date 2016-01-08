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

LIBS += -lprlcommon -lboost_filesystem-mt -lboost_system-mt

libsdk_only {
	# set up rpath to qt libs
	QMAKE_RPATHDIR =
	rpath_cmd =
	# starting from version 4.6.2 qt libs have infix 'Prl' in their names
	infix = 'Prl'
	lessThan(QT_MINOR_VERSION, 6) | equals(QT_VERSION, '4.6.1') : infix =
	qtlibs = Core Xml Gui Network
	for(qtlib, qtlibs) {
		rpath_cmd = $${rpath_cmd}'\\\$\${ORIGIN}/../libQt'$${qtlib}$${infix}'.so.4/:'
	}
	QMAKE_LFLAGS += -Wl,-rpath,$$rpath_cmd
}

include(SDKSources.pri)

