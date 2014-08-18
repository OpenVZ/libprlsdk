/*
 * CVmBinaryEventParameter.cpp: VM event parameter child class that
 * storing event parameter value as binary raw data and provides it
 * outsid as raw data buffer.
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


#include "CVmBinaryEventParameter.h"
#include "Libraries/Logging/Logging.h"

#include "Libraries/PrlDataSerializer/CPrlOpaqueTypeDataSerializer.h"

CVmBinaryEventParameter::CVmBinaryEventParameter()
{
	Init();
}

CVmBinaryEventParameter::CVmBinaryEventParameter(const CVmBinaryEventParameter &_obj)
: CVmEventParameter(_obj)
{
	m_ByteArray = _obj.m_ByteArray;
	Init();
}

CVmBinaryEventParameter::CVmBinaryEventParameter(const QString &sParamName)
: CVmEventParameter(PVE::UnsignedInt, "", sParamName)
{
	Init();
}

void CVmBinaryEventParameter::Init()
{
	m_uiEventParameterClassType = CVmEventParameter::Binary;
	m_Buffer.setBuffer(&m_ByteArray);
	bool bRes = m_Buffer.open(QIODevice::ReadWrite);
	PRL_UNUSED_PARAM(bRes);
	Q_ASSERT(bRes);
}

void CVmBinaryEventParameter::Serialize(QDataStream &_stream)
{
	CVmEventParameter::Serialize(_stream);
	int nBufferSize = m_ByteArray.size();
	PrlOpaqueSerializer(nBufferSize).Serialize(_stream);
	int nWroteData = _stream.writeRawData(m_ByteArray.constData(), nBufferSize);
	Q_ASSERT(nWroteData == nBufferSize);
	if (nWroteData != nBufferSize)
		WRITE_TRACE(DBG_FATAL, "Couldn't to completely write binary event param data. Sent data size: %d actually was written: %d", nBufferSize, nWroteData);
}

void CVmBinaryEventParameter::Deserialize(QDataStream &_stream)
{
	m_ByteArray.clear();
	m_Buffer.reset();
	CVmEventParameter::Deserialize(_stream);

	m_uiEventParameterClassType = CVmEventParameter::Binary;

	int nBufferSize = 0;
	PrlOpaqueSerializer(nBufferSize).Deserialize(_stream);
	if (nBufferSize)
	{
		m_ByteArray.resize(nBufferSize);
		int nReadData = _stream.readRawData(m_ByteArray.data(), nBufferSize);
		Q_ASSERT(nReadData == nBufferSize);
		if (nReadData != nBufferSize)
			WRITE_TRACE(DBG_FATAL, "Couldn't to completely read binary event param  data. Requested data size: %d actually was read: %d", nBufferSize, nReadData);
	}
}

SmartPtr<QDataStream> CVmBinaryEventParameter::getBinaryDataStream(bool bResetSign)
{
	if (bResetSign)//Resetting buffer before extrenal usage
		m_Buffer.reset();
	SmartPtr<QDataStream> pDataStream( new QDataStream(&m_Buffer) );
	pDataStream->setVersion(QDataStream::Qt_4_0);
	return (pDataStream);
}

const QByteArray &CVmBinaryEventParameter::GetByteArray() const
{
	return (m_ByteArray);
}

