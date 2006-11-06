/* -*- c++ -*- */
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#ifndef VDS_SESSION_H
#define VDS_SESSION_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include <stdlib.h>
#include <vdsf/vdsCommon.h>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Forward declaration
class vdsSessionHook;
class vdsProxyObject;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class vdsSession
{
public:
   
   // Two types of transaction management, implicit and explicit.
   //
   // - Implicit transactions, as the name indicates, hides all transactions
   //   from the application. The underlying framework will periodically
   //   save the current work at predetermined "check points".
   //   The method Commit can be called to save the current work but 
   //   Rollback is unavailable.
   //   Upon normal termination, the current work is automatically saved.
   //
   // - Explicit transactions are ideal for real-time processing. Contrary
   //   to some other transaction management software, every call made 
   //   in such a session is part of the transaction (in other words a new
   //   transaction is started immediately after either Commit or RollBack
   //   was called).
   //   Upon normal termination, the current transaction is rolled back.

   // For more information, see the user's manual.

   vdsSession( bool explicitTransaction = false );

   virtual ~vdsSession();
   
   int CreateObject( const char*   objectName,
                     vdsObjectType objectType );
   
   int DestroyObject( const char* objectName );

   int Commit();
   
   int Rollback();
   
private:

   vdsSessionHook* m_pSessionHook;
   
   friend class vdsProxyObject;
   
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif /* VDS_SESSION_H */
