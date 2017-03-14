/*
 * PrlCommon.h
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


#ifndef PrlCommon_H
#define PrlCommon_H

#include <QHash>
#include <QObject>
#include <QMutex>
#include <QList>
#include <QThread>
#include <QWaitCondition>
#include <QAtomicInt>
#include <QReadWriteLock>
#include <QObjectCleanupHandler>
#include "Parallels.h"

#define PRL_STACK_SIZE 1024*1024

#define CHECK_CONNECT_DONE_PROPERLY(connect_expression)\
	{\
		bool bConnectionResult = connect_expression;\
		if (!bConnectionResult)\
			WRITE_TRACE(DBG_FATAL, "Signal connection failed for expression: '%s'", #connect_expression);\
		Q_ASSERT(bConnectionResult);\
	}

#define ASYNC_CHECK_API_INITIALIZED(job_type)\
	PrlSdkStatusReader _status_reader;\
	if (!_status_reader.IsSdkInitialized())\
		return GENERATE_ERROR_HANDLE(PRL_ERR_API_WASNT_INITIALIZED, job_type);

#define SYNC_CHECK_API_INITIALIZED\
	PrlSdkStatusReader _status_reader;\
	if (!_status_reader.IsSdkInitialized())\
		return (PRL_ERR_API_WASNT_INITIALIZED);

#define RETURN_RES(res)\
	{\
		return res;\
	}

extern PRL_BOOL PRL_WRONG_VM_DEVICE_HANDLE_TYPE(PRL_HANDLE_TYPE typo);
extern PRL_BOOL PRL_WRONG_VM_DEVICE_HANDLE(PRL_HANDLE handle);
extern PRL_BOOL PRL_WRONG_VM_DEVICE_TYPE(PRL_DEVICE_TYPE typo);
class PrlHandleJob;
extern PRL_HANDLE store_result(PrlHandleJob *job_obj, PRL_HANDLE hVm = PRL_INVALID_HANDLE);

#define PRL_CntxSw_CHECK_HANDLE_ASYNC(h_value, h_expected_type, job_code)   \
    if ( PRL_WRONG_HANDLE(h_value, h_expected_type) )                   \
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_code)) ;

#define ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(methodname, param, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(param) )\
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname(param);\
	if (!pJob)\
		RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define DISP_ONE_HANDLE_AND_ONE_PTR_PARAM_METH_IMPLEMENTATION(methodname, param, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) || PRL_WRONG_PTR(param) )\
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname(param);\
	if (!pJob)\
		RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_METH_IMPLEMENTATION(methodname, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname();\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION(methodname, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname();\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_STAT_METH_IMPLEMENTATION(methodname, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerStatPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerStat>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname();\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION(methodname, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname();\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_DEPRECATED_METH_IMPLEMENTATION(methodname, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname();\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_METH_IMPLEMENTATION_WITH_FLAGS(methodname, flags, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServer>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname(flags);\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_VM_METH_IMPLEMENTATION_WITH_FLAGS(methodname, flags, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerVmPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerVm>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname(flags);\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_DISP_METH_IMPLEMENTATION_WITH_FLAGS(methodname, flags, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerDispPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDisp>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname(flags);\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_NET_METH_IMPLEMENTATION_WITH_FLAGS(methodname, flags, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerNetPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerNet>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname(flags);\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_SRV_DEPRECATED_METH_IMPLEMENTATION_WITH_FLAGS(methodname, flags, job_type)\
	if ( PRL_WRONG_HANDLE(hServer, PHT_SERVER) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleServerDeprecatedPtr pServer = PRL_OBJECT_BY_HANDLE<PrlHandleServerDeprecated>( hServer );\
	PrlHandleJobPtr pJob = pServer->methodname(flags);\
	if ( !pJob )\
	RETURN_RES(PRL_INVALID_HANDLE)\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_VM_METH_IMPLEMENTATION(methodname, job_type)\
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);\
	PrlHandleJobPtr pJob = pVm->methodname();\
	if (!pJob)\
	RETURN_RES(PRL_INVALID_HANDLE)\
	pJob->SetVmHandle(hVm);\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_AND_ONE_PTR_PARAM_VM_METH_IMPLEMENTATION(methodname, param,  job_type)\
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) || PRL_WRONG_PTR(param) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);\
	PrlHandleJobPtr pJob = pVm->methodname( param );\
	if (!pJob)\
	RETURN_RES(PRL_INVALID_HANDLE)\
	pJob->SetVmHandle(hVm);\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_AND_FLAGS_VM_METH_IMPLEMENTATION(methodname, flags, job_type)\
	if ( PRL_WRONG_HANDLE(hVm, PHT_VIRTUAL_MACHINE) )\
		RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleVmSrvPtr pVm = PRL_OBJECT_BY_HANDLE<PrlHandleVmSrv>(hVm);\
	PrlHandleJobPtr pJob = pVm->methodname(flags);\
	if (!pJob)\
		RETURN_RES(PRL_INVALID_HANDLE)\
	pJob->SetVmHandle(hVm);\
	RETURN_RES(pJob->GetHandle())

#define ONE_HANDLE_VM_DEV_METH_IMPLEMENTATION(methodname, job_type)\
	if ( PRL_WRONG_VM_DEVICE_HANDLE(hVmDevice) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_INVALID_ARG, job_type))\
	PrlHandleVmDevicePtr pDevice = PRL_OBJECT_BY_HANDLE<PrlHandleVmDevice>(hVmDevice);\
	PRL_HANDLE hVm;\
	if ( PRL_FAILED(pDevice->GetVm(&hVm)) )\
	RETURN_RES(GENERATE_ERROR_HANDLE(PRL_ERR_UNINITIALIZED, job_type))\
	PrlHandleJobPtr pJob = pDevice->methodname();\
	if (!pJob)\
	RETURN_RES((PRL_INVALID_HANDLE))\
	pJob->SetVmHandle(hVm);\
	RETURN_RES(pJob->GetHandle())

#ifdef EXTERNALLY_AVAILABLE_BUILD
#define PRL_ELIMINATE_DEPRECATED_FLAGS(flags) flags & ~PCF_LIGHTWEIGHT_CLIENT
#else
#define PRL_ELIMINATE_DEPRECATED_FLAGS(flags) flags
#endif

/**
 * SDK initialization status helpers
 */
class PrlSdkStatusReader
{
public:
	PrlSdkStatusReader();
	bool IsSdkInitialized();

private:
	QReadLocker m_lock;
};

class PrlSdkStatusWriter
{
public:
	PrlSdkStatusWriter();
	~PrlSdkStatusWriter();
	void SetInitStatus(bool bStatus);
};

/**
 * Common SDK mech for service threads destruction
 * There is one instance of the class must be created
 */
class PrlSdkThreadsDestructor : public QObject
{
Q_OBJECT

public:
	/** Registries thread instance for further deletion */
	void RegisterThreadForDeletion(class PrlThread *pThread, Qt::HANDLE threadId_);
	void RegisterThreadForDeletion(struct Heappy *thread_);
	/** Waits for all registered threads and destroys them */
	void WaitAllThreadsAndCleanup();

public slots:
	/** Slot calls by timeout and it goal to check whether threads finalizing they work and delete them */
	void ProcessThreadsInstances();

private:
	/** Threads objects list access synchronization object */
	QMutex m_ThreadsListMutex;
	QWaitCondition m_event;
	/** Processing threads objects list */
	QObjectList m_heappies;
	QList<QThread* > m_pending;
	QHash<Qt::HANDLE, QThread* > m_lstThreadObjs;
};

/**
 * Simple helper which let to count instances of all inherited entities
 */
class CInstancesCounter
{
public:
	/**
	 * Class constructor - increases instances num
	 */
	CInstancesCounter();
	/**
	 * Class destructor - decreases instances num
	 */
	~CInstancesCounter();

public:
	/**
	 * Returns threads instances number
	 */
	static quint32 GetInstancesNum();

private:
	/** Global threads instances counter */
	static QAtomicInt g_nInstancesNum;
};

/**
 * Thread inheritance that incapsulates necessary registration actions
 * at common SDK threads destruction mechanism
 */
class PrlThread : public QThread
{
Q_OBJECT

public:
	/** Class default constructor */
	PrlThread();

private:
	/** Overridden thread function */
	void run();
	/** Template thread function */
	virtual void concreteRun();
};

///////////////////////////////////////////////////////////////////////////////
// struct Heappy
// protect from creation on stack.

struct Heappy: PrlThread, CInstancesCounter
{
	Heappy();

protected:
	~Heappy();
};

//Global threads destructor instance
extern PrlSdkThreadsDestructor *g_pThreadsDestructor;

/**
 * Returns supported guest OSes types list for current API mode
 * @param target host OS type
 * @param pointer to the buffer for storing result
 * @return PRL_RESULT. Possible values:
 *		PRL_ERR_OUT_OF_MEMORY - not enough memory for instantiate list object
 *		PRL_ERR_SUCCESS - operation completed successfully
 */
PRL_RESULT PrlGetSupportedOsesTypesList(PRL_HOST_OS_TYPE nHostOsType, PRL_HANDLE_PTR phOsesTypesList);

/**
 * Returns supported guest OSes list for specified guest OS type at current API mode
 * @param target host OS type
 * @param requesting guest OS type
 * @param pointer to the buffer for storing result
 * @return PRL_RESULT. Possible values:
 *		PRL_ERR_OUT_OF_MEMORY - not enough memory for instantiate list object
 *		PRL_ERR_INVALID_ARG - unknown OS type specified
 *		PRL_ERR_SUCCESS - operation completed successfully
 */
PRL_RESULT PrlGetSupportedOsesVersionsList(PRL_HOST_OS_TYPE nHostOsType, PRL_UINT8 nGuestOsType, PRL_HANDLE_PTR phGuestsList);

/**
 * Returns default guest OS version for specified guest OS type at current API mode
 * @param requesting guest OS type
 * @param pointer to the buffer for storing result
 * @return PRL_RESULT. Possible values:
 *		PRL_ERR_INVALID_ARG - unknown OS type specified
 *		PRL_ERR_SUCCESS - operation completed successfully
 */
PRL_RESULT PrlGetDefaultOsVersion(PRL_UINT8 nGuestOsType, PRL_UINT16_PTR pnOsVersion);

/**
 * Returns default dispatcher listen port for appropriate application mode
 */
PRL_UINT32 PrlGetDefaultListenPort();

/**
* Implementation of the main thread with the event loop for our library.
* Parallels API is designed to be used by 3d party developers
* so we embed Qt application functionality to the library.
*/
class QMainThread : public QThread
{
public:
	QMainThread();
	void run();
	void NotifyThatSdkEventLoopStarted();
	void stop();
	void start(PRL_UINT32 nFlags);

public://Static members set
	static bool IsInitialized();
	static QMainThread *Instance();
	static void Deinit();

private:
	void InitSignals();
	void InitContextSwitcher();
	void InitCommonThreadsDestructionMech();
	void InitSdkParts();
	void DeinitSdkParts();

private:
	/** Main thread interoperationg synchronization objects */
	QMutex m_MainThreadMutex;
	QWaitCondition m_MainThreadCondition;
	bool m_bOwnerOfQApp;
	PRL_UINT32 m_nFlags;
};

#endif //#ifdef PrlCommon_H

