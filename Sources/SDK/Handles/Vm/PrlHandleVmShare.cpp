/*
 * PrlHandleVmShare.cpp
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
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


#include "PrlHandleVmShare.h"
#include <prlxmlmodel/VmConfig/CVmHostSharing.h>
#include <prlxmlmodel/VmConfig/CVmSharedFolder.h>

#ifdef ENABLE_MALLOC_DEBUG
    // By adding this interface we enable allocations tracing in the module
    #include "Interfaces/Debug.h"
#else
    // We're not allowed to throw exceptions from the library -
    // so we need to prevent operator ::new from doing this
    #include <new>
    using std::nothrow;
    #define new new(nothrow)
#endif

#define CHECK_SHARE_OBJECT\
		if (!m_pVmShare)\
			TryToRestoreObjectElement();\
		if (!m_pVmShare)\
				return (PRL_ERR_OBJECT_WAS_REMOVED);

PrlHandleVmShare::PrlHandleVmShare(const PrlHandleVmPtr &pVm)
: PrlHandleBase(PHT_SHARE), PrlControlValidity(GetHandle()), m_pVm(pVm)
{
	m_pVmShare = new CVmSharedFolder;
	SYNCHRO_PARENT_VM_CONFIG
	m_pVm->GetVmConfig().getVmSettings()->getVmTools()->getVmSharing()->getHostSharing()->addSharedFolder(m_pVmShare);
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmShare);
}

PrlHandleVmShare::PrlHandleVmShare(const PrlHandleVmPtr &pVm, CVmSharedFolder *pSharedFolder)
: PrlHandleBase(PHT_SHARE), PrlControlValidity(GetHandle()), m_pVmShare(pSharedFolder), m_pVm(pVm)
{
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmShare);
}

PrlHandleVmShare::~PrlHandleVmShare()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmShare = NULL;
}

PRL_RESULT PrlHandleVmShare::Remove()
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	CVmSharedFolder *pVmShare = m_pVmShare;
	m_pVm->GetVmConfig().getVmSettings()->getVmTools()->getVmSharing()->getHostSharing()->
													m_lstSharedFolders.removeAll(pVmShare);
	PrlControlValidity::MarkAsInvalid(pVmShare);
	delete pVmShare;
	m_pVmShare = NULL;
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmShare::GetName(PRL_STR sVmShareNameBuf, PRL_UINT32_PTR pnVmShareNameBufLength)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	return CopyStringValue(m_pVmShare->getName(), sVmShareNameBuf, pnVmShareNameBufLength);
}

PRL_RESULT PrlHandleVmShare::SetName(PRL_CONST_STR sNewVmShareName)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	QString sNewVmShareNameUtf8 = UTF8_2QSTR(sNewVmShareName);
	m_pVmShare->setName(sNewVmShareNameUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmShare::GetPath(PRL_STR sVmSharePathBuf, PRL_UINT32_PTR pnVmSharePathBufLength)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	return CopyStringValue(m_pVmShare->getPath(), sVmSharePathBuf, pnVmSharePathBufLength);
}

PRL_RESULT PrlHandleVmShare::SetPath(PRL_CONST_STR sNewVmSharePath)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	QString sNewVmSharePathUtf8 = UTF8_2QSTR(sNewVmSharePath);
	m_pVmShare->setPath(sNewVmSharePathUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmShare::GetDescription(PRL_STR sVmShareDescriptionBuf, PRL_UINT32_PTR pnVmShareDescriptionBufLength)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	return CopyStringValue(m_pVmShare->getDescription(), sVmShareDescriptionBuf, pnVmShareDescriptionBufLength);
}

PRL_RESULT PrlHandleVmShare::SetDescription(PRL_CONST_STR sNewVmShareDescription)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	QString sNewVmShareDescriptionUtf8 = UTF8_2QSTR(sNewVmShareDescription);
	m_pVmShare->setDescription(sNewVmShareDescriptionUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmShare::IsEnabled(PRL_BOOL_PTR pbEnabled)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	*pbEnabled = m_pVmShare->isEnabled();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmShare::SetEnabled(PRL_BOOL bEnabled)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	m_pVmShare->setEnabled(bEnabled);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmShare::IsReadOnly(PRL_BOOL_PTR pbReadOnly)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	*pbReadOnly = m_pVmShare->isReadOnly();
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmShare::SetReadOnly(PRL_BOOL bReadOnly)
{
	SYNCHRO_PARENT_VM_CONFIG
	CHECK_SHARE_OBJECT
	m_pVmShare->setReadOnly(bReadOnly);
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmShare::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmShare = NULL;
}

quint32 PrlHandleVmShare::ConcreteGenerateHashCode()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return qHash(m_pVmShare ? m_pVmShare->getName() : QString(""));
}

void PrlHandleVmShare::TryToRestoreObjectElement()
{
	Q_ASSERT(!m_pVmShare);
	Q_ASSERT(m_pVm);
	foreach(CVmSharedFolder *pShare, m_pVm->GetVmConfig().getVmSettings()->getVmTools()->
									getVmSharing()->getHostSharing()->m_lstSharedFolders)
	{
		if (GetHashCode() == qHash(pShare->getName()))
		{
			m_pVmShare = pShare;
			/* Reregister instance with new pointer */
			PrlControlValidity::RegisterInstance(GetHandle(), m_pVmShare);
			break;
		}
	}
}
