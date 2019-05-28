#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# AddMethod.py
#
# Copyright (c) 1999-2017, Parallels International GmbH
# Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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
# <http://www.gnu.org/licenses/> or write to Free Software Foundation,
# 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
#
# Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
# Schaffhausen, Switzerland; http://www.virtuozzo.com/.
#

import os
import sys
import re
import argparse
import collections

WORK_DIR = os.path.dirname(os.path.abspath(__file__))

SDK_HEADER_PATH = os.path.join(WORK_DIR, 'Include')
SDK_HANDLES_PATH = os.path.join(WORK_DIR, 'Handles')
SDK_PYTHON_PATH = os.path.join(WORK_DIR, 'Python', 'sdk2python')
INTERFACES_PATH = os.path.join(WORK_DIR, '..', 'Interfaces')
LIBRARIES_PATH = os.path.join(WORK_DIR, '..', 'Libraries')
TESTS_PATH = os.path.join(WORK_DIR, '..', 'Tests', 'SDKTest')

SDK_SOURCES = collections.OrderedDict([
    ('Core', ('PrlApiCore.h', 'PrlApiCore.cpp')),
    ('Disk', ('PrlDisk.h', 'PrlHandleDiskSDK.cpp')),
    ('Disp', ('PrlApiDisp.h', 'PrlApiDisp.cpp')),
    ('Net', ('PrlApiNet.h', 'PrlApiNet.cpp')),
    ('Stat', ('PrlApiStat.h', 'PrlApiStat.cpp')),
    ('Vm', ('PrlApiVm.h', 'PrlApiVm.cpp')),
])

SDK_PREFIXES = {
    'VmCfg' : 'Vm',
    'DispCfg' : 'DispConfig',
    'SrvCfg' : 'SrvConfig',
    'UsbIdent' : 'UsbIdentity',
}

def locate(subsystem):
    '''
    Build path to API files according to specified subsystem.
    Returns (path-to-.h, path-to-.cpp) as a result.
    '''

    src = SDK_SOURCES.get(subsystem, None)
    if not src:
        raise RuntimeError('Unknown subsystem %s is specified' % subsystem)

    return (os.path.join(SDK_HEADER_PATH, src[0]),
            os.path.join(SDK_HANDLES_PATH, subsystem, src[1]))

def trace(func):
    '''Trace beginning and ending of class method execution.'''

    def wrapper(self):
        print >> sys.stdout, os.linesep, func.__name__

        if func(self) is True:
            print >> sys.stdout, "\t\t\tOK"
            return

        print >> sys.stdout, "\t\t\tFAIL"

    return wrapper

class Method(object):
    def __init__(self, subsystem, method_name, is_async):
        self.subsystem = subsystem
        self.is_async = is_async

        m = re.match("(Prl[A-Z].*)_([A-Z]\w+)\((.*)\)", method_name)
        if not m:
            raise RuntimeError("Method name doesn't conform rules:" \
                "PrlXxx_Xxxxx(X arg1, ... ) !")
        self.prefix = m.group(1)
        self.method_subname = m.group(2)
        self.method_name = self.prefix + "_" + self.method_subname
        self.args = m.group(3).split(',')
        for i in range(len(self.args)):
            self.args[i] = self.args[i].strip()
        self.args_hdr = ''
        self.args_hdr_1line = ''
        self.args_hdr_names = ''
        self.args_hdr_names_1line = ''
        self.args0_hdr = ''
        self.args0_hdr_1line = ''
        self.args0_hdr_names = ''
        self.args0_hdr_names_1line = ''
        self.args_msg_format = ''
        self.args_types = ''
        arg_count = 1
        for arg in self.args:
            arg_name = arg.split().pop()
            arg_type = arg.split()[0]
            if arg_count < len(self.args):
                self.args_hdr += "\t\t%s,\n" % arg
                self.args_hdr_1line += "%s, " % arg
                self.args_hdr_names += "\t\t%s,\n" % arg_name
                self.args_hdr_names_1line += "%s, " % arg_name
                self.args_msg_format += "%s=%%.8X, " % arg_name
                self.args_types += "%s, " % arg_type
                if arg_count > 1:
                    self.args0_hdr += "\t\t%s,\n" % arg
                    self.args0_hdr_1line += "%s, " % arg
                    self.args0_hdr_names += "\t\t%s,\n" % arg_name
                    self.args0_hdr_names_1line += "%s, " % arg_name
            else:
                self.args_hdr += "\t\t%s" % arg
                self.args_hdr_1line += arg
                self.args_hdr_names += "\t\t%s" % arg_name
                self.args_hdr_names_1line += arg_name
                self.args_msg_format += "%s=%%.8X" % arg_name
                self.args_types += arg_type
                if arg_count > 1:
                    self.args0_hdr += "\t\t%s" % arg
                    self.args0_hdr_1line += arg
                    self.args0_hdr_names += "\t\t%s" % arg_name
                    self.args0_hdr_names_1line += arg_name
            arg_count += 1

        # PrlSrv -> SRV
        pr_match = re.match("^Prl([A-Z].*)$", self.prefix)
        if not pr_match:
            raise RuntimeError("Method prefix doesn't conform rules: PrlXxx !")
        self.prefix_short = pr_match.group(1)

        self.prefix_short = SDK_PREFIXES.get(self.prefix_short, self.prefix_short)
        if self.prefix_short == 'Srv' and is_async is False:
            self.prefix_short = 'Server'

        # capped prefix exception Vm -> VM_CONFIGURATION
        if self.prefix_short == 'Vm':
            self.capped_prefix = 'VM_CONFIGURATION'
        else:
            i = 0
            for str in re.findall("[A-Z]+[a-z0-9]*", self.prefix_short):
                if i == 0:
                    self.capped_prefix = str.upper()
                    i = 1
                else:
                    self.capped_prefix += "_" + str.upper()

        # GetCtList -> GET_CT_LIST
        i = 0
        for str in re.findall("[A-Z]+[a-z0-9]*", self.method_subname):
            if i == 0:
                self.capped_method_subname = str.upper()
                i = 1
            else:
                self.capped_method_subname += "_" + str.upper()
        self.capped_method_name = self.capped_prefix + '_' + self.capped_method_subname

    def __str__(self):
        '''Print parsing results.'''

        descr = []

        if self.is_async is True:
            descr.append('Type: Asynchronous Method')
        else:
            descr.append('Type: Synchronous Method')

        descr.extend([
            'Name: %s' % self.method_name,
            'Short Name: %s' % self.method_subname,
            'Prefix: %s' % self.prefix,
            'Short Prefix: %s' % self.prefix_short,
            'Capitalized Name: %s' % self.capped_method_name,
            'Capitalized Prefix: %s' % self.capped_prefix,
            'Arguments: %s' % self.args,
            ])

        return os.linesep.join(descr)

    @trace
    def add_pjoc_enum(self):
        '''
        Add PJOC_SRV_METHOD_NAME to _PRL_JOB_OPERATION_CODE enum
        in SDK/Include/PrlEnums.h.
        '''

        rc = False
        f = os.path.join(SDK_HEADER_PATH, "PrlEnums.h")
        sys.stdout.write("%s..." % f)

        with open(f) as src:
            enums = src.read()

        new_pjoc = '\tPJOC_' + self.capped_method_name + '\t\t\t\t\t= '
        in_joc = 0
        last_num = 0
        out_enums = ''
        for line in enums.splitlines():
            if in_joc == 1:
                mm = re.match("^\s*PJOC_.*\s*=\s*([0-9]+),", line)
                if mm:
                    last_num = mm.group(1)
                if line == '}':
                    last_num = str(int(last_num) + 1)
                    out_enums += new_pjoc + last_num + ',\n'
                    rc = True
                    in_joc = 0
            elif line == 'typedef enum _PRL_JOB_OPERATION_CODE':
                in_joc = 1
            out_enums += line + '\n'

        with open(f, 'w') as dst:
            dst.write(out_enums)

        return rc

    @trace
    def add_wrap(self):
        '''
        Add PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_MethodName )
        to SDK/Include/PrlApiMacro.h.
        '''

        rc = False
        hdr = os.path.join(SDK_HEADER_PATH, "PrlApiMacro.h")
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        new_wrap = 'PRL_SDK_WRAP_FOR_EACH_ITERATOR( ' + self.method_name + ' ) \\'
        in_wrap = 0
        out_data = ''
        for line in data.splitlines():
            if in_wrap == 1 and line == '':
                out_data += new_wrap + '\n'
                rc = True

            if re.match("^\s*PRL_SDK_WRAP_FOR_EACH_ITERATOR.*\\\\", line):
                in_wrap = 1
            else:
                in_wrap = 0
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_api_header(self):
        '''Add PrlSrv_MethodName header to SDK/Include/PrlApi.h.'''

        rc = False

        if self.is_async is False:
            method_decl = 'PRL_METHOD_DECL'
        else:
            method_decl = 'PRL_ASYNC_SRV_METHOD_DECL'

        api_hdr = """/* FIXME There should be description what method does.
   Parameters
   hHandle :  A handle of type PHT_FIXME identifying the FIXME.
   nFlags  :  Reserved parameter.
   Returns
       FIXME There should be information about return values */
%s( PARALLELS_API_VER_5,\n\t\t\t%s, (\n%s\n\t\t) );\n\n""" % \
        (method_decl, self.method_name, self.args_hdr)

        hdr = locate(self.subsystem)[0]
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        in_win = 0
        out_data = ''
        for line in data.splitlines():
            if in_win == 1:
                if re.match("\s*#pragma pack\(pop, save_api_pack\)", line):
                    out_data += api_hdr
                    rc = True

                out_data += '#ifdef _WIN_\n'
            if line == '#ifdef _WIN_':
                in_win = 1
                continue
            else:
                in_win = 0
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_api_body(self):
        '''Add PrlSrv_MethodName method to SDK/Handles/PrlApi.cpp'''

        if self.is_async is False:
            arg0_name = self.args[0].split().pop()
            out_data = """
PRL_METHOD( %s ) (
%s
\t\t)
{
\tSYNC_CHECK_API_INITIALIZED

\tLOG_MESSAGE( DBG_DEBUG, "%%s (%s)",
\t\t__FUNCTION__,
%s
\t);

\tif (PRL_WRONG_HANDLE(%s, PHT_%s))
\t\treturn PRL_ERR_INVALID_ARG;

\tPrlHandle%sPtr p%s = PRL_OBJECT_BY_HANDLE<PrlHandle%s>( %s );

\treturn p%s->%s(%s);
}

""" % \
            (self.method_name, self.args_hdr, \
            self.args_msg_format, self.args_hdr_names, \
            arg0_name, \
            self.capped_prefix, self.prefix_short, self.prefix_short, \
            self.prefix_short, arg0_name, self.prefix_short, \
            self.method_subname, self.args0_hdr_names_1line)
        else:
            out_data = """
void PrlContextSwitcher::%s(%s)
{
\tONE_HANDLE_SRV_METH_IMPLEMENTATION_WITH_FLAGS(%s, %s,
\t\tPJOC_%s)
}

PRL_ASYNC_METHOD( %s ) (
%s
\t)
{
\tASYNC_CHECK_API_INITIALIZED(PJOC_%s)
\tCALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), %s, (%s))
\treturn (PrlContextSwitcher::Instance()->GetAsynchOpResult());
}

""" % \
            (self.method_name, self.args_hdr_1line, self.method_subname, \
            self.args0_hdr_names_1line, self.capped_method_name, \
            self.method_name, self.args_hdr, self.capped_method_name, \
            self.method_name, self.args_hdr_names_1line)

        f = locate(self.subsystem)[1]
        sys.stdout.write("%s..." % f)

        with open(f, 'a') as dst:
            dst.write(out_data)

        return True

    @trace
    def add_pve_header(self):
        '''
        Add DspCmdMethodName as public method to CPveControl
        in SDK/Handles/PveControl.h.
        '''

        rc = False
        hdr = os.path.join(SDK_HANDLES_PATH, "Disp/PveControl.h")
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        in_public = 0
        pve_hdr = """\t/**
\t * FIXME
\t * @return id of performed asynchronous request
\t */
\tQString DspCmd%s(%s);\n\n""" % (self.method_subname, self.args0_hdr_1line)

        out_data = ''
        for line in data.splitlines():
            if in_public == 1 and line == 'private:':
                out_data += pve_hdr
                rc = True
                in_public = 0
            if line == 'public://Protocol methods':
                in_public = 1
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_pve_body(self):
        '''Add DspCmdMethodName method to SDK/Handles/PveControl.cpp.'''

        out_data = """
QString CPveControl::DspCmd%s(%s)
{
\tLOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmd%s()");

\tCHECK_CONNECTION_STATUS

\tCProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
\t\t\t\t\t\t\t\t\tPVE::DspCmd%s,
\t\t\t\t\t\t\t\t\tfalse,
\t\t\t\t\t\t\t\t\t%s);

\treturn SendRequestToServer(pRequest);
}

""" % \
            (self.method_subname, self.args0_hdr_1line, self.method_subname, \
            self.method_subname, self.args0_hdr_names)
        f = os.path.join(SDK_HANDLES_PATH, "Disp/PveControl.cpp")
        sys.stdout.write("%s..." % f)

        with open(f, 'a') as dst:
            dst.write(out_data)

        return True

    @trace
    def add_srv_header(self):
        '''Add MethodName as public method to SDK/Handles/PrlHandleServer.h.'''

        rc = False
        hdr = os.path.join(SDK_HANDLES_PATH, "Disp/PrlHandleServer.h")
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        in_public = 0
        srv_hdr = """
\t/**
\t * FIXME
\t * @param command flags
\t */
\tPrlHandleJobPtr %s(%s);\n\n""" % (self.method_subname, self.args0_hdr_1line)

        out_data = ''
        for line in data.splitlines():
            if in_public == 1 and (line == 'private:' or line == 'protected:'):
                out_data += srv_hdr
                rc = True
                in_public = 0
            if line == 'public://VM operations calls':
                in_public = 1
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_srv_body(self):
        '''
        Add MethodName as public method
        to SDK/Handles/PrlHandleServer.cpp.
        '''

        out_data = """
PrlHandleJobPtr PrlHandleServer::%s(%s)
{
\tQString job_uuid = m_pPveControl->DspCmd%s(%s);

\treturn PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
\t\t\t\t\t\t\tPJOC_%s)));
}

""" % \
            (self.method_subname, self.args0_hdr_1line, self.method_subname, \
            self.args0_hdr_names_1line, self.capped_method_name)
        f = os.path.join(SDK_HANDLES_PATH, "Disp/PrlHandleServer.cpp")
        sys.stdout.write("%s..." % f)

        with open(f, 'a') as dst:
            dst.write(out_data)

        return True

    @trace
    def add_handler_header(self):
        '''Add MethodName as public method to SDK/Handles/PrlHandle*.h.'''

        rc = False
        hdr = os.path.join(SDK_HANDLES_PATH, self.subsystem, "PrlHandle%s.h" % self.prefix_short)
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        in_public = 0
        call_hdr = """\t/**
\t * FIXME
\t * @param command flags
\t */
\tPRL_RESULT %s(%s);\n\n""" % (self.method_subname, self.args0_hdr_1line)

        out_data = ''
        for line in data.splitlines():
            if in_public == 1 and (line == 'private:' or line == 'protected:'):
                out_data += call_hdr
                rc = True
                in_public = 0
            if line == 'public:':
                in_public = 1
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_handler_body(self):
        '''Add MethodName as public method ro SDK/Handles/PrlHandle*.cpp.'''

        out_data = """
PRL_RESULT PrlHandle%s::%s(%s)
{
\tSYNCHRO_INTERNAL_DATA_ACCESS
\t/* FIXME Return actual data from prlxmlmodel */
\treturn PRL_ERR_UNIMPLEMENTED;
}
""" % (self.prefix_short, self.method_subname, self.args0_hdr_1line)
        f = os.path.join(SDK_HANDLES_PATH, self.subsystem, "PrlHandle%s.cpp" % self.prefix_short)
        sys.stdout.write("%s..." % f)

        with open(f, 'a') as dst:
            dst.write(out_data)

        return True

    @trace
    def add_connector(self):
        '''
        Add CONNECT_METH_IN_CTXT_SWITCHER PrlSrv_MethodName
        to SDK/Handles/PrlCommon.cpp.
        '''

        rc = False
        hdr = os.path.join(SDK_HANDLES_PATH, "Core/PrlCommon.cpp")
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        new_line = """\tCONNECT_METH_IN_CTXT_SWITCHER(PrlContextSwitcher::Instance(), %s, \\
            (%s));\n""" % (self.method_name, self.args_types)

        in_connector = 0
        out_data = ''
        for line in data.splitlines():
            if in_connector == 1 and line == '}':
                out_data += new_line
                rc = True
                in_connector = 0
            if re.match('\s*CONNECT_METH_IN_CTXT_SWITCHER.*', line):
                in_connector = 1
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_cont_sw(self):
        '''
        Add 3 lines for PrlSrv_MethodName
        to SDK/Handles/PrlContextSwitcher.h.
        '''

        hdr = os.path.join(SDK_HANDLES_PATH, "Core/PrlContextSwitcher.h")
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        new_line1 = """FRIEND_ASYNC_METH_IN_CTXT_SWITCHER(%s, (%s));\n""" % \
            (self.method_name, self.args_types)
        new_line2 = """\tvoid signal%s(%s);\n""" % \
            (self.method_name, self.args_types)
        new_line3 = """\tvoid %s(%s);\n""" % \
            (self.method_name, self.args_types)

        in_friend = 0
        in_signals = 0
        in_slots = 0
        out_data = ''
        count = 0
        for line in data.splitlines():
            if in_friend == 1 and line == 'signals:':
                out_data += new_line1
                count += 1
                in_friend = 0
            if re.match('FRIEND_ASYNC_METH_IN_CTXT_SWITCHER', line):
                in_friend = 1
            if in_signals == 1 and line == '':
                out_data += new_line2
                count += 1
                in_signals = 0
            if line == 'signals:':
                in_signals = 1
            if in_slots == 1 and line == '':
                out_data += new_line3
                count += 1
                in_slots = 0
            if line == 'public slots:':
                in_slots = 1
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return count == 3

    @trace
    def add_srv_job(self):
        '''
        Add PVE::DspCmdMethodName mapping
        to Libraries/PrlCommonUtilsBase/CommandConvHelper.cpp.
        '''

        rc = False
        hdr = os.path.join(LIBRARIES_PATH, "PrlCommonUtils", "CommandConvHelper.cpp")
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        new_line = """\t(*DispatcherCommandsToJobTypesMap())[PVE::DspCmd%s] = PJOC_%s;\n""" % \
            (self.method_subname, self.capped_method_name)

        in_map = 0
        out_data = ''
        for line in data.splitlines():
            if in_map == 1 and line == '':
                out_data += new_line
                rc = True
                in_map = 0
            if re.match('\s*\(\*DispatcherCommandsToJobTypesMap', line):
                in_map = 1
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_iface(self):
        '''
        Add DspCmdMethodName definition
        to Interfaces/ParallelsNamespace.h.
        '''

        rc = False
        f = os.path.join(INTERFACES_PATH, "ParallelsNamespace.h")
        sys.stdout.write("%s..." % f)

        with open(f) as src:
            data = src.read()

        new_line = '\t\t\tDspCmd' + self.method_subname + '\t\t= DspClientToDispatcherCommandRangeStart + '
        in_enum = 0
        last_num = 0
        out_data = ''
        for line in data.splitlines():
            if in_enum == 1:
                mm = re.match("^\s*DspCmd.*\s*=\s*DspClientToDispatcherCommandRangeStart \+ ([0-9]+),", line)
                if mm:
                    last_num = mm.group(1)
                if re.match('\s*DspClientToDispatcherCommandRangeEnd =', line):
                    last_num = str(int(last_num) + 1)
                    out_data += new_line + last_num + ',\n'
                    rc = True
                    in_enum = 0
            elif re.match('\s*DspClientToDispatcherCommandRangeStart =', line):
                in_enum = 1
            out_data += line + '\n'

        with open(f, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_proto_serializer(self):
        '''
        Add PVE::DspCmdMethodName handling
        to Libraries/ProtoSerializer/CProtoSerializer.cpp.
        '''

        rc = False
        f = os.path.join(LIBRARIES_PATH, "ProtoSerializer", "CProtoSerializer.cpp")
        sys.stdout.write("%s..." % f)

        with open(f) as src:
            data = src.read()

        new_line = '\t\tcase PVE::DspCmd%s:\n' % self.method_subname
        out_data = ''
        for line in data.splitlines():
            if re.match("^\s*pCommand.*ProtoCommandWithoutParams", line):
                out_data += new_line
                rc = True

            out_data += line + '\n'

        with open(f, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_std(self):
        '''
        Add CASE_DISPATCHER_COMMAND( PVE::DspCmdMethodName )
        to Libraries/Std/ParallelsNamespace.cpp.
        '''

        rc = False
        f = os.path.join(LIBRARIES_PATH, "Std", "ParallelsNamespace.cpp")
        sys.stdout.write("%s..." % f)

        with open(f) as src:
            data = src.read()

        new_line = '\t\tCASE_DISPATCHER_COMMAND( DspCmd%s )\n' % self.method_subname
        in_case = 0
        out_data = ''
        for line in data.splitlines():
            if in_case == 1 and line == '':
                out_data += new_line
                rc = True
                in_case = 0
            if re.match('\s*CASE_DISPATCHER_COMMAND', line):
                in_case = 1
            out_data += line + '\n'

        with open(f, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_test_header(self):
        '''
        Add MethodName & MethodNameOnWrongParams testcases
        to Tests/SDKTest/PrlSrvManipulationsTest.h
        '''

        rc = False
        f = os.path.join(TESTS_PATH, "%sManipulationsTest.h" % self.prefix)
        sys.stdout.write("%s..." % f)

        with open(f) as src:
            data = src.read()

        new_line = '\tvoid test%s();\n\tvoid test%sOnWrongParams();\n' % (self.method_subname, self.method_subname)
        in_slots = 0
        out_data = ''
        for line in data.splitlines():
            if in_slots == 1 and line == '':
                out_data += new_line
                rc = True
                in_slots = 0
            if line == 'private slots:':
                in_slots = 1
            out_data += line + '\n'

        with open(f, 'w') as dst:
             dst.write(out_data)

        return rc

    @trace
    def add_test_body(self):
        '''
        Add MethodName & MethodNameOnWrongParams testcases
        to Tests/SDKTest/PrlSrvManipulationsTest.cpp
        '''

        out_data = """void %sManipulationsTest::test%s()
{
\t/* FIXME Add real test here */
}

void %sManipulationsTest::test%sOnWrongParams()
{
\t// Wrong handle
\tCHECK_ASYNC_OP_FAILED(%s(PRL_INVALID_HANDLE, 0), PRL_ERR_INVALID_ARG)
\t// CHECK_ASYNC_OP_FAILED(%s(_hWrongHandle, 0), PRL_ERR_INVALID_ARG)
}

""" % (self.prefix, self.method_subname, self.prefix, self.method_subname, self.method_name, self.method_name)
        f = os.path.join(TESTS_PATH, "%sManipulationsTest.cpp" % self.prefix)
        sys.stdout.write("%s..." % f)

        with open(f, 'a') as dst:
            dst.write(out_data)

        return True

class Handle(object):
    def __init__(self, subsystem, name):
        self.subsystem = subsystem

        m = re.match('PHT_([A-Z_]+)', name)
        if not m:
            raise RuntimeError("Handle name doesn't conform rules:" \
                "PHT_XXX[_XX,..]!")

        self.name = name
        self.short_name = m.group(1)

        self.class_subname = ''.join(i.capitalize() \
            for i in self.short_name.split('_'))

        self.class_name = "PrlHandle" + self.class_subname

    def __str__(self):
        '''Print parsing results.'''

        descr = [
            'Type: Handle',
            'Name: %s' % self.name,
            'Short Name: %s' % self.short_name,
            'Class Name: %s' % self.class_name,
            'Class Sub-Name: %s' % self.class_subname,
        ]

        return os.linesep.join(descr)

    @trace
    def add_python(self):
        '''
        Add declaration of the new handle
        to SDK/Python/sdk2python/PythonComposer.cpp.
        '''

        rc = False
        hdr = os.path.join(SDK_PYTHON_PATH, "PythonComposer.cpp")
        sys.stdout.write("%s..." % hdr)

        with open(hdr) as src:
            data = src.read()

        new_line = """{"%s",   "",             "_Handle",    "", "%s",  "",0,0,0,0},\n""" % \
            (self.class_subname, self.name)

        in_map = 0
        out_data = ''
        for line in data.splitlines():
            if in_map == 1 and line == '};':
                out_data += new_line
                rc = True
                in_map = 0
            if re.match('.*\s+PythonComposer::s_CustomClassInfo\[\].*', line):
                in_map = 1
            out_data += line + '\n'

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return rc

    @trace
    def add_handle_enum(self):
        '''
        Add PHT_NAME to PRL_HANDLE_TYPE enum in SDK/Include/PrlEnums.h.
        '''

        f = os.path.join(SDK_HEADER_PATH, "PrlEnums.h")
        sys.stdout.write("%s..." % f)

        with open(f) as src:
            enums = src.read()

        new_line = '\t' + self.name + '\t\t\t\t\t\t\t= '
        in_enum = 0
        last_num = 0
        num_changes = 0
        out_enums = ''
        for line in enums.splitlines():
            if in_enum == 1:
                mm = re.match("^\s*PHT_.*\s*=\s*(0?x?[0-9A-Fa-f]+),", line)
                if mm:
                    last_num = mm.group(1)
                if line == '':
                    last_num = "0x%x" % (int(last_num, 16) + 1)
                    out_enums += new_line + last_num + ',\n'
                    num_changes = 1
                    in_enum = 0
            elif re.match('\s*PHT_LAST =', line):
                line = '\tPHT_LAST = ' + self.name + ' // should be set to last'
                num_changes = 2
            elif line == 'typedef enum _PRL_HANDLE_TYPE':
                in_enum = 1
            out_enums += line + '\n'

        with open(f, 'w') as dst:
            dst.write(out_enums)

        return num_changes == 2

    @trace
    def add_handler_header(self):
        '''Define handle class in SDK/Handles/PrlHandle*.h.'''

        hdr = os.path.join(SDK_HANDLES_PATH, self.subsystem, '%s.h' % self.class_name)
        sys.stdout.write('%s...' % hdr)

        out_data = '''/*
 * {0}.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#ifndef __PARALLELS_HANDLE_{1}_H__
#define __PARALLELS_HANDLE_{1}_H__


#include "PrlHandleBase.h"
#include <prlxmlmodel/{2}/{2}.h>


/**
 * Handle object represents FIXME OBJECT NAME
 */
class {0}: public PrlHandleBase
{{
public:

\t/**
\t * Class default constructor.
\t */
\t{0}();

\t/**
\t * Class initialize constructor.
\t */
\t{0}(QString s{2});

\t/**
\t * Initialize object from XML string
\t */
\tPRL_RESULT fromString(PRL_CONST_STR sXml);

\t/**
\t * Returns object as XML string
\t */
\tQString toString();

private:

\t{2} m_{2};
}};


typedef PrlHandleSmartPtr<{0}> {0}Ptr;


#endif\t// __PARALLELS_HANDLE_{1}_H__
'''.format(self.class_name, self.short_name, self.class_subname)

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return True

    @trace
    def add_handler_body(self):
        '''Add the new handler class to SDK/Handles/*/PrlHandle*.cpp.'''

        hdr = os.path.join(SDK_HANDLES_PATH, self.subsystem, "%s.cpp" % self.class_name)
        sys.stdout.write("%s..." % hdr)

        out_data = '''/*
 * {0}.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#include "{0}.h"

{0}::{0}()
: PrlHandleBase({1})
{{
}}

{0}::{0}(QString s{2})
: PrlHandleBase({1})
{{
\tm_{2}.fromString(s{2});
}}

PRL_RESULT {0}::fromString(PRL_CONST_STR sXml)
{{
\tSYNCHRO_INTERNAL_DATA_ACCESS
\tif (PRL_SUCCEEDED(m_{2}.fromString(QString::fromUtf8(sXml))))
\t\treturn PRL_ERR_SUCCESS;

\treturn PRL_ERR_INVALID_ARG;
}}

QString {0}::toString()
{{
\tSYNCHRO_INTERNAL_DATA_ACCESS
\treturn m_{2}.toString();
}}
'''.format(self.class_name, self.name, self.class_subname)

        with open(hdr, 'w') as dst:
            dst.write(out_data)

        return True

    @trace
    def add_to_build(self):
        '''Add the new handler to SDK/Handles/SDKSources.pri.'''

        pri = os.path.join(SDK_HANDLES_PATH, 'SDKSources.pri')
        sys.stdout.write(pri + '...')

        blank = '\t$$SRC_LEVEL/SDK/Handles/{0}/{1}{2}'
        header = blank.format(self.subsystem, self.class_name, '.h')
        source = blank.format(self.subsystem, self.class_name, '.cpp')

        output = []
        num_changes = 0
        in_hdrs = in_srcs = False

        with open(pri, 'r+') as src:
            for line in src.read().splitlines():
                if in_hdrs and not line.strip():
                    output.append(header)
                    num_changes += 1
                    in_hdrs = False
                elif in_srcs and not line.strip():
                    output.append(source)
                    num_changes += 1
                    in_srcs = False
                elif (in_hdrs or in_srcs) and not line.endswith('\\'):
                    line += ' \\'
                elif line.startswith('HEADERS'):
                    in_hdrs = True
                elif line.startswith('SOURCES'):
                    in_srcs = True

                output.append(line)

            src.seek(0)
            src.write(os.linesep.join(output))

        return num_changes == 2

def gen_sync_method(subsystem, declaration):
    '''Generate synchronous method code.'''

    method = Method(subsystem, declaration, False)
    print >> sys.stdout, os.linesep, method

    method.add_wrap()
    method.add_api_header()
    method.add_api_body()

    if not method.prefix in ['PrlApi', 'PrlDbg']:
        method.add_handler_header()
        method.add_handler_body()

def gen_async_method(subsystem, declaration):
    '''Generate asynchronous method code.'''

    method = Method(subsystem, declaration, True)
    print >> sys.stdout, os.linesep, method

    method.add_pjoc_enum()
    method.add_wrap()
    method.add_api_header()
    method.add_api_body()
    method.add_pve_header()
    method.add_pve_body()

    if method.capped_prefix == 'SRV':
        method.add_srv_header()
        method.add_srv_body()

# FIXME: adjust methods to new implementation of connector and sw.
    # method.add_connector()
    # method.add_cont_sw()

# FIXME: Related to libprlcommon, need to provide path to the repo somehow.
    # method.add_srv_job()
    # method.add_iface()
    # method.add_std()

# FIXME: SDK tests are part of prl_disp, need to provide path to the repo.
    # self.add_test_header()
    # self.add_test_body()

# FIXME: ProtoSerializer is a part of prlcommon, need to provide path to the repo.
    # method.add_proto_serializer()

def gen_handle(subsystem, declaration):
    '''Generate handle code.'''

    handle = Handle(subsystem, declaration)
    print >> sys.stdout, os.linesep, handle

    handle.add_python()
    handle.add_handle_enum()
    handle.add_handler_header()
    handle.add_handler_body()
    handle.add_to_build()

    print >> sys.stdout, os.linesep, \
        'Don\'t forget to add the new files to Git!'

COMMANDS = collections.OrderedDict([
    ('sync', gen_sync_method),
    ('async', gen_async_method),
    ('handle', gen_handle),
])

def main():
    try:
        parser = argparse.ArgumentParser()

        parser.add_argument('-m', '--mode', choices=COMMANDS.keys(), required=True,
            help='Working mode, e.g. %s ' % COMMANDS.keys())

        parser.add_argument('-s', '--subsystem', choices=SDK_SOURCES.keys(), required=True,
            help='Target subsystem, e.g. %s ' % SDK_SOURCES.keys())

        parser.add_argument('-d', '--declaration', required=True,
            help='Declaration of new method/handler, e.g. PrlSrv_GetCtList(PRL_HANDLE hServer) or PHT_CT_TEMPLATE')

        args = parser.parse_args()

        COMMANDS[args.mode](args.subsystem, args.declaration)

        print >> sys.stdout, os.linesep, "Done"

    except Exception, error:
        print >> sys.stderr, repr(error)
        return 1

    return 0

if __name__ == '__main__':
    sys.exit(main())

