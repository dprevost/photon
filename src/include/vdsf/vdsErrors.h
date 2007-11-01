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
    * crashed process.
    */
   VDS_ENGINE_BUSY                 = 1,

   /** 
    * Not enough memory in the VDS. 
    */
   VDS_NOT_ENOUGH_VDS_MEMORY       = 2,
 
   /** 
    * Not enough heap memory (non-VDS memory). 
    */
   VDS_NOT_ENOUGH_HEAP_MEMORY      = 3,

   /**
    * There are not enough resources to correctly process the call. 
    * This might be due to a lack of POSIX semaphores on systems 
    * where locks are implemented that way or a failure in initializing
    * a pthread_mutex (or on Windows, a critical section).
    */
   VDS_NOT_ENOUGH_RESOURCES        = 4,

   /**
    * The provided handle is of the wrong type.
    */
   VDS_WRONG_TYPE_HANDLE           = 5,
   
   /**
    * The provided handle is NULL (zero).
    */
   VDS_NULL_HANDLE                 = 6,

   /**
    * One of the arguments of an API function is an invalid NULL pointer.
    */
   VDS_NULL_POINTER                = 7,

   /**
    * An invalid length was provided (it will usually indicate that the
    * length value is set to zero.
    */
   VDS_INVALID_LENGTH              = 8,
   
   /**
    * The process was already initialized. Was vdsInit() called for a 
    * second time?
    */
   VDS_PROCESS_ALREADY_INITIALIZED = 21,

   /**
    * The process was not properly initialized. Was vdsInit() called?
    */
   VDS_PROCESS_NOT_INITIALIZED     = 22,

   /** The watchdog address is invalid (empty string, NULL pointer, etc.) */
   VDS_INVALID_WATCHDOG_ADDRESS    = 23,

   /** API - memory-file version mismatch. */
   VDS_INCOMPATIBLE_VERSIONS       = 24,

   /** Generic socket error. */
   VDS_SOCKET_ERROR                = 25,
   
   /** Socket error when trying to connect to the watchdog. */
   VDS_CONNECT_ERROR               = 26,

   /** Socket error when trying to send a request to the watchdog. */
   VDS_SEND_ERROR                  = 27,
   
   /** Socket error when trying to receive a reply from the watchdog. */   
   VDS_RECEIVE_ERROR               = 28,

   /** 
    * The vds backstore file is missing (the name of this file is 
    * provided by the watchdog).
    */
   VDS_BACKSTORE_FILE_MISSING      = 29,
   
   /** Generic i/o error when attempting to open the vds. */
   VDS_ERROR_OPENING_VDS           = 30,

   /**
    * Error accessing the directory for the log files or error opening the 
    * log file itself. */
   VDS_LOGFILE_ERROR               = 41,

   /**
    * Cannot get a lock on the session (a pthread_mutex or a critical 
    * section on Windows). 
    */
   VDS_SESSION_CANNOT_GET_LOCK     = 42,
   
   /**
    * An attempt was made to use a session object (a session handle)
    * after this session was terminated.
    */
   VDS_SESSION_IS_TERMINATED       = 43,
   
   /**
    * Permitted characters for names are alphanumerics, spaces (' '), 
    * dashes ('-') and underlines ('_'). The first character
    * must be alphanumeric.
    */
   VDS_INVALID_OBJECT_NAME         = 51,

   /** The object was not found (but its folder does exist). */
   VDS_NO_SUCH_OBJECT              = 52,

   /** One of the parent folder of an object does not exist. */
   VDS_NO_SUCH_FOLDER              = 53,

   /** Attempt to create an object which already exists. */
   VDS_OBJECT_ALREADY_PRESENT      = 54,
   
   /** The object (data container) is empty. */
   VDS_IS_EMPTY                    = 55,

   /** Attempt to create an object of an unknown object type. */
   VDS_WRONG_OBJECT_TYPE           = 56,

   /**
    * Cannot get lock on the object.
    * This might be the result of either a very busy system where 
    * unused cpu cycles are rare or a lock might be held by a 
    * crashed process.
    */
   VDS_OBJECT_CANNOT_GET_LOCK      = 57,

   /** 
    * The search/iteration reached the end without finding a new 
    * item/record. 
    */
   VDS_REACHED_THE_END             = 58,

   /** An invalid value was used for a vdsIteratorType parameter. */
   VDS_INVALID_ITERATOR            = 59,

   /**
    * The name of the object is too long.
    * The maximum length of a name cannot be more than VDS_MAX_NAME_LENGTH
    * (or VDS_MAX_FULL_NAME_LENGTH for the fully qualified name).
    */
   VDS_OBJECT_NAME_TOO_LONG        = 60,
   
   /**
    * You cannot delete a folder if there are still undeleted objects in
    * it.
    *
    * Technical: a folder does not need to be empty to be deleted but
    * all objects in it must be "marked as deleted" by the current session.
    * This enables writing recursive deletions
    */
   VDS_FOLDER_IS_NOT_EMPTY         = 61,
   
   /**
    * An item with the same key was found.
    */
   VDS_ITEM_ALREADY_PRESENT        = 62,
   
   /**
    * The item was not found in the hash map
    */
   VDS_NO_SUCH_ITEM                = 63,
   
   /**
    * The object is scheduled to be deleted soon. Operations on this data
    * container are not permitted at this time.
    */
   VDS_OBJECT_IS_DELETED           = 64,     

    /** Object must be open first before you can access them. */
   VDS_OBJECT_NOT_INITIALIZED      = 65

} vdsErrors;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifdef __cplusplus
}
#endif
 
#endif /* VDS_ERRORS_H */

