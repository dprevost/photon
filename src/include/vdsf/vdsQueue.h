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

#ifndef VDS_QUEUE_H
#define VDS_QUEUE_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include <stdlib.h>
#include <vdsf/vdsCommon.h>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Forward declarations
class vdsQueueItem;
class vdsSession;
class vdsProxyObject;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class vdsQueue
{
public:

   vdsQueue( vdsSession &session );

   virtual ~vdsQueue();

   int Open( const char* queueName );
   
   int Close();
   
   int InsertItem( const void* pItem, size_t length );
   
   int RemoveItem( vdsQueueItem* pQueueItem );
 
   int RemoveItem( vdsQueueItem& queueItem );

   int RemoveItem( void* pItem, size_t length );

   /// GetItem does not remove the items from the queue.
   int GetItem( vdsIteratorType flag, void* pItem, size_t length );

   /// A struct might be better here
   int Status( size_t *pNumValidItems, 
               size_t *pNumTotalItems );
   
private:

   vdsProxyObject* m_pProxyObject;
   
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif /* VDS_QUEUE_H */
