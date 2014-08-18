#
# IOCommunication.pro
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
CONFIG += staticlib warn_on thread

include(IOCommunication.pri)

HEADERS = \
          IOClient.h \
          IOClientInterface.h \
          IOConnection.h \
          IODataBuffer.h \
          IOProtocol.h \
          IORoutingTable.h \
          IORoutingTableHelper.h \
          IOSendJob.h \
          IOSendJobInterface.h \
          IOServer.h \
          IOServerInterface.h \
          IOServerPool.h \
          \
          Socket/SocketClient_p.h \
          Socket/SocketListeners_p.h \
          Socket/SocketServer_p.h \
          Socket/Socket_p.h \
          IOSSLInterface.h \
          Socket/SslHelper.h


SOURCES = \
          IOClient.cpp \
          IODataBuffer.cpp \
          IOProtocol.cpp \
          IORoutingTable.cpp \
          IORoutingTableHelper.cpp \
          IOSendJob.cpp \
          IOServer.cpp \
          IOServerPool.cpp \
          \
          Socket/SocketClient_p.cpp \
          Socket/SocketServer_p.cpp \
          Socket/Socket_p.cpp \
          IOSSLInterface.cpp \
          Socket/SslHelper.cpp
