/*
 * ConfigDefaults.h: Default Constants
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


#ifndef ___CONFIG_DEFAULTS____
#define ___CONFIG_DEFAULTS____

/**
 *  Config file default values
 */

// Main defaults
//#define DEFAULT_OS_TYPE  	OS_TYPE_OS2
#define DEFAULT_VM_NAME 		"Untitled Virtual Machine"
#define BAD_VM_CONFIG_FILE  	"61E62DFC-6EF6-4129-9E3C-FD1E4E201B7A"
#define DEFAULT_OS_TYPE 		PVS_GUEST_VER_WIN_XP
#define DEFAULT_BOOT_DEVICE 	1
#define DEFAULT_BOOT_SEQUENCE	1
#define DEFAULT_MEMORY_SIZE 	128
#define DEFAULT_VIDEO_SIZE		16

#ifdef BLOCK_SIZE_AS_PARAMETER
#define DEFAULT_MEMORY_BLOCK_SIZE   	"10"
#endif
#define DEFAULT_MULTI_FRAME_VALUE	"-1"

#define DEFAULT_FSM_MESSAGE 	1
#define DEFAULT_FSM 			0
#define DEFAULT_AUTOSTART   	0
#define DEFAULT_AUTOSHUTDOWN	0
#define DEFAULT_OS_ACCEL		1
#define DEFAULT_ACCEL_LEVEL 	1
#define DEFAULT_PC_SPKR_SUPPORT 1

// #if (SPECIAL_FEAT & SPECIAL_MODE)
//  #define DEFAULT_KERNEL_MODE2   	1
// #else
//  #define DEFAULT_KERNEL_MODE2   	0
// #endif
//
// Definition of DEFAULT_KERNEL_MODE2 has been transferred
// to Private/libApp/Constants.h
// Modified by SergeyM
// 2005-04-26
//

#define DEFAULT_CUSTOM_VIDEO_ENABLED	1
#define DEFAULT_CUSTOM_VIDEO_COUNT  			0
#define DEFAULT_CUSTOM_VIDEO_RESNAME	""
#define DEFAULT_CUSTOM_VIDEO_RESW   			0
#define DEFAULT_CUSTOM_VIDEO_RESH   			0
#define DEFAULT_CUSTOM_VIDEO_RES_EN 		0

#define DEFAULT_SFOLDERS_ENABLED				1
#define DEFAULT_SFOLDERS_SHARING_ENABLED		2
#define DEFAULT_SFOLDERS_COUNT  				0
#define DEFAULT_SFOLDER_ENABLED 				0
#define DEFAULT_SFOLDER_READONLY				0
#define DEFAULT_SFOLDER_NAME					""
#define DEFAULT_SFOLDER_PATH					""
#define DEFAULT_SFOLDER_DESCR   				""

// Default foreground priority
#define DEFAULT_FG_PRIORITY		1
// Default background priority
#define DEFAULT_BG_PRIORITY		1

#define DEFAULT_STRONG_CHECKS   0
#define DEFAULT_CACHE_ENABLE	1
#define DEFAULT_TIME_SYNC   	1
#define DEFAULT_BROADCAST_FLT   0
// Devices default state
#define DEFAULT_ENABLED_STATE   0
// FDD 0 defaults
#define DEFAULT_FLOPPY0_STATE   0
#define DEFAULT_FLOPPY0_IMAGE   "floppy0.fdd"
#define DEFAULT_FLOPPY0_CONN	1
// FDD 1 defaults
#define DEFAULT_FLOPPY1_STATE   0
#define DEFAULT_FLOPPY1_IMAGE   "floppy1.fdd"
#define DEFAULT_FLOPPY1_CONN	1
// IDE 0 defaults
#define DEFAULT_IDE00_STATE 	0
#define DEFAULT_IDE00_MEDIA 	0
#define DEFAULT_IDE00_IMAGE 	"disk0.hdd"
#define DEFAULT_IDE00_CYLS  	0
#define DEFAULT_IDE00_HEADS 	0
#define DEFAULT_IDE00_SECTORS   0
#define DEFAULT_IDE00_CONN  	1
#define DEFAULT_IDE00_INSTALL   0
// IDE 1 defaults
#define DEFAULT_IDE01_STATE 	0
#define DEFAULT_IDE01_MEDIA 	0
#define DEFAULT_IDE01_IMAGE 	"disk1.hdd"
#define DEFAULT_IDE01_CYLS  	0
#define DEFAULT_IDE01_HEADS 	0
#define DEFAULT_IDE01_SECTORS   0
#define DEFAULT_IDE01_CONN  	1
#define DEFAULT_IDE01_INSTALL   0
// IDE 2 defaults
#define DEFAULT_IDE02_STATE 	0
#define DEFAULT_IDE02_MEDIA 	0
#define DEFAULT_IDE02_IMAGE 	"disk2.hdd"
#define DEFAULT_IDE02_CYLS  	0
#define DEFAULT_IDE02_HEADS 	0
#define DEFAULT_IDE02_SECTORS   0
#define DEFAULT_IDE02_CONN  	1
#define DEFAULT_IDE02_INSTALL   0
// IDE 3 defaults
#define DEFAULT_IDE03_STATE 	0
#define DEFAULT_IDE03_MEDIA 	0
#define DEFAULT_IDE03_IMAGE 	"disk3.hdd"
#define DEFAULT_IDE03_CYLS  	0
#define DEFAULT_IDE03_HEADS 	0
#define DEFAULT_IDE03_SECTORS   0
#define DEFAULT_IDE03_CONN  	1
#define DEFAULT_IDE03_INSTALL   0
#define DEFAULT_PASSTHROUGH		0
// default serial device name
#define DEFAULT_SERIAL_NAME 	"ttyS"
// COM 1 defaults
#define DEFAULT_COM1_STATE  	0
#define DEFAULT_COM1_CONN   	0
#define DEFAULT_COM1_IMAGE  	"serial1.txt"
#define DEFAULT_COM1_MODE   	0
// COM 2 defaults
#define DEFAULT_COM2_STATE  	0
#define DEFAULT_COM2_CONN   	0
#define DEFAULT_COM2_IMAGE  	"serial2.txt"
#define DEFAULT_COM2_MODE   	0
// COM 3 defaults
#define DEFAULT_COM3_STATE  	0
#define DEFAULT_COM3_CONN   	0
#define DEFAULT_COM3_IMAGE  	"serial3.txt"
#define DEFAULT_COM3_MODE   	0
// COM 4 defaults
#define DEFAULT_COM4_STATE  	0
#define DEFAULT_COM4_CONN   	0
#define DEFAULT_COM4_IMAGE  	"serial4.txt"
#define DEFAULT_COM4_MODE   	0
#define DEFAULT_SERVER_SIGN		0
// default LPT device name
#define DEFAULT_LPT_NAME		"lp"
// LPT 1 defaults
#define DEFAULT_LPT1_STATE  	0
#define DEFAULT_LPT1_IMAGE  	"parallel1.txt"
#define DEFAULT_LPT1_CONN   	1
// LPT 2 defaults
#define DEFAULT_LPT2_STATE  	0
#define DEFAULT_LPT2_IMAGE  	"parallel2.txt"
#define DEFAULT_LPT2_CONN   	1
// LPT 3 defaults
#define DEFAULT_LPT3_STATE  	0
#define DEFAULT_LPT3_IMAGE  	"parallel3.txt"
#define DEFAULT_LPT3_CONN   	1
// Sound default
#define DEFAULT_SOUND_STATE 	0

#if (defined(_LIN_))
	#define DEFAULT_DSP_NAME	"dsp"
#else
	#define DEFAULT_DSP_NAME	"dsp-on-win32"
#endif

#define DEFAULT_MIXER_NAME  	"mixer"
#define DEFAULT_SOUND_CONN  	1
// Network defaults
#define DEFAULT_NET_STATE   	0
#define DEFAULT_NET_CONN		1
#define DEFAULT_NET_PORT		0x300
#define DEFAULT_NET_IRQ 		3
#define DEFAULT_NET_TYPE		0
#define DEFAULT_NET_INDEX   	0
#define DEFAULT_NET_MEM 		0
// Network adapter default is valid (1)
#define DEFAULT_NET_VALID   	1
// USB defaults
#define DEFAULT_USB_STATE   	0
#define DEFAULT_USB_CONN		0
// Additional system parameters defaults
#define DEFAULT_MONITOR_BASE			"70000000"
#define DEFAULT_ANALYSIS_USER   		0
#define DEFAULT_BITD_REMAP  			1
#define DEFAULT_VTX_SUPPORT 			1

#endif // ___CONFIG_DEFAULTS____
