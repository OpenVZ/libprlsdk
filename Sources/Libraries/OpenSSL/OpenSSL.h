/*
 * OpenSSL.h: This class used to allow SSL library be initialized
 * only once Every executable, using SSL library must use currently
 * provided initialization and deinitialization functions with
 * reference counters. Note: DO NOT USE QT HERE. You break
 * compilation of some products if Qt will be inserted. This
 * initialization routines was part of IOService::SSLHelper class,
 * and if you need some more functions - feel free to move them
 * here.
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

#ifndef __OPENSSL_INITIALIZATION_HELPER_ROUTINES__
#define __OPENSSL_INITIALIZATION_HELPER_ROUTINES__

#include "utils/OpenSSL_utils.h"

namespace OpenSSL
{
	extern bool Initialize();
	extern void Finalize();
}

#endif // __OPENSSL_INITIALIZATION_HELPER_ROUTINES__
