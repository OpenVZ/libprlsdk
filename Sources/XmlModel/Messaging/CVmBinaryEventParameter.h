/*
 * CVmBinaryEventParameter.h: VM event parameter child class that
 * storing event parameter value as binary raw data and provides it
 * outside as raw data buffer.
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


#ifndef CVmBinaryEventParameter_H
#define CVmBinaryEventParameter_H

#include "XmlModel/Messaging/CVmEventParameter.h"
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include "Libraries/Std/SmartPtr.h"

/**
  * VM event parameter child class that storing event parameter value as binary raw data and provides it outside
  * as raw data buffer.
 */
class CVmBinaryEventParameter: public CVmEventParameter
{
public:
	/**
	 * Class constructor
	 */
	CVmBinaryEventParameter();
	/**
	 * Class copy constructor
	 */
	CVmBinaryEventParameter(const CVmBinaryEventParameter &_obj);
	/**
	 * Class constructor
	 * @param parameter name
	 */
	CVmBinaryEventParameter(const QString &sParamName);
	/**
	 * Returns data stream instance on internal event params data buffer for external purposes
	 * @param sign whether internal buffer must be reseted before usage externally
	 */
	SmartPtr<QDataStream> getBinaryDataStream(bool bResetSign = true);
	/**
	 * Returns internal binary buffer
	 */
	const QByteArray &GetByteArray() const;

public:
	/**
	 * Overridden method that serializes XML model element into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes XML model element from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** Internal bytearray */
	QByteArray m_ByteArray;
	/** Internal buffer device */
	QBuffer m_Buffer;

private:
	/**
	 * Internal data initialization procedure
	 */
	void Init();
};

#endif
