/*
 * StringUtils.cpp
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


#include <QtCore/QLocale>

#include "StringUtils.h"
#include "Interfaces/ParallelsQt.h"

#include <QMap>

#include <string.h>

namespace Parallels {

QString GenerateFilename(
			const QStringList &_dir_entries,
			const QString &sFilenamePrefix,
			const QString &sFilenameSuffix,
			const QString &sIndexDelimiter
			)
{
	QString sTemplate((sFilenamePrefix.isEmpty() ? QString("tmpfile") : sFilenamePrefix) +
				sIndexDelimiter + "%1" + sFilenameSuffix);
	QString sTmpFilenameValue;
	quint32 nIndex = 0;
	while (true)
	{
		sTmpFilenameValue = (nIndex ? sTemplate.arg(nIndex) : (sFilenamePrefix.isEmpty() ? QString("tmpfile") : sFilenamePrefix) + sFilenameSuffix);
		if (_dir_entries.contains(sTmpFilenameValue, Qt::CaseInsensitive ))
		{
			nIndex++;
			continue;
		}
		else
			break;
	}
	return (sTmpFilenameValue);
}

QString getPrettySizeString( qulonglong sizeBytes, bool includeUnits )
{
	float KB = sizeBytes / 1024.0f;
	float MB = sizeBytes / 1024.0f / 1024.0f;
	//float GB = sizeBytes / 1024.0f / 1024.0f / 1024.0f;

	// according to sk@ by default we should show sizes in MB
	bool showInKB = ( KB <= 1500 );
	bool showInMB = ( KB > 1500 && MB <= 1500 );
	bool showInGB = ( MB > 1500 );

	double res = 0;
	QString units = QObject::tr( " bytes" );

	if ( showInKB )
	{
		res = sizeBytes / 1024.0f;
		units = QObject::tr( " KB" );
	}
	else if ( showInMB )
	{
		res = sizeBytes / 1024.0f / 1024.0f;
		units = QObject::tr( " MB" );
	}
	else if ( showInGB )
	{
		res = sizeBytes / 1024.0f / 1024.0f / 1024.0f;
		units = QObject::tr( " GB" );
	}
	else
		return QString::number( sizeBytes )+units;

	QString resStr = QString::number( res, 'f', 1 );
	if ( includeUnits )
		resStr.append( units );

	/// Update locale depended decimal separator
	resStr = resStr.replace( QChar( '.' ), QLocale().decimalPoint() );

	return resStr;
}

QString getPrettyTimeString( uint timeSeconds, bool includeUnits )
{
	uint hours = timeSeconds / 3600;
	uint minutes = timeSeconds / 60;

	QString resStr;
	if ( hours != 0 )
	{
		resStr = QString::number( hours );
		if ( includeUnits )
			resStr.append( hours == 1 ? QObject::tr( " hour" ) : QObject::tr( " hours" ) );
	}
	else if ( minutes != 0 )
	{
		resStr = QString::number( minutes );
		if ( includeUnits )
			resStr.append( minutes == 1 ? QObject::tr( " minute" ) : QObject::tr( " minutes" ) );
	}
	else if ( timeSeconds < 60 && timeSeconds >= 10 )
	{
		if ( includeUnits )
			resStr = QObject::tr( "less than a minute" );
		else
			resStr = QString::number( timeSeconds );
	}
	else if ( timeSeconds < 10 )
	{
		if ( includeUnits )
			resStr = QObject::tr( "less than 10 seconds" );
		else
			resStr = QString::number( timeSeconds );
	}

	return resStr;
}

int parseCpuMask(const QString &sMask, unsigned int nMaxCpu, char *bitMask, unsigned int nLen)
{
	bool bOk;
	bool bValidRange = (nMaxCpu == 0) ? true : false;

	QStringList lst = QString(sMask).split(",");

	if (sMask.isEmpty()) {
		if (bitMask != NULL)
			memset(bitMask, 0xff, nLen);
		return 0;
	}
	if (bitMask != NULL)
		memset(bitMask, 0, nLen);
	foreach(QString token, lst) {
		 unsigned int i, n, n1, n2;

		QStringList range = token.split("-");
		if (range.size() == 2) {
			n1 = range[0].toInt(&bOk);
			if (!bOk)
				return -1;
			n2 = range[1].toInt(&bOk);
			if (!bOk)
				return -1;
			if (n1 > n2)
				return -1;
		} else if (range.size() == 1) {
			n1 = token.toInt(&bOk);
			if (!bOk)
				return -1;
			n2 = n1;
		} else {
			return -1;
		}
		if (n1 < nMaxCpu)
			bValidRange = true;

		if (bitMask != NULL) {
			if (n1 >= nLen * 8 || n2 >= nLen * 8)
				return -1;
			for (i = n1; i <= n2; i++) {
				n = i / 8;
				bitMask[n] |= (1 << (i % 8));
			}
		}
	}
	if (!bValidRange)
		return -1;
	return 0;
}

bool isValidVmName( const QString& vmName )
{
	QByteArray invalidSymbols( "/\\:*?\"<>|" );
	foreach( const char c, invalidSymbols )
	{
		if ( vmName.contains( c ) )
			return false;
	}

	return true;
}

unsigned int getBitsCount(char *p, unsigned int size)
{
	unsigned int count = 0;

	for (unsigned int i = 0; i < size; i++) {
		for (unsigned int n = 0; n < 8; n++)
			if (p[i] & (1<<n))
				count++;
	}
	return count;
}

QString currencySign( const QString& currencyId )
{
	static QMap<QString, QString> currencySigns;
	if ( currencySigns.isEmpty() )
	{
		currencySigns[ "USD" ] = "$";
		currencySigns[ "CAD" ] = "$";
		currencySigns[ "GBP" ] = "£";
		currencySigns[ "EUR" ] = "€";
		currencySigns[ "JPY" ] = "¥";
	}

	return currencySigns.value( currencyId.toUpper(), currencyId );
}

}
