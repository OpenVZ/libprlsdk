/*
 * PrlControlValidity.h
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


#ifndef __VIRTUOZZO_CONTROL_VALIDITY_H__
#define __VIRTUOZZO_CONTROL_VALIDITY_H__

#include <QMap>
#include <QSet>
#include <QMutex>

#include "PrlHandleBase.h"

typedef QSet<PRL_HANDLE> PrlControlValiditySet;
typedef QHash<void *, PrlControlValiditySet> PrlControlValidityMap;

/**
 * This class let to control instances validity.
 * It has global map which binding class instances with data and provides tools that let to control
 * instances validity: marking instances as invalid that bint to correspondent data.
 */
class PrlControlValidity
{
public:
	/**
	 * Class default constructor.
	 * Lets to create instance without immediate registering at global control validity map.
	 * @param handle to register instance
	 */
	PrlControlValidity(PRL_HANDLE h);

	/**
	 * Class constructor.
	 * Registries instance in global control validity map.
	 * @param data with which instance will be bound
	 * @param handle to register instance
	 */
	PrlControlValidity(void *pData, PRL_HANDLE h);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	virtual ~PrlControlValidity();

	/**
	 * Generates internal unique object hash code
	 */
	void GenerateHashCode();

	/**
	 * Returns object unique hash code
	 */
	quint32 GetHashCode() const;

public://Static tools for control instances validity
	/**
	 * Registries class instance in global control validity map
	 * @param pointer to registering instance
	 * @param data with which instance binding
	 */
	static void RegisterInstance(PRL_HANDLE h, void *pData);

	/**
	 * Unregistries class instance from global control validity map
	 * @param pointer to unregistering instance
	 * @param data with which instance bint
	 */
	static void UnregisterInstance(PRL_HANDLE h, void *pData);

	/**
	 * Unregistries instance from control validity map
	 * @param pointer to unregistering instance
	 */
	static void UnregisterInstance(PRL_HANDLE h);

	/**
	 * Marks all instances bound with corresponding data as invalid
	 * @param corresponding data
	 */
	static void MarkAsInvalid(void *pData);

private:
	/** Control instances validity map */
	static PrlControlValidityMap g_ControlValidityMap;
	/** Control instances validity map access synchronization object */
	static QMutex g_ControlValidityMapMutex;

private:
	/**
	 * Overridable method that should marking internal object state as invalid (setting storing data pointer to null
	 * for example)
	 */
	virtual void MarkAsInvalid() = 0;

	/**
	 * Overridable method that should to generate unique hash code for an object
	 * @return generated hash code
	 */
	virtual quint32 ConcreteGenerateHashCode() = 0;

private:
	/** Internal object unique hash code */
	quint32 m_nHashCode;
	/** Handle on instance */
	PRL_HANDLE m_handle;
};

#endif // __VIRTUOZZO_CONTROL_VALIDITY_H__
