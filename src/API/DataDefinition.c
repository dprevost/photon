/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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

#include "Common/Common.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaValidateDefinition( vdsObjectDefinition * pDefinition )
{
   int i, j;
   
   switch( pDefinition->type ) {

   case VDS_FOLDER:
      return 0;

   case VDS_HASH_MAP:
      return 0;

   case VDS_QUEUE:
      if ( pDefinition->numFields < 0 || 
           pDefinition->numFields > VDS_MAX_FIELDS ) {
         return VDS_INVALID_NUM_FIELDS;
      }

      for ( i = 0; i < pDefinition->numFields; ++i ) {
         switch( pDefinition->fields[i].type ) {

         case VDS_INTEGER:
            if ( pDefinition->fields[i].length != 1 &&
                 pDefinition->fields[i].length != 2 &&
                 pDefinition->fields[i].length != 4 &&
                 pDefinition->fields[i].length != 8 ) {
               return VDS_INVALID_FIELD_LENGTH_INT;
            }
            break;

         case VDS_BINARY:
         case VDS_STRING:
            if ( pDefinition->fields[i].length == 0 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
#if SIZEOF_VOID_P > 4
            /* For 64 bits processors */
            if ( pDefinition->fields[i].length > 4294967295 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
#endif
            break;

         case VDS_DECIMAL:
            if ( pDefinition->fields[i].precision == 0 ||
               pDefinition->fields[i].precision > 30 ) {
               return VDS_INVALID_PRECISION;
            }
            if ( pDefinition->fields[i].scale > 
                 pDefinition->fields[i].precision ) {
               return VDS_INVALID_SCALE;
            }
            break;

         case VDS_BOOLEAN:
            break;

         case VDS_VAR_BINARY:
         case VDS_VAR_STRING:
            /* These 2 types are only valid for the last field. */
            if ( i != (pDefinition->numFields-1) ) return VDS_INVALID_FIELD_TYPE;
            /*
             * Reminder: maxLength set to zero indicates the maximum value
             * allowed which is 4294967295.
             */
#if SIZEOF_VOID_P > 4
            /* For 64 bits processors */
            if ( pDefinition->fields[i].maxLength > 4294967295 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
            /* in case maxLength = 0 */
            if ( pDefinition->fields[i].minLength > 4294967295 ) {
               return VDS_INVALID_FIELD_LENGTH;
            }
#endif
            if ( pDefinition->fields[i].maxLength != 0 ) {
               if ( pDefinition->fields[i].minLength > 
                    pDefinition->fields[i].maxLength ) {
                  return VDS_INVALID_FIELD_LENGTH;
               }
            }
            break;

         default:
            return VDS_INVALID_FIELD_TYPE;
         }
         /*
          * Field names must only allow characters that can be used to build 
          * c/c++ structs: they must start with a letter, and be limited to 
          * ASCII alphanumerics or '_'.
          *
          * The C99 standard allows for more than this (part of unicode 
          * defined in ISO 10646) but not all compilers implement it yet, for 
          * example VS2008:
          *     http://msdn.microsoft.com/en-us/library/565w213d.aspx.
          */
         if ( ! isalpha(pDefinition->fields[i].name[0]) ) {
            return VDS_INVALID_FIELD_NAME;
         }
         for ( j = 1; j < VDS_MAX_FIELD_LENGTH; ++j ) {
            if ( pDefinition->fields[i].name[j] == 0 ) break;
            if ( isalnum(pDefinition->fields[i].name[j]) ) continue;
            if ( pDefinition->fields[i].name[j] == '_' ) continue;
            
            return VDS_INVALID_FIELD_NAME;
         }

         /* We must also make sure that field names are not duplicate. */
         for ( j = 0; j < i; ++j ) {
            if ( strncmp( pDefinition->fields[i].name,
                          pDefinition->fields[j].name,
                          VDS_MAX_FIELD_LENGTH ) == 0 ) {
               return VDS_DUPLICATE_FIELD_NAME;
            }
         }
      }
      return 0;
      
   default:
      return VDS_WRONG_OBJECT_TYPE;
   }

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
