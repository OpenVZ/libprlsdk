#!/usr/bin/env python3
#
# BuildPyPrlSdk.py
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

import os
import sys
__full_file_path__ = os.path.split( os.path.abspath(__file__) )[0]
sys.path.append(os.path.join(__full_file_path__, '../../Build'))
import BuildUtil
import argparse

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
		print('Failed to open "PrlPython.pri"')
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
	# Get Python major version, it will be propagated to C sources
	# Not every source file has Python included, but might still need to know the version
	py_maj_ver = int(py_ver.replace("python","").split(".")[0])
	fd.write('DEFINES += VZ_PY_VER=%d \n' % py_maj_ver)
	if lflags:
		fd.write('QMAKE_LFLAGS += %s\n' % lflags)
	# Make prlsdkapi.py filename version-named to avoid collisions when building for multiple versions
	fd.write('DEFINES += VZ_API_FNAME=\\\"\\\\\\\"$${PYTHON_VER}_prlsdkapi.py\\\\\\\"\\\" \n')

	fd.close ()

def check_retcode(ret):
	if ret:
		sys.exit(ret)

def escape_backslash(path):
	return path.replace('\\', '\\\\')

def parse_args():
	parser = argparse.ArgumentParser()
	parser.add_argument("python_ver",
						choices=["python2.6", "python2.7", "python3.9"],
						help="Python version")
	build_type_group = parser.add_mutually_exclusive_group()
	build_type_group.add_argument("--debug", dest="build_type", action="store_const", const="debug", default="release",
						help="Build debug version")
	build_type_group.add_argument("--release", dest="build_type", action="store_const", const="release",
						help="Build release version (default option)")
	return parser.parse_args()

if '__main__' == __name__:

	args = parse_args()

	build_type = args.build_type
	py_ver = args.python_ver

	cwd = os.getcwd()
	os.chdir(__full_file_path__)

	print('\nBuilding binding for %s\n' % py_ver)
	lflags = None
	if sys.platform.startswith('linux'):
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
			print('Failed to open registry key for %s' % py_ver)
			sys.exit(2)
		python_inst_dir = _winreg.EnumValue(h, 0)[1]
		python_path = escape_backslash(
			os.path.join(python_inst_dir, 'include'))
		lflags = escape_backslash('/LIBPATH:"%s"' % \
			os.path.join(python_inst_dir, 'libs'))
	else:
		RuntimeError('Unsupported platform for "%s" script' % __full_file_path__)
	python_header = os.path.join(python_path, 'Python.h')
	if not os.path.isfile(python_header):
		print('Path doesnt exist: "%s"' % python_header)
		sys.exit(1)
	generate_pri(python_path, py_ver, lflags)
	check_retcode(BuildUtil.os_exec('qmake-qt5'))
	check_retcode(BuildUtil.os_exec('%s clean' % make_cmd))
	check_retcode(BuildUtil.os_exec('qmake-qt5'))
	check_retcode(BuildUtil.os_exec('%s %s' % (make_cmd, build_type)))

	os.chdir(cwd)

