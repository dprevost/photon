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

#ifndef VDS_VALIDATE_H
#define VDS_VALIDATE_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Forward declaration
struct ConfigParams;
struct MemoryHeader;
class  CleanupSession;
class  MemoryManager;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/**
 *  This class initialize the VDS. 
 *    - it will create the VDS if it does not exist.
 *    - it will open it otherwise and make sure that its content is valid
 */

class VdsHandler
{

public:

   VdsHandler();
   
   ~VdsHandler();

   void HandleCrash( pid_t pid );

   int Init( struct ConfigParams * pConfig,
             struct MemoryHeader** ppMemoryAddress );
   
private:   

   void CleanSession( CleanupSession* pSession );
   
   int ValidateVDS();
   
   struct ConfigParams * m_pConfig;

   MemoryManager * m_pMemManager;

   MemoryHeader* m_pMemHeader;
   
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif /* VDS_VALIDATE_H */
