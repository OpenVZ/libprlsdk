/*
 * PrlHandleVmBootDev.h
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


#ifndef __PARALLELS_HANDLE_VM_BOOT_DEV_H__
#define __PARALLELS_HANDLE_VM_BOOT_DEV_H__


#include "PrlHandleVm.h"
#include "PrlControlValidity.h"

/**
 * VM screen resolution handle object.
 */
class PrlHandleVmBootDev : public PrlHandleBase, public PrlControlValidity
{
public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * @param parent VM object pointer
	 */
	PrlHandleVmBootDev(const PrlHandleVmPtr &pVm);

	/**
	 * Class constructor.
	 * Initializes boot device settings from specified object.
	 * @param parent VM object pointer
	 */
	PrlHandleVmBootDev(const PrlHandleVmPtr &pVm, class CVmStartupOptions::CVmBootDevice *pBootDev);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleVmBootDev();

public://API helpers
	/**
	 * Removes VM boot device from parent VM boot devices list.
	 *
	 * @return PRL_RESULT
	 */
	PRL_RESULT Remove();

	/**
	 * Checks whether VM boot device using at boot sequence.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsInUse(PRL_BOOL_PTR pbInUse);

	/**
	 * Sets VM boot device using at boot sequence sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetInUse(PRL_BOOL bInUse);


	/**
	 * Returns VM boot device index.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetIndex(PRL_UINT32_PTR pnDevIndex);

	/**
	 * Sets VM boot device index.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetIndex(PRL_UINT32 nDevIndex);


	/**
	 * Returns VM boot device sequence index.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetSequenceIndex(PRL_UINT32_PTR pnSequenceIndex);

	/**
	 * Sets VM boot device sequence index.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetSequenceIndex(PRL_UINT32 nSequenceIndex);


	/**
	 * Returns VM boot device type.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT GetBootDevType(PRL_DEVICE_TYPE_PTR pnType);

	/**
	 * Sets VM boot device type.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetType(PRL_DEVICE_TYPE nType);


private:
	/// Storing VM boot device XML representation.
	class CVmStartupOptions::CVmBootDevice *m_pVmBootDev;
	/// Pointer to the parent VM object
	PrlHandleVmPtr m_pVm;

private:
	/**
	 * Overridden method that marks device object as invalid (sets to NULL storing screen resolution pointer)
	 */
	void MarkAsInvalid();

	/**
	 * Overridden method that should to generate unique hash code for an object
	 * @return generated hash code
	 */
	quint32 ConcreteGenerateHashCode();

private://Internal class helpers
	/**
	 * Tries to restore internal wrapping XML model element
	 */
	void TryToRestoreObjectElement();
};

typedef PrlHandleSmartPtr<PrlHandleVmBootDev> PrlHandleVmBootDevPtr;

#endif // __PARALLELS_HANDLE_VM_BOOT_DEV_H__
