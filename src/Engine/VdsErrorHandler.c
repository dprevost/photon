/*
 * This file was generated by the program errorParser
 * using the input file vdsf.xml.
 * Date: Tue Jun 17 13:39:19 2008.
 *
 * The version of this interface is 0.3.
 *
 * Copyright (C) 2006-2008 Daniel Prevost
 *
 * This file is part of the vdsf library (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free
 * Software Foundation and appearing in the file COPYING included in
 * the packaging of this library.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <stdlib.h> /* Any system file will do. Needed for NULL. */
#include "VdsErrorHandler.h"

struct vdse_MsgStruct
{
    int  errorNumber;
    const char * message;
};

typedef struct vdse_MsgStruct vdse_MsgStruct;

/* VDS_OK */
vdse_MsgStruct vdse_Msg0 = {
    0, "No error..." };

/* VDS_INTERNAL_ERROR */
vdse_MsgStruct vdse_Msg1 = {
    666, "Abnormal internal error." };

/* VDS_ENGINE_BUSY */
vdse_MsgStruct vdse_Msg2 = {
    1, "Cannot get a lock on a system object, the engine is \"busy\"." };

/* VDS_NOT_ENOUGH_VDS_MEMORY */
vdse_MsgStruct vdse_Msg3 = {
    2, "Not enough memory in the VDS." };

/* VDS_NOT_ENOUGH_HEAP_MEMORY */
vdse_MsgStruct vdse_Msg4 = {
    3, "Not enough heap memory (non-VDS memory)." };

/* VDS_NOT_ENOUGH_RESOURCES */
vdse_MsgStruct vdse_Msg5 = {
    4, "There are not enough resources to correctly process the call." };

/* VDS_WRONG_TYPE_HANDLE */
vdse_MsgStruct vdse_Msg6 = {
    5, "The provided handle is of the wrong type." };

/* VDS_NULL_HANDLE */
vdse_MsgStruct vdse_Msg7 = {
    6, "The provided handle is NULL." };

/* VDS_NULL_POINTER */
vdse_MsgStruct vdse_Msg8 = {
    7, "One of the arguments of an API function is an invalid NULL pointer." };

/* VDS_INVALID_LENGTH */
vdse_MsgStruct vdse_Msg9 = {
    8, "An invalid length was provided." };

/* VDS_PROCESS_ALREADY_INITIALIZED */
vdse_MsgStruct vdse_Msg10 = {
    21, "The process was already initialized." };

/* VDS_PROCESS_NOT_INITIALIZED */
vdse_MsgStruct vdse_Msg11 = {
    22, "The process was not properly initialized." };

/* VDS_INVALID_WATCHDOG_ADDRESS */
vdse_MsgStruct vdse_Msg12 = {
    23, "The watchdog address is invalid (empty string, NULL pointer, etc.)." };

/* VDS_INCOMPATIBLE_VERSIONS */
vdse_MsgStruct vdse_Msg13 = {
    24, "API - memory-file version mismatch." };

/* VDS_SOCKET_ERROR */
vdse_MsgStruct vdse_Msg14 = {
    25, "Generic socket error." };

/* VDS_CONNECT_ERROR */
vdse_MsgStruct vdse_Msg15 = {
    26, "Socket error when trying to connect to the watchdog." };

/* VDS_SEND_ERROR */
vdse_MsgStruct vdse_Msg16 = {
    27, "Socket error when trying to send a request to the watchdog." };

/* VDS_RECEIVE_ERROR */
vdse_MsgStruct vdse_Msg17 = {
    28, "Socket error when trying to receive a reply from the watchdog." };

/* VDS_BACKSTORE_FILE_MISSING */
vdse_MsgStruct vdse_Msg18 = {
    29, "The vds backstore file is missing." };

/* VDS_ERROR_OPENING_VDS */
vdse_MsgStruct vdse_Msg19 = {
    30, "Generic i/o error when attempting to open the vds." };

/* VDS_LOGFILE_ERROR */
vdse_MsgStruct vdse_Msg20 = {
    41, "Error accessing the directory for the log files or error opening the log file itself." };

/* VDS_SESSION_CANNOT_GET_LOCK */
vdse_MsgStruct vdse_Msg21 = {
    42, "Cannot get a lock on the session (a pthread_mutex or a critical section on Windows)." };

/* VDS_SESSION_IS_TERMINATED */
vdse_MsgStruct vdse_Msg22 = {
    43, "An attempt was made to use a session object (a session handle) after this session was terminated." };

/* VDS_INVALID_OBJECT_NAME */
vdse_MsgStruct vdse_Msg23 = {
    51, "Permitted characters for names are alphanumerics, spaces (\' \'), dashes (\'-\') and underlines (\'_\'). The first character must be alphanumeric." };

/* VDS_NO_SUCH_OBJECT */
vdse_MsgStruct vdse_Msg24 = {
    52, "The object was not found (but its folder does exist)." };

/* VDS_NO_SUCH_FOLDER */
vdse_MsgStruct vdse_Msg25 = {
    53, "One of the parent folder of an object does not exist." };

/* VDS_OBJECT_ALREADY_PRESENT */
vdse_MsgStruct vdse_Msg26 = {
    54, "Attempt to create an object which already exists." };

/* VDS_IS_EMPTY */
vdse_MsgStruct vdse_Msg27 = {
    55, "The object (data container) is empty." };

/* VDS_WRONG_OBJECT_TYPE */
vdse_MsgStruct vdse_Msg28 = {
    56, "Attempt to create an object of an unknown object type or to open an object of the wrong type." };

/* VDS_OBJECT_CANNOT_GET_LOCK */
vdse_MsgStruct vdse_Msg29 = {
    57, "Cannot get lock on the object. This might be the result of either a very busy system where unused cpu cycles are rare or a lock might be held by a crashed process." };

/* VDS_REACHED_THE_END */
vdse_MsgStruct vdse_Msg30 = {
    58, "The search/iteration reached the end without finding a new item/record." };

/* VDS_INVALID_ITERATOR */
vdse_MsgStruct vdse_Msg31 = {
    59, "An invalid value was used for a vdsIteratorType parameter." };

/* VDS_OBJECT_NAME_TOO_LONG */
vdse_MsgStruct vdse_Msg32 = {
    60, "The name of the object is too long. The maximum length of a name cannot be more than VDS_MAX_NAME_LENGTH (or VDS_MAX_FULL_NAME_LENGTH for the fully qualified name)." };

/* VDS_FOLDER_IS_NOT_EMPTY */
vdse_MsgStruct vdse_Msg33 = {
    61, "You cannot delete a folder if there are still undeleted objects in it." };

/* VDS_ITEM_ALREADY_PRESENT */
vdse_MsgStruct vdse_Msg34 = {
    62, "An item with the same key was found." };

/* VDS_NO_SUCH_ITEM */
vdse_MsgStruct vdse_Msg35 = {
    63, "The item was not found in the hash map." };

/* VDS_OBJECT_IS_DELETED */
vdse_MsgStruct vdse_Msg36 = {
    64, "The object is scheduled to be deleted soon. Operations on this data container are not permitted at this time." };

/* VDS_OBJECT_NOT_INITIALIZED */
vdse_MsgStruct vdse_Msg37 = {
    65, "Object must be open first before you can access them." };

/* VDS_ITEM_IS_IN_USE */
vdse_MsgStruct vdse_Msg38 = {
    66, "The data item is scheduled to be deleted soon or was just created and is not committed. Operations on this data item are not permitted at this time." };

/* VDS_ITEM_IS_DELETED */
vdse_MsgStruct vdse_Msg39 = {
    67, "The data item is scheduled to be deleted soon. Operations on this data container are not permitted at this time." };

/* VDS_OBJECT_IS_IN_USE */
vdse_MsgStruct vdse_Msg40 = {
    69, "The object is scheduled to be deleted soon or was just created and is not committed. Operations on this object are not permitted at this time." };

/* VDS_INVALID_NUM_FIELDS */
vdse_MsgStruct vdse_Msg41 = {
    101, "The number of fields in the data definition is invalid." };

/* VDS_INVALID_FIELD_TYPE */
vdse_MsgStruct vdse_Msg42 = {
    102, "The data type of the field definition is invalid." };

/* VDS_INVALID_FIELD_LENGTH_INT */
vdse_MsgStruct vdse_Msg43 = {
    103, "The length of an integer field (VDS_INTEGER) is invalid." };

/* VDS_INVALID_FIELD_LENGTH */
vdse_MsgStruct vdse_Msg44 = {
    104, "The length of a field (string or binary) is invalid." };

/* VDS_INVALID_FIELD_NAME */
vdse_MsgStruct vdse_Msg45 = {
    105, "The name of the field contains invalid characters." };

/* VDS_DUPLICATE_FIELD_NAME */
vdse_MsgStruct vdse_Msg46 = {
    106, "The name of the field is already used by another field in the current definition." };

/* VDS_INVALID_PRECISION */
vdse_MsgStruct vdse_Msg47 = {
    107, "The precision of a VDS_DECIMAL field is invalid." };

/* VDS_INVALID_SCALE */
vdse_MsgStruct vdse_Msg48 = {
    108, "The scale of a VDS_DECIMAL field is invalid." };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Array of pointers to previous structs */
vdse_MsgStruct * vdse_MsgArray[49] = {
    &vdse_Msg0,
    &vdse_Msg1,
    &vdse_Msg2,
    &vdse_Msg3,
    &vdse_Msg4,
    &vdse_Msg5,
    &vdse_Msg6,
    &vdse_Msg7,
    &vdse_Msg8,
    &vdse_Msg9,
    &vdse_Msg10,
    &vdse_Msg11,
    &vdse_Msg12,
    &vdse_Msg13,
    &vdse_Msg14,
    &vdse_Msg15,
    &vdse_Msg16,
    &vdse_Msg17,
    &vdse_Msg18,
    &vdse_Msg19,
    &vdse_Msg20,
    &vdse_Msg21,
    &vdse_Msg22,
    &vdse_Msg23,
    &vdse_Msg24,
    &vdse_Msg25,
    &vdse_Msg26,
    &vdse_Msg27,
    &vdse_Msg28,
    &vdse_Msg29,
    &vdse_Msg30,
    &vdse_Msg31,
    &vdse_Msg32,
    &vdse_Msg33,
    &vdse_Msg34,
    &vdse_Msg35,
    &vdse_Msg36,
    &vdse_Msg37,
    &vdse_Msg38,
    &vdse_Msg39,
    &vdse_Msg40,
    &vdse_Msg41,
    &vdse_Msg42,
    &vdse_Msg43,
    &vdse_Msg44,
    &vdse_Msg45,
    &vdse_Msg46,
    &vdse_Msg47,
    &vdse_Msg48
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

const char * vdse_ErrorMessage( int errnum )
{
    int i;

    for ( i = 0; i < 49; ++i ) {
        if ( errnum == vdse_MsgArray[i]->errorNumber ) {
            return vdse_MsgArray[i]->message;
        }
    }

    return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

