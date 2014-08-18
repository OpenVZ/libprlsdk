/*
 * CFeaturesMatrix.cpp: Class Matrix for supported features
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


#include "CFeaturesMatrix.h"
#include "Libraries/PrlDataSerializer/CPrlOpaqueTypeDataSerializer.h"

void CFeaturesMatrix::Initialize( const QSet<PRL_FEATURES_MATRIX> &_features )
{
	m_FeaturesMatrix = _features;
}

PRL_BOOL CFeaturesMatrix::IsFeatureSupported( PRL_FEATURES_MATRIX nFeatureId ) const
{
	return (PRL_BOOL)m_FeaturesMatrix.contains( nFeatureId );
}

void CFeaturesMatrix::Serialize(QDataStream &_stream)
{
	int nSize = m_FeaturesMatrix.size();
	CPrlOpaqueTypeDataSerializer<int>( nSize ).Serialize( _stream );
	foreach(PRL_FEATURES_MATRIX nItem, m_FeaturesMatrix)
		CPrlOpaqueTypeDataSerializer<quint32>( CPrlOpaqueTypeConverter<PRL_FEATURES_MATRIX>( nItem ) ).Serialize( _stream );
}

void CFeaturesMatrix::Deserialize(QDataStream &_stream)
{
	m_FeaturesMatrix.clear();
	int nSize = 0;
	CPrlOpaqueTypeDataSerializer<int>( nSize ).Deserialize( _stream );
	for (int i = 0; i < nSize; ++i)
	{
		PRL_FEATURES_MATRIX nFeatureId = PFSM_UNKNOWN_FEATURE;
		CPrlOpaqueTypeDataSerializer<quint32>( CPrlOpaqueTypeConverter<PRL_FEATURES_MATRIX>( nFeatureId ) ).Deserialize( _stream );
		m_FeaturesMatrix.insert( nFeatureId );
	}
}

