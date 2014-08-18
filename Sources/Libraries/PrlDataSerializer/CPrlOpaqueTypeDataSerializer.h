/*
 * CPrlOpaqueTypeDataSerializer.h: Common solution for serialization
 * of all opaque types.
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


#ifndef CPrlOpaqueTypeDataSerializer_H
#define CPrlOpaqueTypeDataSerializer_H

#include "Libraries/PrlDataSerializer/CPrlDataSerializer.h"

/**
 * Helper class that serves for valid casting enumeration types to quint32 type
 */
template <typename T>
class CPrlOpaqueTypeConverter
{
public:
	/**
	 * Class constructor
	 * @param wrapping value
	 */
	CPrlOpaqueTypeConverter(T &var)
	: m_nWrappingValue(var), m_nServiceValue(static_cast<quint32>(var))
	{}
	/**
	 * Class destructor
	 * Assigning processed value to the original one
	 */
	~CPrlOpaqueTypeConverter()
	{
		m_nWrappingValue = static_cast<T>(m_nServiceValue);
	}
	/**
	 * Overloaded operator that casting object instance into quint32& type
	 */
	operator quint32&()
	{
		return m_nServiceValue;
	}

private:
	/** Wrapping original value reference */
	T &m_nWrappingValue;
	/** Service value that passing outside for external usage of wrapping value as quint32 type */
	quint32 m_nServiceValue;
};

/**
 * Common implementation for serialization of all opaque types
 */
template <typename T>
class CPrlOpaqueTypeDataSerializer : public CPrlDataSerializer
{
public:
	/**
	 * Class constructor
	 * @param reference to the target data object
	 */
	CPrlOpaqueTypeDataSerializer(T &nData)
	: m_nData(nData)
	{}
	/**
	 * Overridable method that must serialize data to corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream)
	{
		_stream<<m_nData;
	}
	/**
	 * Overridable method that must deserialize data from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream)
	{
		_stream>>m_nData;
	}

private:
	/**
	 * Storing reference to target data object
	 */
	T &m_nData;
};

/**
 * Template method for convenience: generates necessary opaque type serialization object
 * @param reference to the target data object
 * @return generated serialization object
 */
template <typename T>
CPrlOpaqueTypeDataSerializer<T> PrlOpaqueSerializer(T &nData)
{
	return CPrlOpaqueTypeDataSerializer<T>(nData);
}

/**
 * Template method for convenience: generates opaque type converter object
 * @param reference to the target data object
 * @return generated converter object
 */
template <typename T>
CPrlOpaqueTypeConverter<T> PrlOpaqueTypeConverter(T &nData)
{
	return CPrlOpaqueTypeConverter<T>(nData);
}

#endif
