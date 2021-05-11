/*
 * Copyright (c) 2016-2017, Parallels International GmbH
 * Copyright (c) 2017-2019 Virtuozzo International GmbH. All rights reserved.
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

static PyObject* sdk_PrlApi_Init(PyObject* self, PyObject* args)
{
		return sdk_InitializeSDKEx(self, args);
}

static PyObject* sdk_PrlApi_InitEx(PyObject* self, PyObject* args)
{
		return sdk_InitializeSDKEx(self, args);
}

static PyObject* sdk_PrlApi_Deinit(PyObject* self, PyObject* args)
{
		return sdk_DeinitializeSDK(self, args);
}

static PRL_RESULT PrlHandle_EventCallbackHandler(PRL_HANDLE hEvent, PRL_VOID_PTR user_data)
{
	PyGILState_STATE gstate = PyGILState_Ensure();
	do {
		PRL_HANDLE_TYPE type;
		PRL_HANDLE handle = (PRL_HANDLE)0;
		PyObject *py_callback_function;
		PyObject *py_user_data;
		if (PrlHandle_GetType(hEvent, &type))
			break;

		if (!PyArg_ParseTuple((PyObject *)user_data, "kOO", &handle, &py_callback_function, &py_user_data))
			break;
		PyObject *arglist;
		arglist = Py_BuildValue("(I,O)", hEvent, py_user_data);
		PyObject *call_obj_result;
		call_obj_result = PyObject_CallObject(py_callback_function, arglist);
		if (PyErr_Occurred())
			PyErr_Print();
		Py_XDECREF(call_obj_result);
		Py_DECREF(arglist);
	} while(0);
	PyGILState_Release(gstate);
	return PRL_ERR_SUCCESS;
}

static PyObject* sdk_PrlApi_SendProblemReport(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_CONST_STR	sProblemReport = (PRL_CONST_STR )0;
		PRL_BOOL	bUseProxy = (PRL_BOOL )0;
		PRL_CONST_STR	sProxyHost = (PRL_CONST_STR )0;
		PRL_UINT32	nProxyPort = (PRL_UINT32 )0;
		PRL_CONST_STR	sProxyUserLogin = (PRL_CONST_STR )0;
		PRL_CONST_STR	sProxyUserPasswd = (PRL_CONST_STR )0;
		PRL_UINT32	nProblemSendTimeout = (PRL_UINT32 )0;
		PRL_UINT32	nReserved = (PRL_UINT32 )0;

		if ( ! PyArg_ParseTuple( args, "sIsIssII:PrlApi_SendProblemReport" , &sProblemReport, &bUseProxy, &sProxyHost, &nProxyPort, &sProxyUserLogin, &sProxyUserPasswd, &nProblemSendTimeout, &nReserved ) )
			break;

		PRL_HANDLE hResultJob;
		Py_BEGIN_ALLOW_THREADS
		hResultJob = PrlApi_SendProblemReport( sProblemReport, bUseProxy, sProxyHost, nProxyPort, sProxyUserLogin, sProxyUserPasswd, nProblemSendTimeout, nReserved, 0, 0 );
		Py_END_ALLOW_THREADS

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResultJob = Py_BuildValue( "k", hResultJob );
		if ( PyList_Append(ret_list, pResultJob) ) {
			Py_DECREF(pResultJob);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResultJob);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlSrv_GetDefaultVmConfig(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE	hSrv = (PRL_HANDLE )0;
		PRL_VM_TYPE nVmType = (PRL_VM_TYPE )0;
		PRL_STR sConfigSample = (PRL_STR )0;
		PRL_UINT32 nOsVersion = (PRL_UINT32 )0;
		PRL_UINT32 nFlags = (PRL_UINT32 )0;

		if ( ! PyArg_ParseTuple( args, "kIsII:PrlSrv_GetDefaultVmConfig" ,				&hSrv, &nVmType, &sConfigSample, &nOsVersion, &nFlags ) )
			break;

		PRL_GET_VM_CONFIG_PARAM_DATA config_data;
		config_data.nVmType = nVmType;
		config_data.sConfigSample = sConfigSample;
		config_data.nOsVersion = nOsVersion;

		PRL_HANDLE hResultJob = PrlSrv_GetDefaultVmConfig(hSrv, &config_data, nFlags);
		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResultJob = Py_BuildValue( "k", hResultJob );
		if ( PyList_Append(ret_list, pResultJob) ) {
			Py_DECREF(pResultJob);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResultJob);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlVmCfg_GetMemGuaranteeSize(PyObject* /*self*/, PyObject* args){
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE hVm;

		if (!PyArg_ParseTuple(args, "k:PrlVmCfg_GetMemGuaranteeSize" , &hVm))
			break;

		PRL_MEMGUARANTEE_DATA data;
		PRL_RESULT prlResult;

		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlVmCfg_GetMemGuaranteeSize(hVm, &data);
		Py_END_ALLOW_THREADS
		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		PyObject *pDataType = Py_BuildValue("I", data.type);
		if (PyList_Append(ret_list, pDataType)) {
			Py_DECREF(pDataType);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pDataType);

		PyObject *pDataValue = Py_BuildValue("I", data.value);
		if (PyList_Append(ret_list, pDataValue)) {
			Py_DECREF(pDataValue);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pDataValue);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlVmCfg_SetMemGuaranteeSize(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE hVm;
		PRL_MEMGUARANTEE_TYPE nType;
		PRL_UINT32 nValue;

		if (!PyArg_ParseTuple(args, "kII:PrlVmCfg_SetMemGuaranteeSize" ,
				&hVm, &nType, &nValue))
			break;

		PRL_MEMGUARANTEE_DATA data;
		data.type = nType;
		data.value = nValue;
		PRL_RESULT prlResult;

		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlVmCfg_SetMemGuaranteeSize(hVm, &data);
		Py_END_ALLOW_THREADS
		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlVmCfg_GetCpuLimitEx(PyObject* /*self*/, PyObject* args){
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE hVm;

		if (!PyArg_ParseTuple(args, "k:PrlVmCfg_GetCpuLimitEx" , &hVm))
			break;

		PRL_CPULIMIT_DATA data = {0, PRL_CPULIMIT_MHZ};
		PRL_RESULT prlResult;

		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlVmCfg_GetCpuLimitEx(hVm, &data);
		Py_END_ALLOW_THREADS
		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		PyObject *pDataType = Py_BuildValue("I", data.type);
		if (PyList_Append(ret_list, pDataType)) {
			Py_DECREF(pDataType);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pDataType);

		PyObject *pDataValue = Py_BuildValue("I", data.value);
		if (PyList_Append(ret_list, pDataValue)) {
			Py_DECREF(pDataValue);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pDataValue);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlVmCfg_SetCpuLimitEx(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE hVm;
		PRL_CPULIMIT_TYPE nType;
		PRL_UINT32 nValue;

		if (!PyArg_ParseTuple(args, "kII:PrlVmCfg_SetCpuLimitEx" ,
				&hVm, &nType, &nValue))
			break;

		PRL_CPULIMIT_DATA data;
		data.type = nType;
		data.value = nValue;
		PRL_RESULT prlResult;

		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlVmCfg_SetCpuLimitEx(hVm, &data);
		Py_END_ALLOW_THREADS
		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlOpTypeList_GetItem(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE	hOpTypeList = (PRL_HANDLE )0;
		PRL_UINT32 nItemIndex = (PRL_UINT32 )0;

		if ( ! PyArg_ParseTuple( args, "kI:PrlOpTypeList_GetItem" , &hOpTypeList, &nItemIndex ) )
			break;

		PRL_INT64	nInt64 = 0LL;
		PRL_UINT64	nUInt64 = 0LL;
		double		dDouble = 0.0;

		PRL_SIZE nTypeSize = 0;
		PRL_RESULT prlResult;
		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlOpTypeList_GetTypeSize(hOpTypeList, &nTypeSize);
		Py_END_ALLOW_THREADS
		if (PRL_SUCCEEDED(prlResult))
		{
			std::string buf(nTypeSize, 0);
			Py_BEGIN_ALLOW_THREADS
			prlResult = PrlOpTypeList_GetItem(hOpTypeList, nItemIndex, (PRL_VOID_PTR )buf.data());
			Py_END_ALLOW_THREADS
			if (PRL_SUCCEEDED(prlResult))
			{
				switch(nTypeSize)
				{
				case 1:
					nInt64 = PRL_INT64(*( (PRL_INT8* )buf.data() ));
					nUInt64 = PRL_UINT64(*( (PRL_UINT8* )buf.data() ));
					break;
				case 2:
					nInt64 = PRL_INT64(*( (PRL_INT16* )buf.data() ));
					nUInt64 = PRL_UINT64(*( (PRL_UINT16* )buf.data() ));
					break;
				case 4:
					nInt64 = PRL_INT64(*( (PRL_INT32* )buf.data() ));
					nUInt64 = PRL_UINT64(*( (PRL_UINT32* )buf.data() ));
					break;
				case 8:
					nInt64 = PRL_INT64(*( (PRL_INT64* )buf.data() ));
					nUInt64 = PRL_UINT64(*( (PRL_UINT64* )buf.data() ));
					dDouble = double(*( (double* )buf.data() ));
					break;
				default:
					PyErr_Format(PyExc_TypeError, "Unsupported opaque type size = %u!", nTypeSize);
					return NULL;
				}
			}
		}

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		PyObject *pnInt64 = Py_BuildValue( "L", nInt64 );
		if ( PyList_Append(ret_list, pnInt64) ) {
			Py_DECREF(pnInt64);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pnInt64);

		PyObject *pnUInt64 = Py_BuildValue( "K", nUInt64 );
		if ( PyList_Append(ret_list, pnUInt64) ) {
			Py_DECREF(pnUInt64);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pnUInt64);

		PyObject *pdDouble = Py_BuildValue( "d", dDouble );
		if ( PyList_Append(ret_list, pdDouble) ) {
			Py_DECREF(pdDouble);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pdDouble);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlReport_GetData(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE	hHandle = (PRL_HANDLE )0;
		if ( ! PyArg_ParseTuple( args, "k:PrlReport_GetData" , &hHandle ) )
			break;

		PyObject* pBufObj = NULL;
		PRL_UINT32 nBufLength = 0;
		PRL_RESULT prlResult;
		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlReport_GetData(hHandle, NULL, &nBufLength);
		Py_END_ALLOW_THREADS
		if (PRL_SUCCEEDED(prlResult))
		{
#if (PY_VERSION_HEX >= 0x02050000)
			Py_ssize_t nBufSize = (Py_ssize_t )nBufLength;
#else
			int nBufSize = (int )nBufLength;
#endif
#if (VZ_PY_VER < 3)
			pBufObj = PyBuffer_New( nBufSize );
#else
			pBufObj = PyByteArray_FromStringAndSize(NULL, nBufSize);
#endif
			if ( ! pBufObj )
				return NULL;

#if (VZ_PY_VER < 3)
			PRL_VOID_PTR pBuf = NULL;
			if ( PyObject_AsWriteBuffer(pBufObj, &pBuf, &nBufSize) )
#else
			Py_buffer *pBuf = NULL;
			if ( PyObject_GetBuffer(pBufObj, pBuf, PyBUF_WRITABLE | PyBUF_C_CONTIGUOUS ) )
#endif
				return NULL;
			Py_BEGIN_ALLOW_THREADS
#if (VZ_PY_VER < 3)
			prlResult = PrlReport_GetData(hHandle, pBuf, &nBufLength);
#else
			prlResult = PrlReport_GetData(hHandle, (PRL_VOID_PTR)pBuf, &nBufLength);
#endif
			Py_END_ALLOW_THREADS
		}

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		if ( pBufObj ) {
			if ( PyList_Append(ret_list, pBufObj) ) {
				Py_DECREF(pBufObj);
				Py_DECREF(ret_list);
				break;
			}
			Py_DECREF(pBufObj);
		}

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlReport_Send(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE	hProblemReport = (PRL_HANDLE )0;
		PRL_BOOL	bUseProxy = (PRL_BOOL )0;
		PRL_CONST_STR	sProxyHost = (PRL_CONST_STR )0;
		PRL_UINT32	nProxyPort = (PRL_UINT32 )0;
		PRL_CONST_STR	sProxyUserLogin = (PRL_CONST_STR )0;
		PRL_CONST_STR	sProxyUserPasswd = (PRL_CONST_STR )0;
		PRL_UINT32	nProblemSendTimeout = (PRL_UINT32 )0;
		PRL_UINT32	nReserved = (PRL_UINT32 )0;

		if ( ! PyArg_ParseTuple( args, "kIsIssII:PrlReport_Send" , &hProblemReport, &bUseProxy, &sProxyHost, &nProxyPort, &sProxyUserLogin, &sProxyUserPasswd, &nProblemSendTimeout, &nReserved ) )
			break;

		PRL_HANDLE hResultJob;
		Py_BEGIN_ALLOW_THREADS
		hResultJob = PrlReport_Send( hProblemReport, bUseProxy, sProxyHost, nProxyPort, sProxyUserLogin, sProxyUserPasswd, nProblemSendTimeout, nReserved, 0, 0 );
		Py_END_ALLOW_THREADS

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResultJob = Py_BuildValue( "k", hResultJob );
		if ( PyList_Append(ret_list, pResultJob) ) {
			Py_DECREF(pResultJob);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResultJob);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlApi_SendPackedProblemReport(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE	hProblemReport = (PRL_HANDLE )0;
		PRL_BOOL	bUseProxy = (PRL_BOOL )0;
		PRL_CONST_STR	sProxyHost = (PRL_CONST_STR )0;
		PRL_UINT32	nProxyPort = (PRL_UINT32 )0;
		PRL_CONST_STR	sProxyUserLogin = (PRL_CONST_STR )0;
		PRL_CONST_STR	sProxyUserPasswd = (PRL_CONST_STR )0;
		PRL_UINT32	nProblemSendTimeout = (PRL_UINT32 )0;
		PRL_UINT32	nReserved = (PRL_UINT32 )0;

		if ( ! PyArg_ParseTuple( args, "kIsIssII:PrlApi_SendPackedProblemReport" , &hProblemReport, &bUseProxy, &sProxyHost, &nProxyPort, &sProxyUserLogin, &sProxyUserPasswd, &nProblemSendTimeout, &nReserved ) )
			break;

		PRL_HANDLE hResultJob;
		Py_BEGIN_ALLOW_THREADS
		hResultJob = PrlApi_SendPackedProblemReport( hProblemReport, bUseProxy, sProxyHost, nProxyPort, sProxyUserLogin, sProxyUserPasswd, nProblemSendTimeout, nReserved, 0, 0 );
		Py_END_ALLOW_THREADS

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResultJob = Py_BuildValue( "k", hResultJob );
		if ( PyList_Append(ret_list, pResultJob) ) {
			Py_DECREF(pResultJob);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResultJob);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject* sdk_PrlDiskMap_Read(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE	hHandle = (PRL_HANDLE )0;
		if ( ! PyArg_ParseTuple( args, "k:PrlDiskMap_Read" , &hHandle ) )
			break;

		PyObject* pBufObj = NULL;
		PRL_UINT32 nBufLength = 0;
		PRL_RESULT prlResult;
		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlDiskMap_Read(hHandle, NULL, &nBufLength);
		Py_END_ALLOW_THREADS
		if (PRL_SUCCEEDED(prlResult))
		{
#if (PY_VERSION_HEX >= 0x02050000)
			Py_ssize_t nBufSize = (Py_ssize_t )nBufLength;
#else
			int nBufSize = (int )nBufLength;
#endif
#if (VZ_PY_VER < 3)
			pBufObj = PyBuffer_New( nBufSize );
#else
			pBufObj = PyByteArray_FromStringAndSize(NULL, nBufSize);
#endif
			if ( ! pBufObj )
				return NULL;
#if (VZ_PY_VER < 3)
			PRL_VOID_PTR pBuf = NULL;
			if ( PyObject_AsWriteBuffer(pBufObj, &pBuf, &nBufSize) )
#else
			Py_buffer *pBuf = NULL;
			if ( PyObject_GetBuffer(pBufObj, pBuf, PyBUF_WRITABLE | PyBUF_C_CONTIGUOUS ) )
#endif
				return NULL;
			Py_BEGIN_ALLOW_THREADS
#if (VZ_PY_VER < 3)
			prlResult = PrlDiskMap_Read(hHandle, pBuf, &nBufLength);
#else
			prlResult = PrlDiskMap_Read(hHandle, (PRL_VOID_PTR)pBuf, &nBufLength);
#endif
			Py_END_ALLOW_THREADS
		}

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		if ( pBufObj ) {
			if ( PyList_Append(ret_list, pBufObj) ) {
				Py_DECREF(pBufObj);
				Py_DECREF(ret_list);
				break;
			}
			Py_DECREF(pBufObj);
		}

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject *sdk_PrlHandle_RegEventHandler(PyObject* /*self*/, PyObject *args)
{
	PRL_SDK_CHECK;
	do {
		PyObject *py_callback_function;
		PyObject *py_user_data;
		PRL_HANDLE handle = (PRL_HANDLE)0;
		if ( ! PyArg_ParseTuple(args, "kOO:PrlHandle_RegEventHandler", &handle, &py_callback_function, &py_user_data))
			break;
		if ( ! PyCallable_Check(py_callback_function)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			break;
		}
		PRL_RESULT prlResult;
		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlHandle_RegEventHandler(handle, PrlHandle_EventCallbackHandler, (PRL_VOID_PTR) args);
		Py_END_ALLOW_THREADS
		if (PRL_SUCCEEDED(prlResult)) {
			Py_XINCREF(args);
			args_vector.push_back(args);
		}
		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject *sdk_PrlHandle_UnregEventHandler(PyObject* /*self*/, PyObject *args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE handle = (PRL_HANDLE)0;
		PyObject *py_callback_function;
		PyObject *py_user_data;
		PRL_RESULT prlResult = PRL_ERR_FAILURE;
		if ( ! PyArg_ParseTuple(args, "kOO:PrlSrv_UnregEventHandler", &handle, &py_callback_function, &py_user_data)) 
			break;
		for (std::vector<PyObject *>::size_type i = 0; i != args_vector.size(); i++) {
			if (PyObject_RichCompareBool(args, args_vector[i], Py_EQ)){ 
				Py_BEGIN_ALLOW_THREADS
				prlResult = PrlHandle_UnregEventHandler(handle, PrlHandle_EventCallbackHandler, (PRL_VOID_PTR) args_vector[i]);
				Py_END_ALLOW_THREADS
				if (PRL_SUCCEEDED(prlResult)) {
					Py_XDECREF(args_vector[i]);
					args_vector.erase(args_vector.begin() + i);
				}
				break;
			}
		}
		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;

		PyObject *pResult = Py_BuildValue( "k", prlResult );
		if ( PyList_Append(ret_list, pResult) ) {
			Py_DECREF(pResult);
			Py_DECREF(ret_list);
			break;
		}
		Py_DECREF(pResult);

		return ret_list;
	} while(0);
	return NULL;
}

static PyObject *sdk_PrlSrv_RegEventHandler(PyObject* self, PyObject *args)
{
	PRL_SDK_CHECK;
	return sdk_PrlHandle_RegEventHandler(self, args);
}

static PyObject *sdk_PrlSrv_UnregEventHandler(PyObject* self, PyObject *args)
{
	PRL_SDK_CHECK;
	return sdk_PrlHandle_UnregEventHandler(self, args);
}

static PyObject *sdk_PrlVm_RegEventHandler(PyObject* self, PyObject *args)
{
	PRL_SDK_CHECK;
	return sdk_PrlHandle_RegEventHandler(self, args);
}

static PyObject *sdk_PrlVm_UnregEventHandler(PyObject* self, PyObject *args)
{
	PRL_SDK_CHECK;
	return sdk_PrlHandle_UnregEventHandler(self, args);
}

static PyObject *sdk_PrlVmCfg_SetIoLimit(PyObject* /*self*/, PyObject *args)
{
	PRL_SDK_CHECK;

	PRL_HANDLE hVmCfg = (PRL_HANDLE )0;
	PRL_UINT32 value = (PRL_UINT32 )0;

	if ( ! PyArg_ParseTuple( args, "kI:PrlVmCfg_SetIoLimit", &hVmCfg, &value ) )
		return NULL;

	PRL_IOLIMIT_DATA iolimit_data;
	iolimit_data.value = value;
	iolimit_data.type = PRL_IOLIMIT_BS;

	PRL_RESULT prlResult;
	Py_BEGIN_ALLOW_THREADS
	prlResult = PrlVmCfg_SetIoLimit(hVmCfg, &iolimit_data);
	Py_END_ALLOW_THREADS

	PyObject* ret_list = PyList_New(0);
	if ( ! ret_list )
		return NULL;

	PyObject *pResult = Py_BuildValue( "k", prlResult );
	if ( PyList_Append(ret_list, pResult) ) {
		Py_DECREF(pResult);
		Py_DECREF(ret_list);
		return NULL;
	}
	Py_DECREF(pResult);

	return ret_list;
}

static PyObject *sdk_PrlVmCfg_GetIoLimit(PyObject* /*self*/, PyObject *args)
{
	PRL_SDK_CHECK;

	PRL_HANDLE hVmCfg = (PRL_HANDLE )0;
	PRL_UINT32 value = (PRL_UINT32 )0;

	if ( ! PyArg_ParseTuple( args, "k:PrlVmCfg_GetIoLimit", &hVmCfg ) )
		return NULL;

	PRL_IOLIMIT_DATA iolimit_data;
	iolimit_data.value = value;
	iolimit_data.type = PRL_IOLIMIT_BS;

	PRL_RESULT prlResult;
	Py_BEGIN_ALLOW_THREADS
	prlResult = PrlVmCfg_GetIoLimit(hVmCfg, &iolimit_data);
	Py_END_ALLOW_THREADS

	PyObject* ret_list = PyList_New(0);
	if ( ! ret_list )
		return NULL;

	PyObject *pResult = Py_BuildValue( "k", prlResult );
	if ( PyList_Append(ret_list, pResult) ) {
		Py_DECREF(pResult);
		Py_DECREF(ret_list);
		return NULL;
	}
	Py_DECREF(pResult);

	PyObject *pValue = Py_BuildValue( "I", iolimit_data.value );
	if ( PyList_Append(ret_list, pValue) ) {
		Py_DECREF(pValue);
		Py_DECREF(ret_list);
		return NULL;
	}
	Py_DECREF(pValue);

	return ret_list;
}
