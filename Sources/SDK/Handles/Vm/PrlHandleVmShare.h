/*
 * PrlHandleVmShare.h
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


#ifndef __PARALLELS_HANDLE_VM_SHARE_H__
#define __PARALLELS_HANDLE_VM_SHARE_H__


#include "PrlHandleVm.h"
#include "PrlControlValidity.h"

/**
 * VM shared folder handle object.
 */
class PrlHandleVmShare : public PrlHandleBase, public PrlControlValidity
{

public:
	/**
	 * Class constructor.
	 * Performs basic initialization of the object state.
	 * @param parent VM object pointer
	 */
	PrlHandleVmShare(const PrlHandleVmPtr &pVm);

	/**
	 * Class constructor.
	 * Initializes shared folder settings from specified object.
	 * @param parent VM object pointer
	 */
	PrlHandleVmShare(const PrlHandleVmPtr &pVm, class CVmSharedFolder *pSharedFolder);

	/**
	 * Class destructor
	 * Performs basic deinitialization of the object state.
	 */
	~PrlHandleVmShare();

public://API helpers
	/**
	 * Removes VM shared folder from parent VM shares list.
	 *
	 * @return PRL_RESULT
	 */
	PRL_RESULT Remove();

	/**
	 * Returns VM shared folder name.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully shared folder name)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetName(PRL_STR sVmShareName, PRL_UINT32_PTR pnVmShareNameBufLength);


	/**
	 * Sets new VM shared folder name.
	 * @param [in] new VM shared folder name string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetName(PRL_CONST_STR sNewVmShareName);


	/**
	 * Returns VM shared folder path.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully shared folder path)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetPath(PRL_STR sVmSharePath, PRL_UINT32_PTR pnVmSharePathBufLength);


	/**
	 * Sets new VM shared folder path.
	 * @param [in] new VM shared folder path string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetPath(PRL_CONST_STR sNewVmSharePath);


	/**
	 * Returns VM shared folder description.
	 * @param [out] buffer for storing result
	 * @param [out] buffer size (on in - how much symbols can be placed into buffer;
	 * on out - how much were placed actually)
	 * @return PRL_RESULT. Possible values:
	 * * PRL_ERR_INVALID_ARG - incorrect passed buffer size (NULL size)
	 * * PRL_ERR_BUFFER_OVERRUN - not enough buffer to completely storing result (in this case buffer will be contain
	 * * non fully shared folder description)
	 * * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT GetDescription(PRL_STR sVmShareDescription, PRL_UINT32_PTR pnVmShareDescriptionBufLength);


	/**
	 * Sets new VM shared folder description.
	 * @param [in] new VM shared folder description string
	 * @return PRL_RESULT. Possible values:
	 * PRL_ERR_SUCCESS - operation completed successfully
	 */
	PRL_RESULT SetDescription(PRL_CONST_STR sNewVmShareDescription);

	/**
	 * Checks whether VM shared folder enabled.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsEnabled(PRL_BOOL_PTR pbEnabled);

	/**
	 * Sets VM shared folder enabled sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetEnabled(PRL_BOOL bEnabled);


	/**
	 * Checks whether VM shared folder read only.
	 *
	 * @param buffer for storing result
	 * @return PRL_RESULT
	 */
	PRL_RESULT IsReadOnly(PRL_BOOL_PTR pbReadOnly);

	/**
	 * Sets VM shared folder read only sign.
	 *
	 * @param assigning value
	 * @return PRL_RESULT
	 */
	PRL_RESULT SetReadOnly(PRL_BOOL bReadOnly);


private:
	/// Storing VM shared folder XML representation.
	class CVmSharedFolder *m_pVmShare;
	/// Pointer to the parent VM object
	PrlHandleVmPtr m_pVm;

private:
	/**
	 * Overridden method that marks device object as invalid (sets to NULL storing share pointer)
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

typedef PrlHandleSmartPtr<PrlHandleVmShare> PrlHandleVmSharePtr;

#endif // __PARALLELS_HANDLE_VM_SHARE_H__
