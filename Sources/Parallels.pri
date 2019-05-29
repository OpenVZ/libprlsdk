#
# Parallels.pri
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

isEmpty(__PARALLELS_PRI__) {
__PARALLELS_PRI__ = 1

include(Makefile.incl)

linux-*-64 {
	CONFIG += x86_64
	CONFIG += no_fixpath
}

# Pick up build dependencies from non-system locations
!isEmpty(ENABLE_LOCAL_DEPS) {
    message($$LOCAL_DEPS_LIBS)
    QMAKE_INCDIR += $$LOCAL_DEPS_INCLUDE
    QMAKE_LIBDIR += $$LOCAL_DEPS_LIBS
    QMAKE_LFLAGS += -Wl,-rpath-link=$$LOCAL_DEPS_LIBS
}

!include(Build/qmake/paths.pri): error(Unable to include qmake/paths.pri)

isEqual(TEMPLATE, subdirs) {
	include($$SRC_LEVEL/Build/Options.pri)
} else {
	# Make 'debug' to be the last active configuration in non-build_pass run
	# in order build as debug by default
	!build_pass {
		CONFIG -= debug
		CONFIG += debug
	}
	CONFIG += debug_and_release
}

contains(QMAKE_HOST.os, Linux):{
	CONFIG += host_linux
}

# Adding our custom compilers
include($$SRC_LEVEL/Build/qmake/gen_header.pri)
win32: include($$SRC_LEVEL/Build/qmake/gen_dxshaders.pri)

# Getting build configuration (generated with Gen.py script)
!include(Build/Build.pri) {
	error("Build.pri not found - run Gen.py to generate it")
}

# Enable precompiled headers in the project by default
USE_PRECOMPILED_HEADERS = 1

x86only {
	CONFIG += x86
	CONFIG -= x86_64
}

# These macro are the only way of the platform definition
# in our project - please, make use of them and only them (!)
win32:DEFINES     += "_WIN_=1"
win32:DEFINES     += "SUPPORT_ASM_MS"
linux-*:DEFINES   += "_LIN_=1"
linux-*:QMAKE_CXXFLAGS += -fno-strict-aliasing

# Release/Debug detection defines
CONFIG(debug, debug|release) {
    DEFINES += _DEBUG
}
CONFIG(release, debug|release) {
    DEFINES += _RELEASE
}

# Build Unix release with debug symbols
linux-* {
	QMAKE_CFLAGS_RELEASE += -g
	QMAKE_CXXFLAGS_RELEASE += -g
}

# Build WIN32 release with debug symbols
!contains(MAKEFILE_GENERATOR, MINGW) {
    win32:QMAKE_CFLAGS_RELEASE += /Zi
    win32:QMAKE_CXXFLAGS_RELEASE += /Zi
    win32:QMAKE_LFLAGS_RELEASE += /DEBUG
}

win32:release {
# /OPT:REF option forces unreferenced code elimination. It's default opption in Visual Studio project.
QMAKE_LFLAGS_RELEASE	+= /OPT:REF

# shows details of eliminated unreferenced code. It's dangerously verbose!
#QMAKE_LFLAGS_RELEASE	+= /VERBOSE:REF
}

win32 {
	# Precompiled headers are not compatible with /MP flag on MSVC, so we made
	# a decision not to use them here (based on performance measurements)
	USE_PRECOMPILED_HEADERS = 0

	# /MP could only be used with disabled precompiled headers
	!no_mp {
		QMAKE_CXXFLAGS += /MP
		QMAKE_CFLAGS += /MP
	}

	# Disabling link-time code generation in release mode to speedup linking
	# /GL option was added to the release mode in qt-4.5.1
	QMAKE_CXXFLAGS_RELEASE -= -GL /GL
	QMAKE_CFLAGS_RELEASE -= -GL /GL
}

# Visual Studio 8 introduced a strange way to force developers
# move to the Microsoft-Specific version of the Standatd
# We're avoiding all these warnings with this define
win32 {
	DEFINES += _CRT_SECURE_NO_DEPRECATE
	DEFINES += _SCL_SECURE_NO_DEPRECATE
}
# We have support of Win2k and older.
win32:DEFINES += _WIN32_WINNT=0x0500
# Explicitly define NTDDI to Win2kSP1
win32:DEFINES += NTDDI_VERSION=0x05000100

# Force using 64 bit filesize/offset
# since they are 32 bit by default on linux
linux-*:DEFINES += _FILE_OFFSET_BITS=64

# These two configs are assential (!) for the dependencies tracking
# Please see the 'Library Dependencies' section of Qt documentation
# http://doc.trolltech.com/4.2/qmake-environment-reference.html
#    CONFIG += create_prl
#    CONFIG += link_prl

# Storing current target name in preprocessor definition _TARGET_
# with this define we can use this name in logging (to see what module it comes from)
DEFINES += "_TARGET_=$$TARGET"

# If Log4CXX is enabled
enable_log4cxx = $$find(DEFINES, "ENABLE_LOG4CXX")

# Force Log4CXX disabling
!isEmpty(DISABLE_LOG4CXX) {
    enable_log4cxx =
    DEFINES = $$replace(DEFINES, "ENABLE_LOG4CXX", "")
}
count(enable_log4cxx, 1) {
     INCLUDEPATH += $$(LOG4CXX_HOME)/include

     # Debug libs
     CONFIG(debug, debug|release) {

        QMAKE_LIBDIR += $$(LOG4CXX_HOME)/build/debug/shared
        QMAKE_LIBDIR += $$(LOG4CXX_HOME)/build/debug/static

        win32:LIBS += -llog4cxxd -laprutil-1d -lapr-1d
        unix:LIBS += -llog4cxx -laprutil-1 -lapr-1

        # May use QT_FATAL_ASSERT
        DEFINES += QT_FATAL_ASSERT
     }

     # Release libs
     CONFIG(release, debug|release) {
        QMAKE_LIBDIR += $$(LOG4CXX_HOME)/build/release/shared
        QMAKE_LIBDIR += $$(LOG4CXX_HOME)/build/release/static
        LIBS += -llog4cxx -laprutil-1 -lapr-1
     }
}

# To get rid of .manifest files for our generated binaries
# We're using option embed_manifest_xxx that is why we're limited
# to the version 4.1.2+ of Qt libraries
win32:CONFIG += embed_manifest_exe

# For the release version we should define the NDEBUG macro to
# work debug C/C++ macroses correctly
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

# We define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0
# It was done according Apple's recommendation in '/usr/include/AssertMacros.h':
# It is necessary to disable macroses without underscore in '/usr/include/AssertMacros.h'
# This header is included everywhere from Carbon frameworks (<CoreServices/CoreServices.h> for example )
# Macroses have simple name like 'check()', 'require()', 'verify()'  and they are silently replaced to empty string.
#
# Example:
# Without this define the compilation of the next code will be broken:
#
# #include <CoreServices/CoreServices.h>
# void f() {
#   S s;
#   if (!s.check()) // <--- ERRORS HERE: 'if (!s.check())' == is replaced to ==> 'if (!s.)'
#     break;
# }
#

!isEmpty(BUILD_NAME) {
	x86_64: {
		DESTDIR = $$ZBUILD_LEVEL/$${BUILD_NAME}64
	} else {
		DESTDIR = $$ZBUILD_LEVEL/$$BUILD_NAME
	}
	PRL_LIBS_PATH = $$DESTDIR
	QMAKE_LIBDIR += $$PRL_LIBS_PATH
	MOC_DIR = $$join(TARGET, "", $$DESTDIR/.moc/)
	win32: MOC_DIR = $$system($$PWD/Build/qmake/relpath.py $$MOC_DIR $$_PRO_FILE_PWD_)
	OBJECTS_LEVEL = $$DESTDIR/.obj/
	OBJECTS_DIR = $$join(TARGET, "", $$OBJECTS_LEVEL, $$OBJDIR_SUFFIX)
	RCC_DIR = $$join(TARGET, "", $$DESTDIR/.rcc/)
	qt_shared {
		MOC_DIR = $$join(MOC_DIR, "", "", "_shared")
		OBJECTS_DIR = $$join(OBJECTS_DIR, "", "", "_shared")
		RCC_DIR = $$join(RCC_DIR, "", "", "_shared")
	}
}

include($$SRC_LEVEL/Build/qmake/copy_file.pri)

# Profiling support
contains(PROFILING, TRUE) {

	unix {
		QMAKE_CXXFLAGS += -pg
		QMAKE_CFLAGS += -pg
		QMAKE_LFLAGS += -pg
	}
}

# Coverage measurement support
contains(COVERAGE, TRUE) {

	unix {
		QMAKE_CC = $$SRC_LEVEL/Build/bin/gcc
		QMAKE_CXX = $$SRC_LEVEL/Build/bin/g++
		QMAKE_CFLAGS += -ftest-coverage -fprofile-arcs
		QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs
		QMAKE_LFLAGS += -ftest-coverage -fprofile-arcs
	}
}

# gcc/clang address sanitizer support
contains(SANITIZE, TRUE) {
	unix {
		QMAKE_CFLAGS += -fsanitize=address -fno-omit-frame-pointer
		QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
		QMAKE_LFLAGS += -fsanitize=address
	}
}

# Switch gcc into deterministic mode
unix {
	QMAKE_CFLAGS += -frandom-seed=parallels
	QMAKE_CXXFLAGS += -frandom-seed=parallels
}

INCLUDEPATH += . $$SRC_LEVEL $$SRC_LEVEL/Interfaces $$SRC_LEVEL/SDK/Include
DEPENDPATH += . $$SRC_LEVEL $$SRC_LEVEL/Interfaces $$SRC_LEVEL/Monitor/Interfaces

clear(_linkable)
equals(TEMPLATE, app): _linkable = 1
equals(TEMPLATE, lib) {
	dll | shared: _linkable = 1
	staticlib | static: clear(_linkable)
}

!isEmpty(_linkable) {

	defineTest(add_postlink) {
		isEmpty(QMAKE_POST_LINK) {
			QMAKE_POST_LINK = $$1
		} else {
			QMAKE_POST_LINK = $$QMAKE_POST_LINK$$escape_expand("\n\t")$$1
		}

		export(QMAKE_POST_LINK)
	}

	win32 {
		win32-msvc2013 {
			# VS2013 uses subsystem version 6.0 by default when linking binaries, but we need lower version
			# to run on an older OS
			console {
				WIN32_SUBSYSTEM = CONSOLE
			} else {
				WIN32_SUBSYSTEM = WINDOWS
			}

			QMAKE_LFLAGS_$$WIN32_SUBSYSTEM -= /SUBSYSTEM:$$WIN32_SUBSYSTEM
			x86_64 {
				QMAKE_LFLAGS_$$WIN32_SUBSYSTEM += /SUBSYSTEM:$$WIN32_SUBSYSTEM","5.02
			} else {
				# 5.01 is the minimum for VS2013 linker, and enough for PAX
				QMAKE_LFLAGS_$$WIN32_SUBSYSTEM += /SUBSYSTEM:$$WIN32_SUBSYSTEM","5.01

				# But guest OS tools need 5.00 to run on W2K. This isn't required for PAX, but we do this
				# always to be closer to ability have single environment for building all Windows components
				add_postlink($$quote(editbin /SUBSYSTEM:$$WIN32_SUBSYSTEM","5.00 /OSVERSION:5.00 $(DESTDIR_TARGET)))
			}
		}

		sign_bins {
			add_postlink($$quote(\"$$SRC_LEVEL/Build/WinUtil/sign.py\" $(DESTDIR_TARGET)))
		}

		!no_prl_manifest {
			isEmpty(REQ_MANIFEST): REQ_MANIFEST="invoker"
			include($$SRC_LEVEL/Build/Manifests.pri)
		}
	}

	unix {
		enable_strip = $$find(DEFINES, "ENABLE_STRIPPING" )
		count(enable_strip, 1): add_postlink($$STRIP_TOOL "$@")
	}
}

# Clean up target static libs. Qmake doesn't insert this in makefiles
# by default for some unknown reason.
equals(TEMPLATE, lib) {
	staticlib | static: QMAKE_CLEAN += $(DESTDIR)$(TARGET)
}

# Dont allow to build shared libs on linux with undefined symbols
equals(TEMPLATE, lib): linux-* {
	dll | shared: QMAKE_LFLAGS *= -Wl,--no-undefined
}

# Utilize -MD options of gcc to generate dependencies on headers. Thus need to add string
# "include $(wildcard $(OBJECTS_DIR)/*.d)" into makefiles to make this options work.
# "-MP" will insert phony targets for each dependent header file to overcome issues when headers
# were removed or renamed.
#
# qmake also generates dependencies on headers by himself. But if you add new header you should remake your
# makefiles which is not needed in solution above.
#
# This would not work on Mac because we are not allowed to use -M option if multiple
# architectures are specified (universal binaries case) :-(
unix {
	QMAKE_CFLAGS += -MD -MP
	QMAKE_CXXFLAGS += -MD -MP
}

build_pass: unix {
    fake_target_for_include.commands = $$escape_expand("\\n")include $(filter $(wildcard $(OBJECTS_DIR)*.d), $(OBJECTS:.o=.d))
    QMAKE_EXTRA_TARGETS += fake_target_for_include
    QMAKE_CLEAN += $$OBJECTS_DIR/*.d
}

# Forcing warnings to be treated as errors in this specific component
include($$SRC_LEVEL/Build/WarningAsError.pri)

}
