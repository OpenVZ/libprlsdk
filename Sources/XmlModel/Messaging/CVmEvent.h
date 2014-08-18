/*
 * CVmEvent.h: Definition of the class CVmEvent. This class
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


#ifndef CVMEVENT_H
#define CVMEVENT_H


#include "CVmEventBase.h"
#include "CVmEventParameter.h"
#include <QObjectList>


/**
 * @brief This class implements generic VM Event
 * @author SergeyM
 */
class CVmEvent : public CVmEventBase, public QEvent
{

public:
	// List of event parameters
	QList<CVmEventParameter* >& m_lstEventParameters;

	virtual void cleanupClassProperties();

public:

	// Standard class constructor
	CVmEvent();
	// Copy constructor
	CVmEvent(CVmEvent *other);
	// Initializing constructor
	CVmEvent(PRL_EVENT_TYPE event_type,
			 QString issuer_id,
			 PRL_EVENT_ISSUER_TYPE issuer_type = PIE_VIRTUAL_MACHINE,
			 PRL_RESULT event_code = PET_VM_INF_UNINITIALIZED_EVENT_CODE,
			 PVE::VmEventRespOption need_response = PVE::EventRespNotRequired,
			 QString event_source = "",
			 PVE::VmEventLevel event_level = PVE::EventLevel0);
	// Initializing constructor
	CVmEvent(QString source_string);

	// return m_uiRcInit ( initialization RC )
	PRL_RESULT getInternalRcInit() { return m_uiRcInit; }

	// Add VM saved state to the list
	void addEventParameter(CVmEventParameter* item);
	// Get event parameter
	CVmEventParameter* getEventParameter(QString param_name) const;
	// Get named event parameter by index of list with same name
	CVmEventParameter* getEventParameter(QString param_name, int name_index ) const;
	// Get count of params token
	int GetParamsCount(QString param_name) const;

public:
	/**
	 * Overridden method that serializes XML model element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes XML model element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:

};


#endif
