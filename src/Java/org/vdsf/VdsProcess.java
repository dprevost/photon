/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

package org.vdsf;

public class VdsProcess {

   static {
      System.loadLibrary("vdsf_jni");
   }

   /**
    * This function initializes access to a VDS. It takes 2 input 
    * arguments, the address of the watchdog and a boolean to indicate 
    * if sessions and other objects (Queues, etc) are shared amongst 
    * threads (in the 
    * current process) and must be protected. Recommendation: always 
    * set protectionNeeded to 0 (false) unless you cannot do it otherwise. 
    * In other words it is recommended to use one session handle for
    * each thread. Also if the same queue needs to be accessed by two 
    * threads it is more efficient to have two different handles instead
    * of sharing a single one.
    * 
    * [Additional note: API objects (or C handles) are just proxies for 
    * the real objects sitting in shared memory. Proper synchronization 
    * is already done in shared memory and it is best to avoid to 
    * synchronize these proxy objects.]
    * 
    * Upon successful completion, the process handle is set. Otherwise 
    * the error code is returned.
    */

//   public native byte[] vdsjInit( String  wdAddress,
//                                  boolean protectionNeeded );


}
