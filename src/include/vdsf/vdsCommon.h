/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework) Library.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef VDS_COMMON_H
#define VDS_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Maximum number of characters (or bytes if not supporting i18n) 
 * of the name of a vds object.
 *
 * If the software was compiled with i18n, this maximum is the number
 * of wide characters (4 bytes). Otherwise it is the number of bytes
 * (which should equal the number of characters unless something funny
 * is going on like using UTF-8 as locale and using ---disable-i18n with
 * configure...).
 *
 * Note: setting this value eliminates a possible loophole since some
 * heap memory must be allocated to hold the wide characters string 
 * for the duration of the operation (open, close, create or destroy).
 */ 
#define VDS_MAX_NAME_LENGTH 2000

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef enum vdsObjectType
{
   VDS_FOLDER = 1,
   VDS_QUEUE  = 2,
   VDS_LAST_OBJECT_TYPE
} vdsObjectType;

typedef enum vdsIteratorType
{
   VDS_FIRST = 1,
   VDS_NEXT  = 2
   
} vdsIteratorType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
  
#endif /* VDS_COMMON_H */
