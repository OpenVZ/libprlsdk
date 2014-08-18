/*
 * LoggingConfig.h: Unified logging library for all user-space
 * components. Logging configuration is situated in this file for
 * configuration ease.
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


#ifndef __LOGGING_CONFIG_H__
#define __LOGGING_CONFIG_H__

/**
 * Globally Enable / Disable logging macro - uncomment to make the log work everywhere.
 */
//#define LOGGING_ON


/**
 * Supported logging levels.
 * Shared between application & monitor
 */

/**
 * Logging priorities are separated to the equal groups
 * of similar priorities for different systems (generic, qt, etc.)
 */
#define DBG_GROUP_SIZE	16

/**
 * Generic group.
 */
#define DBG_GROUP_BASE	(0)

#define DBG_FATAL		( (DBG_GROUP_SIZE * DBG_GROUP_BASE) + 0 )
#define DBG_WARNING		( (DBG_GROUP_SIZE * DBG_GROUP_BASE) + 1 )
#define DBG_INFO		( (DBG_GROUP_SIZE * DBG_GROUP_BASE) + 2 )
#define DBG_DEBUG		( (DBG_GROUP_SIZE * DBG_GROUP_BASE) + 3 )
#define DBG_TRACE		( (DBG_GROUP_SIZE * DBG_GROUP_BASE) + 4 )

/* simple to use numeric log levels */
#define LOG_L0			DBG_FATAL
#define LOG_L1			DBG_WARNING
#define LOG_L2			DBG_INFO
#define LOG_L3			DBG_DEBUG
#define LOG_L4			DBG_TRACE

/**
 * Qt library group.
 */
#define DBG_GROUP_QT	( 1 )

#define DBG_QT_FATAL	( (DBG_GROUP_SIZE * DBG_GROUP_QT) + 0 )
#define DBG_QT_WARNING	( (DBG_GROUP_SIZE * DBG_GROUP_QT) + 1 )
#define DBG_QT_INFO		( (DBG_GROUP_SIZE * DBG_GROUP_QT) + 2 )
#define DBG_QT_DEBUG	( (DBG_GROUP_SIZE * DBG_GROUP_QT) + 3 )

#define DBG_LEVEL_UNKNOWN	(0)
#define DBG_NONE			(-1)


/**
 * Other logging-related definitions
 */
// Logging level (by default)
#define DBG_LEVEL	(DBG_INFO)


/**
 * Log buffer size.
 *
 * This value is used for:
 * 1. Monitor log buffer
 * 2. HPC space temporary buffer
 * 3. Hypervisor log buffer
 * 4. User space log buffer for driver log output
 *
 * NOTE: Better to avoid defitions like PAGE_SIZE here
 * to keep header as portable as possible - using 0x1000 instead
 */
#define LOG_BUFF_PAGES	(32)
#define LOG_BUFF_SIZE	((LOG_BUFF_PAGES) * 0x1000)


/**
 * Default log file names.
 */
#define PRL_LOG_FILE_NAME_PAX		"parallels-pax.log"
#define PRL_LOG_FILE_NAME_DEFAULT	"parallels.log"


#endif // __LOGGING_CONFIG_H__
