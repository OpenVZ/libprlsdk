/*
 * CVmEventParameterList.h: Definition of the class
 * CVmEventParameterList. This class implements a list
 * representation of VM Event parameter.
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


#ifndef __VM_EVENT_PARAMETER_LIST_H__
#define __VM_EVENT_PARAMETER_LIST_H__


#include "CVmEventParameter.h"


/**
\brief This class implements a list representation of VM Event parameter.
@author sandro@
*/
class CVmEventParameterList : public CVmEventParameter
{
public:

	/** Default class constructor */
	CVmEventParameterList();
	// Initializing constructor
	CVmEventParameterList(PVE::ParamFieldDataType param_type, const QStringList& lstValues, QString param_name = "");

	virtual void CustomInit();

public:
	/**
	 * Overridden method that serializes XML model element into corresponding buffer
	 * @param reference to output buffer object
	 */
	virtual void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes XML model element from corresponding buffer
	 * @param reference to input buffer object
	 */
	virtual void Deserialize(QDataStream &_stream);

private:

	// ***DO NOT INSERT IN THIS CLASS ANY PROPERTIES***
	// ***USE BASE CLASS FOR THIS PURPOSE***

};


#endif // __VM_EVENT_PARAMETER_LIST_H__
