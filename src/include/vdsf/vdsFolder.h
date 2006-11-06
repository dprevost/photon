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

#ifndef VDS_FOLDER_H
#define VDS_FOLDER_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Forward declarations
class vdsFolderItem;
class vdsSession;
class vdsProxyObject;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

class vdsFolder
{
public:

   vdsFolder( vdsSession &session );

   virtual ~vdsFolder();

   int Open( const char* folderName );
   
   int Close();

   int GetFirstItem( vdsFolderItem* pFolderItem );

   int GetNextItem( vdsFolderItem* pFolderItem );

   int RemoveAll();
   
//   int ListFolderContent();

private:

   vdsProxyObject* m_pProxyObject;
   
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif /* VDS_FOLDER_H */
