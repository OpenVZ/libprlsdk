/*
 * PrlHandleBase.h
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


#ifndef __PARALLELS_HANDLE_BASE_H__
#define __PARALLELS_HANDLE_BASE_H__


#include "SDK/Include/Parallels.h"
#include <prlcommon/PrlCommonUtilsBase/PrlStringifyConsts.h>
#include <prlcommon/IOService/IOCommunication/IOProtocol.h>

#include <QMutex>
#include <QString>
#include <QMap>

#include <prlcommon/Std/SmartPtr.h>

//For preventing deprecated warnings
#ifdef _WIN_

#define strdup _strdup

#endif

#define PRL_RIGHT_PTR(addr) ((addr != 0) ? PRL_TRUE : PRL_FALSE)
#define PRL_WRONG_PTR(addr) ((addr != 0) ? PRL_FALSE : PRL_TRUE)

#define SYNCHRO_INTERNAL_DATA_ACCESS QMutexLocker _lock(&m_HandleMutex);


/**
 * Base handle object - all objects representation on the client
 * side should follow it's interface by the means of inheritance.
 */
class PrlHandleBase
{

public:

	/**
	 * Increase object's reference count by 1.
	 */
	virtual PRL_UINT32 AddRef();

	/**
	 * Decrease object's reference count by 1.
	 * As soon as object reference count reaches 0 - it deletes itself.
	 */
	virtual PRL_UINT32 Release();

	/**
	 * Returns object's type unique numeric identifier.
	 */
	virtual PRL_HANDLE_TYPE GetType() const;

	/**
	 * Returns handle associated with the object.
	 */
	virtual PRL_HANDLE GetHandle() const;

	/**
	 * Returns handle references count.
	 */
	PRL_UINT32 GetRefCount();

	/**
	 * Object's destructor is virtual to allow correct inheritance
	 */
	virtual ~PrlHandleBase();

	/**
	 * Initialize object from XML string
	 */
	virtual PRL_RESULT fromString(PRL_CONST_STR sXml);

	/**
	 * Returns object as XML string
	 */
	virtual QString toString();

	/**
	 * Returns numeric identifier of package if it applicable for the handle.
	 * At least 2 types of handle has package identifier: PHT_EVENT and PHT_JOB.
	 * @param pointer to a variable that receives the package identifier
	 * @return result of operation
	 */
	virtual PRL_RESULT GetPackageId( PRL_UINT64_PTR id );

protected:

	/**
	 * Object's constructor is protected to prevent direct
	 * object creation in non-inherited blocks of code.
	 */
	PrlHandleBase( PRL_HANDLE_TYPE type );

protected:
	/** Object for synchronizing access to personal handle data */
	mutable QMutex m_HandleMutex;
	PRL_UINT32		m_RefCount;
	PRL_HANDLE		m_Handle;

public:
	/** Statical map holding all handles by their identifiers. */
	typedef QMap< PRL_HANDLE, SmartPtr<PrlHandleBase> > HandleObjMap;
	static HandleObjMap *s_pHandlesMap;

	/** Object for synchronizing access to handles list */
	static QMutex *s_pHandlesMapMutex;

private:
	PRL_HANDLE_TYPE m_HandleType;
};

typedef QMap< PRL_HANDLE, SmartPtr<PrlHandleBase> > HandlesMap;

/**
 * Temporary class that can be using in debug cases
 * Must be eliminated at Release stage
 */
class PrlHandleDummy : public PrlHandleBase
{
public:
	PrlHandleDummy(PRL_HANDLE_TYPE _type)
	: PrlHandleBase(_type)
	{}
};

/**
 * Handle object smart wrapper implementation.
 * Automized AddRef/Release calls.
 */
template <typename PrlHandleType>
class PrlHandleSmartPtr
{
public:
	/**
	 * Class default constructor.
	 */
	PrlHandleSmartPtr()
	: m_pHandle(0)
	{}

	/**
	 * Special case for explicit construction by pointer based on non wrapped native object type
	 * @param pointer to the wrapping handle object
	 */
	explicit PrlHandleSmartPtr(PrlHandleType *pHandle)
	{
		if (pHandle)
		{
			QMutexLocker _lock(PrlHandleBase::s_pHandlesMapMutex);
			HandlesMap::const_iterator it = PrlHandleBase::s_pHandlesMap->find( pHandle->GetHandle() );
			if (it != PrlHandleBase::s_pHandlesMap->end() && it.value().get() == pHandle)
				m_pHandle = it.value();
		}
		if (m_pHandle) m_pHandle->AddRef();
	}

	/**
	 * Class constructor. Increments storing handle reference count
	 * @param pHandle - pointer to the wrapping handle instance
	 */
	explicit PrlHandleSmartPtr( SmartPtr<PrlHandleBase> pHandle )
	: m_pHandle(pHandle)
	{
		if (m_pHandle) m_pHandle->AddRef();
	}

	/**
	 * Class copy constructor. Increments storing handle reference count
	 * @param _ptr - copying smart pointer instance reference
	 */
	PrlHandleSmartPtr( const PrlHandleSmartPtr &_ptr )
	: m_pHandle(_ptr.m_pHandle)
	{
		if (m_pHandle) m_pHandle->AddRef();
	}

	/**
	 * Class destructor. Decrements storing handle reference count
	 */
	~PrlHandleSmartPtr()
	{
		if (m_pHandle) m_pHandle->Release();
	}

#define ASSIGN_POINTER(pointer)\
		SmartPtr<PrlHandleBase> copyPointer = pointer;\
		if (m_pHandle != copyPointer)\
		{\
			if (copyPointer.getImpl()) copyPointer->AddRef();\
			SmartPtr<PrlHandleBase> oldPointer = m_pHandle;\
			m_pHandle = copyPointer;\
			if (oldPointer.getImpl()) oldPointer->Release();\
		}

	/**
	 * Assign operator implementation.
	 * @param _ptr - assigning smart pointer instance reference
	 */
	PrlHandleSmartPtr &operator=( const PrlHandleSmartPtr &_ptr )
	{
		ASSIGN_POINTER(_ptr.m_pHandle)
		return (*this);
	}

	/**
	 * Assign pointer operator implementation.
	 * Special case for explicit assigning of pointer based on non wrapped native object type
	 * @param pointer to the wrapping handle object
	 */
	PrlHandleSmartPtr &operator=( PrlHandleType *pHandle )
	{
		SmartPtr<PrlHandleBase> pWrappedHandle;
		if (pHandle)
		{
			QMutexLocker _lock(PrlHandleBase::s_pHandlesMapMutex);
			HandlesMap::const_iterator it = PrlHandleBase::s_pHandlesMap->find( pHandle->GetHandle() );
			if (it != PrlHandleBase::s_pHandlesMap->end() && it.value().get() == pHandle)
				pWrappedHandle = it.value();
		}
		ASSIGN_POINTER(pWrappedHandle)
		return (*this);
	}
	/**
	 * Assign pointer operator implementation.
	 * @param pHandle - pointer to the new wrapping handle instance
	 */
	PrlHandleSmartPtr &operator=( SmartPtr<PrlHandleBase> pHandle )
	{
		ASSIGN_POINTER(pHandle)
		return (*this);
	}

	/**
	 * Selector -> implementation. Note: AddRef() not doing on that call so
	 * do not storing pointer for further (after smart pointer instance
	 * destruction) usage and do not call Release() after using pointer.
	 */
	PrlHandleType *operator->() const
	{
		return static_cast<PrlHandleType *>(m_pHandle.get());
	}

	/**
	 * Casting to the storing type operator implementation.
	 * Note: AddRef() not doing on that call so
	 * do not storing pointer for further (after smart pointer instance
	 * destruction) usage and do not call Release() after using pointer.
	 */
	operator PrlHandleType *() const
	{
		return static_cast<PrlHandleType *>(m_pHandle.get());
	}

	/**
	 * Same as above: just returns handle.
	 * Note: AddRef() not doing on that call so
	 * do not storing pointer for further (after smart pointer instance
	 * destruction) usage and do not call Release() after using pointer.
	 */
	PrlHandleType *getHandle() const
	{
		return static_cast<PrlHandleType *>(m_pHandle.get());
	}

	/**
	 * Checks if this handle is valid.
	 * Note: AddRef() not doing on that call so
	 * do not storing pointer for further (after smart pointer instance
	 * destruction) usage and do not call Release() after using pointer.
	 */
	PRL_BOOL isValid() const
	{
		return m_pHandle.isValid();
	}

private:

	/** Pointer to the wrapping handle instance */
	SmartPtr<PrlHandleBase> m_pHandle;
};

typedef PrlHandleSmartPtr<PrlHandleBase> PrlHandleBasePtr;
typedef PrlHandleSmartPtr<class PrlHandleServer> PrlHandleServerPtr;
typedef PrlHandleSmartPtr<class PrlHandleServerStat> PrlHandleServerStatPtr;
typedef PrlHandleSmartPtr<class PrlHandleServerNet> PrlHandleServerNetPtr;
typedef PrlHandleSmartPtr<class PrlHandleServerSecurity> PrlHandleServerSecurityPtr;
typedef PrlHandleSmartPtr<class PrlHandleServerDisp> PrlHandleServerDispPtr;
typedef PrlHandleSmartPtr<class PrlHandleServerDeprecated> PrlHandleServerDeprecatedPtr;
typedef PrlHandleSmartPtr<class PrlHandleServerVm> PrlHandleServerVmPtr;
typedef PrlHandleSmartPtr<class PrlHandleVm> PrlHandleVmPtr;
typedef PrlHandleSmartPtr<class PrlHandleVmSrv> PrlHandleVmSrvPtr;
typedef PrlHandleSmartPtr<class PrlHandleVmCfg> PrlHandleVmCfgPtr;
typedef PrlHandleSmartPtr<class PrlHandleVmGuest> PrlHandleVmGuestPtr;
typedef PrlHandleSmartPtr<class PrlHandleVmInfo> PrlHandleVmInfoPtr;
typedef PrlHandleSmartPtr<class PrlHandleVmToolsInfo> PrlHandleVmToolsInfoPtr;
typedef PrlHandleSmartPtr<class PrlHandleJob> PrlHandleJobPtr;
typedef PrlHandleSmartPtr<class PrlHandleEvent> PrlHandleEventPtr;
typedef PrlHandleSmartPtr<class PrlHandleVmEvent> PrlHandleVmEventPtr;
typedef PrlHandleSmartPtr<class PrlHandleIOEvent> PrlHandleIOEventPtr;
typedef PrlHandleSmartPtr<class PrlHandleResult> PrlHandleResultPtr;
typedef PrlHandleSmartPtr<class PrlHandleVmDevice> PrlHandleVmDevicePtr;
typedef PrlHandleSmartPtr<class PrlHandleSrvConfig> PrlHandleSrvConfigPtr;
typedef PrlHandleSmartPtr<class PrlHandleRemoteDevCmd> PrlHandleRemoteDevCmdPtr;
typedef PrlHandleSmartPtr<class PrlHandleLicense> PrlHandleLicensePtr;
typedef PrlHandleSmartPtr<class PrlHandleNetServiceStatus> PrlHandleNetServiceStatusPtr;
typedef PrlHandleSmartPtr<class PrlHandleLoginResponse> PrlHandleLoginResponsePtr;
typedef PrlHandleSmartPtr<class PrlHandleRunningTask> PrlHandleRunningTaskPtr;
typedef PrlHandleSmartPtr<class PrlHandleServerJob> PrlHandleServerJobPtr;
typedef PrlHandleSmartPtr<class PrlHandleOpTypeList> PrlHandleOpTypeListPtr;
typedef PrlHandleSmartPtr<class PrlHandleLoginHelperJob> PrlHandleLoginHelperJobPtr;
typedef PrlHandleSmartPtr<class PrlHandleLoginLocalHelperJob> PrlHandleLoginLocalHelperJobPtr;
typedef PrlHandleSmartPtr<class PrlHandleBackupResult> PrlHandleBackupResultPtr;
typedef PrlHandleSmartPtr<class PrlHandleVcmmdConfig> PrlHandleVcmmdConfigPtr;

/**
 * Get pointer to the object by handle.
 * @param handle to identify
 * @return corresponding object or NULL on wrong handle
 */
template <typename PrlHandleType>
PrlHandleSmartPtr<PrlHandleType> PRL_OBJECT_BY_HANDLE( PRL_HANDLE handle )
{
	QMutexLocker _lock(PrlHandleBase::s_pHandlesMapMutex);
	HandlesMap::iterator it = PrlHandleBase::s_pHandlesMap->find( handle );
	if ( it == PrlHandleBase::s_pHandlesMap->end() )
		return PrlHandleSmartPtr<PrlHandleType>(0);

	return PrlHandleSmartPtr<PrlHandleType>(it.value());
}

/**
 * Validate handle type.
 * It varifies that handles shows to the valid mem and valid type of handle
 * @param handle itself
 * @param type of the handle to verify PTH_ERROR - to verify valid type
 */
PRL_BOOL PRL_RIGHT_HANDLE( PRL_HANDLE handle, PRL_HANDLE_TYPE type = PHT_ERROR );

/**
 * Reverse of the method PRL_RIGHT_HANDLE
 * @see PRL_RIGHT_HANDLE
 */
PRL_BOOL PRL_WRONG_HANDLE( PRL_HANDLE handle, PRL_HANDLE_TYPE type = PHT_ERROR );

/**
 * Validate handle type in handles list.
 * @param handles list
 * @param handle type for checking
 */
PRL_BOOL PRL_WRONG_HANDLES_LIST( PRL_HANDLE hHndlList, PRL_HANDLE_TYPE nType );

/**
 * Copies specified string value to specified buffer.
 * Provides common solution for returning any string values from SDK API.
 *
 * @param copying string value in UTF8 enconding
 * @param pointer to buffer for storing result
 * @param pointer to buffer length
 * @return PRL_RESULT. Possible values:
 * * PRL_ERR_INVALID_ARG			- specified buffer length is null
 * * PRL_ERR_BUFFER_OVERRUN	- specified buffer size do not enough to fully storing value (in this case buffer
 * *														will contain non fully string value)
 * * PRL_ERR_SUCCESS					- operation completed successfully
 */
PRL_RESULT CopyStringValue(const QString &_string_value, PRL_STR sStringBuf, PRL_UINT32_PTR pnStringBufLength);


/**
 * Copies binary data from QByteArray into value to specified buffer.
 *
 * @param[in] source
 * @param[out] pointer to buffer for storing result
 * @param[in,out] pointer to buffer length
 * @return PRL_RESULT. Possible values:
 * PRL_ERR_INVALID_ARG - if pointer on buffer is 0
 * PRL_ERR_BUFFER_OVERRUN - specified buffer size do not enough to fully storing
 *							value (in this case buffer will contain non fully
 *							string value)
 * PRL_ERR_SUCCESS - operation completed successfully
 */
PRL_RESULT CopyByteArrayValue(const QByteArray &value, PRL_VOID_PTR bBuffer,
							  PRL_UINT32_PTR pnBufferLength);

/**
 * Helper method for below method of default device index value calculation
 *
 * @param list of devices
 * @param processing device index value
 * @return sign whether index already presents at devices list
 */
template <typename T>
bool IndexAlreadyPresents(const QList<T *> &lstDevices, const unsigned int nIndex)
{
	foreach(const T *pDevice, lstDevices)
	{
		if (pDevice->getIndex() == nIndex)
			return (true);
	}

	return (false);
}

/**
 * Calculates necessary default device index value
 *
 * @param list of devices
 * @return calculated default device index
 */
template <typename T>
unsigned int CalculateDefaultDevIndex(const QList<T *> &lstDevices)
{
	unsigned int nDefaultIndexValue = 0;
	while (true)
	{
		if (IndexAlreadyPresents(lstDevices, nDefaultIndexValue))
			nDefaultIndexValue++;
		else
			break;

	}
	return (nDefaultIndexValue);
}

/**
 * Internal helper - let to determine current version of API
 */
PRL_UINT32 PrlGetApiVersion();

/**
 * Internal helper - let to determine current API application mode
 */
PRL_APPLICATION_MODE PrlGetApiAppMode();

/**
 * Internal helper - let to determine current API product name
 */
PRL_CONST_STR PrlGetApiProductName();

/**
 * Internal helper - let to determine current API host computer name
 * (for usage at errors text messages)
 */
QString PrlGetHostComputerName();

/**
* Internal helper - let to determine current API trash name
*/
QString PrlGetTrashName();

#endif // __PARALLELS_HANDLE_BASE_H__
