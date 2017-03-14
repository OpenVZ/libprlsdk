#
# Options.pri
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

# Common options for build

!win32: equals(TEMPLATE, subdirs) {
	QMAKE_EXTRA_TARGETS = release debug release-clean debug-clean release-distclean debug-distclean

	release.CONFIG = recursive
	debug.CONFIG = recursive

	release-clean.CONFIG = recursive
	debug-clean.CONFIG = recursive

	release-distclean.CONFIG = recursive
	debug-distclean.CONFIG = recursive
}

# addSubdirs(name, path2pro, deps)
defineTest(addSubdirs) {
    name = $$1
    SUBDIRS += $$name
    proj = $$2
    win32: proj = $$replace(proj, "\\\\", "/")
    eval ($${name}.file = $$proj)
    deps = $$3
    for(dep, deps) {
        eval ($${name}.depends += $$dep)
    }
    eval($${name}.makefile = Makefile_$$name)
    export ($${name}.file)
    export ($${name}.depends)
    export ($${name}.makefile)
    export (SUBDIRS)
}

# addSubdirsDir(name, directory, deps)
defineTest(addSubdirsDir) {
    name = $$1
    SUBDIRS += $$name
    proj = $$2
    win32: proj = $$replace(proj, "\\\\", "/")
    eval ($${name}.subdir = $$proj)
    deps = $$3
    for(dep, deps) {
        eval ($${name}.depends += $$dep)
    }
    eval($${name}.makefile = Makefile_$$name)
    export ($${name}.subdir)
    export ($${name}.depends)
    export ($${name}.makefile)
    export (SUBDIRS)
}

