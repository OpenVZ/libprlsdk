#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:ts=4:sw=4:expandtab
#
# XmlModel.py
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

"""
@copyright: Copyright (c) 1999-2014 Parallels IP Holdings GmbH. All Rights Reserved; U{http://www.parallels.com}
@organization: Parallels IP Holdings GmbH
@contact: U{http://www.parallels.com}
G{classtree BuildLin}
G{packagetree}
G{importgraph}
"""


import os
import sys
import time
import subprocess


# Full path of this python file module
__full_file_path__ = os.path.split( os.path.abspath(__file__) )[0]
if __full_file_path__ == '':
    __full_file_path__ = os.path.abspath( os.path.getcwd() )

sys.path.append( os.path.join( __full_file_path__, '../Build' ) )
from BuildUtil import os_exec

class SingleInstance:
    def __init__(self):
        self.lockfile = os.path.join(__full_file_path__ , '.lock')
        do_exit = False
        if sys.platform == 'win32':
            while True:
                try:
                    # file already exists, we try to remove
                    # (in case previous execution was interrupted)
                    if(os.path.exists(self.lockfile)):
                        os.unlink(self.lockfile)
                    self.fd =  os.open(self.lockfile, os.O_CREAT|os.O_EXCL|os.O_RDWR)
                except OSError, e:
                    if e.errno == 13:
                        time.sleep(1)
                        do_exit = True
                        continue
                    elif e.errno == 17:
                        # 17 - file already exists
                        # Paralell process may create it between
                        # our existance check and calling open()
                        continue
                    print e.errno
                    raise
                break
        else:
            import fcntl
            self.fd = open(self.lockfile, 'w')
            while True:
                try:
                    fcntl.lockf(self.fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
                except IOError:
                    time.sleep(1)
                    do_exit = True
                    continue
                break
        if do_exit:
            sys.exit(0)

    def __del__(self):
        if sys.platform == 'win32':
            os.close(self.fd)
        else:
            self.fd.close()
        try:
            os.unlink(self.lockfile)
        except OSError:
            pass


def xsd2src(xsd_file, out_dir, flags = ''):
    ext = ''
    if sys.platform == 'win32':
        ext = '.exe'

    if sys.platform == 'darwin':
        ext = '.app/Contents/MacOS/xsd2src'

    xsd2src_bin = os.path.abspath( '../../z-Build/Debug/xsd2src' + ext )
    if not os.path.exists(xsd2src_bin):
        xsd2src_bin = os.path.abspath( '../../z-Build/Debug64/xsd2src' + ext )

    # do *not* use os_exec() below: it'll break if your path has spaces
    xsd2src = [xsd2src_bin,
            os.path.abspath(xsd_file),
            os.path.abspath(out_dir)]
    xsd2src.extend(flags.split())

    print " ".join(xsd2src)
    if subprocess.call(xsd2src):
        raise RuntimeError('Code generation failed for scheme "%s"' % xsd_file)

def safe_remove(file):
    if os.path.exists( file ):
        os.remove( file )

def build_xsd2src():
    print 'Building xsd2src utility...'

    ext = ''
    if sys.platform == 'win32':
        ext = '.exe'

    # Remove old binaries for both arch
    safe_remove( os.path.abspath( '../../z-Build/Debug/xsd2src' + ext ) )
    safe_remove( os.path.abspath( '../../z-Build/Debug64/xsd2src' + ext ) )

    os.chdir('./Utils/xsd2src')

    qmake_cmd = 'qmake'
    for arg in sys.argv[1:]:
        if arg.startswith('--qmake='):
            qmake_cmd = arg.split('=')[1]

    if not qmake_cmd == 'qmake' and \
            sys.platform == 'win32' and \
            os.environ['PROCESSOR_ARCHITECTURE'] == 'x86' and \
            'QTDIR_SHARED' in os.environ and \
            'QTDIR_SHARED64' in os.environ:
        qmake64   = os.path.join(os.environ['QTDIR_SHARED64'], 'bin', 'qmake.exe')
        qmake64   = os.path.normpath(os.path.abspath(qmake64))
        qmake64   = os.path.normcase(qmake64)
        qmake_cmd = os.path.normpath(os.path.abspath(qmake_cmd))
        qmake_cmd = os.path.normcase(qmake_cmd)
        if qmake64 == qmake_cmd:
            # On Windows x86 we cannot execute x64 xsd2src.exe, so build is via x86 Qt
            # xsd2src.exe will use Qt dlls on execution; so PATH must points to x86 version ones
            qmake_cmd = os.path.join(os.environ['QTDIR_SHARED'], 'bin', 'qmake.exe')
            os.environ['PATH'] = '%s;%s' % (
                os.path.join(os.environ['QTDIR_SHARED'], 'bin'),
                os.environ['PATH'])

    if os_exec('%s -o Makefile xsd2src.pro' % qmake_cmd, True):
        raise RuntimeError('qmake xsd2src.pro failed')

    if sys.platform == 'win32':
        ret = os_exec('nmake debug', True)
    else:
        ret = os_exec('make debug', True)
    if ret:
        raise RuntimeError('xsd2src utility build failed')

    os.chdir('../..')

def main():
    force = False

    if sys.argv.count( '--force' ):
        force = True

    schemas = (
        ('ClientPreferences', 'ClientPreferences', '', 'pmobile'),
        ('DispConfig', 'DispConfig', '--merge --properties', 'pmobile'),
        ('HostHardwareInfo', 'HostHardwareInfo', '', ''),
        ('FileSystemInfo', 'HostHardwareInfo', '', ''),
        ('ProblemReport', 'ProblemReport', '', 'pmobile'),
        ('UpdaterConfig', 'Updater', '', ''),
        ('Updates', 'Updater', '', ''),
        ('VmConfig', 'VmConfig', '--signals --merge  --properties', 'pmobile'),
        ('VmDirectories', 'VmDirectory', '--merge --properties', 'pmobile'),
        ('VmEvent', 'Messaging', '--merge  --properties', 'pmobile'),
        ('GuestOsInformation', 'GuestOsInformation', '', 'pmobile'),
        ('UserInformation', 'UserInformation', '', 'pmobile'),
        ('Licenses', 'Licenses', '', ''),
        ('NetworkConfig', 'NetworkConfig', '--merge', ''),
        ('KeyboardMouse', 'KeyboardMouse', '', 'pmobile'),
        ('XKeyMaps', 'XKeyMaps', '', ''),
        ('BackupTree', 'BackupTree', '', ''),
        ('DiskImageInfo', 'DiskImageInfo', '', ''),
        ('Appliance', 'Appliance', '--signals --merge --properties', 'pmobile'),
        ('Reports', 'Reports', '', 'pmobile'),
        ('AppLists', 'AppLists', '', ''),
        ('IscsiStorage', 'IscsiStorage', '', ''),
        ('CtTemplate', 'CtTemplate', '', ''),
        ('DiskDescriptor', 'DiskDescriptor', '', ''),
        ('VmInfo', 'VmInfo', '', 'pmobile'),
        ('VmDataStatistic', 'VmDataStatistic', '--properties', ''),
        ('InterfaceInfo', 'InterfaceInfo', '', ''),
        ('CpuFeatures', 'CpuFeatures', '--properties --merge', ''),
    )

    schemas2build = []
    for schema, schema_dir, opts, product in schemas:

        xsd_file = os.path.join('Schema', '%s.xsd' % schema)
        pri_file = os.path.join(schema_dir, '%s.pri' % schema)
        xsd_ts = os.path.getmtime(xsd_file)
        if not os.path.exists(pri_file):
            schemas2build.append((schema, schema_dir, opts))
        else:
            pri_ts = os.path.getmtime(pri_file)
            if xsd_ts > pri_ts or force:
                schemas2build.append((schema, schema_dir, opts))

    if not schemas2build:
        return 0

    try:
        build_xsd2src()
        print 'Generating XmlModel code...'
        for schema, schema_dir, opts in schemas2build:
            xsd2src('./Schema/%s.xsd' % schema, schema_dir, opts)
    except RuntimeError, e:
        print e
        return 1
    except KeyboardInterrupt:
        return 1

    return 0

if ( __name__ == '__main__' ):

    me = SingleInstance()

    # Switching current dir to the folder, where script is
    saved_path = os.getcwd()
    os.chdir( __full_file_path__ )
    ret = main()
    os.chdir( saved_path )

    # returning main status
    sys.exit( ret )
