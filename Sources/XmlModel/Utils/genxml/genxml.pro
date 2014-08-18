#
# genxml.pro
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

TEMPLATE = app
CONFIG += console
QT += xml network
QT -= gui

include(genxml.deps)

PRL_SRC_DIR = $$LEVEL/Sources
PRL_BLD_DIR = $$LEVEL/z-Build

XMLMODEL_INCLUDES = $$PRL_SRC_DIR \
									$$PRL_SRC_DIR/XmlModel/ParallelsObjects \
									$$PRL_SRC_DIR/XmlModel/VmConfig \
									$$PRL_SRC_DIR/XmlModel/DispConfig \
									$$PRL_SRC_DIR/XmlModel/VmDirectory \
									$$PRL_SRC_DIR/XmlModel/ProblemReport \
									$$PRL_SRC_DIR/XmlModel/Messaging \
									$$PRL_SRC_DIR/XmlModel/HostHardwareInfo \
									$$PRL_SRC_DIR/XmlModel/ClientPreferences \
									$$PRL_SRC_DIR/XmlModel/Updater \
									$$PRL_SRC_DIR/XmlModel/DiskDescriptor \

XMLMODEL_LIBS = -lprl_xml_model

# ====================================================

isEmpty(XMLMODEL_INCLUDES): \
	error(Please specify the location of your XmlModel includes in genxml.pro)
isEmpty(XMLMODEL_LIBS): \
	error(Please specify the location of your XmlModel libraries in genxml.pro)

INCLUDEPATH += $$PWD $$XMLMODEL_INCLUDES
DEPENDPATH += $$PWD
LIBS += $$XMLMODEL_LIBS

win32:DEFINES += WIN32

SOURCES += main.cpp
