/*
 * CVmEvent.cpp: Implementation of the class CVmEvent. This class
 * implements generic VM Event.
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


#include <QTextStream>
#include "Libraries/PrlUuid/Uuid.h"
#include "CVmEvent.h"
#include "CVmEventParameter.h"
#include "CVmEventParameterList.h"
#include "CVmBinaryEventParameter.h"

#include "Libraries/Logging/Logging.h"

// By adding this interface we enable allocations tracing in the module
#include "Interfaces/Debug.h"


/**
 * @brief Standard class constructor.
 * @param parent
 * @return
 */
CVmEvent::CVmEvent()
: CVmEventBase(), QEvent((QEvent::Type)PVE::CVmEventType),
  m_lstEventParameters(m_lstBaseEventParameters[0]->m_lstEventParameter)
{
}


/**
 * @brief Copy constructor
 * @param other
 * @param parent
 * @return
 */
CVmEvent::CVmEvent(CVmEvent *other)
 : CVmEventBase(), QEvent((QEvent::Type)PVE::CVmEventType),
  m_lstEventParameters(m_lstBaseEventParameters[0]->m_lstEventParameter)
{
	if (other)
	{
		fromString(other->toString());

		m_uiRcInit = other->m_uiRcInit;
		m_szErrMsg = other->m_szErrMsg;
		m_iErrLine = other->m_iErrLine;
		m_iErrCol = other->m_iErrCol;

		for(int i = 0; i < m_lstEventParameters.size() && i < other->m_lstEventParameters.size(); i++)
		{
			CVmEventParameter* pParam = other->m_lstEventParameters[i];
			if (pParam)
			{
				if (pParam->getEventParameterClassType() == CVmEventParameter::List)
				{
					m_lstEventParameters[i]->setEventParameterClassType(CVmEventParameter::List);
				}
				else if (pParam->getEventParameterClassType() == CVmEventParameter::Binary)
				{
					int nItemId = m_lstEventParameters[i]->getItemId();
					delete m_lstEventParameters[i];

					CVmBinaryEventParameter *pBinaryParam = static_cast<CVmBinaryEventParameter *>(pParam);
					m_lstEventParameters[i] = new CVmBinaryEventParameter(*pBinaryParam);
					m_lstEventParameters[i]->setItemId(nItemId);
				}
			}
		}
	}
}


/**
 * @brief Initializing constructor.
 * @param event_type
 * @param event_code
 * @param need_response
 * @param event_source
 * @param parent
 * @return
 */
CVmEvent::CVmEvent(
	PRL_EVENT_TYPE event_type,
	QString issuer_id,
	PRL_EVENT_ISSUER_TYPE issuer_type,
	PRL_RESULT event_code,
	PVE::VmEventRespOption need_response,
	QString event_source,
	PVE::VmEventLevel event_level
	)
 : CVmEventBase(), QEvent((QEvent::Type)PVE::CVmEventType),
  m_lstEventParameters(m_lstBaseEventParameters[0]->m_lstEventParameter)
{

//	if( isCorrectEventType( event_type ) )
		setEventType(event_type);
//	else
//		m_uiEventType = PET_VM_INF_UNINITIALIZED_EVENT_CODE;

//	if( isCorrectEventLevel( event_level ) )
		setEventLevel(event_level);
//	else
//		m_uiEventLevel = PVE::EventLevel0;

//	if( isCorrectEventCode( event_code ) )
		setEventCode(event_code);
//	else
//		m_ulEventCode = PET_VM_INF_UNINITIALIZED_EVENT_CODE;

//	if( isCorrectEventRespOption( need_response ) )
		setRespRequired(need_response);
//	else
//		m_uiEventRespOption = PVE::EventRespNotRequired;

//	if( isCorrectEventIssuerType( issuer_type ) )
		setEventIssuerType(issuer_type);
//	else
//		m_uiEventIssuerType = PIE_VIRTUAL_MACHINE;

	setEventIssuerId(issuer_id);
	setEventSource(event_source);

}


/**
 * @brief Initializing constructor.
 * @param event_type
 * @param event_code
 * @param need_response
 * @param event_source
 * @param parent
 * @return
 */
CVmEvent::CVmEvent(QString source_string)
 : QEvent((QEvent::Type)PVE::CVmEventType),
  m_lstEventParameters(m_lstBaseEventParameters[0]->m_lstEventParameter)
{
	fromString(source_string);
}

void CVmEvent::cleanupClassProperties()
{
	m_lstBaseEventParameters[0]->ClearLists();
	m_lstBaseEventParameters[0]->InitLists();
	m_lstBaseEventParameters[0]->setDefaults();

	setDefaults();
}

/**
 * @brief Add event parameter to the list.
 * @param item
 */
void CVmEvent::addEventParameter(CVmEventParameter* item)
{
	m_lstEventParameters.append( item );

}


/**
 * @brief Get event parameter from the list.
 * @param param_name
 */
CVmEventParameter* CVmEvent::getEventParameter(QString param_name) const
{
	QList<CVmEventParameter*>::const_iterator i;

	for (i = m_lstEventParameters.begin(); i != m_lstEventParameters.end(); ++i)
	{
		if ( (*i)->getParamName() == param_name )
			return *i;
	}

	return NULL;

}

// Get named event parameter by index of list with same name
CVmEventParameter* CVmEvent::getEventParameter(QString param_name, int name_index ) const
{
	QList<CVmEventParameter*>::const_iterator i;
	int currIndexOfName = -1;
	for (i = m_lstEventParameters.begin(); i != m_lstEventParameters.end(); ++i)
	{
		if ( (*i)->getParamName() == param_name && ++currIndexOfName == name_index)
			return *i;
	}

	return NULL;
}

/**
 * @brief Get count of params token.
 * @param param_name
 */
int CVmEvent::GetParamsCount(QString param_name) const
{
	QList<CVmEventParameter*>::const_iterator i;
	int nCount = 0;

	for (i = m_lstEventParameters.begin(); i != m_lstEventParameters.end(); ++i)
	{
		if ( (*i)->getParamName() == param_name )
			nCount++;
	}

	return nCount;

}

void CVmEvent::Serialize(QDataStream &_stream)
{
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventType)).Serialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventIssuerType)).Serialize(_stream);
	CPrlStringDataSerializer(m_qsEventIssuerId).Serialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventLevel)).Serialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventCode)).Serialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventNeedResponse)).Serialize(_stream);
	CPrlStringDataSerializer(m_qsEventSource).Serialize(_stream);
	CPrlStringDataSerializer(m_qsEventInitialRequestId).Serialize(_stream);
	int nListSize = m_lstEventParameters.size();
	PrlOpaqueSerializer(nListSize).Serialize(_stream);
	foreach(CVmEventParameter *pParam, m_lstEventParameters)
		pParam->Serialize(_stream);
}

void CVmEvent::Deserialize(QDataStream &_stream)
{
	cleanupClassProperties();
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventType)).Deserialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventIssuerType)).Deserialize(_stream);
	CPrlStringDataSerializer(m_qsEventIssuerId).Deserialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventLevel)).Deserialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventCode)).Deserialize(_stream);
	PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(m_ctEventNeedResponse)).Deserialize(_stream);
	CPrlStringDataSerializer(m_qsEventSource).Deserialize(_stream);
	CPrlStringDataSerializer(m_qsEventInitialRequestId).Deserialize(_stream);
	int nListSize = 0;
	PrlOpaqueSerializer(nListSize).Deserialize(_stream);
	for(int i = 0; i < nListSize; ++i)
	{
		CVmEventParameter::EventParameterClassType nClassType;
		PrlOpaqueSerializer((quint32 &)PrlOpaqueTypeConverter(nClassType)).Deserialize(_stream);
		CVmEventParameter *pParam = NULL;
		if (nClassType == CVmEventParameter::BaseType)
			pParam = new CVmEventParameter;
		else if (nClassType == CVmEventParameter::List)
			pParam = new CVmEventParameterList;
		else if (nClassType == CVmEventParameter::Binary)
			pParam = new CVmBinaryEventParameter;
		if (pParam)
		{
			pParam->Deserialize(_stream);
			m_lstEventParameters.append(pParam);
		}
		else
			WRITE_TRACE(DBG_FATAL, "Unknown event parameter class type: %u", nClassType);
	}
}
