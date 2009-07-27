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

package org.photon.serializer;

import java.io.ObjectOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.ObjectInputStream;
import java.io.ByteArrayInputStream;
import java.io.Serializable;
import java.io.IOException;

public class JavaSerializer<T extends Serializable> implements PSOSerialize<T> {
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
   public JavaSerializer() {}
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public byte[] packObject( T obj ) throws SerializerException {

      ByteArrayOutputStream stream;
      ObjectOutputStream out;
      byte [] data;
      
      stream = new ByteArrayOutputStream();
      try {
         out = new ObjectOutputStream(stream);
         out.writeObject(obj);
         out.close();
      }
      catch( IOException e ) {
         throw new SerializerException( 
            e.getCause(), 
            SerializerErrors.IO_EXCEPTION );
      }
      
      data = stream.toByteArray();
      
      return data;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void setDefinition( byte[] dataDef ) {}

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   @SuppressWarnings("unchecked")
   public T unpackObject( byte[] buffer ) throws SerializerException {

      ByteArrayInputStream stream;
      ObjectInputStream in;
      T obj;
      
      stream = new ByteArrayInputStream( buffer );
      
      try {
         in = new ObjectInputStream( stream );
      
         // Unchecked cast. In theory, readObject might return an object 
         // which is not of type T. We could make sure that the result is
         // correct by using reflection (getting the class type) but this
         // would be slow.
         obj = (T)in.readObject();
         in.close();
      }
      catch( IOException e ) {
         throw new SerializerException( 
            e.getCause(), 
            SerializerErrors.IO_EXCEPTION );
      }
      catch( ClassNotFoundException e ) {
         throw new SerializerException( 
            e.getCause(), 
            SerializerErrors.CLASS_NOT_FOUND_EXCEPTION );
      }

      return obj;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}
