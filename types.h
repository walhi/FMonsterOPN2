/************************************************************************/
/**
 * \file	types.h
 * \brief	The usual datatype definitions, used for portability.
 * \author	Jesús Alonso (doragasu)
 * \date	2013
 ****************************************************************************/
/* This file is part of vmg-pod source package.
 *
 * vgm-pod is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Some open source application is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vgm-pod.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TYPES_H_
#define _TYPES_H_
/*#include <stdint.h>*/
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned long int uint32_t;
typedef signed long int int32_t;
/*typedef unsigned long long u64;
typedef long long i64;*/
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef union
{
	uint32_t dw;
	uint8_t b[4];
} QBYTE;

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif /*NULL*/
#ifndef TRUE
#define TRUE 1
#define FALSE 0

#endif /*TRUE*/
#endif /*_TYPES_H_*/
