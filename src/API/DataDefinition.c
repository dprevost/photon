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

/* 
 * Note: the type of object must be filled by the caller.
 */
int vdsaGetDefinition( vdseFieldDef         * pInternalDef,
                       uint16_t               numFields,
                       vdsObjectDefinition ** ppDefinition )
{
   unsigned int i;
   vdsObjectDefinition * ptr;
   
   ptr = (vdsObjectDefinition *)calloc( offsetof(vdsObjectDefinition,fields) +
      numFields * sizeof(vdsFieldDefinition), 1 );
   if ( ptr == NULL ) return VDS_NOT_ENOUGH_HEAP_MEMORY;

   ptr->numFields = numFields;

   for ( i = 0; i < numFields; ++i ) {

      ptr->fields[i].type = pInternalDef[i].type;
      memcpy( ptr->fields[i].name, pInternalDef[i].name, VDS_MAX_FIELD_LENGTH );

      switch( pInternalDef[i].type ) {

      case VDS_BINARY:
      case VDS_STRING:
      case VDS_INTEGER:
         ptr->fields[i].length = pInternalDef[i].length1;
         
         break;

      case VDS_DECIMAL:
         ptr->fields[i].precision = pInternalDef[i].length1;
         ptr->fields[i].scale     = pInternalDef[i].length2;

         break;

      case VDS_BOOLEAN:

         break;

      case VDS_VAR_BINARY:
      case VDS_VAR_STRING:

         ptr->fields[i].minLength = pInternalDef[i].length1;
         ptr->fields[i].maxLength = pInternalDef[i].length2;

         break;
      }
   }

   *ppDefinition = ptr;
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


void vdsaGetLimits( vdseFieldDef * pDefinition,
                    uint16_t       numFields,
                    size_t       * pMinLength,
                    size_t       * pMaxLength )
{
   unsigned int i;
   size_t minLength = 0, maxLength = 0;
   
   /*
    * The first field is special - the alignment offset is always zero
    * since we just started.
    */
   switch( pDefinition[0].type ) {

   case VDS_INTEGER:
      minLength = pDefinition[0].length1;
      break;

   case VDS_BINARY:
   case VDS_STRING:
      minLength = pDefinition[0].length1;
      break;

   case VDS_DECIMAL:
      minLength = pDefinition[0].length1 + 2;
      break;

   case VDS_BOOLEAN:
      minLength = sizeof(bool);
      break;

   case VDS_VAR_BINARY:
   case VDS_VAR_STRING:
      minLength = pDefinition[0].length1;
      maxLength = pDefinition[0].length2;
      if ( maxLength == 0 ) maxLength = 4294967295UL;
      break;
   }
   
   for ( i = 1; i < numFields; ++i ) {

      switch( pDefinition[i].type ) {

      case VDS_INTEGER:
         if ( pDefinition[i].length1 == 1 ) {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         }
         else if ( pDefinition[i].length1 == 2 ) {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_INT16 + 1)*VDSC_ALIGNMENT_INT16;
         }
         else if ( pDefinition[i].length1 == 4 ) {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_INT32 + 1)*VDSC_ALIGNMENT_INT32;
         }
         else {
            minLength = ((minLength-1)/VDSC_ALIGNMENT_INT64 + 1)*VDSC_ALIGNMENT_INT64;
         }
         
         minLength += pDefinition[i].length1;

         break;

      case VDS_BINARY:
      case VDS_STRING:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1;

         break;

      case VDS_DECIMAL:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1 + 2;

         break;

      case VDS_BOOLEAN:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_BOOL + 1)*VDSC_ALIGNMENT_BOOL;
         minLength += sizeof(bool);
         break;

      case VDS_VAR_BINARY:
      case VDS_VAR_STRING:
         minLength = ((minLength-1)/VDSC_ALIGNMENT_CHAR + 1)*VDSC_ALIGNMENT_CHAR;
         minLength += pDefinition[i].length1;

         if ( pDefinition[i].length2 == 0 ||
            pDefinition[i].length2 >= 4294967295UL - minLength ) {
            maxLength = 4294967295UL;
         }
         else {
            maxLength = minLength + pDefinition[i].length2;
         }
         
         break;
      }
   }
   
   if ( maxLength == 0 ) maxLength = minLength;
   *pMinLength = minLength;
   *pMaxLength = maxLength;
}

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
               pDefinition->fields[i].precision > VDS_FIELD_MAX_PRECISION ) {
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
