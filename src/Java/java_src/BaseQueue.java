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

import java.lang.*;
import java.util.*;

/**
 * Base (super) Queue class for the Photon library.
 *
 * This class manipulates the data as an array of bytes; it does not 
 * attempt to interpret the data as a java object or some other construct.
 * <p>
 * Although this class can be used directly, it is recommended to use it 
 * as a superclass. See the Photon Queue class as an example.
 */
public class BaseQueue {

   /** To save the native pointer/handle of the C struct. */
   protected long handle = 0;

   private Session session;

   private DataDefinition definition;
   
   private String name;
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   static {
      initIDs();
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * A default constructor. You must use open() to access a FIFO queue in 
    * shared memory.
    */
   public BaseQueue() {}
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Open an existing FIFO queue.
    * <p>
    * @param session The session we belong to.
    * @param name    The fully qualified name of the queue. 
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public BaseQueue( Session session, String name ) throws PhotonException {
      
      int errcode;
      
      errcode = psoOpen( session, name );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }

      /*
java.lang.reflect
Class Constructor<T>
public T newInstance(Object... initargs)
              throws InstantiationException,
                     IllegalAccessException,
                     IllegalArgumentException,
                     InvocationTargetException
                     
--------------------
Foo.class
---------------
forName

public static Class<?> forName(String className)
                        throws ClassNotFoundException
                        

                     
                     */
      
      
      this.session = session;
      this.name = name;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Close a queue.
    *
    * This function terminates the current access to the queue in shared 
    * memory - the queue itself is untouched.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void close() { 
      psoClose(handle);
      handle = 0;
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Cleanup the object before GC.
    */
   protected void finalize() throws Throwable {     
      
      try {
         psoClose(handle);
      } finally {
         handle = 0;
         super.finalize();
      }
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Retrieve the data definition of the queue.
    *
    * @return A new data definition of the queue.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public DataDefinition getDataDefinition() throws PhotonException {

      int errcode;
      DataDefinition definition;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }
      definition = new DataDefinition();
      
      errcode = psoDataDefinition( handle, definition );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      
      return definition;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** 
    * Initiates an iteratation through the queue - no data items are 
    * removed from the queue by this function.
    *
    * @param buffer The buffer provided by the user to hold the content 
    *               of the first element.
    *
    * @return 0 on success or IS_EMPTY if the queue is empty.
    *
    * @exception PhotonException On an error with the Photon library.
    */

   public int getFirstRaw( byte [] buffer ) throws PhotonException {
      
      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetFirst( handle, buffer );
      if ( errcode == 0 || errcode == PhotonErrors.IS_EMPTY.getErrorNumber() ) { 
         return errcode;
      }
      
      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /** 
    * Iterate through the queue - no data items are removed from the queue
    * by this function.
    * <p>
    * Evidently, you must call ::getFirst to initialize the iterator. 
    * Not so evident - calling ::pop will reset the iteration to the
    * last element (they use the same internal storage). If this cause a 
    * problem, please let us know.
    *
    * @param buffer The buffer provided by the user to hold the content of
    *               the next element.
    *
    * @return 0 on success or REACHED_THE_END when the iteration reaches
    *           the end of the queue.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public int getNextRaw( byte[] buffer ) throws PhotonException {

      int errcode;

      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetNext( handle, buffer );
      if ( errcode == 0 || errcode == PhotonErrors.REACHED_THE_END.getErrorNumber() ) { 
         return errcode;
      }
      
      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Retrieve the data definition of the last accessed record.
    * <p>
    * Queues will usually contain data records with an identical layout (data 
    * definition of the items). This layout was defined when the queue was 
    * created. 
    * <p>
    * You can also insert and retrieve data records with different layouts if
    * the object was created with the flag PSO_MULTIPLE_DATA_DEFINITIONS. The
    * layout defined when a queue is created is then used as the default one.
    * <p>
    * The DataDefinition object returned by this method can be used to 
    * access the layout on a record-by-record base.
    * <p>
    * Note: you only need to get this object once. The hidden fields 
    * associated with this object will be updated after each record is
    * retrieved. The object will point to the data definition of the 
    * queue itself when initially constructed.
    *
    * @return A new data definition of the items retrieved from the queue.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public DataDefinition getRecordDefinition() throws PhotonException {
      
      int errcode;
      DataDefinition definition = new DataDefinition();
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetRecordDefinition( handle, definition );
      if ( errcode == 0 ) return definition;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Return the status of the queue.
    *
    * @return A new status object.
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public ObjectStatus getStatus() throws PhotonException {
      
      int errcode;
      ObjectStatus status = new ObjectStatus();
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoGetStatus( handle, status );
      if ( errcode == 0 ) return status;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   /**
    * Open an existing FIFO queue (see Session::CreateObject to create a new 
    * queue).
    *
    * @param session The session we belong to.
    * @param name The fully qualified name of the queue. 
    *
    * @exception PhotonException On an error with the Photon library.
    */
   public void open( Session session, String name ) throws PhotonException {
      
      int errcode;
            
      if ( handle != 0 ) {
         throw new PhotonException( PhotonErrors.ALREADY_OPEN );
      }

      errcode = psoOpen( session, name );
      if ( errcode != 0 ) {
         throw new PhotonException( PhotonErrors.getEnum(errcode) );
      }
      this.session = session;
      this.name = name;
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public byte [] popRaw() throws PhotonException {
      
      int errcode;
      byte [] buffer = null;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPop( handle, buffer );
      if ( errcode == 0 ) return buffer;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void pushRaw( byte[] buffer ) throws PhotonException {

      int errcode = 0; // remove = 0; later
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPush( handle, buffer );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }
   
   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   public void pushNowRaw( byte[] buffer ) throws PhotonException {

      int errcode;
      
      if ( handle == 0 ) {
         throw new PhotonException( PhotonErrors.NULL_HANDLE );
      }

      errcode = psoPushNow( handle, buffer );
      if ( errcode == 0 ) return;

      throw new PhotonException( PhotonErrors.getEnum(errcode) );
   }

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

   private static native void initIDs();

   private native void psoClose( long handle );

   private native int psoDataDefinition( long handle, DataDefinition definition );

   protected native int psoGetFirst( long   handle,
                                   byte[] buffer );

   protected native int psoGetNext( long   handle,
                                  byte[] buffer );

   private native int psoGetRecordDefinition( long           handle, 
                                              DataDefinition definition );

   private native int psoGetStatus( long         handle,
                                    ObjectStatus status );

   private native int psoOpen( Session session, String  name );

   protected native int psoPop( long handle, byte[] data );

   protected native int psoPush( long handle, byte[] data );

   protected native int psoPushNow( long handle, byte[] data );

   // --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
}

