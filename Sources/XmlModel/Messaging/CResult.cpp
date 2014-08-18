/*
 * CResult.cpp: Implementation of the class CResult. This class
 * implements generic resultset.
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


#include "CResult.h"
#include "CVmEvent.h"
#include "Libraries/Logging/Logging.h"

// By adding this interface we enable allocations tracing in the module
#include "Interfaces/Debug.h"


/**
 * @brief Standard class constructor.
 * @param name
 * @return
 */
CResult::CResult()
:	QEvent((QEvent::Type)PVE::CResultType),
	m_pBinaryContent(NULL)
{
    m_pError = NULL;
    cleanupClassProperties();

} // CResult::CResult()

/**
 * @brief Copy constructor.
 * @param other
 * @return
 */
CResult::CResult(CResult* other)
:	QEvent((QEvent::Type)PVE::CResultType),
	m_pBinaryContent(NULL)
{
    m_pError = NULL;
    cleanupClassProperties();

    m_strRequestId = other->getRequestId();
    m_strExecutiveServer = other->getExecutiveServer();
    m_uiResultObjectType = other->getResultObjectType();
    m_uiReturnCode = other->getReturnCode();
    m_uiOpCode = other->getOpCode();
	m_uiPackageId = other->getPackageId();

    if( m_pError )
        delete m_pError;
    m_pError = new CVmEvent( other->GetError()->toString() );

    m_hashResultSet = other->m_hashResultSet;

	if (other->m_pBinaryContent)
		m_pBinaryContent = new CVmEvent(other->m_pBinaryContent);
} // CResult::CResult()


/**
 * @brief Initializing constructor.
 * @param request_id
 * @param ret_code
 * @param obj_type
 * @param p_error
 * @param exec_server
 * @return
 */
CResult::CResult(QString request_id, PVE::IDispatcherCommands op_code, PRL_RESULT ret_code,
    PVE::ResultType obj_type, CVmEvent* p_error, QString exec_server)
 : QEvent(QEvent::User), m_pBinaryContent(NULL)
{
    m_pError = NULL;
    cleanupClassProperties();

    m_strRequestId = request_id;
    m_strExecutiveServer = exec_server;

    m_uiOpCode = op_code;

    if( isCorrectResultObjectType( obj_type ) )
        m_uiResultObjectType = obj_type;
    else
        m_uiResultObjectType = PVE::GenericResult;

    if( isCorrectReturnCode( ret_code ) )
        m_uiReturnCode = ret_code;
    else
        m_uiReturnCode = PRL_ERR_SUCCESS;

    if( m_pError )
        delete m_pError;
    m_pError = p_error;

} // CResult::CResult()


/**
 * @brief Class destructor.
 * @return
 */
CResult::~CResult()
{
    if( m_pError )
        delete m_pError;
    m_pError = NULL;

    m_hashResultSet.clear();

	delete m_pBinaryContent;
} // CResult::~CResult()

/**
 * @brief Standard copy constructor.
 * @param _obj - copying object reference
 * @return
 */
CResult::CResult(const CResult &_obj) : QEvent(_obj),
   m_strRequestId(_obj.m_strRequestId),
   m_uiOpCode(_obj.m_uiOpCode),
   m_strExecutiveServer(_obj.m_strExecutiveServer),
   m_uiResultObjectType(_obj.m_uiResultObjectType),
   m_uiReturnCode(_obj.m_uiReturnCode),
   m_pError(NULL), m_pBinaryContent(NULL),
   m_uiPackageId(_obj.m_uiPackageId)
{
    if (_obj.m_pError)
        m_pError = new CVmEvent(_obj.m_pError->toString());
    m_hashResultSet = _obj.m_hashResultSet;
	if (_obj.m_pBinaryContent)
		m_pBinaryContent = new CVmEvent(_obj.m_pBinaryContent);
}

/**
 * @brief Standard assign operator.
 * @param _obj - copying object reference
 * @return reference to the current object
 */
CResult &CResult::operator=(const CResult &_obj) {
    if (this != &_obj) {//Preventing copying object to itself
        m_strRequestId = _obj.m_strRequestId;
        m_strExecutiveServer = _obj.m_strExecutiveServer;
        m_uiResultObjectType = _obj.m_uiResultObjectType;
        m_uiReturnCode = _obj.m_uiReturnCode;
        m_uiOpCode = _obj.m_uiOpCode;
        m_uiPackageId = _obj.m_uiPackageId;

        if( m_pError ) {
            delete m_pError;
            m_pError = NULL;
        }
        if (_obj.m_pError)
            m_pError = new CVmEvent(_obj.m_pError->toString());
        m_hashResultSet = _obj.m_hashResultSet;

        if( m_pBinaryContent ) {
            delete m_pBinaryContent;
            m_pBinaryContent = NULL;
        }
        if (_obj.m_pBinaryContent)
            m_pBinaryContent = new CVmEvent(_obj.m_pBinaryContent);
    }
    return (*this);
}

/**
 * @brief Add item to result set.
 * @param item
 */
void CResult::addResultSetItem(int key, QString item)
{
    m_hashResultSet.insert(key, item);

} // CResult::addResultSetItem()

void CResult::addResultSetItem(int key, const char* item)
{
    addResultSetItem(key, UTF8_2QSTR(item));
}

QString CResult::getResultItemByKey( int key )
{
    if ( m_hashResultSet.contains( key ) )
        return m_hashResultSet.value( key );

    LOG_MESSAGE( DBG_WARNING, "Can't get result by key %d", key );
    return "";
}


/**
 * @brief Add param token to result set.
 * @param item
 */
void CResult::addParamToken(QString item)
{
    for (int key = PVE::MultipleArgStart;; key++)
    {
        if (!m_hashResultSet.contains(key))
        {
            addResultSetItem(key, item);
            break;
        }
    }

} // CResult::addParamToken()

void CResult::addParamToken(const char* item)
{
    addParamToken(UTF8_2QSTR(item));
}

/**
 * @brief Get count of params token.
 * @param item
 */
int CResult::GetParamsCount() const
{
    QList <int> all_keys = m_hashResultSet.keys();

    qSort(all_keys);

    if ( all_keys.isEmpty() )
        return 0;

    return all_keys.last() - PVE::MultipleArgStart + 1;

} // CResult::GetParamsCount()


/**
 * @brief Get param token by index.
 * @param item
 */
QString CResult::GetParamToken(int nIndex) const
{
    return m_hashResultSet[PVE::MultipleArgStart + nIndex];

} // CResult::GetParamToken()


/**
 * @brief Set request ID.
 * @param value
 */
void CResult::setRequestId(QString value)
{
    m_strRequestId = value;

} // CResult::setRequestId()


/**
 * @brief Get request ID.
 * @return
 */
QString CResult::getRequestId() const
{
    return m_strRequestId;

} // CResult::getRequestId()


/**
 * @brief Set server.
 * @param value
 */
void CResult::setExecutiveServer(QString value)
{
    m_strExecutiveServer = value;

} // CResult::setExecutiveServer()


/**
 * @brief Get server.
 * @return
 */
QString CResult::getExecutiveServer() const
{
    return m_strExecutiveServer;

} // CResult::getExecutiveServer()


/**
 * @brief Set result type.
 * @param value
 */
void CResult::setResultObjectType(PVE::ResultType value)
{
    if( isCorrectResultObjectType( value ) )
        m_uiResultObjectType = value;
    else
        m_uiResultObjectType = PVE::GenericResult;

} // CResult::setResultObjectType()


/**
 * @brief Get result type.
 * @return
 */
PVE::ResultType CResult::getResultObjectType() const
{
    return m_uiResultObjectType;

} // CResult::getResultObjectType()


/**
 * @brief Set return code.
 * @param value
 */
void CResult::setReturnCode(PRL_RESULT value)
{
    if( isCorrectReturnCode( value ) )
        m_uiReturnCode = value;
    else
        m_uiReturnCode = PRL_ERR_SUCCESS;

} // CResult::setReturnCode()


/**
 * @brief Get return code.
 * @return
 */
PRL_RESULT CResult::getReturnCode() const
{
    return m_uiReturnCode;

} // CResult::getReturnCode()

/**
 * @brief Get vm event.
 * @return
 */
CVmEvent* CResult::GetError () const
{
    return m_pError;
};

/**
 * @brief Get vm event.
 * @return
 */
void CResult::setError ( CVmEvent* err )
{
    delete m_pError;
    m_pError = err;
};


/**
 * @brief Set operation code.
 * @param value
 */
void CResult::setOpCode(PVE::IDispatcherCommands value)
{
    m_uiOpCode = value;
} // CResult::setOpCode()


/**
 * @brief Get operation code.
 * @return
 */
PVE::IDispatcherCommands CResult::getOpCode() const
{
    return m_uiOpCode;

} // CResult::getOpCode()


/**
 * @brief Clean up class-specific properties.
 */
void CResult::cleanupClassProperties()
{
    m_strRequestId = "";
    m_strExecutiveServer = "";
    m_uiResultObjectType = PVE::GenericResult;
    m_uiReturnCode = PRL_ERR_SUCCESS;
    m_uiOpCode = PVE::DspIllegalCommand;
    m_uiPackageId = 0;

    if( m_pError )
        delete m_pError;
    m_pError = new CVmEvent();

    m_hashResultSet.clear();

} // CResult::cleanupClassProperties()


/**
 * @brief Check if option is set correctly.
 * @param value
 * @return
 */
bool CResult::isCorrectResultObjectType(PVE::ResultType value)
{
    switch( value )
    {
    case PVE::GenericResult:
    case PVE::ServerGeneratedEvent:
        break;
    default:
        return false;
    }

    return true;

} // CResult::isCorrectResultObjectType()


/**
 * @brief Check if option is set correctly.
 * @param value
 * @return
 */
bool CResult::isCorrectReturnCode(PRL_RESULT value)
{
    Q_UNUSED( value );

    /*  if( (value < PRL_ERR_SUCCESS) || (value >= PRL_ERR_RETURN_CODE_RANG_EEND) )
        return false;*/

    return true;

} // CResult::isCorrectReturnCode()

void CResult::setBinaryContent(CVmEvent *pBinaryContent)
{
	delete m_pBinaryContent;
	m_pBinaryContent = pBinaryContent;
}

CVmEvent *CResult::getBinaryContent()
{
	return (m_pBinaryContent);
}

void CResult::setPackageId(PRL_UINT64 id)
{
	m_uiPackageId = id;
}

PRL_UINT64 CResult::getPackageId() const
{
	return m_uiPackageId;
}
