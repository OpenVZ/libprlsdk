/*
 * ConvRoutines.h: Conversion from VM image formats to RAW 32 bit
 * format. Conversion from VM image formats to RAW 32 bit format.
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


#ifndef __CONVERSION_ROUTINES_H__
#define __CONVERSION_ROUTINES_H__

#ifndef min
    #define min(a,b) ( (a)<(b)?(a):(b) )
#endif

#ifndef max
    #define max(a,b) ( (a)<(b)?(b):(a) )
#endif

#define RGB2Q(r,g,b) ( (r&0xFF)<<16 | (g&0xFF)<<8 | (b&0xFF) | 0xff000000)
#define CONV_PAL2RAW(color, Palette) (Palette[color] | 0xff000000)

#define MASK_32k
#define RMASK_32k  	0x00007C00
#define GMASK_32k	0x000003E0
#define BMASK_32k	0x0000001F

#define CONV_32K2RAW(color) ( ((color&RMASK_32k)<<9) | ((color&GMASK_32k)<<6) | ((color&BMASK_32k)<<3) | 0xff000000)


#define MASK_64k
#define RMASK_64k  	0x0000F800
#define GMASK_64k	0x000007E0
#define BMASK_64k	0x0000001F

#define CONV_64K2RAW(color) ( ((color&RMASK_64k)<<8) | ((color&GMASK_64k)<<5) | ((color&BMASK_64k)<<3) | 0xff000000)

#endif // __CONVERSION_ROUTINES_H__
