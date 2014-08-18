/*
 * PrlStringifyConsts.h: This file is the part of parallels public
 * SDK library.
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


#ifndef __PRL_STRINGIFY_CONSTS_H__
#define __PRL_STRINGIFY_CONSTS_H__

#include "SDK/Include/PrlErrors.h"
#include "SDK/Include/PrlPrivateErrorsValues.h"
#include "SDK/Include/PrlEnums.h"

#ifdef __cplusplus
extern "C" {
#endif

const char * PrlResultToString(unsigned int value) ;
const char * EventTypeToString(unsigned int value) ;
const char * HandleTypeToString(unsigned int value) ;
const char * VmStateToString(unsigned int value) ;
const char * LicRestrictionToString(unsigned int value) ;
const char * JobOperationCodeToString(unsigned int value) ;


#define PRL_RESULT_TO_STRING( value )           \
    PrlResultToString(value)

#define PRL_HANDLE_TYPE_TO_STRING( value )      \
    HandleTypeToString(value)

#define PRL_EVENT_TYPE_TO_STRING( value )       \
    EventTypeToString(value)

#define PRL_VM_STATE_TO_STRING( value )       \
	VmStateToString(value)

#define PRL_LICENSE_RESTR_KEY_TO_STRING( value ) \
	LicRestrictionToString(value)

#define PRL_JOC_TO_STRING( value )       \
	JobOperationCodeToString(value)

#ifdef __cplusplus
}
#endif


#endif //__PRL_STRINGIFY_CONSTS_H__
