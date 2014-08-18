#!/usr/bin/env python
#
# gen_src.py
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

import sys, os, errno, tempfile, time

__file_dir__ = os.path.abspath(os.path.dirname(__file__))

def log(msg):
    print('gen_src [%s] %s' % (os.getpid(), msg))
    sys.stdout.flush()

class SingleInstance:
    def __init__(self):
        log('SingleInstance')
        self.lockfile = os.path.join(__file_dir__ , '.lock')
        if sys.platform == 'win32':
            while True:
                try:
                    # file already exists, we try to remove (in case previous execution was interrupted)
                    if(os.path.exists(self.lockfile)):
                        os.unlink(self.lockfile)
                    self.fd =  os.open(self.lockfile, os.O_CREAT|os.O_EXCL|os.O_RDWR)
                except OSError, e:
                    if e.errno == 13:
                        log('Another instance is already running')
                        time.sleep(1)
                        continue
                    print e.errno
                    raise
                break
        else: # non Windows
            import fcntl
            self.fd = open(self.lockfile, 'w')
            while True:
                try:
                    fcntl.lockf(self.fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
                except IOError:
                    log('Another instance is already running')
                    time.sleep(1)
                    continue
                break
            log('out of loop')

    def __del__(self):
        log('destructor')
        if sys.platform == 'win32':
            os.close(self.fd)
        else:
            self.fd.close()
            log('lock file-closed')
            try:
                os.unlink(self.lockfile)
            except OSError:
                log('no lock-file')
                pass
        log('exiting')

me = SingleInstance()
log('running XmlModel')
sys.exit(os.system(os.path.join(__file_dir__, '../XmlModel.py')))
