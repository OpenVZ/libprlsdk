#
# gen_xmlmodel_src.pri
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

equals(TEMPLATE, lib) | equals(TEMPLATE, app) {
       !contains(QMAKE_EXTRA_TARGETS, gen_xmlmodel_src) {
               QMAKE_EXTRA_TARGETS += gen_xmlmodel_src
               gen_xmlmodel_src.commands += \
                   $$PYTHON_INTERP \
                       $$SRC_LEVEL/XmlModel/XmlModel.py
					gen_xmlmodel_src.commands += --qmake=$$quote(\"$$QMAKE_QMAKE\")
               win32: gen_xmlmodel_src.commands = $$replace(gen_xmlmodel_src.commands, /, \\)
       }
       win32 {
               # Use ALL_DEPS instead of PRE_TARGETDEPS for one tricky reason.
               # If we have in makefile dependence all -> <target> -> gen_xmlmodel_src
               # target would be rebuilt each time, even if objects are not changed.
               # But introducing dependence all -> gen_xmlmodel_src
               #                               \-> <target>
               # solves this issue. gen_xmlmodel_src is processed stricktly before target
               # but target does not depend on gen_xmlmodel_src itself.
               ALL_DEPS += gen_xmlmodel_src
       } else {
               !contains(QMAKE_EXTRA_COMPILERS, gen_xmlmodel_headrs) {
                       gen_xmlmodel_headrs.input = SOURCES
                       gen_xmlmodel_headrs.commands = @true
                       gen_xmlmodel_headrs.clean = _fake
                       gen_xmlmodel_headrs.output = ${QMAKE_FILE_IN}
                       gen_xmlmodel_headrs.variable_out = _fake_var
                       gen_xmlmodel_headrs.depends = gen_xmlmodel_src
                       gen_xmlmodel_headrs.CONFIG += explicit_dependencies
                       QMAKE_EXTRA_COMPILERS += gen_xmlmodel_headrs
               }
       }
}
