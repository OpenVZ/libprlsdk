/*
 * CVmEventParameterList.cpp: Implementation of the class
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


#include "CVmEventParameterList.h"
#include <prlcommon/Interfaces/ParallelsDomModel.h>


CVmEventParameterList::CVmEventParameterList()
: CVmEventParameter()
{
	CustomInit();
}

CVmEventParameterList::CVmEventParameterList(PVE::ParamFieldDataType param_type, const QStringList& lstValues, QString param_name)
: CVmEventParameter(param_type, QString(), param_name)
{
	CustomInit();

	if (!m_lstValues.isEmpty())
	{
		m_lstValues[0]->setListItem(lstValues);
	}
}

void CVmEventParameterList::CustomInit()
{
	setIsList(true);
	m_uiEventParameterClassType = CVmEventParameter::List;
}

void CVmEventParameterList::Serialize(QDataStream &_stream)
{
	CVmEventParameter::Serialize(_stream);
	QStringList lstValuesList = getValuesList();
	int nListSize = lstValuesList.size();
	PrlOpaqueSerializer(nListSize).Serialize(_stream);
	foreach(QString sValue, lstValuesList)
		CPrlStringDataSerializer(sValue).Serialize(_stream);
}

void CVmEventParameterList::Deserialize(QDataStream &_stream)
{
	CVmEventParameter::Deserialize(_stream);

	CustomInit();

	int nListSize = 0;
	PrlOpaqueSerializer(nListSize).Deserialize(_stream);

	QStringList lstValues;
	for(int i = 0; i < nListSize; ++i)
	{
		QString sValue;
		CPrlStringDataSerializer(sValue).Deserialize(_stream);
		lstValues += sValue;
	}

	if (!m_lstValues.isEmpty())
	{
		m_lstValues[0]->setListItem(lstValues);
	}
}
