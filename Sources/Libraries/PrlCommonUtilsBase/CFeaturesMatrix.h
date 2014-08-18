/*
 * CFeaturesMatrix.h: Class Matrix for supported features
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


#ifndef CFeaturesMatrix_H
#define CFeaturesMatrix_H

#include "SDK/Include/PrlTypes.h"
#include "SDK/Include/PrlEnums.h"
#include <QSet>
#include "Libraries/PrlDataSerializer/CPrlDataSerializer.h"

class CFeaturesMatrix : public CPrlDataSerializer
{
public:
	/**
	 * Initializes matrix with specified features list
	 */
	void Initialize( const QSet<PRL_FEATURES_MATRIX> &_features );
	/**
	 * Checks whether specified feature supported
	 */
	PRL_BOOL IsFeatureSupported( PRL_FEATURES_MATRIX nFeatureId ) const;
	/**
	 * Overridden method that serializes object into corresponding buffer
	 * @param reference to output buffer object
	 */
	void Serialize(QDataStream &_stream);
	/**
	 * Overridden method that deserializes object data from corresponding buffer
	 * @param reference to input buffer object
	 */
	void Deserialize(QDataStream &_stream);

private:
	/** List of features */
	QSet<PRL_FEATURES_MATRIX> m_FeaturesMatrix;
};

#endif// CFeaturesMatrix_H

