/*
 * This file was generated by the program errorParser
 * using the input file vdsf.xml.
 * Date: Tue Aug 19 20:30:14 2008.
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
#include "psoErrorHandler.h"

struct psn_MsgStruct
{
    int  errorNumber;
    const char * message;
};

typedef struct psn_MsgStruct psn_MsgStruct;

/* PSO_OK */
psn_MsgStruct psn_Msg0 = {
    0, "No error..." };

/* PSO_INTERNAL_ERROR */
psn_MsgStruct psn_Msg1 = {
    666, "Abnormal internal error." };

/* PSO_ENGINE_BUSY */
psn_MsgStruct psn_Msg2 = {
    1, "Cannot get a lock on a system object, the engine is \"busy\"." };

/* PSO_NOT_ENOUGH_PSO_MEMORY */
psn_MsgStruct psn_Msg3 = {
    2, "Not enough memory in the PSO." };

/* PSO_NOT_ENOUGH_HEAP_MEMORY */
psn_MsgStruct psn_Msg4 = {
    3, "Not enough heap memory (non-PSO memory)." };

/* PSO_NOT_ENOUGH_RESOURCES */
psn_MsgStruct psn_Msg5 = {
    4, "There are not enough resources to correctly process the call." };

/* PSO_WRONG_TYPE_HANDLE */
psn_MsgStruct psn_Msg6 = {
    5, "The provided handle is of the wrong type." };

/* PSO_NULL_HANDLE */
psn_MsgStruct psn_Msg7 = {
    6, "The provided handle is NULL." };

/* PSO_NULL_POINTER */
psn_MsgStruct psn_Msg8 = {
    7, "One of the arguments of an API function is an invalid NULL pointer." };

/* PSO_INVALID_LENGTH */
psn_MsgStruct psn_Msg9 = {
    8, "An invalid length was provided." };

/* PSO_PROCESS_ALREADY_INITIALIZED */
psn_MsgStruct psn_Msg10 = {
    21, "The process was already initialized." };

/* PSO_PROCESS_NOT_INITIALIZED */
psn_MsgStruct psn_Msg11 = {
    22, "The process was not properly initialized." };

/* PSO_INVALID_WATCHDOG_ADDRESS */
psn_MsgStruct psn_Msg12 = {
    23, "The watchdog address is invalid (empty string, NULL pointer, etc.)." };

/* PSO_INCOMPATIBLE_VERSIONS */
psn_MsgStruct psn_Msg13 = {
    24, "API - memory-file version mismatch." };

/* PSO_SOCKET_ERROR */
psn_MsgStruct psn_Msg14 = {
    25, "Generic socket error." };

/* PSO_CONNECT_ERROR */
psn_MsgStruct psn_Msg15 = {
    26, "Socket error when trying to connect to the watchdog." };

/* PSO_SEND_ERROR */
psn_MsgStruct psn_Msg16 = {
    27, "Socket error when trying to send a request to the watchdog." };

/* PSO_RECEIVE_ERROR */
psn_MsgStruct psn_Msg17 = {
    28, "Socket error when trying to receive a reply from the watchdog." };

/* PSO_BACKSTORE_FILE_MISSING */
psn_MsgStruct psn_Msg18 = {
    29, "The vds backstore file is missing." };

/* PSO_ERROR_OPENING_VDS */
psn_MsgStruct psn_Msg19 = {
    30, "Generic i/o error when attempting to open the vds." };

/* PSO_LOGFILE_ERROR */
psn_MsgStruct psn_Msg20 = {
    41, "Error accessing the directory for the log files or error opening the log file itself." };

/* PSO_SESSION_CANNOT_GET_LOCK */
psn_MsgStruct psn_Msg21 = {
    42, "Cannot get a lock on the session (a pthread_mutex or a critical section on Windows)." };

/* PSO_SESSION_IS_TERMINATED */
psn_MsgStruct psn_Msg22 = {
    43, "An attempt was made to use a session object (a session handle) after this session was terminated." };

/* PSO_INVALID_OBJECT_NAME */
psn_MsgStruct psn_Msg23 = {
    51, "Permitted characters for names are alphanumerics, spaces (\' \'), dashes (\'-\') and underlines (\'_\'). The first character must be alphanumeric." };

/* PSO_NO_SUCH_OBJECT */
psn_MsgStruct psn_Msg24 = {
    52, "The object was not found (but its folder does exist)." };

/* PSO_NO_SUCH_FOLDER */
psn_MsgStruct psn_Msg25 = {
    53, "One of the parent folder of an object does not exist." };

/* PSO_OBJECT_ALREADY_PRESENT */
psn_MsgStruct psn_Msg26 = {
    54, "Attempt to create an object which already exists." };

/* PSO_IS_EMPTY */
psn_MsgStruct psn_Msg27 = {
    55, "The object (data container) is empty." };

/* PSO_WRONG_OBJECT_TYPE */
psn_MsgStruct psn_Msg28 = {
    56, "Attempt to create an object of an unknown object type or to open an object of the wrong type." };

/* PSO_OBJECT_CANNOT_GET_LOCK */
psn_MsgStruct psn_Msg29 = {
    57, "Cannot get lock on the object. This might be the result of either a very busy system where unused cpu cycles are rare or a lock might be held by a crashed process." };

/* PSO_REACHED_THE_END */
psn_MsgStruct psn_Msg30 = {
    58, "The search/iteration reached the end without finding a new item/record." };

/* PSO_INVALID_ITERATOR */
psn_MsgStruct psn_Msg31 = {
    59, "An invalid value was used for a vdsIteratorType parameter." };

/* PSO_OBJECT_NAME_TOO_LONG */
psn_MsgStruct psn_Msg32 = {
    60, "The name of the object is too long. The maximum length of a name cannot be more than PSO_MAX_NAME_LENGTH (or PSO_MAX_FULL_NAME_LENGTH for the fully qualified name)." };

/* PSO_FOLDER_IS_NOT_EMPTY */
psn_MsgStruct psn_Msg33 = {
    61, "You cannot delete a folder if there are still undeleted objects in it." };

/* PSO_ITEM_ALREADY_PRESENT */
psn_MsgStruct psn_Msg34 = {
    62, "An item with the same key was found." };

/* PSO_NO_SUCH_ITEM */
psn_MsgStruct psn_Msg35 = {
    63, "The item was not found in the hash map." };

/* PSO_OBJECT_IS_DELETED */
psn_MsgStruct psn_Msg36 = {
    64, "The object is scheduled to be deleted soon. Operations on this data container are not permitted at this time." };

/* PSO_OBJECT_NOT_INITIALIZED */
psn_MsgStruct psn_Msg37 = {
    65, "Object must be open first before you can access them." };

/* PSO_ITEM_IS_IN_USE */
psn_MsgStruct psn_Msg38 = {
    66, "The data item is scheduled to be deleted soon or was just created and is not committed. Operations on this data item are not permitted at this time." };

/* PSO_ITEM_IS_DELETED */
psn_MsgStruct psn_Msg39 = {
    67, "The data item is scheduled to be deleted soon. Operations on this data container are not permitted at this time." };

/* PSO_OBJECT_IS_IN_USE */
psn_MsgStruct psn_Msg40 = {
    69, "The object is scheduled to be deleted soon or was just created and is not committed. Operations on this object are not permitted at this time." };

/* PSO_OBJECT_IS_READ_ONLY */
psn_MsgStruct psn_Msg41 = {
    70, "The object is read-only and update operations on it are not permitted." };

/* PSO_NOT_ALL_EDIT_ARE_CLOSED */
psn_MsgStruct psn_Msg42 = {
    71, "All edits must be closed before a commit." };

/* PSO_A_SINGLE_UPDATER_IS_ALLOWED */
psn_MsgStruct psn_Msg43 = {
    72, "Read-only objects can only have one updater at a time." };

/* PSO_INVALID_NUM_FIELDS */
psn_MsgStruct psn_Msg44 = {
    101, "The number of fields in the data definition is invalid." };

/* PSO_INVALID_FIELD_TYPE */
psn_MsgStruct psn_Msg45 = {
    102, "The data type of the field definition is invalid." };

/* PSO_INVALID_FIELD_LENGTH_INT */
psn_MsgStruct psn_Msg46 = {
    103, "The length of an integer field (PSO_INTEGER) is invalid." };

/* PSO_INVALID_FIELD_LENGTH */
psn_MsgStruct psn_Msg47 = {
    104, "The length of a field (string or binary) is invalid." };

/* PSO_INVALID_FIELD_NAME */
psn_MsgStruct psn_Msg48 = {
    105, "The name of the field contains invalid characters." };

/* PSO_DUPLICATE_FIELD_NAME */
psn_MsgStruct psn_Msg49 = {
    106, "The name of the field is already used by another field in the current definition." };

/* PSO_INVALID_PRECISION */
psn_MsgStruct psn_Msg50 = {
    107, "The precision of a PSO_DECIMAL field is invalid." };

/* PSO_INVALID_SCALE */
psn_MsgStruct psn_Msg51 = {
    108, "The scale of a PSO_DECIMAL field is invalid." };

/* PSO_INVALID_KEY_DEF */
psn_MsgStruct psn_Msg52 = {
    109, "The key definition is invalid or missing." };

/* PSO_XML_READ_ERROR */
psn_MsgStruct psn_Msg53 = {
    201, "Error reading the XML buffer." };

/* PSO_XML_INVALID_ROOT */
psn_MsgStruct psn_Msg54 = {
    202, "The root element is not the expected root." };

/* PSO_XML_NO_SCHEMA_LOCATION */
psn_MsgStruct psn_Msg55 = {
    203, "The attribute xsi:schemaLocation was not found or is invalid." };

/* PSO_XML_PARSER_CONTEXT_FAILED */
psn_MsgStruct psn_Msg56 = {
    204, "The creation of a new schema parser context failed." };

/* PSO_XML_PARSE_SCHEMA_FAILED */
psn_MsgStruct psn_Msg57 = {
    205, "Failure when parsing the XML schema." };

/* PSO_XML_VALID_CONTEXT_FAILED */
psn_MsgStruct psn_Msg58 = {
    206, "The creation of a new XML Schemas validation context failed." };

/* PSO_XML_VALIDATION_FAILED */
psn_MsgStruct psn_Msg59 = {
    207, "Document validation for the xml buffer failed." };

/* PSO_SEM_DESTROY_ERROR */
psn_MsgStruct psn_Msg60 = {
    601, "Abnormal internal error with sem_destroy." };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Array of pointers to previous structs */
psn_MsgStruct * psn_MsgArray[61] = {
    &psn_Msg0,
    &psn_Msg1,
    &psn_Msg2,
    &psn_Msg3,
    &psn_Msg4,
    &psn_Msg5,
    &psn_Msg6,
    &psn_Msg7,
    &psn_Msg8,
    &psn_Msg9,
    &psn_Msg10,
    &psn_Msg11,
    &psn_Msg12,
    &psn_Msg13,
    &psn_Msg14,
    &psn_Msg15,
    &psn_Msg16,
    &psn_Msg17,
    &psn_Msg18,
    &psn_Msg19,
    &psn_Msg20,
    &psn_Msg21,
    &psn_Msg22,
    &psn_Msg23,
    &psn_Msg24,
    &psn_Msg25,
    &psn_Msg26,
    &psn_Msg27,
    &psn_Msg28,
    &psn_Msg29,
    &psn_Msg30,
    &psn_Msg31,
    &psn_Msg32,
    &psn_Msg33,
    &psn_Msg34,
    &psn_Msg35,
    &psn_Msg36,
    &psn_Msg37,
    &psn_Msg38,
    &psn_Msg39,
    &psn_Msg40,
    &psn_Msg41,
    &psn_Msg42,
    &psn_Msg43,
    &psn_Msg44,
    &psn_Msg45,
    &psn_Msg46,
    &psn_Msg47,
    &psn_Msg48,
    &psn_Msg49,
    &psn_Msg50,
    &psn_Msg51,
    &psn_Msg52,
    &psn_Msg53,
    &psn_Msg54,
    &psn_Msg55,
    &psn_Msg56,
    &psn_Msg57,
    &psn_Msg58,
    &psn_Msg59,
    &psn_Msg60
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

const char * psn_ErrorMessage( int errnum )
{
    int i;

    for ( i = 0; i < 61; ++i ) {
        if ( errnum == psn_MsgArray[i]->errorNumber ) {
            return psn_MsgArray[i]->message;
        }
    }

    return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
