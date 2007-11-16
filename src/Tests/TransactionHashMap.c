/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"

#include <vdsf/vds.h>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

VDS_HANDLE g_session1, g_session2;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* 
 * This program tests the transaction capabilities of VDS (commit, rollback,
 * etc.).
 *
 * Shorthand:
 *
 * O1, O2 : Object 1, Object 2
 * S1, S2 : Session 1, 2
 *
 * test cases:
 * 
 *  1) create object in S1, attempt to open it from S1 (should succeed)
 *  2) create object in S1, attempt to open it from S2 (should fail)
 *  3) create object in S1, commit S1, attempt to open it from S2
 *     (should succeed)
 *  4) create object in S1, rollback S1, attempt to open it from S1
 *     (should fail) 
 *  5) create object in S1, commit S1, open it with S2, remove it with S1
 *     (remove should succeed)
 *  -- for 6 to 10, initialize by create object in S1, commit S1
 *  6) remove object in S1, attempt to open it from S1 (should fail)
 *  7) remove object in S1, attempt to open it from S2 (should succeed)
 *  8) remove object in S1, commit S1, attempt to open it from S2
 *     (should fail)
 *  9) remove object in S1, rollback S1, attempt to open it from S1
 *     (should fail - if S1 is implicit, rollback should fail) 
 * 10) remove object in S1, rollback S1, attempt to open it from S2
 *     (should fail - if S1 is implicit, rollback should fail) 
 * 11) create folder in S1, create object in folder in S2 (should fail)
 * 12) create folder in S1, commit S1, create object in folder in S2 
 *     (should succeed)
 * 13) create folder in S1, rollback S1, create object in folder in S2 
 *     (should fail)
 *
 * 21) insert in O1 (S1), attempt to get data from O1 (S1) (should succeed)
 * 22) insert in O1 (S1), attempt to get data from O2 (S1) (  "      "    )
 * 23) insert in O1 (S1), attempt to get data from O2 (S2) (should fail)
 * 24) insert in O1 (S1), commit S1, attempt to get data from O2 (S2)
 *     (should succeed)
 * 25) insert in O1 (S1), rollback S1, attempt to get data from O1 (S1)
 *     (should fail - if S1 is implicit, rollback should fail)
 *  -- for 26 to 33, initialize by insert in O1(S1), commit S1
 * 26) remove in O1 (S1), attempt to get data from O1 (S1) (should fail)
 * 27) remove in O1 (S1), attempt to get data from O2 (S1) (  "      "    )
 * 28) remove in O1 (S1), attempt to get data from O2 (S2) (should fail)
 * 29) remove in O1 (S1), commit S1, attempt to get data from O2 (S2)
 *     (should fail)
 * 30) remove in O1 (S1), rollback S1, attempt to get data from O1 (S1)
 *     (should succeed)
 * 31) remove in O1 (S1), rollback S1, attempt to get data from O2 (S1)
 *     (should succeed)
 * 32) remove in O1 (S1), rollback S1, attempt to get data from O2 (S2)
 *     (should succeed)
 *
 *
 *
 *
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test1()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errcode );
      goto end;
   }

end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );
   
   if ( errcode != 0 )
      return -1;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test2()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errcode );
      goto end;
   }
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT, 
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = VDS_OK;

end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -2;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test3()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errcode );
      goto end;
   }

end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -3;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test4()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT, 
              errcode );
       errcode = -1;
      goto end;
   }
   errcode = 0;
   
end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -4;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test5()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errcode );
      goto end;
   }
   
end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -5;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test6()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL; 
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsDestroyObject( g_session1, "A Map", strlen("A Map") );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT,
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;
   
end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -6;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test7()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL; 
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsDestroyObject( g_session1, "A Map", strlen("A Map") );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT,
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;
   
end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -7;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test8()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsDestroyObject( g_session1, "A Map", strlen("A Map") );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_NO_SUCH_OBJECT )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_OBJECT,
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;
   
end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -8;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test9()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsDestroyObject( g_session1, "A Map", strlen("A Map") );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errcode );
      goto end;
   }
   
end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -9;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test10()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsDestroyObject( g_session1, "A Map", strlen("A Map") );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errcode );
      goto end;
   }
   
end:

   if ( h1 != NULL ) vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -10;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test11()
{
   int errcode = 0;

   errcode = vdsCreateObject( g_session1, "A Folder", strlen("A Folder"), VDS_FOLDER );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsCreateObject( g_session2, "A Folder/A Queue", strlen("A Folder/A Queue"), VDS_HASH_MAP );
   if ( errcode != VDS_NO_SUCH_FOLDER )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_FOLDER,
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;

end:

   vdsDestroyObject( g_session2, "A Folder/A Queue", strlen("A Folder/A Queue") ); 
   vdsCommit( g_session2 );
   vdsDestroyObject( g_session1, "A Folder", strlen("A Folder") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -11;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test12()
{
   int errcode = 0;

   errcode = vdsCreateObject( g_session1, "A Folder", strlen("A Folder"), VDS_FOLDER );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsCreateObject( g_session2, "A Folder/A Queue", strlen("A Folder/A Queue"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errcode );
      goto end;
   }

end:

   vdsDestroyObject( g_session2, "A Folder/A Queue", strlen("A Folder/A Queue") ); 
   vdsCommit( g_session2 );
   vdsDestroyObject( g_session1, "A Folder", strlen("A Folder") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -12;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test13()
{
   int errcode = 0;

   errcode = vdsCreateObject( g_session1, "A Folder", strlen("A Folder"), VDS_FOLDER );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsCreateObject( g_session2, "A Folder/A Queue", strlen("A Folder/A Queue"), VDS_HASH_MAP );
   if ( errcode != VDS_NO_SUCH_FOLDER )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_FOLDER,
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;

end:

   vdsDestroyObject( g_session2, "A Folder/A Queue", strlen("A Folder/A Queue") ); 
   vdsCommit( g_session2 );
   vdsDestroyObject( g_session1, "A Folder", strlen("A Folder") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -13;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test21()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h1, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK,
              errcode );
      goto end;
   }

end:

   vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -21;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test22()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";

   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errcode );
      goto end;
   }

end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -22;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test23()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   // So that the object can be open by session 2
   errcode = vdsCommit( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_ITEM, 
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;
   
end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -23;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test24()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   // So that the object can be open by session 2
   errcode = vdsCommit( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errcode );
      goto end;
   }

end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session2 );
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -24;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test25()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );   
   if ( errcode != VDS_OK )
      goto end;
   // So that the object can be open by session 2
   errcode = vdsCommit( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h1,  key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_ITEM, 
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;
   
end:

   vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -25;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test26()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapDelete( h1, key1, strlen(key1) );
   if ( errcode != VDS_OK )
      goto end;   

   // A bit trivial...
   errcode = vdsHashMapGet( h1, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_ITEM, 
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;
   
end:

   vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -26;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test27()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapDelete( h1, key1, strlen(key1) );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_ITEM, 
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;

end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -27;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test28()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapDelete( h1, key1, strlen(key1) );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_ITEM, 
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;

end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -28;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test29()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapDelete( h1, key1, strlen(key1) );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_NO_SUCH_ITEM )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_NO_SUCH_ITEM, 
              errcode );
      errcode = -1;
      goto end;
   }
   errcode = 0;

end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -29;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test30()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapDelete( h1, key1, strlen(key1) );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapGet( h1, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errcode );
      goto end;
   }

end:

   vdsHashMapClose( h1 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -30;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test31()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapDelete( h1, key1, strlen(key1) );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errcode );
      goto end;
   }

end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );

   if ( errcode != 0 )
      return -31;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Test32()
{
   int errcode = 0;
   VDS_HANDLE h1 = NULL, h2 = NULL;
   char str[25];
   size_t returnLength;
   char * key1 = "Key 1";
   
   errcode = vdsCreateObject( g_session1, "A Map", strlen("A Map"), VDS_HASH_MAP );
   if ( errcode != VDS_OK )
      goto end;
   
   errcode = vdsHashMapOpen( g_session1, "A Map", strlen("A Map"), &h1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapInsert( h1, key1, strlen(key1), "1234567890123", 14 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsCommit( g_session1 );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsHashMapOpen( g_session2, "A Map", strlen("A Map"), &h2 );
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapDelete( h1, key1, strlen(key1) );
   if ( errcode != VDS_OK )
      goto end;
   errcode = vdsRollback( g_session1 ); 
   if ( errcode != VDS_OK )
      goto end;

   errcode = vdsHashMapGet( h2, key1, strlen(key1), str, 25, &returnLength );
   if ( errcode != VDS_OK )
   {
      printf( " Expected error = %d, returned error = %d\n", 
              VDS_OK, 
              errcode );
      goto end;
   }

end:

   vdsHashMapClose( h1 );
   vdsHashMapClose( h2 );
   vdsDestroyObject( g_session1, "A Map", strlen("A Map") ); 
   vdsCommit( g_session1 );
   vdsCommit( g_session2 );

   if ( errcode != 0 )
      return -32;
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Run()
{
   int errcode = 0;

   printf( " Executing test 1\n" );
   if ( (errcode = Test1())  != 0 ) return errcode;
   printf( " Executing test 2\n" );
   if ( (errcode = Test2())  != 0 ) return errcode;
   printf( " Executing test 3\n" );
   if ( (errcode = Test3())  != 0 ) return errcode;
   printf( " Executing test 4\n" );
   if ( (errcode = Test4()) != 0 ) return errcode;
   printf( " Executing test 5\n" );
   if ( (errcode = Test5()) != 0 ) return errcode;
   printf( " Executing test 6\n" );
   if ( (errcode = Test6()) != 0 ) return errcode;
   printf( " Executing test 7\n" );
   if ( (errcode = Test7()) != 0 ) return errcode;
   printf( " Executing test 8\n" );
   if ( (errcode = Test8()) != 0 ) return errcode;
   printf( " Executing test 9\n" );
   if ( (errcode = Test9()) != 0 ) return errcode;
   printf( " Executing test 10\n" );
   if ( (errcode = Test10()) != 0 ) return errcode;
   printf( " Executing test 11\n" );
   if ( (errcode = Test11())  != 0 ) return errcode;
   printf( " Executing test 12\n" );
   if ( (errcode = Test12())  != 0 ) return errcode;
   printf( " Executing test 13\n" );
   if ( (errcode = Test13()) != 0 ) return errcode;

   printf( " Executing test 21\n" );
   if ( (errcode = Test21())  != 0 ) return errcode;
   printf( " Executing test 22\n" );
   if ( (errcode = Test22())  != 0 ) return errcode;
   printf( " Executing test 23\n" );
   if ( (errcode = Test23())  != 0 ) return errcode;
   printf( " Executing test 24\n" );
   if ( (errcode = Test24())  != 0 ) return errcode;
   printf( " Executing test 25\n" );
   if ( (errcode = Test25())  != 0 ) return errcode;

   printf( " Executing test 26\n" );
   if ( (errcode = Test26())  != 0 ) return errcode;
   printf( " Executing test 27\n" );
   if ( (errcode = Test27())  != 0 ) return errcode;
   printf( " Executing test 28\n" );
   if ( (errcode = Test28())  != 0 ) return errcode;
   printf( " Executing test 29\n" );
   if ( (errcode = Test29())  != 0 ) return errcode;
   printf( " Executing test 30\n" );
   if ( (errcode = Test30())  != 0 ) return errcode;
   printf( " Executing test 31\n" );
   if ( (errcode = Test31())  != 0 ) return errcode;
   printf( " Executing test 32\n" );
   if ( (errcode = Test32())  != 0 ) return errcode;

   return errcode;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main(int argc, char *argv[]) 
{
   int errcode = 0;

   if ( argc != 2 )
   {
      printf( " Usage: %sWatchdogAddress \n", argv[0] );
      return -1;
   }

   errcode = vdsInit( argv[1], 0 );   
   if ( errcode != VDS_OK )
   {
      printf( " Error opening VDS = %d\n", errcode );
      return -1;
   }
   
   errcode = vdsInitSession( &g_session1 );
   if ( errcode != VDS_OK )
   {
      printf( " Error opening session 1 = %d\n", errcode );
      return -1;
   }
   errcode = vdsInitSession( &g_session2 );
   if ( errcode != VDS_OK )
   {
      printf( " Error opening session 2 = %d\n", errcode );
      return -1;
   }
   
   if ( (errcode = Run()) != 0 ) goto error;
   
   printf( "Terminating normally\n" );

   return 0;

error:
   fprintf( stderr, " Error in test %d, Aborting anormally\n", errcode ); 

   return -1;
}

