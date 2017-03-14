#
# commonlib.pri
#
# Copyright (c) 1999-2017, Parallels International GmbH
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
# Our contact details: Parallels International GmbH, Vordergasse 59, 8200
# Schaffhausen, Switzerland; http://www.parallels.com/.
#

# check necessary vars
isEmpty(LIBTARGET): error($$_FILE_ : LIBTARGET is not set)
isEmpty(PROJ_FILE): error($$_FILE_ : PROJ_FILE is not set)
!exists($$PROJ_FILE): error($$PROJ_FILE does not exist)

!isEqual(TEMPLATE, subdirs) {
	# TARGET must be set before including common.pri
	# because this variable is checked there.
	isEmpty(_TARGET_IS_SET) {
		TARGET = $$LIBTARGET
		isEmpty(QTCONFIG) {
			CONFIG -= qt
			QT =
		} else {
			CONFIG *= qt
			QT = $$QTCONFIG
		}
		_TARGET_IS_SET = 1
	} else {
		!isEmpty(QTCONFIG) {
			CONFIG *= qt
			QT *= $$QTCONFIG
		}
	}

	unset(QTCONFIG)
}

include(common.pri)

isEqual(TEMPLATE, subdirs) | isEqual(TEMPLATE, vcsubdirs) {
	!contains(SKIP_BUILD, $$LIBTARGET) {
		!contains(SUBDIRS, $$LIBTARGET) {
			message($$LIBTARGET $$PROJ_FILE -> $$TARGET)
			SUBDIRS += $$LIBTARGET
			_staticlib {
				eval($${LIBTARGET}.file = $$PROJ_FILE)
			    eval($${LIBTARGET}.makefile = Makefile)
				}
			}
			_sharedlib {
				eval($${LIBTARGET}.file = $$dirname(PROJ_FILE)/build.target)
				eval($${LIBTARGET}.makefile = Makefile_$$LIBTARGET)
			}
		}
		eval($${TARGET}.depends *= $$LIBTARGET)
} else {
	# INCLUDEPATH += $$PROJ_DIR
	# QMAKE_LIBDIR += $$PROJ_DIR
	isEmpty(_DONT_LINK_LIBTARGET) {
		win32: LIBS *= -l$$LIBTARGET
		else: LIBS += -l$$LIBTARGET
	}
}
