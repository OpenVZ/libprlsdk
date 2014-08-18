/*
 * NetworkUtils.cpp
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


#include <QHostAddress>
#include <QStringList>
#include "NetworkUtils.h"

bool NetworkUtils::ValidateAndConvertIpMask(QString &ip_mask)
{
	QString ip, mask;
	bool ret = ParseIpMask(ip_mask, ip, mask);
	if (ret)
		ip_mask = ip + "/" + mask;
	return ret;
}

bool NetworkUtils::ParseIpMask(const QString &ip_mask, QString &ip)
{
	QString mask;
	return ParseIpMask(ip_mask, ip, mask);
}

bool NetworkUtils::ParseIpMask(const QString &ip_mask, QString &ip, QString &mask)
{
	QHostAddress ip_addr;

	if (ip_mask.contains('/')) {
		// parse ip
		QStringList splited_ip_mask = ip_mask.split('/');
		if (splited_ip_mask.size() != 2)
			return false;

		if (!ip_addr.setAddress(splited_ip_mask[0]))
			return false;
		// parse mask
		QPair<QHostAddress, int> pair = QHostAddress::parseSubnet(ip_mask);
		if (pair.second == -1)
			return false;
		if (ip_addr.protocol() == QAbstractSocket::IPv4Protocol) {
			quint32 u_mask = 0;
			for (int i = 0; i < pair.second; i++)
				u_mask = u_mask | (1 << (31-i));

			QHostAddress mask_addr;
			// convert ipv4 mask to dot notation
			mask_addr.setAddress(u_mask);
			mask = mask_addr.toString();
		} else {
			mask = QString("%1").arg(pair.second);
		}
	} else {
		if (!ip_addr.setAddress(ip_mask))
			return false;

		if (ip_addr.protocol() == QAbstractSocket::IPv4Protocol)
			mask = "255.255.255.0";
		else
			mask = "64";
	}

	ip = ip_addr.toString();

	return true;
}
