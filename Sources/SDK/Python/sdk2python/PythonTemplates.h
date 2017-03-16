/*
 * PythonTemplates.h: Definitions of the text templates for module
 * composer
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef PYTHON_TEMPLATES_H
#define PYTHON_TEMPLATES_H

#define PTN_PRL_PYTHON_SDK_PY	"prlsdkapi.py"

#define PTN_CAPTION \
	"#///////////////////////////////////////////////////////////////////////////////\n" \
	"#//\n" \
	"#// "PTN_PRL_PYTHON_SDK_PY"\n" \
	"#//\n" \
	"#// ***DO NOT EDIT THIS FILE***\n" \
	"#// ***IT IS GENERATED AUTOMATICALLY BY sdk2python UTILITY***\n" \
	"#//\n" \
	"#// Copyright (c) 1999-2017, Parallels International GmbH\n" \
	"#// All rights reserved.\n" \
	"#//\n" \
	"#///////////////////////////////////////////////////////////////////////////////\n\n"

#define PTN_IMPORT_MODULES \
	"import os, sys, imp, string, time\n\n" \
	"prlsdk = None\n" \
	"try:\n" \
	"\timport prlsdk\n" \
	"except Exception, e:\n" \
	"\ttry:\n" \
	"\t\tprlsdk_module = os.environ['PRLSDK']\n" \
	"\t\tsdklib = os.environ['SDKLIB']\n" \
	"\t\tprlsdk = imp.load_dynamic('prlsdk', prlsdk_module)\n" \
	"\t\tprlsdk.SetSDKLibraryPath(sdklib)\n" \
	"\texcept:\n" \
	"\t\tprint e\n" \
	"\t\terr = 'Cannot import module \"prlsdk\" !'\n" \
	"\t\traise ImportError, err\n\n"

#define PTN_SDK_MODULE \
	"SDK = prlsdk\n" \
	"consts = prlsdk.consts\n" \
	"errors = prlsdk.errors\n\n"

#define PTN_PRL_SDK_ERROR \
	"class PrlSDKError(Exception):\n\n" \
	"\tdef __init__(self, result, error_code, err_obj):\n" \
	"\t\tself.result = result\n" \
	"\t\tself.error_code = error_code\n" \
	"\t\tself.err_obj = err_obj\n" \
	"\t\tException.__init__(self, 'SDK error: 0x%.8X: %s' % (error_code, self.get_details()))\n\n" \
	"\tdef get_result(self):\n" \
	"\t\treturn self.result\n\n" \
	"\tdef get_details(self):\n" \
	"\t\tif self.err_obj != None:\n" \
	"\t\t\tres_s = errors.PRL_ERR_SUCCESS\n" \
	"\t\t\tres_l = errors.PRL_ERR_SUCCESS\n" \
	"\t\t\tdetails_s = self.err_obj.get_err_string(1, 0)\n" \
	"\t\t\tdetails_l = self.err_obj.get_err_string(0, 0)\n" \
	"\t\telse:\n" \
	"\t\t\tres_s, details_s, = SDK.PrlApi_GetResultDescription(self.error_code, 1, 0)\n" \
	"\t\t\tres_l, details_l, = SDK.PrlApi_GetResultDescription(self.error_code, 0, 0)\n" \
	"\t\tdetails = details_s + ' ' + details_l\n" \
	"\t\tif res_s == errors.PRL_ERR_SUCCESS and res_l == errors.PRL_ERR_SUCCESS:\n" \
	"\t\t\treturn details\n" \
	"\t\treturn 'No error description!'\n\n" \
	"prlsdk.PrlSDKError = PrlSDKError\n\n"

#define PTN_SDK_CHECK_RESULT \
	"def conv_error(err):\n" \
	"\tif err < 0:\n" \
	"\t\terr = 0x100000000 + err\n" \
	"\treturn err\n" \
	"def sdk_check_result(result, err_obj = None):\n" \
	"\tif result != errors.PRL_ERR_SUCCESS:\n" \
	"\t\traise PrlSDKError, (result, conv_error(result), err_obj)\n\n"

#define PTN_DEINIT_SDK \
	"class __DeinitSDK:\n" \
	"\tdef __init__(self):\n" \
	"\t\tself.SDK = SDK\n" \
	"\t\tself.id = 0\n" \
	"\tdef __call__(self):\n" \
	"\t\tif self.SDK != None and self.SDK.IsSDKInitialized()[1]:\n" \
	"\t\t\tres = self.SDK.DeinitializeSDK()[0]\n" \
	"\t\t\tif res:\n" \
	"\t\t\t\traise Exception, ('SDK deinitialization failed! Error = 0x%.8X' % conv_error(res))\n" \
	"\t\t\tself.id += 1\n" \
	"\tdef __del__(self):\n" \
	"\t\tself.__call__()\n\n" \
	"deinit_sdk = __DeinitSDK()\n\n"

#define PTN_CONV_HANDLE_ARG \
	"def conv_handle_arg(arg):\n" \
	"\ttry:\n" \
	"\t\thandle = arg.handle\n" \
	"\t\treturn handle\n" \
	"\texcept AttributeError:\n" \
	"\t\treturn arg\n\n"

#define PTN_PASS \
	"\tpass\n\n"

#define PTN_CALL_SDK_FUNCTION \
	"def call_sdk_function(func_name, *args):\n" \
	"\tresult_list = getattr(SDK, func_name)(*args)\n" \
	"\tif result_list[0] != conv_error(errors.PRL_ERR_UNIMPLEMENTED):\n" \
	"\t\tsdk_check_result(result_list[0])\n" \
	"\t\tif len(result_list) == 2:\n" \
	"\t\t\treturn result_list[1]\n" \
	"\t\tif len(result_list) > 2:\n" \
	"\t\t\treturn result_list[1:]\n\n"

#define PTN_SET_SDK_LIBRARY_PATH \
	"def set_sdk_library_path(path):\n" \
	"\tcall_sdk_function('SetSDKLibraryPath', path)\n\n"
#define PTN_GET_SDK_LIBRARY_PATH \
	"def get_sdk_library_path():\n" \
	"\treturn call_sdk_function('GetSDKLibraryPath')\n\n"
#define PTN_IS_SDK_INITIALIZED \
	"def is_sdk_initialized():\n" \
	"\treturn bool(call_sdk_function('IsSDKInitialized'))\n\n"
#define PTN_INIT_DESKTOP_SDK \
	"def init_desktop_sdk():\n" \
	"\tcall_sdk_function('InitializeSDK', consts.PAM_DESKTOP_MAC)\n\n"
#define PTN_INIT_DESKTOP_WL_SDK \
	"def init_desktop_wl_sdk():\n" \
	"\tcall_sdk_function('InitializeSDK', consts.PAM_DESKTOP_WL)\n\n"
#define PTN_INIT_WORKSTATION_SDK \
	"def init_workstation_sdk():\n" \
	"\tcall_sdk_function('InitializeSDK', consts.PAM_WORKSTATION_EXTREME)\n\n"
#define PTN_INIT_PLAYER_SDK \
	"def init_player_sdk():\n" \
	"\tcall_sdk_function('InitializeSDK', consts.PAM_PLAYER)\n\n"
#define PTN_INIT_SERVER_SDK \
	"def init_server_sdk():\n" \
	"\tcall_sdk_function('InitializeSDK', consts.PAM_SERVER)\n\n"

#define PTN_CLASS \
	"class %1%2:\n"

#define PTN_PRIVATE_CLASS \
	"\t\tif self.__class__ is %1:\n" \
	"\t\t\traise TypeError, 'class %2 is a private class'\n\n"
#define PTN_PRIVATE_CLASS_INIT \
	"\tdef __init__(self):\n" \
	PTN_PRIVATE_CLASS

#define PTN_SELF_HANDLE \
	"self.handle"
#define PTN_TOP_CLASS_INIT \
	"\tdef __init__(self, handle = consts.PRL_INVALID_HANDLE):\n" \
	"\t\t"PTN_SELF_HANDLE" = handle\n" \
	"\t\tself.id = deinit_sdk.id\n" \
	PTN_PRIVATE_CLASS
#define PTN_TOP_CLASS_DEL \
	"\tdef __del__(self):\n" \
	"\t\tif self.handle != consts.PRL_INVALID_HANDLE and self.id == deinit_sdk.id:\n" \
	"\t\t\tself.free()\n\n"
#define PTN_TOP_GET_HANDLE_TYPE \
	"\tdef get_handle_type(self):\n" \
	"\t\treturn call_sdk_function('PrlHandle_GetType', self.handle)\n\n"

#define PTN_CLASS_INIT \
	"\tdef __init__(self, handle = consts.PRL_INVALID_HANDLE):\n" \
	"\t\tself.handle = handle\n" \
	"\t\tif self.handle == consts.PRL_INVALID_HANDLE:\n" \
	"\t\t\tself.handle = %1\n" \
	"\t\t_Handle.__init__(self, self.handle)\n\n"
#define PTN_ARG_TO_HANDLE \
	"conv_handle_arg(%1)"
#define PTN_CLASS_METHOD \
	"\tdef %1(self%2):\n%3"
#define PTN_ASYNC_CLASS_METHOD \
	PTN_CLASS_METHOD \
	"\t\treturn Job(SDK.%4(%5)[0])\n\n"
#define PTN_GET_CLASS_METHOD \
	PTN_CLASS_METHOD \
	"\t\treturn call_sdk_function('%4'%5)\n\n"
#define PTN_GET_OBJECT_CLASS_METHOD \
	PTN_CLASS_METHOD \
	"\t\treturn handle_to_object(call_sdk_function('%4'%5))\n\n"
#define PTN_GET_BOOLEAN_CLASS_METHOD \
	PTN_CLASS_METHOD \
	"\t\treturn bool(call_sdk_function('%4'%5))\n\n"
#define PTN_SET_CLASS_METHOD \
	PTN_CLASS_METHOD \
	"\t\tcall_sdk_function('%4'%5)\n\n"

#define PTN_HANDLE_TO_OBJECT \
	"def handle_to_object(handle):\n"
#define PTN_HANDLE_TO_OBJECT_INIT \
	"\tobj = None\n" \
	"\tif handle == None:\n" \
	"\t\treturn obj\n" \
	"\thandle_type = call_sdk_function('PrlHandle_GetType', handle)\n"
#define PTN_HANDLE_TO_OBJECT_CHECK_TYPE \
	"handle_type == consts.PHT_"
#define PTN_HANDLE_TO_OBJECT_BY_TYPE \
	"\tif %1:\n" \
	"\t\tobj = %2(handle)\n"
#define PTN_END_HANDLE_TO_OBJECT \
	"\tif obj == None:\n" \
	"\t\tsdk_check_result(errors.PRL_ERR_INVALID_HANDLE)\n" \
	"\treturn obj\n\n"

#define PTN_JOB_WAIT_METHOD \
	PTN_CLASS_METHOD \
	"\t\tcall_sdk_function('%4'%5)\n" \
	"\t\terr_obj = None\n" \
	"\t\ttry:\n" \
	"\t\t\terr_obj = self.get_error()\n" \
	"\t\texcept:\n" \
	"\t\t\tpass\n" \
	"\t\tsdk_check_result(self.get_ret_code(), err_obj)\n" \
	"\t\treturn self.get_result()\n\n"

#define PTN_ITER_METHODS \
	"\tdef __len__(self):\n" \
	"\t\treturn self.%1()\n\n" \
	"\tdef __getitem__(self, index):\n" \
	"\t\tif index < 0 or index >= self.__len__():\n" \
	"\t\t\traise IndexError\n" \
	"\t\treturn self.%2(index)\n\n" \
	"\tdef __iter__(self):\n" \
	"\t\tfor i in xrange(self.__len__()):\n" \
	"\t\t\tyield self.__getitem__(i)\n\n"

#define PTN_PRL_IO_DISPLAY_SCREEN_SIZE \
	"class IoDisplayScreenSize:\n\n" \
	"\tdef __init__(self):\n" \
	"\t\tself.width = 0\n" \
	"\t\tself.height = 0\n" \
	"\t\tself.depth = 0\n" \
	"\t\tself.lineBytes = 0\n" \
	"\t\tself.display = 0\n" \
	"\t\tself.displayX = 0\n" \
	"\t\tself.displayY = 0\n" \
	"\n" \
	"\tdef to_list(self):\n" \
	"\t\treturn [self.width, self.height, self.depth, self.lineBytes, self.display, self.displayX, self.displayY]\n\n"

#define PTN_VM_IO_SEND_KEY_EVENT \
	"\tdef send_key_event(self, hVm, key_list, ev = consts.PKE_CLICK, delay = 250):\n" \
	"\t\tif ev == consts.PKE_CLICK:\n" \
	"\t\t\tself.send_key_event(hVm, key_list, consts.PKE_PRESS, delay)\n" \
	"\t\t\tself.send_key_event(hVm, key_list, consts.PKE_RELEASE, delay)\n" \
	"\t\t\treturn\n" \
	"\t\tfor key in key_list:\n" \
	"\t\t\t_VmIOKeyboard.send_key_event(self, hVm, key, ev)\n" \
	"\t\tif delay > 0:\n" \
	"\t\t\ttime.sleep(float(delay)/1000.0)\n\n"
#define PTN_VM_IO_SEND_KEY_EVENT_EX \
	"\tdef send_key_event_ex(self, hVm, key, ev = consts.PKE_CLICK, delay = 250):\n" \
	"\t\tif ev == consts.PKE_CLICK:\n" \
	"\t\t\tself.send_key_event_ex(hVm, key, consts.PKE_PRESS, delay)\n" \
	"\t\t\tself.send_key_event_ex(hVm, key, consts.PKE_RELEASE, delay)\n" \
	"\t\t\treturn\n" \
	"\t\t_VmIOKeyboard.send_key_event_ex(self, hVm, key, ev)\n" \
	"\t\tif delay > 0:\n" \
	"\t\t\ttime.sleep(float(delay)/1000.0)\n\n"
#define PTN_VM_IO_DISPLAY_SET_CONFIGURATION \
	"\tdef display_set_configuration(self, hVm, config_list):\n" \
	"\t\tlist_args = []\n" \
	"\t\tfor cfg in config_list:\n" \
	"\t\t\tlist_args.append(cfg.to_list())\n" \
	"\t\tcall_sdk_function('PrlDevDisplay_SetConfiguration', conv_handle_arg(hVm), list_args)\n\n"

#define PTN_OP_TYPE_LIST_SIGNED \
	"\tdef signed(self, nItemIndex):\n" \
	"\t\treturn self.__getitem__(nItemIndex)[0]\n\n"
#define PTN_OP_TYPE_LIST_UNSIGNED \
	"\tdef unsigned(self, nItemIndex):\n" \
	"\t\treturn self.__getitem__(nItemIndex)[1]\n\n"
#define PTN_OP_TYPE_LIST_DOUBLE \
	"\tdef double(self, nItemIndex):\n" \
	"\t\treturn self.__getitem__(nItemIndex)[2]\n\n"

#define PTN_GET_VM_CONFIG_PARAMS \
	"nVmType, sConfigSample, nOsVersion"

#define PTN_MEMGUARANTEE_SIZE_PARAMS \
	"nType, nValue"

#define PTN_EVENT_HANDLER_PARAMS \
	"callback_func"

#define PTN_IOLIMIT_DATA_PARAMS \
	"nValue"

#endif	// PYTHON_TEMPLATES_H
