#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:ts=4:sw=4:expandtab
#
# BuildUtil.py
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

@copyright: Copyright (c) 2005-2014 Parallels IP Holdings GmbH, Inc. All Rights Reserved; U{http://www.parallels.com}
@organization: Parallels IP Holdings GmbH
@contact: U{http://www.parallels.com}
G{packagetree}
G{importgraph}

"""

__docformat__='epytext'

import os
import urllib
import time
import subprocess

# This is for python 2.3 used in
# build of Parallels Transporter
try:
    import string24 as string
except:
    import string

import array
import sys
import platform
import shutil
import tempfile
import traceback
import threading
import fileinput
import getpass
import socket
import random
import ConfigParser
import re
import xml.dom.minidom
import multiprocessing

try:
    import cStringIO as StringIO
except:
    import StringIO


#: Full path of this python file module
__full_file_path__ = os.path.split(os.path.abspath(__file__))[0]

def callersname():
    """
    this uses argument 1, because the call to whoami is now frame 0. and similarly:
    """
    return sys._getframe(2).f_code.co_name



def TRACE(msg=""):
    """
    This method is intended for debug purposes mainly - provides more 'beautiful' output with timings, etc.
    Should be used by all build system modules!
    """
    message = '%s %s(...)   %s' % (
        time.strftime('%H:%M:%S', time.localtime()),
        callersname(),
        msg)
    print message
    return message + '\n'

def os_exec(str, silent = True, envp = None):
    """
    Execute system console command and return status
    On unixes this will most likely call bash, meanwhile on windows this will definitely be cmd.exe
    """

    if not silent:
        print 'cmd "%s" ...' % str

    cmd = subprocess.Popen(
        args = str,
        stdout = None,
        stderr = None,
        env = envp,
        shell = True)
    cmd.communicate()
    ret = cmd.returncode

    if ret and not silent: # - No matter if its silent of not we print an error
        print '  (!) cmd result = %i ' % (ret)
        print

    return ret

# ---------------------------------------------------------
#
# ---------------------------------------------------------
def os_exec_pipe(str, silent = True, envp = None):
    """
    Execute system console command and return status + stdout
    On unixes this will most likely call bash, meanwhile on windows this will definitely be cmd.exe
    """

    if not silent:
        print 'cmd "%s" ...' % str

    cmd = subprocess.Popen(
        args = str,
        stdout = subprocess.PIPE,
        stderr = subprocess.STDOUT,
        env = envp,
        shell = True)
    out, err = cmd.communicate()
    ret = cmd.returncode

    if ret and not silent: # - No matter if its silent of not we print an error
        print '  (!) cmd result = %i ' % (ret)
        print

    return ret, out


def py_exec(cmd, silent = True, unbuffered=False):
    """
    Execute python script and return status
    """

    app = ''
    verbose = ' ' # ' -v '
    if sys.platform == 'win32':
        app = os.path.join(os.path.dirname(sys.executable), 'python.exe') + verbose
    else:
        app = os.path.join(sys.executable) + verbose

    if unbuffered:
        app += '-u '

    command = app + cmd

    # tracing executing command in more readability mode
    TRACE()
    TRACE(command)
    TRACE()

    return os_exec(command, silent)

def mkdir_ensure(path):
    """
    Ensurely make folder (without generating exception)
    """

    path = os.path.normpath(path)

    if os.path.exists(path):
        return

    xx = []
    while path != '':
        h, t = os.path.split(path)
        if t == '':
            xx.append(h)
            break
        path = h
        xx.append(t)

    xx.reverse()

    p = ''
    for x in xx:
        p = os.path.join(p, x)
        if os.path.exists(p):
            continue
        try:
            os.mkdir(p)

        except Exception, e:
            print ' FAILED to create directory %s ' % p
            print e
            pass

def get_cpu_number():
    """
    Determine the number of logical CPUs on the system
    """

    ret = 1

    try:

        if sys.platform == 'win32':
            if os.environ.has_key('NUMBER_OF_PROCESSORS'):
                ret = int(os.environ['NUMBER_OF_PROCESSORS'])
            else:
                ret = 1

        elif sys.platform == 'darwin':

            log = 'sysctl.log'
            cmd = 'sysctl hw.ncpu >%s 2>&1' % log
            if os_exec(cmd) == 0:
                s = file_to_string(log)
                os.remove(log)
                s = s[s.find(' ') :]
                ret = int(s)

        elif sys.platform == 'linux2':
            ret = 0
            fd = open('/proc/cpuinfo', 'r')
            for line in  fd.readlines():
                if line.find('processor', 0) != -1:
                    ret = ret + 1
            fd.close()
    except Exception, err:
        print 'Unable get number of CPUs \'%s\', use default value (1)' % err
        ret = 1
        pass

    return ret


if __name__ == '__main__':

    print 'This module is not supposed to be run inteructivally.'

