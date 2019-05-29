#
# copy_file.pri
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

build_pass: isEmpty(DESTDIR): error(DESTDIR variable must be set for this module)

defineReplace(get_target_path) {
	out_path = $$DESTDIR/$$basename(1)
	return($$out_path)
}

copy_file.input = FILES2COPY
copy_file.output_function = get_target_path
copy_file.dependency_type = TYPE_C
copy_file.commands = $(COPY_FILE) \"${QMAKE_FILE_IN}\" \"${QMAKE_FILE_OUT}\"
copy_file.variable_out = PRE_TARGETDEPS
copy_file.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += copy_file
