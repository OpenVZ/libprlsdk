/*
 * PrlPluginClasses.h: Definitions of supported classes and their
 * interfaces used in Parallels plugins mechanic.
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

#ifndef __PRL_PLUGINS_CLASSES_DEFINED__
#define __PRL_PLUGINS_CLASSES_DEFINED__

#include "PrlTypes.h"

#ifdef _WIN_
#pragma pack(push, save_pack, 1)
#endif

// Null GUID, used to determine end of array in classes arrays
static const PRL_GUID PRL_CLS_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };

// Base interface {823067EA-8E15-474f-A3AE-C6A068461256}
static const PRL_GUID PRL_CLS_BASE = { 0x823067ea, 0x8e15, 0x474f,
	{ 0xa3, 0xae, 0xc6, 0xa0, 0x68, 0x46, 0x12, 0x56 } };
#define GUID_CLS_BASE_STR			"{823067ea-8e15-474f-a3ae-c6a068461256}"

/*
 * Generic information about plugin
 */
typedef struct _IPluginInfo
{
	PRL_CONST_STR Vendor;
	PRL_CONST_STR Copyright;
	PRL_CONST_STR DescShort;
	PRL_CONST_STR DescLong;
	PRL_UINT32 Major;
	PRL_UINT32 Minor;
	PRL_UINT32 Build;
	PRL_GUID Type;
} PRL_STRUCT(IPluginInfo);
typedef IPluginInfo*	IPluginInfoPtr;

/*
 * Base interface
 */
typedef struct _PrlPlugin
{
	void (PRL_CALL *Release)(struct _PrlPlugin* _self);
	// Create specified object
	PRL_RESULT (PRL_CALL *QueryInterface)(struct _PrlPlugin* _self, PRL_GUID* Class,
				PRL_VOID_PTR_PTR _obj);
	// Get information about plugin
	PRL_RESULT (PRL_CALL *GetInfo)(struct _PrlPlugin* _self, IPluginInfoPtr Info);
} PRL_STRUCT( PrlPlugin );

// Encryption class
static const PRL_GUID PRL_CLS_ENCRYPTION = { 0x564820fc, 0xe265, 0x4d69,
	{ 0x9f, 0xb0, 0x3d, 0x18, 0x39, 0x6f, 0x1f, 0x8d } };
#define GUID_CLS_ENCRYPTION_STR		"{564820fc-e265-4d69-9fb0-3d18396f1f8d}"

/*
 * Information about encryption plugin
 */
typedef struct _ICryptInfo
{
	IPluginInfo PluginInfo;
	PRL_UINT32 KeySize;
	PRL_UINT32 BlockSize;
} PRL_STRUCT(ICryptInfo);
typedef ICryptInfo*	ICryptInfoPtr;

/*
 * Encryption interface
 */
typedef struct _PrlCrypt
{
	// Inheritance from PrlPlugin
	struct _PrlPlugin Plugin;
	// Initialize engine
	PRL_RESULT (PRL_CALL *Init)(struct _PrlCrypt* _self);
	// Encrypt data block
	PRL_RESULT (PRL_CALL *Encrypt)(struct _PrlCrypt* _self, PRL_VOID_PTR Data,
						const PRL_UINT32 Size, const PRL_UINT8_PTR v);
	// Decrypt data block
	PRL_RESULT (PRL_CALL *Decrypt)(struct _PrlCrypt* _self, PRL_VOID_PTR Data,
						const PRL_UINT32 Size, const PRL_UINT8_PTR v);
	// Set key to encryptor, key size must be equal or larger than described in Info
	PRL_RESULT (PRL_CALL *SetKey)(struct _PrlCrypt* _self, const PRL_UINT8_PTR Key);
	// Set initial initialization vector, its size must be
	// equal or larger than described in Info as block size
	PRL_RESULT (PRL_CALL *SetInitIV)(struct _PrlCrypt* _self, const PRL_UINT8_PTR v);
	// Get information about plugin
	PRL_RESULT (PRL_CALL *GetInfo)(struct _PrlCrypt* _self, ICryptInfoPtr Info);
} PRL_STRUCT( PrlCrypt );

#ifdef _WIN_
#pragma pack(pop, save_pack)
#endif

#endif
