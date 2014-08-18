/*
 * CGuestOsesHelper.h: Class helper for supported guest OSes
 * operations
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


#ifndef GuestOsesHelper_H
#define GuestOsesHelper_H


#include "SDK/Include/PrlTypes.h"
#include "SDK/Include/PrlErrors.h"
#include "SDK/Include/PrlEnums.h"
#include "SDK/Include/PrlOses.h"
#include <QList>
#include "Libraries/PrlDataSerializer/CPrlOpaqueTypeDataSerializer.h"

struct COpaqueTypeListImpl
{
	virtual ~COpaqueTypeListImpl() {};
	virtual PRL_UINT32 GetItemsCount() const {return 0;};
	virtual void AddItem(PRL_CONST_VOID_PTR pnItem) {PRL_UNUSED_PARAM(pnItem);};
	virtual PRL_RESULT GetItem(PRL_UINT32 nItemIndex, PRL_VOID_PTR pnItem) const
	{
		PRL_UNUSED_PARAM(nItemIndex);
		PRL_UNUSED_PARAM(pnItem);
		return (PRL_ERR_UNIMPLEMENTED);
	};
	virtual void RemoveItem(PRL_UINT32 nItemIndex)
	{
		PRL_UNUSED_PARAM(nItemIndex);
	};
	virtual PRL_SIZE GetTypeSize() const {return (0);}

	static COpaqueTypeListImpl *CreateInstance(PRL_SIZE nTypeSize);

protected:
	COpaqueTypeListImpl() {};
};

template <typename T>
class TOpaqueTypeList : public COpaqueTypeListImpl, public CPrlDataSerializer
{
public:
	virtual PRL_UINT32 GetItemsCount() const
	{
		return m_List.size();
	};
	virtual void AddItem(PRL_CONST_VOID_PTR pnItem)
	{
		m_List.append(*reinterpret_cast<const T *>(pnItem));
	};
	virtual PRL_RESULT GetItem(PRL_UINT32 nItemIndex, PRL_VOID_PTR pnItem) const
	{
		T *pnCastedItem = reinterpret_cast<T *>(pnItem);
		*pnCastedItem = m_List.at(nItemIndex);
		return (PRL_ERR_SUCCESS);
	};
	virtual void RemoveItem(PRL_UINT32 nItemIndex)
	{
		m_List.removeAt(nItemIndex);
	};
	virtual PRL_SIZE GetTypeSize() const {return sizeof(T);}

	bool operator==(const TOpaqueTypeList<T> &_obj) const
	{
		return (m_List == _obj.m_List);
	}
	/**
	 * Overridden method that serializes string into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream)
	{
		int nSize = m_List.size();
		CPrlOpaqueTypeDataSerializer<int>(nSize).Serialize(_stream);
		for (int i = 0; i < nSize; ++i)
			CPrlOpaqueTypeDataSerializer<quint32>(CPrlOpaqueTypeConverter<T>(m_List[i])).Serialize(_stream);
	}
	/**
	 * Overridden method that deserializes string data from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream)
	{
		m_List.clear();
		int nSize = 0;
		CPrlOpaqueTypeDataSerializer<int>(nSize).Deserialize(_stream);
		for (int i = 0; i < nSize; ++i)
		{
			T nValue = (T)0;
			CPrlOpaqueTypeDataSerializer<quint32>(CPrlOpaqueTypeConverter<T>(nValue)).Deserialize(_stream);
			m_List.append(nValue);
		}
	}

public:
	QList<T> &GetContainer() {return (m_List);}

private:
	QList<T> m_List;
};

/**
 * Simple helper which lets to determine supported OSes types/versions
 */
class CGuestOsesHelper
{
public:
	/** Returns supported guest OSes types
	 *  @param host OS type
	 *  @return list of supported guest OSes types for specified host OS type
	 *  and current product mode
	 */
	static TOpaqueTypeList<PRL_UINT8> GetSupportedOsesTypes(PRL_HOST_OS_TYPE nHostOsType);

	/** Returns supported guest OSes versions
	 *  @param host OS type
	 *  @param requesting guest OS type
	 *  @return list of supported guest OSes types for specified host OS type
	 *  guest OS type and current product mode
	 */
	static TOpaqueTypeList<PRL_UINT16> GetSupportedOsesVersions(PRL_HOST_OS_TYPE nHostOsType, PRL_UINT8 nOsType);

	/** Returns default OS version
	 *  @param requesting guest OS type
	 *  @return default OS version for specified guest OS type
	 */
	static PRL_UINT16 GetDefaultOsVersion(PRL_UINT8 nOsType);
	/**
	 * Processes passed host OS type and determines current OS type automatically
	 * if passed host type equals PHO_UNKNOWN
	 * @param passed OS type
	 */
	static PRL_HOST_OS_TYPE GetHostOsType(PRL_HOST_OS_TYPE nPassedOsType);
	/**
	 * Checks whether specified OS type valid
	 */
	static bool CheckGuestOsType(PRL_UINT8 nOsType);
	/**
	 * Checks OS for EFI compatibility
	 */
	static bool SupportsEfiBios(PRL_UINT32 uOsVer);
};

/**
 * Complex structure contains all necessary data info about supported guest
 * types/versions which can be transfered through communication protocol on
 * remote side
 */
class COsesMatrix : public CPrlDataSerializer
{
public:
	/**
	 * Simple structure describes guest OS type element
	 */
	struct TGuestTypeDescription
	{
		PRL_UINT8 m_nGuestType;
		TOpaqueTypeList<PRL_UINT16> m_lstOsesVersions;
		PRL_UINT16 m_nDefaultOsVersion;
		TGuestTypeDescription()
		: m_nGuestType(PVS_GUEST_TYPE_OTHER), m_nDefaultOsVersion(PVS_GUEST_VER_OTH_OTHER)
		{}
		TGuestTypeDescription(PRL_HOST_OS_TYPE nHostType, PRL_UINT8 nOsType) :
		m_nGuestType(nOsType),
		m_lstOsesVersions(CGuestOsesHelper::GetSupportedOsesVersions(nHostType, nOsType)),
		m_nDefaultOsVersion(CGuestOsesHelper::GetDefaultOsVersion(nOsType))
		{}
		bool operator==(const TGuestTypeDescription &_obj) const
		{
			return (m_nGuestType == _obj.m_nGuestType &&
					m_lstOsesVersions == _obj.m_lstOsesVersions &&
					m_nDefaultOsVersion == _obj.m_nDefaultOsVersion);
		}
		/**
		 * Overridden method that serializes string into corresponding buffer
		 * @param reference to output buffer object
		 */
		void Serialize(QDataStream &_stream)
		{
			CPrlOpaqueTypeDataSerializer<PRL_UINT8>(m_nGuestType).Serialize(_stream);
			m_lstOsesVersions.Serialize(_stream);
			CPrlOpaqueTypeDataSerializer<PRL_UINT16>(m_nDefaultOsVersion).Serialize(_stream);
		}
		/**
		 * Overridden method that deserializes string data from corresponding buffer
		 * @param reference to input buffer object
		 */
		void Deserialize(QDataStream &_stream)
		{
			CPrlOpaqueTypeDataSerializer<PRL_UINT8>(m_nGuestType).Deserialize(_stream);
			m_lstOsesVersions.Deserialize(_stream);
			CPrlOpaqueTypeDataSerializer<PRL_UINT16>(m_nDefaultOsVersion).Deserialize(_stream);
		}
	};
public:
	/**
	 * Adds new host type to OSes matrix
	 * @param type of host OS
	 * @param type of guest OS
	 */
	void AddOsType(PRL_HOST_OS_TYPE nHostOsType, PRL_UINT8 nGuestOsType);
	/**
	 * Returns supported OSes types
	 */
	TOpaqueTypeList<PRL_UINT8> GetSupportedOsesTypes();
	/**
	 * Returns supported OSes versions for specified guest OS type
	 * @param guest OS type
	 */
	TOpaqueTypeList<PRL_UINT16> GetSupportedOsesVersions(PRL_UINT8 nOsType);
	/**
	 * Returns default OS version for specified guest OS type
	 * @param guest OS type
	 */
	PRL_UINT16 GetDefaultOsVersion(PRL_UINT8 nOsType);
	/**
	 * Operator equals
	 * @param compare object reference
	 * @return sign whether objects are equal
	 */
	bool operator==(const COsesMatrix &_obj) const;
	/**
	 * Overridden method that serializes string into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes string data from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** Guest OSes matrix */
	QList<TGuestTypeDescription> m_OsesMatrix;
};

#endif// GuestOsesHelper_H

