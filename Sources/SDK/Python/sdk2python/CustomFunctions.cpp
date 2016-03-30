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
		if (type == PHT_EVENT) {
			if (!PyArg_ParseTuple((PyObject *)user_data, "kOO", &handle, &py_callback_function, &py_user_data))
				break;
			PyObject *arglist;
			arglist = Py_BuildValue("(i,O)", hEvent, py_user_data);
			PyObject *call_obj_result;
			call_obj_result = PyObject_CallObject(py_callback_function, arglist);
			if (PyErr_Occurred())
				PyErr_Print();
			Py_XDECREF(call_obj_result);
			Py_DECREF(arglist);
		}
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
		if ( PyList_Append(ret_list, Py_BuildValue( "k", hResultJob )) )
			break;
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
		if ( PyList_Append(ret_list, Py_BuildValue( "k", hResultJob )) )
			break;
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
		if (PyList_Append(ret_list, Py_BuildValue("k", prlResult)))
			break;
		if (PyList_Append(ret_list, Py_BuildValue("I", data.type)))
			break;
		if (PyList_Append(ret_list, Py_BuildValue("I", data.value)))
			break;
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
		if (PyList_Append(ret_list, Py_BuildValue("k", prlResult)))
			break;
		return ret_list;
	} while(0);
	return NULL;
}

static int generateGuid(PRL_GUID *guid, char *buf, size_t len)
{
        int ret = snprintf(buf, len, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                        guid->Data1,
                        guid->Data2,
                        guid->Data3,
                        guid->Data4[0],
                        guid->Data4[1],
                        guid->Data4[2],
                        guid->Data4[3],
                        guid->Data4[4],
                        guid->Data4[5],
                        guid->Data4[6],
                        guid->Data4[7]);
        if (ret < 0 || ret >= (ssize_t)len)
                return -1;
        return 0;
}

static PyObject* sdk_PrlDisk_GetDiskInfo(PyObject* /*self*/, PyObject* args)
{
	PRL_SDK_CHECK;
	do {
		PRL_HANDLE	hDisk = (PRL_HANDLE )0;
		PyObject *info = NULL;
		if ( ! PyArg_ParseTuple( args, "kO:PrlDisk_GetDiskInfo" , &hDisk, &info) )
			break;
		if (info == NULL)
			break;

		PRL_DISK_PARAMETERS_PTR disk_params = NULL;
		PRL_UINT32 size = 0;
		PRL_RESULT prlResult;
		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlDisk_GetDiskInfo(hDisk, disk_params, &size);
		Py_END_ALLOW_THREADS

		char *buf = new char[size];
		memset(buf, 0, size);
		disk_params = (PRL_DISK_PARAMETERS_PTR) buf;
		Py_BEGIN_ALLOW_THREADS
		prlResult = PrlDisk_GetDiskInfo(hDisk, disk_params, &size);
		Py_END_ALLOW_THREADS

		if ( PyObject_SetAttrString(info, "heads", Py_BuildValue("i", disk_params->m_Heads)))
			break;
		if ( PyObject_SetAttrString(info, "cylinders", Py_BuildValue("i", disk_params->m_Cylinders)))
			break;
		if ( PyObject_SetAttrString(info, "sectors", Py_BuildValue("i", disk_params->m_Sectors)))
			break;
		if ( PyObject_SetAttrString(info, "sizeInSectors", Py_BuildValue("L", disk_params->m_SizeInSectors)))
			break;
		if ( PyObject_SetAttrString(info, "blockSize", Py_BuildValue("i", disk_params->m_BlockSize)))
			break;
		if ( PyObject_SetAttrString(info, "parts", Py_BuildValue("i", disk_params->m_Parts)))
			break;

		PyObject *storages = PyList_New(0);
		int i;
		for (i=0; i < (int) disk_params->m_Parts; ++i) {
			if( PyList_Append(storages, Py_BuildValue("s", disk_params->m_Storages[i].pFileName)))
				break;
		}
		if (i < (int) disk_params->m_Parts)
			break;
		if ( PyObject_SetAttrString(info, "storages", storages))
			break;

		char guid[37];
		generateGuid(&disk_params->Uid, guid, sizeof(guid));
		if ( PyObject_SetAttrString(info, "uid", Py_BuildValue("s", guid)))
			break;
		if ( PyObject_SetAttrString(info, "name", Py_BuildValue("s", disk_params->Name)))
			break;
		delete[] buf;

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;
		if ( PyList_Append(ret_list, Py_BuildValue( "k", prlResult )) )
			break;
		if ( PyList_Append(ret_list, info) )
			break;
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
		if ( PyList_Append(ret_list, Py_BuildValue( "k", prlResult )) )
			break;
		if ( PyList_Append(ret_list, Py_BuildValue( "L", nInt64 )) )
			break;
		if ( PyList_Append(ret_list, Py_BuildValue( "K", nUInt64 )) )
			break;
		if ( PyList_Append(ret_list, Py_BuildValue( "d", dDouble )) )
			break;
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
			pBufObj = PyBuffer_New( nBufSize );
			if ( ! pBufObj )
				return NULL;
			PRL_VOID_PTR pBuf = NULL;
			if ( PyObject_AsWriteBuffer(pBufObj, &pBuf, &nBufSize) )
				return NULL;
			Py_BEGIN_ALLOW_THREADS
			prlResult = PrlReport_GetData(hHandle, pBuf, &nBufLength);
			Py_END_ALLOW_THREADS
		}

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;
		if ( PyList_Append(ret_list, Py_BuildValue( "k", prlResult )) )
			break;
		if ( pBufObj )
			if ( PyList_Append(ret_list, pBufObj) )
				break;
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
		if ( PyList_Append(ret_list, Py_BuildValue( "k", hResultJob )) )
			break;
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
		if ( PyList_Append(ret_list, Py_BuildValue( "k", hResultJob )) )
			break;
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
			pBufObj = PyBuffer_New( nBufSize );
			if ( ! pBufObj )
				return NULL;
			PRL_VOID_PTR pBuf = NULL;
			if ( PyObject_AsWriteBuffer(pBufObj, &pBuf, &nBufSize) )
				return NULL;
			Py_BEGIN_ALLOW_THREADS
			prlResult = PrlDiskMap_Read(hHandle, pBuf, &nBufLength);
			Py_END_ALLOW_THREADS
		}

		PyObject* ret_list = PyList_New(0);
		if ( ! ret_list )
			break;
		if ( PyList_Append(ret_list, Py_BuildValue( "k", prlResult )) )
			break;
		if ( pBufObj )
			if ( PyList_Append(ret_list, pBufObj) )
				break;
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
		if ( PyList_Append(ret_list, Py_BuildValue( "k", prlResult )) )
			break;
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
		if ( PyList_Append(ret_list, Py_BuildValue( "k", prlResult )) )
			break;
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
