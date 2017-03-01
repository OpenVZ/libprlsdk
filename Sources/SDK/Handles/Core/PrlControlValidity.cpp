/*
 * PrlControlValidity.cpp
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


#include "PrlControlValidity.h"
#include <prlcommon/Std/PrlAssert.h>

PrlControlValidityMap PrlControlValidity::g_ControlValidityMap;
QMutex PrlControlValidity::g_ControlValidityMapMutex(QMutex::Recursive);

PrlControlValidity::PrlControlValidity(PRL_HANDLE h)
: m_nHashCode(0), m_handle(h)
{}

PrlControlValidity::PrlControlValidity(void *pData, PRL_HANDLE h)
: m_nHashCode(0), m_handle(h)
{
	RegisterInstance(m_handle, pData);
}

PrlControlValidity::~PrlControlValidity()
{
	UnregisterInstance(m_handle);
}

void PrlControlValidity::RegisterInstance(PRL_HANDLE h, void *pData)
{
	QMutexLocker _lock(&g_ControlValidityMapMutex);
	g_ControlValidityMap[pData].insert(h);
}

void PrlControlValidity::UnregisterInstance(PRL_HANDLE h, void *pData)
{
	QMutexLocker _lock(&g_ControlValidityMapMutex);
	PrlControlValidityMap::iterator _data_it = g_ControlValidityMap.find(pData);
	if (_data_it != g_ControlValidityMap.end())
	{
		_data_it.value().remove(h);
		if (!_data_it.value().size())
			g_ControlValidityMap.erase(_data_it);
	}
}

void PrlControlValidity::UnregisterInstance(PRL_HANDLE h)
{
	QMutexLocker _lock(&g_ControlValidityMapMutex);
	PrlControlValidityMap::iterator _data_it = g_ControlValidityMap.begin();
	while (_data_it != g_ControlValidityMap.end())
	{
		_data_it.value().remove(h);
		if (_data_it.value().isEmpty())
			_data_it = g_ControlValidityMap.erase(_data_it);
		else
			++_data_it;
	}
}

void PrlControlValidity::MarkAsInvalid(void *pData)
{
	PrlControlValiditySet v;
	{
		QMutexLocker _lock(&g_ControlValidityMapMutex);
		PrlControlValidityMap::iterator _data_it = g_ControlValidityMap.find(pData);
		if (_data_it != g_ControlValidityMap.end())
		{
			v = _data_it.value();
			//Clear storage now to prevent possibility of further mark as invalid attemptions on this data pointer
			g_ControlValidityMap.erase(_data_it);
		}
	}
	foreach (PRL_HANDLE h, v)
	{
		PrlHandleBasePtr pObj = PRL_OBJECT_BY_HANDLE<PrlHandleBase>(h);
		if ( pObj )
		{
			PrlControlValidity *pInstance = dynamic_cast<PrlControlValidity *>( pObj.getHandle() );
			PRL_ASSERT(pInstance);
			pInstance->GenerateHashCode();
			pInstance->MarkAsInvalid();
		}
	}
}

quint32 PrlControlValidity::GetHashCode() const
{
	return (m_nHashCode);
}

void PrlControlValidity::GenerateHashCode()
{
	m_nHashCode = ConcreteGenerateHashCode();
}

