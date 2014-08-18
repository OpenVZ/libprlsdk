#!/usr/bin/env python
#
# BuildPyPrlSdk.py
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

import os
import sys
__full_file_path__ = os.path.split( os.path.abspath(__file__) )[0]
sys.path.append(os.path.join(__full_file_path__, '../../Build'))
import BuildUtil
import BuildConfig

make_cmd = 'make'
if sys.platform == 'win32':
    import _winreg
    make_cmd = 'nmake'


def generate_pri(include_path, py_ver, lflags=None):
	"""
	"""

	try:
		fd = open( 'PrlPython.pri', 'w' )
	except IOError:
		print 'Failed to open "PrlPython.pri"'
		sys.exit(1)

	fd.write( '\n' )
	fd.write( '# //////////////////////////////////////////////////////////////////////////////////////\n' )
	fd.write( '# //\n' )
	fd.write( '# // ! AUTOMATICALLY GENERATED HEADER FROM THE SCRIPT !\n' )
	fd.write( '# // ! ALL MODIFICATIONS WILL BE OVERRIDEN IN THE BuildPyPrlSdk.py SCRIPT !\n' )
	fd.write( '# //\n' )
	fd.write( '# //////////////////////////////////////////////////////////////////////////////////////\n' )
	fd.write( '\n' )
	fd.write( '\n' )

	fd.write('INCLUDEPATH += %s \n' % include_path)
	fd.write('PYTHON_VER = %s \n' % py_ver)
	if lflags:
		fd.write('QMAKE_LFLAGS += %s\n' % lflags)

	fd.close ()

def check_retcode(ret):
	if ret:
		sys.exit(ret)

def escape_backslash(path):
	return path.replace('\\', '\\\\')

if '__main__' == __name__:

	build_type = 'release'
	if len(sys.argv) > 1 and \
			sys.argv[1] in ['debug', 'release']:
		build_type = sys.argv[1]

	cwd = os.getcwd()
	os.chdir(__full_file_path__)

	for py_ver in BuildConfig.supported_python_vers:
		print '\nBuilding binding for %s\n' % py_ver
		lflags = None
		if 'linux2' == sys.platform:
			python_path = os.path.join('/usr/include', py_ver)
		elif 'darwin' == sys.platform:
			# py_ver is smth. like python2.x
			# So strip 'python' string from first instance of py_ver
			common_python_path = 'Python.framework/Versions/%s/include/%s' % (py_ver[6:], py_ver)
			python_path = os.path.join('/Library/Frameworks', common_python_path)
			if not os.path.isdir(python_path):
				python_path = os.path.join('/System/Library/Frameworks', common_python_path)
		elif 'win32' == sys.platform:
			try:
				h = _winreg.OpenKeyEx(
					_winreg.HKEY_LOCAL_MACHINE,
					r'Software\Python\PythonCore\%s\InstallPath' % py_ver[6:])
			except WindowsError:
				print 'Failed to open registry key for %s' % py_ver
				continue
			python_inst_dir = _winreg.EnumValue(h, 0)[1]
			python_path = escape_backslash(
				os.path.join(python_inst_dir, 'include'))
			lflags = escape_backslash('/LIBPATH:"%s"' % \
				os.path.join(python_inst_dir, 'libs'))
		else:
			RuntimeError('Unsupported platform for "%s" script' % __full_file_path__)
		python_header = os.path.join(python_path, 'Python.h')
		if not os.path.isfile(python_header):
			print 'Path doesnt exist: "%s"' % python_header
			continue
		if os.path.isfile('Makefile64'):
			check_retcode(BuildUtil.os_exec('%s -f Makefile64 distclean' % make_cmd))
		elif os.path.isfile('Makefile'):
			check_retcode(BuildUtil.os_exec('%s distclean' % make_cmd))
		generate_pri(python_path, py_ver, lflags)
		check_retcode(BuildUtil.os_exec('qmake'))
		if os.path.isfile('Makefile64'):
			mkfile_arg = '-f Makefile64'
		else:
			mkfile_arg = ''
		check_retcode(BuildUtil.os_exec('%s %s %s' % (make_cmd, mkfile_arg, build_type)))

	os.chdir(cwd)

