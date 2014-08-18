/*
 * Common.h: file to use special edition of common functions
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

#ifndef _STD_COMMON_H_
#define _STD_COMMON_H_

#include <QString>

namespace Prl
{
	/* returns enviroment varialbles with unicode values
	 */
	QString getenvU(const QString &parameter);

	/* Finds known environment variables in passed string and replaces
	 * its names with its values. Variable name should be
	 * enclosed in % (%SYSTEMDRIVE%, %HOME%, etc)
	 */
	void ProcessEnvVariables(QString& path);

	/*
	 * @return Items count in fixed-size array
	 *
	 * Example:
	 * @code
	 * static const int nums[10];
	 * printf("Items in nums: %i\n", Prl::arrayItemsCount(nums));
	 * @endcode
	 */
	template <class T>
	size_t arrayItemsCount(const T &m)
	{
		(void)m;
		return (sizeof(m)/sizeof((m)[0]));
	}
}

#endif // _STD_COMMON_H_
