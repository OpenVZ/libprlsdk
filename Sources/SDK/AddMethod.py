#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim:ts=4:sw=4:expandtab
#
# GenSdk.py
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

import os, sys, string, re

# Full path of this python file module
__full_file_path__ = os.path.split( os.path.abspath(__file__) )[0]
if __full_file_path__ == '':
    __full_file_path__ = os.path.abspath( os.path.getcwd() )

# Importing BuildUtil Library
sys.path.append( os.path.join( __full_file_path__, '..' ) )
sys.path.append( os.path.join( __full_file_path__, '../Build' ) )

# Importing build utilites - allowing to call TRACE without prefix
import BuildUtil
from BuildUtil import TRACE

SDK_HEADER_PATH = os.path.join(__full_file_path__, 'Include')
SDK_HANDLES_PATH = os.path.join(__full_file_path__, 'Handles')
SDK_PYTHON_PATH = os.path.join(__full_file_path__, 'Python', 'sdk2python')
INTERFACES_PATH = os.path.join(__full_file_path__, '..', 'Interfaces')
LIBRARIES_PATH = os.path.join(__full_file_path__, '..', 'Libraries')
TESTS_PATH = os.path.join(__full_file_path__, '..', 'Tests', 'SDKTest')

def read_file(file_name, path=''):
    file_name = path and os.path.join(path, file_name) or file_name
    return open(file_name).read()

class AddMethod:
	def __init__(self, method_name, is_async):
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
		# short prefix exceptions: VmCfg -> Vm, DispCfg -> DispConfig, UsbIdent -> UsbIdentity
		if self.prefix_short == 'VmCfg':
			self.prefix_short = 'Vm'
		elif self.prefix_short == 'DispCfg':
			self.prefix_short = 'DispConfig'
		elif self.prefix_short == 'SrvCfg':
			self.prefix_short = 'SrvConfig'
		elif self.prefix_short == 'UsbIdent':
			self.prefix_short = 'UsbIdentity'
		elif self.prefix_short == 'Srv' and is_async == 0:
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

		# Show parsing results to the user
		print "Method Name: %s" % self.method_name
		print "Short Method Name: %s" % self.method_subname
		print "Method Prefix: %s" % self.prefix
		print "Short Method Prefix: %s" % self.prefix_short
		print "Capitalized Method Name: %s" % self.capped_method_name
		print "Capitalized Method Prefix: %s" % self.capped_prefix
		print "Method arguments: %s" % self.args
		print ""

	# SDK/Include/PrlEnums.h - add PJOC_SRV_METHOD_NAME to _PRL_JOB_OPERATION_CODE enum
	def add_pjoc_enum(self):
		f = os.path.join(SDK_HEADER_PATH, "PrlEnums.h")
		sys.stdout.write("%s..." % f)
		enums = read_file(f)

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
					print "\t\t\tOK"
					in_joc = 0
			elif line == 'typedef enum _PRL_JOB_OPERATION_CODE':
				in_joc = 1
			out_enums += line + '\n'
		open(f, 'wt').write(out_enums)

	# SDK/Include/PrlApiMacro.h - add PRL_SDK_WRAP_FOR_EACH_ITERATOR( PrlSrv_MethodName ) \
	def add_wrap(self):
		hdr = os.path.join(SDK_HEADER_PATH, "PrlApiMacro.h")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		new_wrap = 'PRL_SDK_WRAP_FOR_EACH_ITERATOR( ' + self.method_name + ' ) \\'
		in_wrap = 0
		out_data = ''
		for line in data.splitlines():
			if in_wrap == 1 and line == '':
				out_data += new_wrap + '\n'
				print "\t\t\tOK"
			if re.match("^\s*PRL_SDK_WRAP_FOR_EACH_ITERATOR.*\\\\", line):
				in_wrap = 1
			else:
				in_wrap = 0
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# SDK/Include/PrlApi.h - add PrlSrv_MethodName header
	def add_api_header(self, is_sync=0):
		if is_sync == 1:
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

		hdr = os.path.join(SDK_HEADER_PATH, "PrlApi.h")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		in_win = 0
		out_data = ''
		for line in data.splitlines():
			if in_win == 1:
				if re.match("\s*#pragma pack\(pop, save_api_pack\)", line):
					out_data += api_hdr
					print "\t\t\tOK"
				out_data += '#ifdef _WIN_\n'
			if line == '#ifdef _WIN_':
				in_win = 1
				continue
			else:
				in_win = 0
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# SDK/Handles/PrlApi.cpp - add PrlSrv_MethodName method
	def add_api_body(self, is_sync=0):
		if is_sync==1:
			arg0_name = self.args[0].split().pop()
			out_data="""PRL_METHOD( %s ) (
%s
		)
{
	SYNC_CHECK_API_INITIALIZED

	LOG_MESSAGE( DBG_DEBUG, "%%s (%s)",
		__FUNCTION__,
%s
	);

	if (PRL_WRONG_HANDLE(%s, PHT_%s))
		return PRL_ERR_INVALID_ARG;

	PrlHandle%sPtr p%s = PRL_OBJECT_BY_HANDLE<PrlHandle%s>( %s );

	return p%s->%s(%s);
}

""" % \
			(self.method_name, self.args_hdr, \
			self.args_msg_format, self.args_hdr_names, \
			arg0_name, \
			self.capped_prefix, self.prefix_short, self.prefix_short, \
			self.prefix_short, arg0_name, self.prefix_short, \
			self.method_subname, self.args0_hdr_names_1line)
		else:
			out_data = """void PrlContextSwitcher::%s(%s)
{
	ONE_HANDLE_SRV_METH_IMPLEMENTATION_WITH_FLAGS(%s, %s,
		PJOC_%s)
}

PRL_ASYNC_METHOD( %s ) (
%s
	)
{
	ASYNC_CHECK_API_INITIALIZED(PJOC_%s)
	CALL_THROUGH_CTXT_SWITCHER(PrlContextSwitcher::Instance(), %s, (%s))
	return (PrlContextSwitcher::Instance()->GetAsynchOpResult());
}

""" % \
			(self.method_name, self.args_hdr_1line, self.method_subname, \
			self.args0_hdr_names_1line, self.capped_method_name, \
			self.method_name, self.args_hdr, self.capped_method_name, \
			self.method_name, self.args_hdr_names_1line)
		f = os.path.join(SDK_HANDLES_PATH, "PrlApi.cpp")
		sys.stdout.write("%s..." % f)
		open(f, 'a').write(out_data)
		print "\t\t\tOK"

	# SDK/Handles/PveControl.h - add DspCmdMethodName as public method to CPveControl
	def add_pve_header(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "PveControl.h")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		in_public = 0
		pve_hdr="""\t/**
	 * FIXME
	 * @return id of performed asynchronous request
	 */
	QString DspCmd%s(%s);\n\n""" % (self.method_subname, self.args0_hdr_1line)

		out_data = ''
		for line in data.splitlines():
			if in_public == 1 and line == 'private:':
				out_data += pve_hdr
				print "\t\t\tOK"
				in_public = 0
			if line == 'public://Protocol methods':
				in_public = 1
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# SDK/Handles/PveControl.cpp - add DspCmdMethodName method
	def add_pve_body(self):
		out_data = """QString CPveControl::DspCmd%s(%s)
{
	LOG_MESSAGE(DBG_DEBUG, "CPveControl::DspCmd%s()");

	CHECK_CONNECTION_STATUS

	CProtoCommandPtr pRequest = CProtoSerializer::CreateProtoCommandWithoutParams(
									PVE::DspCmd%s,
									false,
									%s);

	return SendRequestToServer(pRequest);
}

""" % \
			(self.method_subname, self.args0_hdr_1line, self.method_subname, \
			self.method_subname, self.args0_hdr_names)
		f = os.path.join(SDK_HANDLES_PATH, "PveControl.cpp")
		sys.stdout.write("%s..." % f)
		open(f, 'a').write(out_data)
		print "\t\t\tOK"

	# SDK/Handles/PrlHandleServer.h - add MethodName as public method
	def add_srv_header(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "PrlHandleServer.h")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		in_public = 0
		srv_hdr="""\t/**
	 * FIXME
	 * @param command flags
	 */
	PrlHandleJobPtr %s(%s);\n\n""" % (self.method_subname, self.args0_hdr_1line)

		out_data = ''
		for line in data.splitlines():
			if in_public == 1 and line == 'private:':
				out_data += srv_hdr
				print "\t\t\tOK"
				in_public = 0
			if line == 'public://VM operations calls':
				in_public = 1
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# SDK/Handles/PrlHandleServer.cpp - add MethodName as public method
	def add_srv_body(self):
		out_data = """PrlHandleJobPtr PrlHandleServer::%s(%s)
{
    QString job_uuid = m_pPveControl->DspCmd%s(%s);

    return PrlHandleJobPtr((PrlHandleJob *)(new PrlHandleServerJob( PrlHandleServerPtr(this), job_uuid,
							PJOC_%s)));
}

""" % \
			(self.method_subname, self.args0_hdr_1line, self.method_subname, \
			self.args0_hdr_names_1line, self.capped_method_name)
		f = os.path.join(SDK_HANDLES_PATH, "PrlHandleServer.cpp")
		sys.stdout.write("%s..." % f)
		open(f, 'a').write(out_data)
		print "\t\t\tOK"

	# SDK/Handles/PrlHandle*.h - add MethodName as public method
	def add_handler_header(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "PrlHandle%s.h" % self.prefix_short)
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		in_public = 0
		call_hdr="""\t/**
	 * FIXME
	 * @param command flags
	 */
	PRL_RESULT %s(%s);\n\n""" % (self.method_subname, self.args0_hdr_1line)

		out_data = ''
		for line in data.splitlines():
			if in_public == 1 and line == 'private:':
				out_data += call_hdr
				print "\t\t\tOK"
				in_public = 0
			if line == 'public:':
				in_public = 1
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# SDK/Handles/PrlHandle*.cpp - add MethodName as public method
	def add_handler_body(self):
		out_data = """
PRL_RESULT PrlHandle%s::%s(%s)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	/* FIXME Return actual data from XmlModel */
	return PRL_ERR_UNIMPLEMENTED;
}
""" % (self.prefix_short, self.method_subname, self.args0_hdr_1line)
		f = os.path.join(SDK_HANDLES_PATH, "PrlHandle%s.cpp" % self.prefix_short)
		sys.stdout.write("%s..." % f)
		open(f, 'a').write(out_data)
		print "\t\t\tOK"

	# SDK/Handles/PrlCommon.cpp - add CONNECT_METH_IN_CTXT_SWITCHER PrlSrv_MethodName
	def add_connector(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "PrlCommon.cpp")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		new_line="""\tCONNECT_METH_IN_CTXT_SWITCHER(PrlContextSwitcher::Instance(), %s, \\
			(%s));\n""" % (self.method_name, self.args_types)

		in_connector = 0
		out_data = ''
		for line in data.splitlines():
			if in_connector == 1 and line == '}':
				out_data += new_line
				print "\t\t\tOK"
				in_connector = 0
			if re.match('\s*CONNECT_METH_IN_CTXT_SWITCHER.*', line):
				in_connector = 1
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# SDK/Handles/PrlContextSwitcher.h - add 3 lines for PrlSrv_MethodName
	def add_cont_sw(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "PrlContextSwitcher.h")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		new_line1="""FRIEND_ASYNC_METH_IN_CTXT_SWITCHER(%s, (%s));\n""" % \
			(self.method_name, self.args_types)
		new_line2="""\tvoid signal%s(%s);\n""" % \
			(self.method_name, self.args_types)
		new_line3="""\tvoid %s(%s);\n""" % \
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
		open(hdr, 'wt').write(out_data)
		if count == 3:
			print "\t\t\tOK"

	# Libraries/PrlCommonUtilsBase/CommandConvHelper.cpp - add PVE::DspCmdMethodName mapping
	def add_srv_job(self):
		hdr = os.path.join(LIBRARIES_PATH, "PrlCommonUtils", "CommandConvHelper.cpp")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		new_line="""\t(*DispatcherCommandsToJobTypesMap())[PVE::DspCmd%s] = PJOC_%s;\n""" % \
			(self.method_subname, self.capped_method_name)

		in_map = 0
		out_data = ''
		for line in data.splitlines():
			if in_map == 1 and line == '':
				out_data += new_line
				print "\t\t\tOK"
				in_map = 0
			if re.match ('\s*\(\*DispatcherCommandsToJobTypesMap', line):
				in_map = 1
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# Interfaces/ParallelsNamespace.h - add DspCmdMethodName definition
	def add_iface(self):
		f = os.path.join(INTERFACES_PATH, "ParallelsNamespace.h")
		sys.stdout.write("%s..." % f)
		data = read_file(f)

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
					print "\t\t\tOK"
					in_enum = 0
			elif re.match('\s*DspClientToDispatcherCommandRangeStart =', line):
				in_enum = 1
			out_data += line + '\n'
		open(f, 'wt').write(out_data)

	# Libraries/ProtoSerializer/CProtoSerializer.cpp - add PVE::DspCmdMethodName handling
	def add_proto_serializer(self):
		f = os.path.join(LIBRARIES_PATH, "ProtoSerializer", "CProtoSerializer.cpp")
		sys.stdout.write("%s..." % f)
		data = read_file(f)

		new_line = '\t\tcase PVE::DspCmd%s:\n' % self.method_subname
		out_data = ''
		for line in data.splitlines():
			if re.match("^\s*pCommand.*ProtoCommandWithoutParams", line):
				out_data += new_line
				print "\t\t\tOK"
			out_data += line + '\n'
		open(f, 'wt').write(out_data)

	# Libraries/Std/ParallelsNamespace.cpp - add CASE_DISPATCHER_COMMAND( PVE::DspCmdMethodName )
	def add_std(self):
		f = os.path.join(LIBRARIES_PATH, "Std", "ParallelsNamespace.cpp")
		sys.stdout.write("%s..." % f)
		data = read_file(f)

		new_line = '\t\tCASE_DISPATCHER_COMMAND( DspCmd%s )\n' % self.method_subname
		in_case = 0
		out_data = ''
		for line in data.splitlines():
			if in_case == 1 and line == '':
				out_data += new_line
				print "\t\t\tOK"
				in_case = 0
			if re.match ('\s*CASE_DISPATCHER_COMMAND', line):
				in_case = 1
			out_data += line + '\n'
		open(f, 'wt').write(out_data)

	# Tests/SDKTest/PrlSrvManipulationsTest.h add MethodName & MethodNameOnWrongParams testcases
	def add_test_header(self):
		f = os.path.join(TESTS_PATH, "%sManipulationsTest.h" % self.prefix)
		sys.stdout.write("%s..." % f)
		data = read_file(f)

		new_line = '\tvoid test%s();\n\tvoid test%sOnWrongParams();\n' % (self.method_subname, self.method_subname)
		in_slots = 0
		out_data = ''
		for line in data.splitlines():
			if in_slots == 1 and line == '':
				out_data += new_line
				print "\t\t\tOK"
				in_slots = 0
			if line == 'private slots:':
				in_slots = 1
			out_data += line + '\n'
		open(f, 'wt').write(out_data)

	# Tests/SDKTest/PrlSrvManipulationsTest.cpp add MethodName & MethodNameOnWrongParams testcases
	def add_test_body(self):
		out_data = """void %sManipulationsTest::test%s()
{
	/* FIXME Add real test here */
}

void %sManipulationsTest::test%sOnWrongParams()
{
	// Wrong handle
	CHECK_ASYNC_OP_FAILED(%s(PRL_INVALID_HANDLE, 0), PRL_ERR_INVALID_ARG)
	// CHECK_ASYNC_OP_FAILED(%s(_hWrongHandle, 0), PRL_ERR_INVALID_ARG)
}

""" % (self.prefix, self.method_subname, self.prefix, self.method_subname, self.method_name, self.method_name)
		f = os.path.join(TESTS_PATH, "%sManipulationsTest.cpp" % self.prefix)
		sys.stdout.write("%s..." % f)
		open(f, 'a').write(out_data)
		print "\t\t\tOK (Modify before commit!)"

	def gen_async_method(self):
		print "Generating Asynchronous Method code"
		self.add_pjoc_enum()
		self.add_wrap()
		self.add_api_header()
		self.add_api_body()
		self.add_pve_header()
		self.add_pve_body()
		if self.capped_prefix == 'SRV':
			self.add_srv_header()
			self.add_srv_body()
		self.add_connector()
		self.add_cont_sw()
		self.add_srv_job()
		self.add_iface()
		self.add_proto_serializer()
		self.add_std()
		self.add_test_header()
		self.add_test_body()
		print "Done."

	def gen_sync_method(self):
		print "Generating Synchronous Method code"
		self.add_wrap()
		self.add_api_header(1)
		self.add_api_body(1)
		self.add_handler_header()
		self.add_handler_body()
		print "Done."


class AddHandle:
	def __init__(self, handle_name):
		m = re.match("PHT_([A-Z_]+)", handle_name)
		if not m:
			raise RuntimeError("Handle name doesn't conform rules:" \
				"PHT_XXX[_XX,..]!")
		self.handle_name = handle_name
		self.short_handle_name = m.group(1)
		self.name_groups = m.group(1).split('_')
		for i in range(len(self.name_groups)):
			self.name_groups[i] = self.name_groups[i].strip()
		self.short_handle_class	= ""
		for str in self.name_groups:
			self.short_handle_class	+= str.capitalize()
		self.handle_class = "PrlHandle" + self.short_handle_class

		# Show parsing results to the user
		print "Handle Name: %s" % self.handle_name
		print "Short Handle Name: %s" % self.short_handle_name
		print "Handle Class Name: %s" % self.handle_class
		print "Handle Class Sub-Name: %s" % self.short_handle_class
		print ""

	# SDK/Python/sdk2python/PythonComposer.cpp
	def add_python(self):
		hdr = os.path.join(SDK_PYTHON_PATH, "PythonComposer.cpp")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		new_line="""{"%s",   "",             "_Handle",    "", "%s",  "",0,0,0,0},\n""" % \
			(self.short_handle_class, self.handle_name)

		in_map = 0
		out_data = ''
		for line in data.splitlines():
			if in_map == 1 and line == '};':
				out_data += new_line
				print "\t\t\tOK"
				in_map = 0
			if re.match ('.*\s+PythonComposer::s_CustomClassInfo\[\].*', line):
				in_map = 1
			out_data += line + '\n'
		open(hdr, 'wt').write(out_data)

	# SDK/Include/PrlEnums.h - add PHT_HANDLE_NAME to PRL_HANDLE_TYPE enum
	def add_handle_enum(self):
		f = os.path.join(SDK_HEADER_PATH, "PrlEnums.h")
		sys.stdout.write("%s..." % f)
		enums = read_file(f)

		new_line = '\t' + self.handle_name + '\t\t\t\t\t\t\t= '
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
			elif re.match ('\s*PHT_LAST =', line):
				line = '\tPHT_LAST = ' + self.handle_name + ' // should be set to last'
				num_changes = 2
			elif line == 'typedef enum _PRL_HANDLE_TYPE':
				in_enum = 1
			out_enums += line + '\n'
		open(f, 'wt').write(out_enums)
		if num_changes == 2:
			print "\t\t\tOK"
		else:
			print "\t\t\tFAIL"

	# SDK/Handles/PrlHandle*.h - define handle class
	def add_handler_header(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "%s.h" % self.handle_class)
		sys.stdout.write("%s..." % hdr)

		out_data = """
///////////////////////////////////////////////////////////////////////////////
///
/// @file %s.h
///
/// This file is the part of parallels public sdk library
///
/// Copyright (c) 2005-2011 Parallels Software International, Inc.
/// All rights reserved.
/// http://www.parallels.com
///
///////////////////////////////////////////////////////////////////////////////
#ifndef __PARALLELS_HANDLE_%s_H__
#define __PARALLELS_HANDLE_%s_H__


#include "PrlHandleBase.h"
#include "XmlModel/%s/%s.h"


/**
 * Handle object represents FIXME OBJECT NAME
 */
class %s: public PrlHandleBase
{
public:

	/**
	 * Class default constructor.
	 */
	%s();

	/**
	 * Class initialize constructor.
	 */
	%s(QString s%s);

	/**
	 * Initialize object from XML string
	 */
	PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	QString toString();

private:

	%s m_%s;
};


typedef PrlHandleSmartPtr<%s> %sPtr;


#endif	// __PARALLELS_HANDLE_%s_H__
""" %	(self.handle_class, self.short_handle_name, self.short_handle_name, \
	self.short_handle_class, self.short_handle_class, self.handle_class, \
	self.handle_class, self.handle_class, self.short_handle_class, \
	self.short_handle_class, self.short_handle_class, \
	self.handle_class, self.handle_class, self.short_handle_name)
		open(hdr, 'wt').write(out_data)
		print "\t\t\tOK"

	# SDK/Handles/PrlHandle*.cpp - add handler class
	def add_handler_body(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "%s.cpp" % self.handle_class)
		sys.stdout.write("%s..." % hdr)

		out_data = """
///////////////////////////////////////////////////////////////////////////////
///
/// @file %s.cpp
///
/// This file is the part of parallels public sdk library
///
/// Copyright (c) 2005-2011 Parallels Software International, Inc.
/// All rights reserved.
/// http://www.parallels.com
///
///////////////////////////////////////////////////////////////////////////////

#include "%s.h"

%s::%s()
: PrlHandleBase(%s)
{
}

%s::%s(QString s%s)
: PrlHandleBase(%s)
{
	m_%s.fromString(s%s);
}

PRL_RESULT %s::fromString(PRL_CONST_STR sXml)
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        if (PRL_SUCCEEDED(m_%s.fromString(QString::fromUtf8(sXml))))
                return PRL_ERR_SUCCESS;

        return PRL_ERR_INVALID_ARG;
}

QString %s::toString()
{
        SYNCHRO_INTERNAL_DATA_ACCESS
        return m_%s.toString();
}
""" %	(self.handle_class, self.handle_class, \
	self.handle_class, self.handle_class, self.handle_name, \
	self.handle_class, self.handle_class, self.short_handle_class, self.handle_name, \
	self.short_handle_class, self.short_handle_class, \
	self.handle_class, self.short_handle_class, \
	self.handle_class, self.short_handle_class)
		open(hdr, 'wt').write(out_data)
		print "\t\t\tOK"

	# SDK/Handles/SDK.pro
	def add_sdk_pro(self):
		hdr = os.path.join(SDK_HANDLES_PATH, "SDK.pro")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		new_line_hdr="""\t%s.h\n""" % (self.handle_class)
		new_line_src="""\t%s.cpp\n""" % (self.handle_class)

		in_hdrs = 0
		in_srcs = 0
		num_changes = 0
		out_data = ''
		for line in data.splitlines():
			if in_hdrs == 1 and line == '':
				out_data += new_line_hdr
				num_changes = num_changes + 1
				in_hdrs = 0
			elif in_hdrs == 1 and re.match('\s*PrlHandle\w+\.h$', line):
				line += ' \\'
			elif line == 'HEADERS += \\':
				in_hdrs = 1
			elif in_srcs == 1 and line == '':
				out_data += new_line_src
				num_changes = num_changes + 1
				in_srcs = 0
			elif in_srcs == 1 and re.match("\s*PrlHandle\w+\.cpp$", line):
				line += ' \\'
			elif line == 'SOURCES += \\':
				in_srcs = 1
			out_data += line + '\n'
		if in_srcs == 1:
			out_data += new_line_src
			num_changes = num_changes + 1
		open(hdr, 'wt').write(out_data)
		if num_changes == 2:
			print "\t\t\tOK"
		else:
			print "\t\t\tFAIL"

	# Tests/SDKTest/SDKPrivateTest/SDKPrivateTest.pro
	def add_tests_pro(self):
		hdr = os.path.join(TESTS_PATH, "SDKPrivateTest", "SDKPrivateTest.pro")
		sys.stdout.write("%s..." % hdr)
		data = read_file(hdr)

		new_line_hdr="""\t\t\t$$SRC_LEVEL/SDK/Handles/%s.h\n""" % (self.handle_class)
		new_line_src="""\t\t\t$$SRC_LEVEL/SDK/Handles/%s.cpp\n""" % (self.handle_class)

		in_hdrs = 0
		in_srcs = 0
		num_changes = 0
		out_data = ''
		for line in data.splitlines():
			if in_hdrs == 1 and line == '':
				out_data += new_line_hdr
				num_changes = num_changes + 1
				in_hdrs = 0
			elif in_hdrs == 1 and re.match('\s*.*PrlHandle\w+\.h$', line):
				line += ' \\'
			elif re.match('HEADERS \+=.*\\\\', line):
				in_hdrs = 1
			elif in_srcs == 1 and line == '':
				out_data += new_line_src
				num_changes = num_changes + 1
				in_srcs = 0
			elif in_srcs == 1 and re.match("\s*.*PrlHandle\w+\.cpp$", line):
				line += ' \\'
			elif re.match('SOURCES \+=.*\\\\', line):
				in_srcs = 1
			out_data += line + '\n'
		if in_srcs == 1:
			out_data += new_line_src
			num_changes = num_changes + 1
		open(hdr, 'wt').write(out_data)
		if num_changes == 2:
			print "\t\t\tOK"
		else:
			print "\t\t\tFAIL"


	def gen_handle(self):
		print "Generating Handle code"
		self.add_python()
		self.add_handle_enum()
		self.add_handler_header()
		self.add_handler_body()
		self.add_sdk_pro()
		self.add_tests_pro()
		print "Done."
		print "Do not forget to add generated Handle's definition files to SVN!"

def main():
	try:
		argc = len(sys.argv) - 1
		count = 0
		if sys.argv.count('async'):
			mode = 'async'
			count = count + 1
		elif sys.argv.count('sync'):
			mode = 'sync'
			count = count + 1
		elif sys.argv.count('handle'):
			mode = 'handle'
			count = count + 1
		else:
			mode = BuildUtil.raw_choice(
				'Is it sync or async method or handle [sync, async, handle]?',
				[ 'sync', 'async', 'handle' ])

		if count == argc and mode != 'handle':
			method_name = BuildUtil.user_input(
				'Please specify method declaration (\'PrlSrv_GetCtList(PRL_HANDLE hServer)\', for example): ' )
		elif count == argc:
			method_name = BuildUtil.user_input(
				'Please specify handle declaration (\'PHT_CT_TEMPLATE\', for example): ' )
		else:
			method_name = sys.argv[argc]

		if mode == 'async':
			am = AddMethod(method_name, 1)
			am.gen_async_method()
		elif mode == 'sync':
			am = AddMethod(method_name, 0)
			am.gen_sync_method()
		else:
			ah = AddHandle(method_name)
			ah.gen_handle()
	except Exception, e:
		print e
		return 1
	return 0

if ( __name__ == '__main__' ):
    sys.exit( main() )
