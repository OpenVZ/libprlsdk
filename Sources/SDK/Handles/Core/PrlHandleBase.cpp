/*
 * PrlHandleBase.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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


#include "PrlCommon.h"
#include "PrlHandleBase.h"
#include "PrlHandleHandlesList.h"

//#define FORCE_LOGGING_ON
//#define FORCE_LOGGING_LEVEL DBG_DEBUG

#include <prlcommon/Logging/Logging.h>
#include <prlcommon/Std/AtomicOps.h>

#include "Build/Current.ver"

static PRL_UINT32 s_UniqueHandle = 0x0000FACE;
extern PRL_UINT32 g_SdkSequenceNum;

HandlesMap *PrlHandleBase::s_pHandlesMap = new HandlesMap;
QMutex *PrlHandleBase::s_pHandlesMapMutex = new QMutex(QMutex::Recursive);

/**
 * Object's constructor is protected to prevent direct
 * object creation in non-inherited blocks of code.
 */
PrlHandleBase::PrlHandleBase( PRL_HANDLE_TYPE type )
:	m_HandleMutex(QMutex::Recursive), m_HandleType( type )
{
	QMutexLocker _lock(s_pHandlesMapMutex);

	// Object is always being created with reference count = 1
	m_RefCount = 1;
	forever
	{
		ULONG_PTR id = (s_UniqueHandle++ | (g_SdkSequenceNum << 29));
		PRL_HANDLE h = (PRL_HANDLE)id;
		if (PRL_INVALID_HANDLE != h && !s_pHandlesMap->contains(h))
		{
			m_Handle = h;
			break;
		}
	}
	PrlSdkStatusReader r;
	if (r.IsSdkInitialized())
	{
		// Registering handle in the map of handles
		(*s_pHandlesMap)[ m_Handle ] = SmartPtr<PrlHandleBase>(this);
	}
	else
		m_HandleType = (PRL_HANDLE_TYPE)-m_HandleType;

	LOG_MESSAGE( DBG_DEBUG, "OBJ_CREATED: type=%s(%.8X) handle=%.8X this=%.8X",
		PRL_HANDLE_TYPE_TO_STRING(type), type, m_Handle, this );
}


/**
 * Object's destructor is virtual to allow correct inheritance
 */
PrlHandleBase::~PrlHandleBase()
{
	if ( m_RefCount )
	{
		// Deleted object with non-zero reference count ????
	}

	LOG_MESSAGE( DBG_DEBUG, "OBJ_DELETED: type=%s(%.8X) handle=%.8X this=%.8X",
		PRL_HANDLE_TYPE_TO_STRING(GetType()), GetType(), m_Handle, this );
}

/**
 * Increase object's reference count by 1.
 */
PRL_UINT32 PrlHandleBase::AddRef()
{
    PRL_UINT32 uOldVal = AtomicInc( (unsigned int*) &m_RefCount );
	return (uOldVal + 1);
}


/**
 * Decrease object's reference count by 1.
 * As soon as object reference count reaches 0 - it deletes itself.
 */
PRL_UINT32 PrlHandleBase::Release()
{
	// Object with zero reference count
	// are supposed to delete themselves
	PRL_UINT32 uOldVal = AtomicDec( (unsigned int*) &m_RefCount );

	if ( uOldVal == 1 )
	{
		QMutexLocker _lock(s_pHandlesMapMutex);
		// Removing object from the handles list
		if (0 == s_pHandlesMap->remove(m_Handle) && m_HandleType < 0)
			SmartPtr<PrlHandleBase>(this);

		return 0;
	}

	return (uOldVal - 1);
}


/**
 * Returns object's type unique numeric identifier.
 */
PRL_HANDLE_TYPE PrlHandleBase::GetType() const
{
	return (PRL_HANDLE_TYPE)abs(m_HandleType);
}


/**
 * Returns handle associated with the object.
 */
PRL_HANDLE PrlHandleBase::GetHandle() const
{
	return m_Handle;
}

/**
 * Returns handle references count.
 */
PRL_UINT32 PrlHandleBase::GetRefCount()
{
	return AtomicRead((unsigned int *)&m_RefCount);
}

/**
 * Initialize object from XML string
 */
PRL_RESULT PrlHandleBase::fromString(PRL_CONST_STR sXml)
{
	Q_UNUSED(sXml);
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Returns object as XML string
 */
QString PrlHandleBase::toString()
{
	return QString();
}

PRL_RESULT PrlHandleBase::GetPackageId( PRL_UINT64_PTR id )
{
	Q_UNUSED(id)
	return PRL_ERR_UNIMPLEMENTED;
}

/**
 * Validate handle type.
 * It varifies that handles shows to the valid mem and valid type of handle
 * @param handle itself
 * @param type of the handle to verify PTH_ERROR - to verify valid type
 */
PRL_BOOL PRL_RIGHT_HANDLE( PRL_HANDLE handle, PRL_HANDLE_TYPE type /* = PHT_ERROR */ )
{
	PrlHandleBasePtr pHandle = PRL_OBJECT_BY_HANDLE<PrlHandleBase>( handle );

	// Handles should be valid pointers
	if ( !pHandle )
		return PRL_FALSE;

	PRL_HANDLE_TYPE typo = pHandle->GetType();

	// We also need to verify handle type, if we were passed with its value
	if ( type != PHT_ERROR )
	{
		if ( typo != type )
			return PRL_FALSE;
	}
	else
		return PRL_BOOL(typo >= PHT_SERVER && typo <= PHT_LAST);

	// We have done what we were asked for
	return PRL_TRUE;
}


/**
 * Reverse of the method PRL_RIGHT_HANDLE
 * @see PRL_RIGHT_HANDLE
 */
PRL_BOOL PRL_WRONG_HANDLE( PRL_HANDLE handle, PRL_HANDLE_TYPE type /* = PHT_ERROR */ )
{
	return !PRL_RIGHT_HANDLE( handle, type );
}

PRL_BOOL PRL_WRONG_HANDLES_LIST(PRL_HANDLE hHndlList, PRL_HANDLE_TYPE nType)
{
	if (hHndlList == PRL_INVALID_HANDLE)	// as empty list
		return PRL_FALSE;

	PrlHandleHandlesListPtr pList = PRL_OBJECT_BY_HANDLE<PrlHandleHandlesList>(hHndlList);
	QList<PrlHandleBasePtr> list = pList->GetHandlesList();

	for(int i = 0; i < list.size(); i++)
	{
		if (PRL_WRONG_HANDLE(list[i]->GetHandle(), nType))
			return PRL_TRUE;
	}
	return PRL_FALSE;
}

PRL_RESULT CopyStringValue(const QString &_string_value, PRL_STR sStringBuf, PRL_UINT32_PTR pnStringBufLength)
{
	if (!pnStringBufLength)
		return (PRL_ERR_INVALID_ARG);

	size_t required_buff_len = _string_value.toUtf8().size() + 1;

	if (!sStringBuf || *pnStringBufLength == 0)
	{
		*pnStringBufLength = required_buff_len ;
		return (PRL_ERR_SUCCESS);
	}

	QByteArray _string_value_utf8 = _string_value.toUtf8();
	if (*pnStringBufLength < PRL_UINT32(required_buff_len))
	{
		strncpy(sStringBuf, _string_value_utf8.constData(), *pnStringBufLength);
		*pnStringBufLength = required_buff_len ;
		return (PRL_ERR_BUFFER_OVERRUN);
	}
	else
	{
		strcpy(sStringBuf, _string_value_utf8.constData());
		*pnStringBufLength = required_buff_len ;
	}

	return (PRL_ERR_SUCCESS);
}


PRL_RESULT CopyByteArrayValue(const QByteArray &value, PRL_VOID_PTR bBuffer,
							  PRL_UINT32_PTR pnBufferLength)
{
	if (!pnBufferLength)
		return (PRL_ERR_INVALID_ARG);
	const PRL_UINT32 size = PRL_UINT32(value.size());

	if ( ! bBuffer || *pnBufferLength == 0)
	{
		*pnBufferLength = size;
		return (PRL_ERR_SUCCESS);
	}

	if (PRL_WRONG_PTR(bBuffer)) return (PRL_ERR_INVALID_ARG);

	if (size > *pnBufferLength)
	{
		memcpy(bBuffer, value.constData(), *pnBufferLength);
		*pnBufferLength = size;
		return (PRL_ERR_BUFFER_OVERRUN);
	}

	memcpy(bBuffer, value.constData(), size);
	*pnBufferLength = size;

	return (PRL_ERR_SUCCESS);
}
