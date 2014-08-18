#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:ts=4:sw=4:expandtab

import sys
import ConfigParser
from datetime import datetime
from os.path import dirname, abspath, join, basename

__full_file_path__ = dirname(abspath(__file__))

GLOBAL_TEMPLATE = """\
%(header)s

#ifndef %(guard_def)s
#define %(guard_def)s

%(def_macroses)s

%(ver_macroses)s

/* Current Parallels API version. */
%(curr_ver_macro)s

/* Current major version of SDK library */
%(curr_sdk_lib_maj)s

#endif // %(guard_def)s

"""

HEADER = """\
/*
 * %s
 *
 * Copyright (C) 2006-%s Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */
"""

DEF_MACROSES = """\
/* Converts major and minor versions of the API to the format
   understood by SDK library. */
#define TO_PARALLELS_API_VER(majorV, minorV) \\
     ((PRL_UINT32)(((PRL_UINT32)majorV) << 16 | ((PRL_UINT16)minorV)))

/* Converts API version from the library native format to
   major and minor values. */
#define FROM_PARALLELS_API_VER(version, majorV, minorV) \\
     majorV = ((PRL_UINT32)version) >> 16; \\
     minorV = ((PRL_UINT32)version) & 0x0000FFFF
"""

PRL_API_VER_MACRO = '#define PARALLELS_API_VER_%u TO_PARALLELS_API_VER(%u,%u)'
CURR_VER_MACRO = '#define PARALLELS_API_VER PARALLELS_API_VER_%u'
CURR_SDK_LIB_MAJ = '#define PARALLELS_SDK_LIB_MAJ "%u"'

def gen_sdk_api_ver(sdk_major):
    target_header = join(__full_file_path__, 'Include/PrlApiVersion.h')
    guard_def = '__%s__' % basename(target_header).upper().replace('.', '_')

    ver_macroses = [PRL_API_VER_MACRO % (1, 0, 1)]
    for i in xrange(2, sdk_major + 1):
        ver_macroses.append(PRL_API_VER_MACRO % (i, i, 0))
    ver_macroses = '\n'.join(ver_macroses)

    f = open(target_header, 'wb')
    f.write(GLOBAL_TEMPLATE % {
        'header': HEADER % (basename(target_header), datetime.now().year),
        'guard_def': guard_def,
        'def_macroses': DEF_MACROSES,
        'ver_macroses': ver_macroses,
        'curr_ver_macro': CURR_VER_MACRO % sdk_major,
        'curr_sdk_lib_maj': CURR_SDK_LIB_MAJ % sdk_major,
        })
    f.close()

def main():
    build_cfg = join(__full_file_path__, '../Build/Build.cfg')
    cfg = ConfigParser.ConfigParser()
    cfg.readfp(open(build_cfg, 'r'))
    gen_sdk_api_ver(int(cfg.get('sdk', 'major')))
    return 0

if  __name__ == '__main__':
    sys.exit(main())
