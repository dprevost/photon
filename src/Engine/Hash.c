/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Hash.h"
#include "SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define HASH_ARRAY_MIN_SIZE 13   /* a prime number */

static unsigned int g_lowDensity  = 30;
static unsigned int g_highDensity = 50;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

   /** Do nothing - all is done by Close() */
   ~HashMap();

   bool FindKey  ( HashArray*  pArray,
                   const char* pKey,
                   size_t      keyLength, 
                   size_t*     pRowNumber,
                   vdseSessionContext* pContext );

   /** Case insensitive version */
   bool FindKeyCI( HashArray*  pArray,
                   const char* pKey, 
                   size_t      keyLength, 
                   size_t*     pRowNumber,
                   vdseSessionContext* pContext );

   size_t HashAKey  ( char*  pKey, 
                      size_t keyLength );
   /** Case insensitive version */
   size_t HashAKeyCI( char*  pKey, 
                      size_t keyLength );
   
   enum ListErrors ResizeArray( vdseSessionContext* pContext );

   bool TimeToResize()
   {
      return (((m_numberOfRows + 1)*100)/m_arraySize) >= m_highDensity;
   }


   /** Crash recovery attributes and functions */

   friend class TreeRecovery;
   friend class Folder;

   void DeleteEntry( size_t rowNumber,
                     vdseMemAlloc* pAlloc );

   int ResetArray( vdseMemAlloc* pAlloc );

   bool SelfTest( vdseMemAlloc* pAlloc );
   
   /** Return the pointer to the array so that it can be included in
    * the MemoryAllocator validation
    */
   void* GetArray(struct vdseMemAlloc* pAlloc) const
   { return GET_PTR( m_arrayOffset, HashArray, pAlloc ); }
#endif





#if 0

HashList::HashList( int flag )
   : BaseObject   ( IDENT_HASH_LIST ),
     m_arrayOffset  ( NULL_OFFSET ),
     m_numberOfRows ( 0     ),
     m_arraySize    ( 0     ),
     m_totalSize    ( 0     ),
     m_flag         ( flag )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors 
vdseHashInit( vdseHash*           pHash,
              int                 caseSensitiveFlag,
              size_t              initialSize, 
              vdseSessionContext* pContext )
{
   enum ListErrors errCode = LIST_OK;
   size_t len;

   VDS_PRE_CONDITION( pHash != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   
   initialSize = (100*initialSize)/g_lowDensity;

   if ( initialSize < HASH_ARRAY_MIN_SIZE ) 
      initialSize = HASH_ARRAY_MIN_SIZE;

   len = sizeof(HashArray) - sizeof(BaseObject) + 
      initialSize * sizeof(ptrdiff_t);
   fprintf( stderr, "%d %d\n",   sizeof(HashArray), sizeof(BaseObject) );
   
   HashArray* ptr = (HashArray*)
      vdseMalloc( pContext->pAllocator, len, &pContext->errorHandler );
   if ( ptr == NULL )
      errCode = LIST_NO_MEMORY;
   else
   {
      new (ptr) HashArray;

      for ( unsigned int i = 0; i < initialSize; ++i)
         ptr->Set( i, NULL_OFFSET );
      
      m_arraySize = initialSize;
      m_arrayOffset = SET_OFFSET( ptr, pContext->pAllocator );
   }
   
   return errCode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

HashList::~HashList()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void HashList::Close( vdseSessionContext* pContext )
{
   EmptyList( pContext );

   /* EmptyList deallocates the rows but not the main array of */
   /* RowDescriptor* */
   HashArray* pArray;
   SET_PTR( pArray, m_arrayOffset, HashArray, pContext->pAllocator );
   
   if ( pArray != NULL )
   {
      pArray->~HashArray();
      vdseFree( pContext->pAllocator, pArray, &pContext->errorHandler );
   }
   m_arrayOffset = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline u_long
hash_pjw (const char *str, size_t len)
{
   u_long hash = 0;

   for (size_t i = 0; i < len; i++)
   {
      const char temp = str[i];
      hash = (hash << 4) + (temp * 13);

      u_long g = hash & 0xf0000000;

      if (g)
      {
         hash ^= (g >> 24);
         hash ^= g;
      }
   }

   return hash;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 *
 * Never call this function directly!
 *
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

size_t HashList::HashAKey( char*  pKey, 
                           size_t keyLength )
{
   return hash_pjw( pKey, keyLength );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 *
 * Never call this function directly!
 * Case insensitive version
 *
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

size_t HashList::HashAKeyCI( char*  pKey, 
                             size_t keyLength )
{
   char searchKey[MAX_KEY_LENGTH];
   size_t i;
   
   VDS_ASSERT( keyLength <= MAX_KEY_LENGTH );
   
   for ( i = 0; i < keyLength; ++i )
      searchKey[i] = tolower( pKey[i] );
   
   return hash_pjw( searchKey, keyLength );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool HashList::FindKeyCI( HashArray*  pArray,
                          const char* pKey, 
                          size_t      keyLength, 
                          size_t*     pRowNumber,
                          vdseSessionContext* pContext )
{
   char searchKey[MAX_KEY_LENGTH];
   char foundKey[MAX_KEY_LENGTH];
   size_t i;
   
   VDS_ASSERT( keyLength <= MAX_KEY_LENGTH );

   for ( i = 0; i < keyLength; ++i )
      searchKey[i] = tolower( pKey[i] );
   
   size_t rowNumber = hash_pjw( searchKey, keyLength ) % m_arraySize;
   
   while (1)
   {
      RowDescriptor* pRow;
      SET_PTR( pRow, 
               pArray->Get(rowNumber), 
               RowDescriptor, 
               pContext->pAllocator );

      if ( pRow == NULL )
      {
         *pRowNumber = rowNumber;
         return false;
      }
     
      if ( keyLength == pRow->keyLength )
      {
         char* pOldKey = GET_PTR(pRow->keyOffset, char, pContext->pAllocator );
         
         for ( i = 0; i < keyLength; ++i )
            foundKey[i] = tolower( pOldKey[i] ); 

         if ( memcmp( searchKey, foundKey, keyLength ) == 0 )
         {
            *pRowNumber = rowNumber;
            return true;
         }
      }
      
      ++rowNumber; /* Advance to next possible emplacement */

      /* Go back to the beginning of the array (since the key might have been */
      /* hashed close to the end of the array, the requested location might */
      /* be at the beginning */
      if ( rowNumber == m_arraySize )
         rowNumber = 0;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool HashList::FindKey( HashArray*  pArray,
                        const char* pKey, 
                        size_t      keyLength, 
                        size_t*     pRowNumber,
                        vdseSessionContext* pContext )
{
   size_t rowNumber = hash_pjw( pKey, keyLength ) % m_arraySize;

   while (1)
   {
      RowDescriptor* pRow;
      SET_PTR( pRow, 
               pArray->Get(rowNumber), 
               RowDescriptor, 
               pContext->pAllocator );

      if ( pRow == NULL )
      {
         *pRowNumber = rowNumber;
         return false;
      }
     
      if ( keyLength == pRow->keyLength )
      {
         char* pOldKey = GET_PTR( pRow->keyOffset, 
                                  char, 
                                  pContext->pAllocator );
         if ( memcmp( pKey, pOldKey, keyLength ) == 0 )
         {
            *pRowNumber = rowNumber;
            return true;
         }
      }
      
      ++rowNumber; /* Advance to next possible emplacement */

      /* Go back to the beginning of the array (since the key might have been */
      /* hashed close to the end of the array, the requested location might */
      /* be at the beginning */
      if ( rowNumber == m_arraySize )
         rowNumber = 0;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::GetByKey( const char* pKey,
                                    size_t      keyLength,
                                    void **     ppData,
                                    size_t*     pDataLength,
                                    vdseSessionContext* pContext,
                                    size_t*     pRowNumber )
{
   size_t rowNumber;
   bool   keyFound = false;
   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pContext->pAllocator );
   VDS_ASSERT( pArray != NULL );

   if ( m_flag == HASH_CASE_INSENSITIVE )
      keyFound = FindKeyCI( pArray, pKey, keyLength, &rowNumber, pContext );
   else
      keyFound = FindKey( pArray, pKey, keyLength, &rowNumber, pContext );
   
   if ( keyFound )
   {
      RowDescriptor* pRow  = GET_PTR( pArray->Get(rowNumber), 
                                      RowDescriptor,
                                      pContext->pAllocator );

      *ppData      = GET_PTR( pRow->dataOffset, void, pContext->pAllocator );
      *pDataLength = pRow->dataLength;
      if ( pRowNumber )
         *pRowNumber  = rowNumber;
      
      return LIST_OK;
   }
   return LIST_KEY_NOT_FOUND;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void HashList::EmptyList( vdseSessionContext* pContext )
{
   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pContext->pAllocator );
   VDS_ASSERT( pArray != NULL );

   for ( size_t i = 0; i < m_arraySize; ++i )
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         RowDescriptor* pRow = GET_PTR( pArray->Get(i), 
                                        RowDescriptor,
                                        pContext->pAllocator );

         /* The allocation was done in one block - it must be freed the */
         /* same way */
         pRow->~RowDescriptor();
         vdseFree( pContext->pAllocator, pRow, &pContext->errorHandler );
         pArray->Set( i, NULL_OFFSET );
      }
   }
   
   m_numberOfRows = 0;
   m_totalSize = 0;

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::GetFirst( size_t*         pNewRowNumber, 
                                    RowDescriptor** ppRow,
                                    vdseMemAlloc*   pAlloc )
{
   if ( m_numberOfRows == 0 )
      return LIST_EMPTY;

   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pAlloc );
   VDS_ASSERT( pArray != NULL );
  
   for ( size_t i = 0; i < m_arraySize; ++i )
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         *ppRow  = GET_PTR( pArray->Get(i), 
                            RowDescriptor, 
                            pAlloc );
         *pNewRowNumber = i;

         return LIST_OK;
      }
   }
   if ( m_numberOfRows > 0 ) 
      fprintf( stderr, " ERROR 1 - %d \n", m_numberOfRows );
   
   return LIST_EMPTY; /* Should never occur */
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::GetNext( size_t  oldRowNumber,
                                   size_t* pNewRowNumber, 
                                   RowDescriptor** ppRow,
                                   vdseMemAlloc* pAlloc )
{
   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pAlloc );
   VDS_ASSERT( pArray != NULL );

   for ( size_t i = oldRowNumber+1; i < m_arraySize; ++i )
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         *ppRow  = GET_PTR( pArray->Get(i), 
                            RowDescriptor, 
                            pAlloc );
         *pNewRowNumber = i;

         return LIST_OK;
      }
   }
   return LIST_END_OF_LIST;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::InsertByKey( const char*     pKey,
                                       size_t          keyLength,
                                       void*           pData,
                                       size_t          dataLength,
                                       ptrdiff_t*      pSelf,
                                       vdseSessionContext* pContext )
{
   HashArray* pArray;   
   size_t rowNumber = 0;
   bool   keyFound = false;

   /* We resize first in order to get the "right" row number immediately */
   if ((( (m_numberOfRows + 1)*100)/m_arraySize) >= m_highDensity )
   {
      enum ListErrors rc = ResizeArray( pContext );
      if ( rc != 0 )
         return rc;
   }

   SET_PTR( pArray, m_arrayOffset, HashArray, pContext->pAllocator );
   VDS_ASSERT( pArray != NULL );
   
   if ( m_flag == HASH_CASE_INSENSITIVE )
      keyFound = FindKeyCI( pArray, pKey, keyLength, &rowNumber, pContext );
   else
      keyFound = FindKey( pArray, pKey, keyLength, &rowNumber, pContext );

   if ( keyFound )
      return LIST_KEY_FOUND;

   /* The whole row is allocated in one step, header+data, to minimize */
   /* overheads of the memory allocator (bget) */
   RowDescriptor* pRow = (RowDescriptor*) 
      vdseMalloc( pContext->pAllocator, 
                  sizeof(RowDescriptor) + keyLength + dataLength,
                  &pContext->errorHandler );
   if ( pRow == NULL ) return LIST_NO_MEMORY;
   
   new (pRow) RowDescriptor;
   
   pRow->dataOffset = SET_OFFSET( (char*)pRow + sizeof(RowDescriptor),
                                  pContext->pAllocator );
   pRow->keyOffset  = SET_OFFSET( (char*)pRow + sizeof(RowDescriptor) + 
                                  dataLength,
                                  pContext->pAllocator );
   
   memcpy( (char*)pRow + sizeof(RowDescriptor) + dataLength,  pKey,  
           keyLength );
   memcpy( (char*)pRow + sizeof(RowDescriptor), pData, dataLength );
   
   pRow->keyLength = keyLength;
   pRow->dataLength = dataLength;

   m_totalSize += dataLength;
   m_numberOfRows++;

   pArray->Set( rowNumber, SET_OFFSET(pRow, pContext->pAllocator) );

   *pSelf = pArray->Get( rowNumber );
   
   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::DeleteByKey( const char*     pKey, 
                                       size_t          keyLength,
                                       vdseSessionContext* pContext  )
{
   size_t rowNumber = 0;
   HashArray* pArray;
   bool keyFound = false;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pContext->pAllocator );
   VDS_ASSERT( pArray != NULL );

   if ( m_flag == HASH_CASE_INSENSITIVE )
      keyFound = FindKeyCI( pArray, pKey, keyLength, &rowNumber, pContext );
   else
      keyFound = FindKey( pArray, pKey, keyLength, &rowNumber, pContext );

   if ( keyFound )   
   {
      RowDescriptor* pRow = GET_PTR( pArray->Get(rowNumber), 
                                     RowDescriptor,
                                     pContext->pAllocator );

      m_totalSize -= pRow->dataLength;
      pRow->~RowDescriptor();
      vdseFree( pContext->pAllocator, pRow, &pContext->errorHandler );
      pArray->Set( rowNumber, NULL_OFFSET );
      m_numberOfRows--;

      /* The algorithm for deletions in a hash array look complex but */
      /* is in fact pretty basic: */
      /* Suppose you're deleting entry 13 and you have a chain of 5  */
      /* non-null entries, 13 to 17. */ 
      /* If we leave 13 empty and either one of the four others */
      /* hash to 13, then it won't be found in a search. */
      /* So, to be able to find this (say entry 14 hash to 14 but entry */
      /* 15 hash to 13) we have to move it into the empty slot. */
      /* Moving this guy to the empty slot creates a new empty slot */
      /* (15) and any subsequent entry in the chain must be checked */
      /* to make sure they don't hash to that new slot. If they do, */
      /* you have to move it... repeat the process until the end of */
      /* the chain of non-empty entries. */

      /* To complicate matter, the chain might wrape around the end of */
      /* the array (as in, 101, 102, 0, 1 if the array has 103 entries). */

      size_t curr; /* Current entry we are checking */
      size_t hole; /* Location of the hole */
      size_t hashValue; /* Hash value for current entry */

      curr = rowNumber;

      while( true )
      {
         pArray->Set( curr, NULL_OFFSET );
         hole = curr;

         do
         {
            curr++;
            if ( curr == m_arraySize )
               curr = 0;

            if( pArray->Get(curr) == NULL_OFFSET )
            {
               return LIST_OK;
            }
            
            pRow = GET_PTR( pArray->Get(curr), 
                            RowDescriptor,
                            pContext->pAllocator );
            char* pKey = GET_PTR( pRow->keyOffset, 
                                  char, 
                                  pContext->pAllocator );
            if ( m_flag == HASH_CASE_INSENSITIVE )
               hashValue =  HashAKeyCI( pKey, pRow->keyLength ) % m_arraySize;
            else
               hashValue =  HashAKey( pKey, pRow->keyLength ) % m_arraySize;

         } while( ((hole < hashValue) && (hashValue <= curr)) ||
                  ((curr < hole)  && (hole < hashValue)) ||
                  ((hashValue <= curr)  && (curr < hole)) );

         /* To explain this while condition:
          *
          * Curr < Hole    Hash value is in region:
          *
          *  _________     __
          * |         | 
          * |         |    No move (condition 3)
          * |         |
          * | Current |    __     
          * |         |
          * |         |    Move curr into hole
          * |         |
          * |  Hole   |    __
          * |         |
          * |         |    No move (condition 2)
          * |_________|    __

          * Curr > Hole    Hash value is in region:
          *
          *  _________     __
          * |         | 
          * |         |    Move curr into hole
          * |         |
          * |  Hole   |    __     
          * |         |
          * |         |    No move (condition 1)
          * |         |
          * | Current |    __
          * |         |
          * |         |    Move curr into hole
          * |_________|    __ 
          */

         pArray->Set( hole, pArray->Get(curr) );

      } /* end of infinite while (true) loop */

   } /* end if ( keyFound )    */


   return LIST_KEY_NOT_FOUND;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::ReplaceByKey( const char*     pKey,
                                        size_t          keyLength,
                                        void*           pData,
                                        size_t          dataLength,
                                        vdseSessionContext* pContext )
{
   size_t rowNumber = 0;
   bool keyFound = false;
   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pContext->pAllocator );
   VDS_ASSERT( pArray != NULL );
   
   if ( m_flag == HASH_CASE_INSENSITIVE )
      keyFound = FindKeyCI( pArray, pKey, keyLength, &rowNumber, pContext );
   else
      keyFound = FindKey( pArray, pKey, keyLength, &rowNumber, pContext );

   if ( keyFound )   
   {
      RowDescriptor* pOldRow = 
         GET_PTR( pArray->Get(rowNumber), 
                  RowDescriptor, 
                  pContext->pAllocator );

      /* Sadly, if ( dataLength == pOldRow->dataLength ) */
      /* we cannot just copy the new data into the row - this */
      /* is not "crash-recovery safe"!!! */

      /* mem alloc first - the only thing that might fail */
      RowDescriptor* pRow = (RowDescriptor*) 
         vdseMalloc( pContext->pAllocator, 
                     sizeof(RowDescriptor) + keyLength + dataLength,
                     &pContext->errorHandler );
      if ( pRow == NULL )
         return LIST_NO_MEMORY;

      /* initialize the new row */

      new (pRow) RowDescriptor;

      pRow->dataOffset = SET_OFFSET( (char*)pRow + sizeof(RowDescriptor),
                                     pContext->pAllocator );
      pRow->keyOffset  = SET_OFFSET( (char*)pRow + sizeof(RowDescriptor) + 
                                     dataLength,
                                     pContext->pAllocator );
   
      memcpy( (char*)pRow + sizeof(RowDescriptor) + dataLength,  pKey,  
              keyLength );
      memcpy( (char*)pRow + sizeof(RowDescriptor), pData, dataLength );
   
      pRow->keyLength = keyLength;
      pRow->dataLength = dataLength;
      
      /* Eliminate the old row */
      m_totalSize -= pOldRow->dataLength;
      pOldRow->~RowDescriptor();
      vdseFree( pContext->pAllocator, pOldRow, &pContext->errorHandler );

      m_totalSize += dataLength;

      pArray->Set( rowNumber, SET_OFFSET( pRow, pContext->pAllocator ) );

      return LIST_OK;

   }
   return LIST_KEY_NOT_FOUND;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum ListErrors HashList::ResizeArray( vdseSessionContext* pContext )
{
   size_t i, rowNumber = 0;

   HashArray* pArray;
   SET_PTR( pArray, m_arrayOffset, HashArray, pContext->pAllocator );
   VDS_ASSERT( pArray != NULL );

   size_t newSize = (m_arraySize * 100) / g_lowDensity + 1;

   fprintf( stderr, "Resizing array %d\n", m_arraySize );
   
   /* bug - new size should really be a prime number!!! */

   size_t len = sizeof(HashArray) - sizeof(BaseObject) + 
      newSize * sizeof(ptrdiff_t);
   fprintf( stderr, "Resizing array2 %d %d\n", newSize, len );

   HashArray* ptr = (HashArray*)
      vdseMalloc( pContext->pAllocator, len, &pContext->errorHandler );
   if ( ptr == NULL )
      return LIST_NO_MEMORY;

   new (ptr) HashArray;

   fprintf( stderr, "Resizing array3 %p %p\n", pArray, ptr );

   for ( i = 0; i < newSize; ++i)
      ptr->Set( i, NULL_OFFSET );

   for ( i = 0; i < m_arraySize; ++i)
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         RowDescriptor* pRow = GET_PTR( pArray->Get(i), 
                                        RowDescriptor,
                                        pContext->pAllocator );
         char* pKey = GET_PTR( pRow->keyOffset, char, pContext->pAllocator );
         
         if ( m_flag == HASH_CASE_INSENSITIVE )
            rowNumber =  HashAKeyCI( pKey, pRow->keyLength ) % newSize;
         else
            rowNumber =  HashAKey( pKey, pRow->keyLength ) % newSize;
         
         while (1)
         {
            if ( ptr->Get(rowNumber) == NULL_OFFSET )
            {
               /* We found a valid spot */
               ptr->Set( rowNumber, pArray->Get(i) );
               break;
            }
            ++rowNumber;
            
            if ( rowNumber == newSize )
               rowNumber = 0;
         }
      }
   }

   /* All pointers are copied - we can finish */
/*     m_zCurrentBuffer = m_arrayOffset; */
/*     m_zNewArray = SET_OFFSET(ptr,pContext->pAllocator); */
/*     m_nCurrentRowNumber = m_arraySize; */
/*     m_echeckPoint = eResize; */

/*     MemoryManager::Instance()->Sync(); */

   /* The order of the next two statements is important */
   m_arraySize = newSize;
   m_arrayOffset = SET_OFFSET( ptr, pContext->pAllocator );

   pArray->~HashArray();
   vdseFree( pContext->pAllocator, pArray, &pContext->errorHandler );

/*     m_echeckPoint = eOK; */
/*     m_zCurrentBuffer = NULL_OFFSET; */
/*     m_nCurrentRowNumber = (size_t) -1; */
/*     SelfTest(); */
   
   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*  void HashList::Recover( char**     pArrayBuffers, */
/*                          size_t*    pnNumOfAllocatedBuffer, */
/*                          vds_lock_T lockValue ) */
/*  { */
/*     HashArray* pArray; */
/*     SET_PTR( pArray, m_arrayOffset, HashArray ); */

/*     RowDescriptor* pRow; */

/*     if ( m_echeckPoint != eOK ) */
/*        fprintf( stderr, "HashList %d %d %d\n", */
/*                 m_echeckPoint, */
/*                 m_zCurrentBuffer, */
/*                 m_currentRowNumber ); */

/*     switch( m_echeckPoint ) */
/*     { */
/*     case eOK: */

/*        break; */
      
/*     case eInitialize: */
/*     { */
/*        HashArray* ptr; */
      
/*        if ( m_zCurrentBuffer == NULL_OFFSET ) */
/*        { */
/*           ptr = (HashArray*) pContext->pAllocator->Malloc(  */
/*              m_currentRowNumber * sizeof ( ptrdiff_t ), lockValue ); */
/*           if ( ptr == NULL ) */
/*              break; */
/*           m_arrayOffset = SET_OFFSET( ptr, pContext->pAllocator ); */
/*        } */
/*        else */
/*           ptr = GET_PTR( m_zCurrentBuffer, HashArray ); */

/*        m_arrayOffset = SET_OFFSET( ptr, pContext->pAllocator ); */
/*        for ( unsigned int i = 0; i < m_currentRowNumber; ++i) */
/*           ptr[i] = NULL_OFFSET; */
/*        m_arraySize = m_currentRowNumber; */

/*        break; */
/*     } */
   
/*     case eClose: */

/*        if ( m_arrayOffset != NULL_OFFSET ) */
/*        { */
/*           // Is the array still in the memory allocator */
/*           if ( ! pAllocator->IsBufferFree( pArray ) ) */
/*           { */
/*              for ( size_t i = 0; i < *pnNumOfAllocatedBuffer; ++i ) */
/*              { */
/*                 if ( (char*)pArray  == pArrayBuffers[i] ) */
/*                 { */
/*                    pArrayBuffers[i] = NULL; */
/*                    if ( i != *pnNumOfAllocatedBuffer-1 ) */
/*                    { */
/*                       pArrayBuffers[i] =  */
/*                          pArrayBuffers[*pnNumOfAllocatedBuffer-1]; */
/*                       pArrayBuffers[*pnNumOfAllocatedBuffer-1] = NULL; */
/*                    } */
/*                    (*pnNumOfAllocatedBuffer)--; */
         
/*                    break; */
/*                 } */
/*              } */
/*              pContext->pAllocator->Free( pArray, lockValue ); */
/*           } */
/*           m_arrayOffset = NULL_OFFSET; */
/*           pArray = NULL; */
/*        } */
      
/*        break; */

/*     case eEmpty: */

/*        if ( m_currentRowNumber < m_arraySize ) */
/*        { */
/*           size_t rowNumber = m_currentRowNumber; */
/*           if ( pArray[m_currentRowNumber] != NULL_OFFSET ) */
/*           {          */
/*              pRow = GET_PTR( pArray[m_currentRowNumber], RowDescriptor ); */
/*              if ( ! pContext->pAllocator->IsBufferFree( pRow ) ) */
/*              { */
/*                 for ( size_t i = 0; i < *pnNumOfAllocatedBuffer; ++i ) */
/*                 { */
/*                    if ( (char*)pArray  == pArrayBuffers[i] ) */
/*                    { */
/*                       pArrayBuffers[i] = NULL; */
/*                       if ( i != *pnNumOfAllocatedBuffer-1 ) */
/*                       { */
/*                          pArrayBuffers[i] =  */
/*                             pArrayBuffers[*pnNumOfAllocatedBuffer-1]; */
/*                          pArrayBuffers[*pnNumOfAllocatedBuffer-1] = NULL; */
/*                       } */
/*                       (*pnNumOfAllocatedBuffer)--; */
         
/*                       break; */
/*                    } */
/*                 } */
/*                 pContext->pAllocator->Free( pRow, lockValue ); */
/*              } */
/*              pArray[m_currentRowNumber] = NULL_OFFSET; */
/*           } */
/*           for ( m_currentRowNumber = rowNumber+1; */
/*                 m_currentRowNumber < m_arraySize; ++m_currentRowNumber ) */
/*           { */
/*              if ( pArray[m_currentRowNumber] != NULL_OFFSET ) */
/*              {       */   
/*                 RowDescriptor* pRow = GET_PTR( pArray[m_currentRowNumber],  */
/*                                                RowDescriptor ); */
/*                 for ( size_t i = 0; i < *pnNumOfAllocatedBuffer; ++i ) */
/*                 { */
/*                    if ( (char*)pArray  == pArrayBuffers[i] ) */
/*                    { */
/*                       pArrayBuffers[i] = NULL; */
/*                       if ( i != *pnNumOfAllocatedBuffer-1 ) */
/*                       { */
/*                          pArrayBuffers[i] =  */
/*                             pArrayBuffers[*pnNumOfAllocatedBuffer-1]; */
/*                          pArrayBuffers[*pnNumOfAllocatedBuffer-1] = NULL; */
/*                       } */
/*                       (*pnNumOfAllocatedBuffer)--; */
         
/*                       break; */
/*                    } */
/*                 } */
/*                 pContext->pAllocator->Free( pRow, lockValue ); */
/*                 pArray[m_currentRowNumber] = NULL_OFFSET; */
/*              } */
/*           } */
/*        } */
/*        break; */
      
/*     case eDelete: */
/*     { */
/*        if ( pArray[m_currentRowNumber] != NULL_OFFSET ) */
/*        { */
/*           // Attempt to delete the row */
/*           pRow = GET_PTR( pArray[m_currentRowNumber], RowDescriptor ); */
/*           if ( ! pContext->pAllocator->IsBufferFree( pRow ) ) */
/*           { */
/*              for ( size_t i = 0; i < *pnNumOfAllocatedBuffer; ++i ) */
/*              { */
/*                 if ( (char*)pArray  == pArrayBuffers[i] ) */
/*                 { */
/*                    pArrayBuffers[i] = NULL; */
/*                    if ( i != *pnNumOfAllocatedBuffer-1 ) */
/*                    { */
/*                       pArrayBuffers[i] =  */
/*                          pArrayBuffers[*pnNumOfAllocatedBuffer-1]; */
/*                       pArrayBuffers[*pnNumOfAllocatedBuffer-1] = NULL; */
/*                    } */
/*                    (*pnNumOfAllocatedBuffer)--; */
         
/*                    break; */
/*                 } */
/*              } */
/*              pContext->pAllocator->Free( pRow, lockValue ); */
/*           } */
         
/*           pArray[m_currentRowNumber] = NULL_OFFSET; */
/*        } */

/*        // And repeat the whole procedure... */

/*        size_t curr; // Current entry we are checking */
/*        size_t hole; // Location of the hole */
/*        size_t hash; // Hash value for current entry */

/*        bool bBreak = false; // To get out of the two loops */
      
/*        curr = m_currentRowNumber; */
      
/*        for(;;) */
/*        { */
/*           pArray[curr] = NULL_OFFSET; */
/*           hole = curr; */

/*           do */
/*           { */
/*              curr++; */
/*              if ( curr == m_arraySize ) */
/*                 curr = 0; */

/*              if( pArray[curr] == NULL_OFFSET ) */
/*              { */
/*                 bBreak = true; */
/*                 break; */
/*              } */
            
//              pRow = GET_PTR( pArray[curr], RowDescriptor );
//              char* pKey = GET_PTR( pRow->keyOffset, char );
//              if ( m_flag == HASH_CASE_INSENSITIVE )
//                 hash =  HashAKeyCI( pKey, pRow->keyLength ) % m_arraySize;
//              else
//                 hash =  HashAKey( pKey, pRow->keyLength ) % m_arraySize;

//           } while( ((hole < hash) && (hash <= curr)) ||
//                    ((curr < hole)  && (hole < hash)) ||
//                    ((hash <= curr)  && (curr < hole)) );

//           if ( bBreak )
//              break;
         
//           pArray[hole] = pArray[curr];
//        }
//        break;
//     }
   
//     case eResize:

//        if ( m_arrayOffset == m_zCurrentBuffer )
//           m_arraySize = m_currentRowNumber;
      
//        break;

//     case eInsert:
//     case eReplace:

//        if ( pArray[m_currentRowNumber] != m_zCurrentBuffer )
//           pArray[m_currentRowNumber] = m_zCurrentBuffer;
      
//        break;
//     } // End of switch statement

//     // #of rows and size might be invalid! Recalculate them
//     if ( m_echeckPoint != eOK && pArray != NULL )
//     {
//        m_numberOfRows = 0;
//        m_totalSize = 0;
//        for ( size_t i = 0; i < m_arraySize; ++i )
//        {
//           if ( pArray[i] != NULL_OFFSET )
//           {
//              m_numberOfRows++;
//              pRow  = GET_PTR( pArray[i], RowDescriptor );
//              m_totalSize += pRow->dataLength;
//           }
//        }
//     }
   
//     size_t ijk = *pnNumOfAllocatedBuffer; */
   
//     if ( m_arrayOffset != NULL_OFFSET ) */
//     { */
//  //      fprintf(stderr, " hash list array = %p\n", pArray ); */
      
//        // The main array */
//        for ( size_t i = 0; i < *pnNumOfAllocatedBuffer; ++i ) */
//        { */
//           if ( (char*)pArray  == pArrayBuffers[i] ) */
//           { */
//              pArrayBuffers[i] = NULL; */
//              if ( i != *pnNumOfAllocatedBuffer-1 ) */
//              { */
//                 pArrayBuffers[i] =  */
//                    pArrayBuffers[*pnNumOfAllocatedBuffer-1]; */
//                 pArrayBuffers[*pnNumOfAllocatedBuffer-1] = NULL; */
//              } */
//              (*pnNumOfAllocatedBuffer)--; */
         
//              break; */
//           } */
//        } */
//        // the rows
//        for ( size_t n = 0; n < m_arraySize; ++n ) */
//        { */
//           if ( pArray[n] != NULL_OFFSET ) */
//           { */
//              pRow  = GET_PTR( pArray[n], RowDescriptor ); */
//              for ( size_t i = 0; i < *pnNumOfAllocatedBuffer; ++i )
//              {
//                 if ( (char*)pRow  == pArrayBuffers[i] )
//                 {
//                    pArrayBuffers[i] = NULL;
//                    if ( i != *pnNumOfAllocatedBuffer-1 )
//                    {
//                       pArrayBuffers[i] = 
//                          pArrayBuffers[*pnNumOfAllocatedBuffer-1];
//                       pArrayBuffers[*pnNumOfAllocatedBuffer-1] = NULL;
//                    } */
//                    (*pnNumOfAllocatedBuffer)--; */
         
//                    break; */
//                 } */
//              } */
//           } */
//        } */
//     } */
//     if ( ijk != (*pnNumOfAllocatedBuffer)+m_numberOfRows+1 ) */
//        fprintf( stderr, "ijk = %d %d %d\n", ijk, (*pnNumOfAllocatedBuffer), */
//                 m_numberOfRows ); */

//     m_echeckPoint = eOK; */
//     m_zCurrentBuffer =  NULL_OFFSET; */
//     m_currentRowNumber = (size_t) -1; */
//  } */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void HashList::DeleteEntry( size_t rowNumber, 
                            vdseMemAlloc* pAlloc )
{
   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pAlloc );
   VDS_ASSERT( pArray != NULL );

   pArray->Set( rowNumber, NULL_OFFSET );
   m_numberOfRows--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Rehash the whole array back into the current array */
int HashList::ResetArray( vdseMemAlloc* pAlloc )
{
   size_t i, rowNumber = 0;
   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pAlloc );
   VDS_ASSERT( pArray != NULL );

   /* Create a temporary array on the heap to hold the pointers */
   ptrdiff_t* ptr = (ptrdiff_t*) 
      malloc( m_arraySize * sizeof ( ptrdiff_t ) );
   if ( ptr == NULL )
      return -1;

   /* Copy to the temp array */
   for ( i = 0; i < m_arraySize; ++i)
      ptr[i] = pArray->Get(i);

   /* Clear the existing array */
   for ( i = 0; i < m_arraySize; ++i)
      pArray->Set( i, NULL_OFFSET );
   
   /* Repopulate the array using our hash function */
   for ( i = 0; i < m_arraySize; ++i)
   {
      if ( ptr[i] != NULL_OFFSET )
      {
         RowDescriptor* pRow = GET_PTR( ptr[i], 
                                        RowDescriptor,
                                        pAlloc );
         char* pKey = GET_PTR( pRow->keyOffset, char, pAlloc );
         
         if ( m_flag == HASH_CASE_INSENSITIVE )
            rowNumber =  HashAKeyCI( pKey, pRow->keyLength ) % m_arraySize;
         else
            rowNumber =  HashAKey( pKey, pRow->keyLength ) % m_arraySize;
         
         while (1)
         {
            if ( pArray->Get(rowNumber) == NULL_OFFSET )
            {
               /* We found a valid spot */
               pArray->Set( rowNumber, ptr[i] );
               break;
            }
            ++rowNumber;
            
            if ( rowNumber == m_arraySize )
               rowNumber = 0;
         }
      }
   }
   free( ptr );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool HashList::SelfTest( vdseMemAlloc* pAlloc )
{
   size_t i, j, rowNumber, numRows = 0, sum = 0;
   HashArray* pArray;
   
   SET_PTR( pArray, m_arrayOffset, HashArray, pAlloc );
   VDS_ASSERT( pArray != NULL );

   for ( i = 0; i < m_arraySize; ++i)
   {
      if ( pArray->Get(i) != NULL_OFFSET )
      {
         RowDescriptor* pRow = GET_PTR( pArray->Get(i), 
                                        RowDescriptor,
                                        pAlloc );
         char* pKey = GET_PTR( pRow->keyOffset, char, pAlloc );

         if ( pRow->keyLength > MAX_KEY_LENGTH )
            fprintf( stderr, "Wrong key length\n" );
         if ( pRow->dataLength != 8 )
            fprintf( stderr, "Wrong data length\n" );
      
         numRows++;
         sum += pRow->dataLength;
         
         /* Check the key */
         if ( m_flag == HASH_CASE_INSENSITIVE )
            rowNumber =  HashAKeyCI( pKey, pRow->keyLength ) % m_arraySize;
         else
            rowNumber =  HashAKey( pKey, pRow->keyLength ) % m_arraySize;
         
         if ( rowNumber == i ) continue;
         
         /* The row is at "i" but the key hashes to "rowNumber" - which means */
         /* that there was something at rowNumber already if all is well (and */
         /* at rowNumber+1, rowNumber+2, etc. if it applies) */
         /* Let's see... */

         j = rowNumber;
         while ( j != i )
         {
            if ( pArray->Get(j) == NULL_OFFSET )
            {
               fprintf( stderr, "Key at wrong place in hash list\n" );
               VDS_ASSERT( 0 );
               return false;
            }
            j++;
            if ( j == m_arraySize )
               j = 0;
         }
      }
   }
   if ( numRows != m_numberOfRows )
      fprintf( stderr, "Invalid number of rows %d %d\n", numRows, m_numberOfRows );
   if ( sum != m_totalSize )
      fprintf( stderr, "Invalid total sum %d %d \n", sum, m_totalSize );

   m_numberOfRows = numRows;
   m_totalSize = sum;
   
   return BaseObject::SelfTest();
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
