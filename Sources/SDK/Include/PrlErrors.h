/*
 * PrlErrors.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
 * Copyright (c) 2017-2019, Virtuozzo International GmbH, All rights reserved
 *
 * This file is part of Virtuozzo SDK. Virtuozzo SDK is free
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
 * Our contact details: Virtuozzo International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.virtuozzo.com/.
 */


#ifndef __PARALLELS_API_ERRORS_H__
#define __PARALLELS_API_ERRORS_H__

#ifdef __cplusplus
extern "C" {
#endif

// Declare PRL_RESULT
#include "PrlTypes.h"

/**
 * Success and failure result codes
 */
#define PRL_SUCCEEDED( err )	( ( PRL_RESULT )( err ) >= 0 )
#define PRL_FAILED( err )	( ( PRL_RESULT )( err ) < 0 )


/**
 * Declaration of the error value goes here
 */
#define PRL_RESULT_DECLARE_ERROR( err ) ( (PRL_RESULT) (0x80000000 + 0x##err) )
#define PRL_RESULT_DECLARE_SUCCESS( err ) ( (PRL_RESULT) (err) )

/**
 * Actual errors values are kept in the externally-generated header
 */
#include "PrlErrorsValues.h"

/**
 * Actual events values are kept in the externally-generated header
 */
#include "PrlEventsValues.h"


/**
 * Success license state
 */
#define PRL_LICENSE_IS_VALID(x)	\
	( (PRL_RESULT) ( ( x == PRL_ERR_LICENSE_VALID ) || \
		( x == PRL_ERR_LICENSE_GRACED ) ) )

#define PRL_LICENSE_IS_EXPIRED( x )	\
	( ( x == PRL_ERR_LICENSE_EXPIRED ) || \
		( x == PRL_ERR_VZLICENSE_KA_LICENSE_EXPIRED ) )

#ifdef __cplusplus
}
#endif

#endif // __PARALLELS_API_ERRORS_H__
