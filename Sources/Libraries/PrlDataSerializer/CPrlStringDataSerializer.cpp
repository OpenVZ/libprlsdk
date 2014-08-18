/*
 * CPrlStringDataSerializer.cpp: String data serializer
 * implementation
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


#include "CPrlStringDataSerializer.h"

#include "Interfaces/ParallelsQt.h"
#include "Libraries/Logging/Logging.h"

CPrlStringDataSerializer::CPrlStringDataSerializer(QString &sData)
: m_sData(sData)
{}

void CPrlStringDataSerializer::Serialize(QDataStream &_stream)
{
	QByteArray sUtf8Data = m_sData.toUtf8();
	quint32 nStringDataSize = sUtf8Data.size();
	_stream<<nStringDataSize;
	if (nStringDataSize)
	{
		quint32 nWrittenBytesNum = _stream.writeRawData(sUtf8Data.constData(), nStringDataSize);
		if ( nWrittenBytesNum != nStringDataSize)
			WRITE_TRACE(DBG_FATAL, "Fatal error on string serialization: attempted to write %u bytes but just %u were written", nStringDataSize, nWrittenBytesNum);
	}
}

void CPrlStringDataSerializer::Deserialize(QDataStream &_stream)
{
	quint32 nStringDataSize = 0;
	_stream>>nStringDataSize;
	if (nStringDataSize)
	{
		QByteArray sDataBuf;
		sDataBuf.resize(nStringDataSize);
		quint32 nReadBytesNum = _stream.readRawData(sDataBuf.data(), nStringDataSize);
		if (nReadBytesNum != nStringDataSize)
			WRITE_TRACE(DBG_FATAL, "Fatal error on string deserialization: expected %u bytes but just %u were read", nStringDataSize, nReadBytesNum);
		else
			m_sData = UTF8_2QSTR(sDataBuf);
	}
}
