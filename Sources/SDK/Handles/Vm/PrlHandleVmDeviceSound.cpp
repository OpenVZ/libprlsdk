/*
 * PrlHandleVmDeviceSound.cpp
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


#include "PrlHandleVmDeviceSound.h"
#include <prlxmlmodel/VmConfig/CVmConfiguration.h>
#include <prlxmlmodel/VmConfig/CVmSoundDevice.h>
#include <prlxmlmodel/VmConfig/CVmSoundInputs.h>
#include <prlxmlmodel/VmConfig/CVmSoundOutputs.h>

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


#define CHECK_VM_DEV_SOUND_ELEM\
	if (!m_pVmSound)\
		TryToRestoreObjectElement();\
	if (!m_pVmSound)\
		return (PRL_ERR_OBJECT_WAS_REMOVED);

static CVmSoundDevice* CreateSoundDevice()
{
	CVmSoundDevice* pSound = new CVmSoundDevice;
	pSound->getSoundInputs()->m_lstSoundDevices += new CVmSoundDevice();
	pSound->getSoundInputs()->m_lstSoundDevices.last()->setUserFriendlyName(PRL_SOUND_DEFAULT_DEVICE_NAME);
	pSound->getSoundOutputs()->m_lstSoundDevices += new CVmSoundDevice();
	pSound->getSoundOutputs()->m_lstSoundDevices.last()->setUserFriendlyName(PRL_SOUND_DEFAULT_DEVICE_NAME);
	return pSound;
}

PrlHandleVmDeviceSound::PrlHandleVmDeviceSound()
: PrlHandleVmDevice(PHT_VIRTUAL_DEV_SOUND)
{
	m_pVmSound = CreateSoundDevice();
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstSoundDevices.append(m_pVmSound);
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmSound);
}

PrlHandleVmDeviceSound::PrlHandleVmDeviceSound( const PrlHandleVmPtr &pVm, CVmSoundDevice *pSound )
: PrlHandleVmDevice(pVm, PHT_VIRTUAL_DEV_SOUND), m_pVmSound(pSound)
{
	if (!m_pVmSound)
	{
		m_pVmSound = CreateSoundDevice();
		if(m_pVm)
			m_pVm->GetVmConfig().getVmHardwareList()->m_lstSoundDevices.append(m_pVmSound);
	}
	SYNCHRO_VMDEV_ITEM_IDS
	PrlControlValidity::RegisterInstance(GetHandle(), m_pVmSound);
}

PrlHandleVmDeviceSound::~PrlHandleVmDeviceSound()
{
	if (!m_pVm.getHandle())//Seems we have an VM device object that wasn't bound to VM
	{
		delete m_pVmSound;
		m_pVmSound = NULL;
	}
}

PRL_RESULT PrlHandleVmDeviceSound::fromString( PRL_CONST_STR sXml )
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_VM_DEV_SOUND_ELEM
	if (PRL_SUCCEEDED(m_pVmSound->fromString(UTF8_2QSTR(sXml))))
		return PRL_ERR_SUCCESS;
	return PRL_ERR_UNEXPECTED;
}

QString PrlHandleVmDeviceSound::toString()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if (!m_pVmSound)
		return QString();
	return m_pVmSound->toString();
}

PRL_RESULT PrlHandleVmDeviceSound::ConcreteRemove()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	if(m_pVm)
		m_pVm->GetVmConfig().getVmHardwareList()->m_lstSoundDevices.removeAll(m_pVmSound);
	m_pVmSound = NULL;
	return (PRL_ERR_SUCCESS);
}

CVmDevice *PrlHandleVmDeviceSound::GetVmDeviceElem()
{
	if (!m_pVmSound)
		TryToRestoreObjectElement();
	return (m_pVmSound);
}

PRL_RESULT PrlHandleVmDeviceSound::SetEmulatedType(PRL_VM_DEV_EMULATION_TYPE nEmulatedType)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_VM_DEV_SOUND_ELEM
	m_pVmSound->setEmulatedType((PVE::SoundDeviceEmulatedType)nEmulatedType);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceSound::GetOutputDev(PRL_STR sOutputDev, PRL_UINT32_PTR pnOutputDevBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_VM_DEV_SOUND_ELEM
	if ( ! m_pVmSound->getSoundOutputs()->m_lstSoundDevices.isEmpty() )
		return CopyStringValue(m_pVmSound->getSoundOutputs()->m_lstSoundDevices[0]->getUserFriendlyName(),
								sOutputDev, pnOutputDevBufLength);
	else
		return CopyStringValue(m_pVmSound->getOutputDevice(), sOutputDev, pnOutputDevBufLength);
}

PRL_RESULT PrlHandleVmDeviceSound::SetOutputDev(PRL_CONST_STR sNewOutputDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_VM_DEV_SOUND_ELEM
	QString sNewOutputDevUtf8 = UTF8_2QSTR(sNewOutputDev);
	if ( ! m_pVmSound->getSoundOutputs()->m_lstSoundDevices.isEmpty() )
		m_pVmSound->getSoundOutputs()->m_lstSoundDevices[0]->setUserFriendlyName(sNewOutputDevUtf8);
	else
		m_pVmSound->setOutputDevice(sNewOutputDevUtf8);
	return (PRL_ERR_SUCCESS);
}

PRL_RESULT PrlHandleVmDeviceSound::GetMixerDev(PRL_STR sMixerDev, PRL_UINT32_PTR pnMixerDevBufLength)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_VM_DEV_SOUND_ELEM
	if ( ! m_pVmSound->getSoundInputs()->m_lstSoundDevices.isEmpty() )
		return CopyStringValue(m_pVmSound->getSoundInputs()->m_lstSoundDevices[0]->getUserFriendlyName(),
								sMixerDev, pnMixerDevBufLength);
	else
		return CopyStringValue(m_pVmSound->getMixerDevice(), sMixerDev, pnMixerDevBufLength);
}

PRL_RESULT PrlHandleVmDeviceSound::SetMixerDev(PRL_CONST_STR sNewMixerDev)
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	CHECK_VM_DEV_SOUND_ELEM
	QString sNewMixerDevUtf8 = UTF8_2QSTR(sNewMixerDev);
	if ( ! m_pVmSound->getSoundInputs()->m_lstSoundDevices.isEmpty() )
		m_pVmSound->getSoundInputs()->m_lstSoundDevices[0]->setUserFriendlyName(sNewMixerDevUtf8);
	else
		m_pVmSound->setMixerDevice(sNewMixerDevUtf8);
	return (PRL_ERR_SUCCESS);
}

void PrlHandleVmDeviceSound::MarkAsInvalid()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	m_pVmSound = NULL;
}

static QString GetSoundPattern(CVmSoundDevice *pSound)
{
	QString qsPattern;

	if ( ! pSound )
		return qsPattern;

	if ( ! pSound->getSoundInputs()->m_lstSoundDevices.isEmpty() )
		qsPattern += pSound->getSoundInputs()->m_lstSoundDevices[0]->getUserFriendlyName();
	else
		qsPattern += pSound->getMixerDevice();

	if ( ! pSound->getSoundOutputs()->m_lstSoundDevices.isEmpty() )
		qsPattern += pSound->getSoundOutputs()->m_lstSoundDevices[0]->getUserFriendlyName();
	else
		qsPattern += pSound->getOutputDevice();

	return qsPattern;
}

void PrlHandleVmDeviceSound::TryToRestoreObjectElement()
{
	if (m_pVm)
	{
		foreach(CVmSoundDevice *pSound, m_pVm->GetVmConfig().getVmHardwareList()->m_lstSoundDevices)
		{
			if (GetHashCode() == qHash(GetSoundPattern(pSound)))
			{
				m_pVmSound = pSound;
				/* Reregister instance with new pointer */
				PrlControlValidity::RegisterInstance(GetHandle(), m_pVmSound);
				break;
			}
		}
	}
}

quint32 PrlHandleVmDeviceSound::ConcreteGenerateHashCode()
{
	SYNCHRO_INTERNAL_DATA_ACCESS
	return qHash( GetSoundPattern(m_pVmSound) );
}
