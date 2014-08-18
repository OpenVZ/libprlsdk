/*
 * CXmlModelHelper.h: Set of simple methods which help to work with
 * XML model elements.
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


#ifndef CXmlModelHelper_H
#define CXmlModelHelper_H

class CVmDevice;
class CVmConfiguration;

/**
 * Checks whether two elements equal
 */
namespace CXmlModelHelper {
template <typename T>
bool IsEqual( T *elem1, T *elem2)
{
	if ( elem1 && elem2 )
	{
		if ( elem1 == elem2 )
			return true;
		return *elem1 == *elem2;
	}
	else
		return elem1 == elem2;
}

/**
 * Virtual device specific helper - checks whether just Connected
 * property different for two devices.
 */
template <typename T>
bool JustConnectedPropWasChanged( T *elem1, T *elem2 )
{
	if ( IsEqual( elem1, elem2 ) )
		return false;
	if ( elem1 && elem2 )
	{
		T _e1, _e2;
		_e1.fromString( elem1->toString() );
		_e2.fromString( elem2->toString() );
		_e1.setConnected( _e2.getConnected() );
		return IsEqual( &_e1, &_e2 );
	}
	return false;
}

/**
 * Checks whether specified element presents at list
 */
template <typename T>
T *IsElemInList( T *elem, const QList<T *> &lst )
{
	foreach(T *_item, lst)
	{
		if ( _item->getItemId() == elem->getItemId() )
			return _item;
	}
	return NULL;
}

/**
 * Simple helper which quadrates two lists of XML model elements
 */
template<class T>
void QuadrateLists( QList<T*> &oldList, QList<T*> &newList )
{
	//First cleanup elements which were removed
	typename QList<T*>::iterator _it = oldList.begin();
	while ( _it != oldList.end() )
	{
		if ( ! CXmlModelHelper::IsElemInList( *_it, newList ) )
		{
			//Just remove element from list - no actions required
			delete *_it;
			_it = oldList.erase( _it );
		}
		else
			++_it;
	}

	//Now add new elements
	foreach(T *elem, newList)
	{
		if ( ! CXmlModelHelper::IsElemInList( elem, oldList ) )
		{
			T *pNewElem = new T;
			pNewElem->fromString( elem->toString() );
			oldList.append( pNewElem );
		}
	}
}

bool indexOrderLessThan(const CVmDevice* pDev1, const CVmDevice* pDev2);

/**
 * Get unused device index
 */
template <class T>
unsigned int GetUnusedDeviceIndex(QList<T* >& lstDevs)
{
	unsigned int nIndex = 0;
	qSort(lstDevs.begin(), lstDevs.end(), indexOrderLessThan);
	foreach(T* pDev, lstDevs)
	{
		if (pDev->getIndex() == nIndex)
			++nIndex;
		else
			break;
	}
	return nIndex;
}

/**
 * Sort device list by index
 */

template <class T>
void SortDeviceListByIndex(QList<T* >& lstDevs)
{
	qSort(lstDevs.begin(), lstDevs.end(), indexOrderLessThan);
}

/**
 * Find device with a given index.
 */
template <typename T>
T* GetDeviceByIndex(const QList<T *>& hay_, unsigned int needle_)
{
        for (uint i = 0, e = hay_.size(); i < e; ++i)
        {
                T* d = hay_.at(i);
                if (d && d->getIndex() == needle_)
                        return d;
        }
        return NULL;
}

PRL_CLUSTERED_DEVICE_SUBTYPE syncScsiDevicesSubType(CVmConfiguration* pVmConfigNew,
	const CVmConfiguration* pVmConfigOld);

PRL_CLUSTERED_DEVICE_SUBTYPE getDefaultScsiSubTypeByOsVersion(unsigned int nOsVersion);

void printConfigDiff( const CVmConfiguration&,
					  const CVmConfiguration&,
					  uint ,
					  const QString& qsPrefix = "diff");

}//namespace CXmlModelHelper

#endif//CXmlModelHelper_H
