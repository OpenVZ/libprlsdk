#!/usr/bin/env python2
#
# Gen.py
#
# Copyright (c) 2015-2017, Parallels International GmbH
# Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
#
# This file is part of OpenVZ. OpenVZ is free software; you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2 of the License,
# or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
# Schaffhausen, Switzerland.
#

import os
import sys
import ConfigParser
import time
import subprocess
import optparse

optparser = optparse.OptionParser()
optparser.add_option('--pysdk',
	dest = 'pysdk',
	action = 'store_true',
	default = False,
	help = 'generate files for Python SDK')

(opts, args) = optparser.parse_args()

file_path = os.path.split(os.path.abspath(__file__))[0]
if file_path == '':
	file_path = os.path.abspath(os.path.getcwd())

cfg = ConfigParser.ConfigParser()
cfg.readfp(open('Build/Build.cfg'))

fd = open('../Makefile.version')
versionfile = fd.read().strip().split('.')
cfg.add_section('sdk')
cfg.set('sdk', 'major', versionfile[0])
cfg.set('sdk', 'minor', versionfile[1])
cfg.set('sdk', 'version_patch', versionfile[2])
fd.close()

try:
	fd = open('Build/Build.pri', 'w')
except IOError:
	print 'Failed to open "Build/Build.pri"'
	sys.exit(1)

fd.write("DEFINES += PRL_PROD_SERVER=1\n")
fd.write('DEFINES += _LIN_\n')


fd.write('\n')
fd.write('# Product name\n')
fd.write('PRODUCT = server\n')

fd.write( '\n' )
fd.write( '# SDK library versions\n' )
fd.write( 'SDK_VER_MAJ = "%s"\n' % cfg.getint('sdk', 'major'))
fd.write( 'SDK_VER_MIN = "%s"\n' % cfg.getint('sdk', 'minor'))
fd.write( 'SDK_VER_PAT = "%s"\n' % cfg.getint('sdk', 'version_patch'))

fd.write('PREFIX = "%s"' % cfg.get('Build.cfg', 'install_prefix'))

fd.close()


time_fmt = "%a, %d %b %Y %H:%M:%S"
v_date = cfg.get("Build.cfg", "version_date")
v_ts = int(time.mktime(time.strptime(v_date, time_fmt)))

fd = open('Build/Current-gen.ver', 'w')

fd.write("""/* generated by Gen.py; DO NOT EDIT */

#define VER_FULL_BUILD_NUMBER_RELEASE_MAJOR %(version_major)d
#define VER_FULL_BUILD_NUMBER_RELEASE_MINOR %(version_minor)d
#define VER_FULL_BUILD_NUMBER_RELEASE_PATCH %(version_patch)d
#define VER_FULL_BUILD_NUMBER_VERSION_MAJOR %(version_build)d
#define VER_FULL_BUILD_NUMBER_VERSION_MINOR %(version_revision)d
#define VER_BUILD_TIMESTAMP %(version_timestamp)d
#define VER_SPECIAL_BUILD_STR "%(version_date)s"
""" % {'version_major': cfg.getint('SumerBM', 'version_major'),
	'version_minor': cfg.getint('SumerBM', 'version_minor'),
	'version_patch': cfg.getint('SumerBM', 'version_patch'),
	'version_build': cfg.getint('Build.cfg', 'version_build'),
	'version_revision': cfg.getint('Build.cfg', 'version_revision'),
	'version_date': v_date,
	'version_timestamp': v_ts})

fd.write('#define VER_COPYRIGHT_YEAR %d\n' % time.localtime().tm_year)
fd.write('#define VER_COPYRIGHT_YEAR %d\n' % time.localtime().tm_year)
fd.close()


if opts.pysdk:
	# Updating version strings to be up-to-date
	try:
		fd = open('SDK/Python/PrlPython.pri', 'w')
	except IOError:
		print 'Failed to open "SDK/Python/PrlPython.pri"'
		sys.exit(1)

	fd.write('\n')
	fd.write('# //////////////////////////////////////////////////////////////////////////////////////\n')
	fd.write('# //\n')
	fd.write('# // ! AUTOMATICALLY GENERATED HEADER FROM THE SCRIPT !\n')
	fd.write('# // ! ALL MODIFICATIONS WILL BE OVERRIDEN IN THE GEN.PY SCRIPT !\n')
	fd.write('# //\n')
	fd.write('# //////////////////////////////////////////////////////////////////////////////////////\n')
	fd.write('\n')
	fd.write('\n')

	# Python headers include
	python_include_path = sys.exec_prefix + '/include/python' + sys.version[:3]
	python_include_path = os.path.abspath(python_include_path)

	if not os.path.exists(os.path.join(python_include_path, 'Python.h')):
		print
		print 'Error (!) Python headers were not found at path (%s)' % python_include_path
		print 'Description:'
		print 'You probably need to install python-devel package or update this script'
		print 'to choose your python package from the correct path'
		print

		sys.exit(1)

	fd.write('INCLUDEPATH += %s \n' % python_include_path)
	fd.write('\n')

	fd.close()


sys.path.append(os.path.join(file_path, 'SDK'))
import GenApiVer

GenApiVer.gen_sdk_api_ver(cfg.getint('sdk', 'major'))


ver = "%d.%d.%d" % (cfg.getint('sdk', 'major'),
		cfg.getint('sdk', 'minor'),
		cfg.getint('sdk', 'version_patch'))
parallels_sdk_pc = """prefix=%(prefix)s
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${exec_prefix}/lib64

Name: libprlsdk
Description: The Virtualization SDK
Version: %(version)s
Cflags: -I${includedir}/prlsdk
Libs: -L${libdir} -l:libprl_sdk.so.%(version)s
""" % {'prefix': cfg.get('Build.cfg', 'install_prefix'),
	'version': ver}

fd = open("SDK/Handles/parallels-sdk.pc", 'w')
fd.write(parallels_sdk_pc)
fd.close()
