/*
 * ModuleTemplates.h: Definitions of the text templates for module
 * composer.
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
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


#ifndef MODULE_TEMPLATES_H
#define MODULE_TEMPLATES_H

#define MDL_PRL_PYTHON_SDK				"PrlPythonSDK"
#define MDL_PRL_PYTHON_SDK_CPP			"PrlPythonSDK.cpp"

#define MDL_CAPTION \
	"////////////////////////////////////////////////////////////////////////////////\n" \
	"///\n" \
	"/// "MDL_PRL_PYTHON_SDK_CPP"\n" \
	"///\n" \
	"/// ***DO NOT EDIT THIS FILE***\n" \
	"/// ***IT IS GENERATED AUTOMATICALLY BY sdk2python UTILITY***\n" \
	"///\n" \
	"/// Copyright (c) 2005-2009 Parallels Software International, Inc.\n" \
	"/// All rights reserved.\n" \
	"/// http://www.parallels.com\n" \
	"///\n" \
	"////////////////////////////////////////////////////////////////////////////////\n\n"

#define MDL_INCLUDES \
	"#include <string>\n" \
	"#ifdef _LIN_\n" \
	"#ifdef _POSIX_C_SOURCE\n" \
	"#undef _POSIX_C_SOURCE\n" \
	"#endif\n" \
	"#ifdef _XOPEN_SOURCE\n" \
	"#undef _XOPEN_SOURCE\n" \
	"#endif\n" \
	"#endif\n" \
	"#include \"SDK/Python/PrlPython.h\"\n" \
	"#include \"SDK/Wrappers/SdkWrap/SdkWrap.h\"\n" \
	"#include \"Interfaces/Debug.h\"\n\n"

#define MDL_PRL_SDK_CHECK \
	"#define PRL_SDK_CHECK \\\n" \
	"\tif ( ! s_bIsSDKInitialized ) \\\n" \
	"\t{ \\\n" \
	"\t\tPyErr_SetString(PyExc_RuntimeError, \"SDK was not initialized\"); \\\n" \
	"\t\treturn NULL; \\\n" \
	"\t} \n\n"


#define MDL_SDK_PYTHON_FUNCTION \
	"static PyObject* sdk_%1(PyObject* /*self*/, PyObject* %2)\n" \
	"{\n" \
	"\tPRL_SDK_CHECK;\n" \
	"\tdo {\n"
#define MDL_PY_ARGS \
	"args"
#define MDL_DEF_VAR \
	"\t\t%1\t%2 = (%3 )0;\n"
#define MDL_IN_VALUE \
	", &%1"
#define MDL_PY_ARG_PARSE_TUPLE \
	"\t\tif ( ! PyArg_ParseTuple( args, \"%1\" %2 ) )\n" \
	"\t\t\tbreak;\n\n"
#define MDL_STR_DEF_VAR \
	"\t\tstd::string std_%1(%2, '\\0');\n" \
	"\t\t%3 = (PRL_STR )std_%4.data();\n"
#define MDL_DECLARE_SDK_RESULT \
	"\t\t%1%2;\n"
#define MDL_CALL_SDK_FUNCTION \
	"\t\tPy_BEGIN_ALLOW_THREADS\n" \
	"\t\t%1 = %2( %3 );\n" \
	"\t\tPy_END_ALLOW_THREADS\n\n"
#define MDL_NEW_RETURN_LIST \
	"\t\tPyObject* ret_list = PyList_New(0);\n" \
	"\t\tif ( ! ret_list )\n" \
	"\t\t\tbreak;\n"
#define MDL_PY_LIST_APPEND_PY_BUILD_VALUE \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"%1\", %2 )) )\n" \
	"\t\t\tbreak;\n"
#define MDL_END_SDK_PYTHON_FUNCTION \
	"\t\treturn ret_list;\n" \
	"\t} while(0);\n" \
	"\treturn NULL;\n" \
	"}\n\n"

#define MDL_SDK_PYTHON_BUNDLE \
	"static PyMethodDef SdkPythonBundle[] = \n" \
	"{\n"
#define MDL_BIND_SDK_FUNCTION \
	"\t{\"%1\", sdk_%2, %3, \"%4\"},\n"
#define MDL_METH_VARARGS \
	"METH_VARARGS"
#define MDL_METH_NOARGS \
	"METH_NOARGS"
#define MDL_END_SDK_PYTHON_BUNDLE \
	"\t{ 0, 0, 0, 0 }\n" \
	"};\n\n"

#define MDL_ENTRY_FUNCTION \
	"#ifndef PYTHON_MODULE_NAME\n" \
	"\t#error \"Unknown python module name!\"\n" \
	"#endif\n\n" \
	"#define TO_STR2(t) #t\n" \
	"#define TO_STR(t) TO_STR2(t)\n" \
	"#define ENTRY_MODULE_NAME(x) TO_STR(x)\n\n" \
	"extern \"C\" void entry(void )\n" \
	"{\n"
#define MDL_INIT_MODULE \
	"\tPyObject* parent_module = Py_InitModule( ENTRY_MODULE_NAME(PYTHON_MODULE_NAME), SdkPythonBundle );\n" \
	"\tstd::string parent_name = PyModule_GetName(parent_module);\n" \
	"\tPyObject* module = NULL;\n" \
	"\tstd::string name;\n\n"
#define MDL_ADD_MODULE \
	"\tmodule = Py_InitModule( (char* )(parent_name + \".\" + name).c_str(), NULL );\n" \
	"\tPy_INCREF(module);\n" \
	"\tPyModule_AddObject( parent_module, (char* )name.c_str(), module );\n\n"
#define MDL_INIT_CONSTS_MODULE \
	"\tname = \"consts\";\n" \
	MDL_ADD_MODULE
#define MDL_INIT_ERRORS_MODULE \
	"\tname = \"errors\";\n" \
	MDL_ADD_MODULE
#define MDL_REGISTER_CONSTANT \
	"\tPyModule_AddIntConstant( module, \"%1\", %2 );\n"
#define MDL_INIT_SCAN_CODES \
	"\tPyObject* scan_codes = Py_BuildValue(\"{}\");\n" \
	"\tPyModule_AddObject( module, \"ScanCodesList\", scan_codes );\n\n"
#define MDL_ADD_SCAN_CODE \
	"\tPyDict_SetItemString( scan_codes, \"%1\", Py_BuildValue( \"%2\", %3 ) );\n"
#define MDL_END_ENTRY_FUNCTION \
	"}\n"

// Extention

#define MDL_PKE_CLICK \
	"PKE_CLICK"

#define MDL_SDK_LIBRARY_PATH \
	"static std::string s_strSdkLibraryPath;\n" \
	"static bool s_bIsSDKInitialized = false;\n\n"

#define MDL_SET_SDK_LIBRARY_PATH_NAME \
	"SetSDKLibraryPath"
#define MDL_SET_SDK_LIBRARY_PATH \
	"static PyObject* sdk_"MDL_SET_SDK_LIBRARY_PATH_NAME"(PyObject* /*self*/, PyObject* args)\n" \
	"{\n" \
	"\tdo {\n" \
	"\t\tPRL_CONST_STR sSdkLibraryPath;\n\n" \
	"\t\tif ( ! PyArg_ParseTuple( args, \"s\", &sSdkLibraryPath ) )\n" \
	"\t\t\tbreak;\n\n" \
	"\t\ts_strSdkLibraryPath = sSdkLibraryPath;\n\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", PRL_ERR_SUCCESS )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_GET_SDK_LIBRARY_PATH_NAME \
	"GetSDKLibraryPath"
#define MDL_GET_SDK_LIBRARY_PATH \
	"static PyObject* sdk_"MDL_GET_SDK_LIBRARY_PATH_NAME"(PyObject* /*self*/, PyObject* )\n" \
	"{\n" \
	"\tdo {\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", PRL_ERR_SUCCESS )) )\n" \
	"\t\t\tbreak;\n" \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"s\", s_strSdkLibraryPath.c_str() )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_IS_SDK_INITIALIZED_NAME \
	"IsSDKInitialized"
#define MDL_IS_SDK_INITIALIZED \
	"static PyObject* sdk_"MDL_IS_SDK_INITIALIZED_NAME"(PyObject* /*self*/, PyObject* )\n" \
	"{\n" \
	"\tdo {\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", PRL_ERR_SUCCESS )) )\n" \
	"\t\t\tbreak;\n" \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", s_bIsSDKInitialized )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

// Obsolete - needs only for compatibility
#define MDL_INITIALIZE_SDK_NAME \
	"InitializeSDK"
#define MDL_INITIALIZE_SDK \
	"static PyObject* sdk_"MDL_INITIALIZE_SDK_NAME"(PyObject* /*self*/, PyObject* args)\n" \
	"{\n" \
	"\tdo {\n" \
	"\t\tPRL_APPLICATION_MODE nMode;\n\n" \
	"\t\tif ( ! PyArg_ParseTuple( args, \"I\", &nMode ) )\n" \
	"\t\t\tbreak;\n\n" \
	"\t\tPRL_RESULT prlResult = PRL_ERR_SUCCESS;\n" \
	"\t\tif (s_strSdkLibraryPath.empty())\n" \
	"\t\t\tprlResult = SdkWrap_LoadLibFromStdPaths() ? PRL_ERR_SUCCESS : PRL_ERR_FAILURE;\n" \
	"\t\telse\n" \
	"\t\t\tprlResult = SdkWrap_Load(s_strSdkLibraryPath.c_str());\n\n" \
	"\t\tprlResult = PRL_SUCCEEDED(prlResult) ? PrlApi_InitEx(PARALLELS_API_VER, nMode, 0, 0) : prlResult;\n" \
	"\t\ts_bIsSDKInitialized = PRL_SUCCEEDED(prlResult);\n\n" \
	"\t\tif (PRL_FAILED(prlResult))\n" \
	"\t\t{\n" \
	"\t\t\tSdkWrap_Unload();\n" \
	"\t\t}\n" \
	"\t\telse\n" \
	"\t\t{\n" \
	"\t\t\tPrlApi_SwitchConsoleLogging(PRL_FALSE);\n" \
	"\t\t\tPrlApi_InitCrashHandler(0);\n" \
	"\t\t}\n\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", prlResult )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_INITIALIZE_SDK_NAME_EX \
	"InitializeSDKEx"
#define MDL_INITIALIZE_SDK_EX \
	"static PyObject* sdk_"MDL_INITIALIZE_SDK_NAME_EX"(PyObject* /*self*/, PyObject* args)\n" \
	"{\n" \
	"\tdo {\n" \
	"\t\tPRL_UINT32 nVersion;\n" \
	"\t\tPRL_APPLICATION_MODE nMode = PAM_SERVER;\n" \
	"\t\tPRL_UINT32 nFlags = 0;\n" \
	"\t\tPRL_UINT32 nReserved = 0;\n\n" \
	"\t\tif ( ! PyArg_ParseTuple( args, \"I|III\", &nVersion, &nMode, &nFlags, &nReserved ) )\n" \
	"\t\t\tbreak;\n\n" \
	"\t\tPRL_RESULT prlResult = PRL_ERR_SUCCESS;\n" \
	"\t\tif (s_strSdkLibraryPath.empty())\n" \
	"\t\t\tprlResult = SdkWrap_LoadLibFromStdPaths() ? PRL_ERR_SUCCESS : PRL_ERR_FAILURE;\n" \
	"\t\telse\n" \
	"\t\t\tprlResult = SdkWrap_Load(s_strSdkLibraryPath.c_str());\n\n" \
	"\t\tprlResult = PRL_SUCCEEDED(prlResult) ? PrlApi_InitEx(nVersion, nMode, nFlags, nReserved) : prlResult;\n" \
	"\t\ts_bIsSDKInitialized = PRL_SUCCEEDED(prlResult);\n\n" \
	"\t\tif (PRL_FAILED(prlResult))\n" \
	"\t\t{\n" \
	"\t\t\tSdkWrap_Unload();\n" \
	"\t\t}\n" \
	"\t\telse\n" \
	"\t\t{\n" \
	"\t\t\tPrlApi_SwitchConsoleLogging(PRL_FALSE);\n" \
	"\t\t\tPrlApi_InitCrashHandler(0);\n" \
	"\t\t}\n\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", prlResult )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_PRL_API_INIT \
	"PrlApi_Init"
#define MDL_PRL_API_INIT_IMPL \
	"static PyObject* sdk_"MDL_PRL_API_INIT"(PyObject* self, PyObject* args)\n" \
	"{\n" \
	"\treturn sdk_"MDL_INITIALIZE_SDK_NAME_EX"(self, args);\n" \
	"}\n\n"
#define MDL_PRL_API_INIT_EX \
	"PrlApi_InitEx"
#define MDL_PRL_API_INIT_EX_IMPL \
	"static PyObject* sdk_"MDL_PRL_API_INIT_EX"(PyObject* self, PyObject* args)\n" \
	"{\n" \
	"\treturn sdk_"MDL_INITIALIZE_SDK_NAME_EX"(self, args);\n" \
	"}\n\n"

#define MDL_DEINITIALIZE_SDK_NAME \
	"DeinitializeSDK"
#define MDL_DEINITIALIZE_SDK \
	"static PyObject* sdk_"MDL_DEINITIALIZE_SDK_NAME"(PyObject* /*self*/, PyObject* )\n" \
	"{\n" \
	"\tPRL_SDK_CHECK;\n" \
	"\tdo {\n" \
	"\t\tPRL_RESULT prlResult = PrlApi_Deinit();\n" \
	"\t\ts_bIsSDKInitialized = ( ! PRL_SUCCEEDED(prlResult) );\n\n" \
	"\t\t// FIXME: Uncomment unload SDK library after bug #441458 will be fixed\n" \
	"\t\t// SdkWrap_Unload();\n\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", prlResult )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_PRL_API_DEINIT \
	"PrlApi_Deinit"
#define MDL_PRL_API_DEINIT_IMPL \
	"static PyObject* sdk_"MDL_PRL_API_DEINIT"(PyObject* self, PyObject* args)\n" \
	"{\n" \
	"\treturn sdk_"MDL_DEINITIALIZE_SDK_NAME"(self, args);\n" \
	"}\n\n"

// Non-standard SDK functions

#define MDL_PRL_OP_TYPE_LIST_GET_ITEM \
	"PrlOpTypeList_GetItem"
#define MDL_PRL_OP_TYPE_LIST_GET_ITEM_IMPL \
	"static PyObject* sdk_"MDL_PRL_OP_TYPE_LIST_GET_ITEM"(PyObject* /*self*/, PyObject* args)\n" \
	"{\n" \
	"\tPRL_SDK_CHECK;\n" \
	"\tdo {\n" \
	"\t\tPRL_HANDLE	hOpTypeList = (PRL_HANDLE )0;\n" \
	"\t\tPRL_UINT32 nItemIndex = (PRL_UINT32 )0;\n\n" \
	"\t\tif ( ! PyArg_ParseTuple( args, \"kI:"MDL_PRL_OP_TYPE_LIST_GET_ITEM"\" , &hOpTypeList, &nItemIndex ) )\n" \
	"\t\t\tbreak;\n\n" \
	"\t\tPRL_INT64\tnInt64 = 0LL;\n" \
	"\t\tPRL_UINT64\tnUInt64 = 0LL;\n" \
	"\t\tdouble\t\tdDouble = 0.0;\n\n" \
	"\t\tPRL_SIZE nTypeSize = 0;\n" \
	"\t\tPRL_RESULT prlResult;\n" \
	"\t\tPy_BEGIN_ALLOW_THREADS\n" \
	"\t\tprlResult = PrlOpTypeList_GetTypeSize(hOpTypeList, &nTypeSize);\n" \
	"\t\tPy_END_ALLOW_THREADS\n" \
	"\t\tif (PRL_SUCCEEDED(prlResult))\n" \
	"\t\t{\n" \
	"\t\t\tstd::string buf(nTypeSize, 0);\n" \
	"\t\t\tPy_BEGIN_ALLOW_THREADS\n" \
	"\t\t\tprlResult = PrlOpTypeList_GetItem(hOpTypeList, nItemIndex, (PRL_VOID_PTR )buf.data());\n" \
	"\t\t\tPy_END_ALLOW_THREADS\n" \
	"\t\t\tif (PRL_SUCCEEDED(prlResult))\n" \
	"\t\t\t{\n" \
	"\t\t\t\tswitch(nTypeSize)\n" \
	"\t\t\t\t{\n" \
	"\t\t\t\tcase 1:\n" \
	"\t\t\t\t\tnInt64 = PRL_INT64(*( (PRL_INT8* )buf.data() ));\n" \
	"\t\t\t\t\tnUInt64 = PRL_UINT64(*( (PRL_UINT8* )buf.data() ));\n" \
	"\t\t\t\t\tbreak;\n" \
	"\t\t\t\tcase 2:\n" \
	"\t\t\t\t\tnInt64 = PRL_INT64(*( (PRL_INT16* )buf.data() ));\n" \
	"\t\t\t\t\tnUInt64 = PRL_UINT64(*( (PRL_UINT16* )buf.data() ));\n" \
	"\t\t\t\t\tbreak;\n" \
	"\t\t\t\tcase 4:\n" \
	"\t\t\t\t\tnInt64 = PRL_INT64(*( (PRL_INT32* )buf.data() ));\n" \
	"\t\t\t\t\tnUInt64 = PRL_UINT64(*( (PRL_UINT32* )buf.data() ));\n" \
	"\t\t\t\t\tbreak;\n" \
	"\t\t\t\tcase 8:\n" \
	"\t\t\t\t\tnInt64 = PRL_INT64(*( (PRL_INT64* )buf.data() ));\n" \
	"\t\t\t\t\tnUInt64 = PRL_UINT64(*( (PRL_UINT64* )buf.data() ));\n" \
	"\t\t\t\t\tdDouble = double(*( (double* )buf.data() ));\n" \
	"\t\t\t\t\tbreak;\n" \
	"\t\t\t\tdefault:\n" \
	"\t\t\t\t\tPyErr_Format(PyExc_TypeError, \"Unsupported opaque type size = %u!\", nTypeSize);\n" \
	"\t\t\t\t\treturn NULL;\n" \
	"\t\t\t\t}\n" \
	"\t\t\t}\n" \
	"\t\t}\n\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", prlResult )) )\n" \
	"\t\t\tbreak;\n" \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"L\", nInt64 )) )\n" \
	"\t\t\tbreak;\n" \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"K\", nUInt64 )) )\n" \
	"\t\t\tbreak;\n" \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"d\", dDouble )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_PRL_REPORT_GET_DATA \
	"PrlReport_GetData"
#define MDL_PRL_REPORT_GET_DATA_IMPL \
	"static PyObject* sdk_"MDL_PRL_REPORT_GET_DATA"(PyObject* /*self*/, PyObject* args)\n" \
	"{\n" \
	"\tPRL_SDK_CHECK;\n" \
	"\tdo {\n" \
	"\t\tPRL_HANDLE	hReport = (PRL_HANDLE )0;\n" \
	"\t\tif ( ! PyArg_ParseTuple( args, \"k:"MDL_PRL_REPORT_GET_DATA"\" , &hReport ) )\n" \
	"\t\t\tbreak;\n\n" \
	"\t\tPyObject* pBufObj = NULL;\n" \
	"\t\tPRL_UINT32 nBufLength = 0;\n" \
	"\t\tPRL_RESULT prlResult;\n" \
	"\t\tPy_BEGIN_ALLOW_THREADS\n" \
	"\t\tprlResult = PrlReport_GetData(hReport, NULL, &nBufLength);\n" \
	"\t\tPy_END_ALLOW_THREADS\n" \
	"\t\tif (PRL_SUCCEEDED(prlResult))\n" \
	"\t\t{\n" \
	"#if (PY_VERSION_HEX >= 0x02050000)\n" \
	"\t\t\tPy_ssize_t nBufSize = (Py_ssize_t )nBufLength;\n" \
	"#else\n" \
	"\t\t\tint nBufSize = (int )nBufLength;\n" \
	"#endif\n" \
	"\t\t\tpBufObj = PyBuffer_New( nBufSize );\n" \
	"\t\t\tif ( ! pBufObj )\n" \
	"\t\t\t\treturn NULL;\n" \
	"\t\t\tPRL_VOID_PTR pBuf = NULL;\n" \
	"\t\t\tif ( PyObject_AsWriteBuffer(pBufObj, &pBuf, &nBufSize) )\n" \
	"\t\t\t\treturn NULL;\n" \
	"\t\t\tPy_BEGIN_ALLOW_THREADS\n" \
	"\t\t\tprlResult = PrlReport_GetData(hReport, pBuf, &nBufLength);\n" \
	"\t\t\tPy_END_ALLOW_THREADS\n" \
	"\t\t}\n\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", prlResult )) )\n" \
	"\t\t\tbreak;\n" \
	"\t\tif ( pBufObj )\n" \
	"\t\t\tif ( PyList_Append(ret_list, pBufObj) )\n" \
	"\t\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_PRL_REPORT_SEND \
	"PrlReport_Send"
#define MDL_PRL_API_SEND_PROBLEM_REPORT \
	"PrlApi_SendProblemReport"
#define MDL_PRL_API_SEND_PACKED_PROBLEM_REPORT \
	"PrlApi_SendPackedProblemReport"

#define MDL_PRL_DEV_DISPLAY_SET_CONFIGURATION \
	"PrlDevDisplay_SetConfiguration"
#define MDL_PRL_DEV_DISPLAY_SET_CONFIGURATION_IMPL \
	"static PyObject* sdk_"MDL_PRL_DEV_DISPLAY_SET_CONFIGURATION"(PyObject* /*self*/, PyObject* args)\n" \
	"{\n" \
	"\tPRL_SDK_CHECK;\n" \
	"\tdo {\n" \
	"\t\tPRL_HANDLE	hVm = (PRL_HANDLE )0;\n" \
	"\t\tPyObject* pList = (PyObject* )0;\n\n" \
	"\t\tif ( ! PyArg_ParseTuple( args, \"kO:"MDL_PRL_DEV_DISPLAY_SET_CONFIGURATION"\" , &hVm, &pList ) )\n" \
	"\t\t\tbreak;\n\n" \
	"\t\tPRL_INT32 nCount = (PRL_INT32 )PyList_Size(pList);\n" \
	"\t\tif ( nCount < 0 ) break;\n" \
	"\t\tPRL_IO_DISPLAY_SCREEN_SIZE_PTR displays = new PRL_IO_DISPLAY_SCREEN_SIZE[nCount];\n\n" \
	"\t\tfor(PRL_INT32 i = 0; i < nCount; ++i)\n" \
	"\t\t{\n" \
	"#if (PY_VERSION_HEX >= 0x02050000)\n" \
	"\t\t\tPyObject* pDisplayList = PyList_GetItem(pList, (Py_ssize_t )i);\n" \
	"#else\n" \
	"\t\t\tPyObject* pDisplayList = PyList_GetItem(pList, (int )i);\n" \
	"#endif\n" \
	"\t\t\tint nSize = (int )PyList_Size(pDisplayList);\n" \
	"\t\t\tif ( nSize != (int )(sizeof(*displays)/sizeof(PRL_INT32 )) )\n" \
	"\t\t\t{\n" \
	"\t\t\t\tdelete [] displays;\n" \
	"\t\t\t\tPyErr_SetString(PyExc_RuntimeError, \"Parameter list size mismatches to PRL_IO_DISPLAY_SCREEN_SIZE structure size!\");\n" \
	"\t\t\t\treturn NULL;\n" \
	"\t\t\t}\n" \
	"\t\t\tmemset(&displays[i], 0, sizeof(*displays));\n" \
	"\t\t\tchar* pBuf = (char* )&displays[i];\n" \
	"\t\t\tfor(int j = 0; j < nSize; ++j)\n" \
	"\t\t\t{\n" \
	"#if (PY_VERSION_HEX >= 0x02050000)\n" \
	"\t\t\t\tPyObject* pField = PyList_GetItem(pDisplayList, (Py_ssize_t )j);\n" \
	"#else\n" \
	"\t\t\t\tPyObject* pField = PyList_GetItem(pDisplayList, j);\n" \
	"#endif\n" \
	"\t\t\t\tPRL_INT32 nField = (PRL_INT32 )PyInt_AsLong(pField);\n" \
	"\t\t\t\tPyObject* pErr = PyErr_Occurred();\n" \
	"\t\t\t\tif ( pErr )\n" \
	"\t\t\t\t{\n" \
	"\t\t\t\t\tdelete [] displays;\n" \
	"\t\t\t\t\tchar err_str[128];\n" \
	"\t\t\t\t\tsprintf(err_str, \"Cannot convert object to the expected integer value (config index: %d, field No: %d) !\", i, j + 1);\n" \
	"\t\t\t\t\tPyErr_SetString(pErr, err_str);\n" \
	"\t\t\t\t\treturn NULL;\n" \
	"\t\t\t\t}\n" \
	"\t\t\t\tmemcpy(pBuf, &nField, sizeof(nField));\n" \
	"\t\t\t\tpBuf += sizeof(PRL_INT32 );\n" \
	"\t\t\t}\n" \
	"\t\t}\n\n" \
	"\t\tPRL_RESULT prlResult;\n" \
	"\t\tPy_BEGIN_ALLOW_THREADS\n" \
	"\t\tprlResult = "MDL_PRL_DEV_DISPLAY_SET_CONFIGURATION"(hVm, displays, nCount);\n" \
	"\t\tPy_END_ALLOW_THREADS\n" \
	"\t\tdelete [] displays;\n\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", prlResult )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#define MDL_PRL_SRV_GET_DEFAULT_VM_CONFIG \
	"PrlSrv_GetDefaultVmConfig"
#define MDL_PRL_SRV_GET_DEFAULT_VM_CONFIG_IMPL \
	"static PyObject* sdk_"MDL_PRL_SRV_GET_DEFAULT_VM_CONFIG"(PyObject* /*self*/, PyObject* args)\n" \
	"{\n" \
	"\tPRL_SDK_CHECK;\n" \
	"\tdo {\n" \
	"\t\tPRL_HANDLE	hSrv = (PRL_HANDLE )0;\n" \
	"\t\tPRL_VM_TYPE nVmType = (PRL_VM_TYPE )0;\n" \
	"\t\tPRL_STR sConfigSample = (PRL_STR )0;\n" \
	"\t\tPRL_UINT32 nOsVersion = (PRL_UINT32 )0;\n" \
	"\t\tPRL_UINT32 nFlags = (PRL_UINT32 )0;\n\n" \
	"\t\tif ( ! PyArg_ParseTuple( args, \"kIsII:"MDL_PRL_SRV_GET_DEFAULT_VM_CONFIG"\" ," \
	"\t\t\t\t&hSrv, &nVmType, &sConfigSample, &nOsVersion, &nFlags ) )\n" \
	"\t\t\tbreak;\n\n" \
	"\t\tPRL_GET_VM_CONFIG_PARAM_DATA config_data;\n" \
	"\t\tconfig_data.nVmType = nVmType;\n" \
	"\t\tconfig_data.sConfigSample = sConfigSample;\n" \
	"\t\tconfig_data.nOsVersion = nOsVersion;\n\n" \
	"\t\tPRL_HANDLE hResultJob = "MDL_PRL_SRV_GET_DEFAULT_VM_CONFIG"(hSrv, &config_data, nFlags);\n" \
	MDL_NEW_RETURN_LIST \
	"\t\tif ( PyList_Append(ret_list, Py_BuildValue( \"k\", hResultJob )) )\n" \
	"\t\t\tbreak;\n" \
	MDL_END_SDK_PYTHON_FUNCTION

#endif	// MODULE_TEMPLATES_H
