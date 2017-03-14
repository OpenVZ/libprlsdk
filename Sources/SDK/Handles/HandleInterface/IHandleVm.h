/*
 * IHandleVm.h
 *
 * Copyright (c) 1999-2017, Parallels International GmbH
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
 * Our contact details: Parallels International GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#pragma once

#include <prlcommon/IOService/IOCommunication/IOProtocol.h>
#include <SDK/Include/PrlTypes.h>
#include <SDK/Include/PrlEnums.h>
#include <SDK/Include/PrlIOStructs.h>

class PrlHandleJob;

class IHandleVm
{
public:
	virtual ~IHandleVm() {}

	virtual PRL_RESULT VmConnect(PrlHandleJob **const pJob) = 0;
	virtual PRL_RESULT VmDisconnect() = 0;
	virtual PRL_RESULT VmDisconnectForcibly() = 0;
};
