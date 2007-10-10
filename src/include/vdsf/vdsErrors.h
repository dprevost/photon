/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDS_ERRORS_H
#define VDS_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef enum vdsErrors
{
   /** No error... */
   VDS_OK                          = 0,

   /** Abnormal internal error - it should not happen! */
   VDS_INTERNAL_ERROR              = 666,

   /**
    * Cannot get a lock on a system object, the engine is "busy".
    * This might be the result of either a very busy system where 
    * unused cpu cycles are rare or a lock might be held by a 
    * crashed process (however the watchdog should catch this one).
    */
   VDS_ENGINE_BUSY                 = 1,

   /** Not enough memory in the VDS. */
   VDS_NOT_ENOUGH_VDS_MEMORY       = 2,
 
   /** Not enough heap memory (non-VDS memory). */
   VDS_NOT_ENOUGH_HEAP_MEMORY      = 3,

   /**
    * Permitted characters for names are alphanumerics [a-z,0-9], 
    * spaces (' '), dashes ('-') and underlines ('_'). The first character
    * must be alphanumeric.
    */
   VDS_INVALID_OBJECT_NAME         = 4,

   /** The object was not found (but its folder does exist). */
   VDS_NO_SUCH_OBJECT              = 5,

   /** Invalid folder name. */
   VDS_NO_SUCH_FOLDER              = 6,

   /** Attempt to create an object which already exists. */
   VDS_OBJECT_ALREADY_PRESENT      = 7,

   /**
    *  Attempt to delete an object which is currently in use. The 
    * object might be used either by the current session or by 
    * another session.
    */
   VDS_OBJECT_IN_USE               = 8,

   VDS_INVALID_LENGTH_FIELD        = 9,
   
   /** The object (data container) is empty. */
   VDS_IS_EMPTY                    = 10,
   
   /** Object must be open first before you can access them. */
   VDS_OBJECT_NOT_INITIALIZED      = 11,
   
   VDS_CONFIG_ERROR                = 12,
   VDS_INVALID_WATCHDOG_ADDRESS    = 13,
   VDS_WRONG_OBJECT_TYPE           = 14,
   VDS_PROCESS_NOT_INITIALIZED     = 15,
   VDS_OBJECT_CANNOT_GET_LOCK      = 16,
   VDS_OBJECT_ALREADY_OPEN         = 17,
   /**
    * You must call vdsProcess::Terminate() before calling 
    * vdsProcess::Initialize() a second time. 
    */
   VDS_PROCESS_ALREADY_INITIALIZED = 18,

   /** API - memory-file version mismatch. */
   VDS_INCOMPATIBLE_VERSIONS       = 19,

   /** Rollback operations are not permitted for implicit transactions. */
   VDS_NO_ROLLBACK                 = 20,

   /** Error opening the log file (used for transactions). */
   VDS_LOGFILE_ERROR               = 21,

   /** The search reached the end without finding a new item/record. */
   VDS_REACHED_THE_END             = 22,

   /** An invalid value was used for a vdsIteratorType parameter. */
   VDS_INVALID_ITERATOR            = 23,

   /**
    * There are not enough resources to correctly process the call. 
    * This might be due to a lack of POSIX semaphores on systems 
    * where locks are implemented that way or a failure in initializing
    * a pthread_mutex (or on Windows, a critical section).
    */
   VDS_NOT_ENOUGH_RESOURCES        = 24,

   VDS_NO_CONFIG_FILE              = 25,
   VDS_SOCKET_ERROR                = 26,
   VDS_CONNECT_ERROR               = 27,
   VDS_SEND_ERROR                  = 28,
   VDS_RECEIVE_ERROR               = 29,
   VDS_BACKSTORE_FILE_MISSING      = 30,
   VDS_ERROR_OPENING_VDS           = 31,
   VDS_FOLDER_IS_NOT_EMPTY         = 32,
   
   /**
    * The provided handle is of the wrong type.
    */
   VDS_WRONG_TYPE_HANDLE           = 33,
   
   /**
    * The provided handle is NULL (zero).
    */
   VDS_NULL_HANDLE                 = 34,

   /**
    * The name of the object is too long.
    * The maximum length of a name cannot be more than VDS_MAX_NAME_LENGTH
    * (or VDS_MAX_FULL_NAME_LENGTH for the fully qualified name).
    */
   VDS_OBJECT_NAME_TOO_LONG        = 35,
   
   /**
    * Cannot get a lock on the session (a pthread_mutex or a critical 
    * section on Windows). 
    */
   VDS_SESSION_CANNOT_GET_LOCK     = 36,
   
   /**
    * An attempt was made to use a session object (a session handle)
    * after this session was terminated.
    */
   VDS_SESSION_IS_TERMINATED       = 37,
   
   /**
    * An item with the same key was found.
    */
   VDS_ITEM_ALREADY_PRESENT        = 38,
   
   /**
    * The item was not found in the hash map
    */
   VDS_NO_SUCH_ITEM                = 39,
   
   /**
    * The object is scheduled to be deleted soon. Operations on this data
    * container are not permitted at this time.
    */
   VDS_OBJECT_IS_DELETED           = 40     

} vdsErrors;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
 
#endif /* VDS_ERRORS_H */
