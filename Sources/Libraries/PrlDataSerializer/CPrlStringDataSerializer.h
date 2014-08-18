/*
 * CPrlStringDataSerializer.h: String data serializer
 * implementation.
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


#ifndef CPrlStringDataSerializer_H
#define CPrlStringDataSerializer_H

#include "Libraries/PrlDataSerializer/CPrlDataSerializer.h"
#include <QString>

/**
 * String data serializer implementation
 */
class CPrlStringDataSerializer : public CPrlDataSerializer
{
public:
	/**
	 * Class constructor.
	 * @param reference to processing string data
	 */
	CPrlStringDataSerializer(QString &sData);

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
	/**
	 * Reference to processing string data object
	 */
	QString &m_sData;
};

#endif
