/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

package org.photon;

import java.io.ObjectOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.ObjectInputStream;
import java.io.ByteArrayInputStream;

public class JavaSerialize<T extends Serializable> implements PSOSerialize {
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
   public JavaSerialize() {}
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public byte[] packObject() {

      ByteArrayOutputStream stream;
      ObjectOutputStream out;
      byte [] data;
      
      stream = new ByteArrayOutputStream;
      out = new ObjectOutputStream(stream);
      out.writeObject(this);
      out.close();
      
      data = stream.toByteArray();
      
      return data;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public T unpackObject( byte[] buffer ) {

      ByteArrayInputStream stream;
      ObjectInputStream in;
    
      stream = new ByteArrayInputStream( buffer );
      in = new ObjectInputStream( stream );
      T = (T)in.readObject();
      in.close();
      
      return T;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
