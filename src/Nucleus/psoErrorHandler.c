/*
 * This file was generated by the program errorParser
 * using the input file photon.xml.
 * Date: Thu Jun 25 16:41:44 2009.
 *
 * The version of the XML error file is 0.5.
 *
 * Copyright (C) 2006-2008 Daniel Prevost
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the 
 * GNU General Public License version 2 or version 3 as published by 
 * the Free Software Foundation and appearing in the file COPYING.GPL2 
 * and COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this 
 * file in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <stdlib.h> /* Needed for NULL. */

#define BULDING_ERROR_MESSAGE
#include "psoErrorHandler.h"

struct pson_MsgStruct
{
    int  errorNumber;
    const char * message;
};

typedef struct pson_MsgStruct pson_MsgStruct;

/* PSO_OK */
pson_MsgStruct pson_Msg0 = {
    0, "No error..." };

/* PSO_INTERNAL_ERROR */
pson_MsgStruct pson_Msg1 = {
    666, "Abnormal internal error." };

/* PSO_ENGINE_BUSY */
pson_MsgStruct pson_Msg2 = {
    1, "Cannot get a lock on a system object, the engine is \"busy\"." };

/* PSO_NOT_ENOUGH_PSO_MEMORY */
pson_MsgStruct pson_Msg3 = {
    2, "Not enough memory in the PSO." };

/* PSO_NOT_ENOUGH_HEAP_MEMORY */
pson_MsgStruct pson_Msg4 = {
    3, "Not enough heap memory (non-PSO memory)." };

/* PSO_NOT_ENOUGH_RESOURCES */
pson_MsgStruct pson_Msg5 = {
    4, "There are not enough resources to correctly process the call." };

/* PSO_WRONG_TYPE_HANDLE */
pson_MsgStruct pson_Msg6 = {
    5, "The provided handle is of the wrong type." };

/* PSO_NULL_HANDLE */
pson_MsgStruct pson_Msg7 = {
    6, "The provided handle is NULL." };

/* PSO_NULL_POINTER */
pson_MsgStruct pson_Msg8 = {
    7, "One of the arguments of an API function is an invalid NULL pointer." };

/* PSO_INVALID_LENGTH */
pson_MsgStruct pson_Msg9 = {
    8, "An invalid length was provided." };

/* PSO_PROCESS_ALREADY_INITIALIZED */
pson_MsgStruct pson_Msg10 = {
    21, "The process was already initialized." };

/* PSO_PROCESS_NOT_INITIALIZED */
pson_MsgStruct pson_Msg11 = {
    22, "The process was not properly initialized." };

/* PSO_INVALID_QUASAR_ADDRESS */
pson_MsgStruct pson_Msg12 = {
    23, "The server address is invalid (empty string, NULL pointer, etc.)." };

/* PSO_INCOMPATIBLE_VERSIONS */
pson_MsgStruct pson_Msg13 = {
    24, "API - memory-file version mismatch." };

/* PSO_SOCKET_ERROR */
pson_MsgStruct pson_Msg14 = {
    25, "Generic socket error." };

/* PSO_CONNECT_ERROR */
pson_MsgStruct pson_Msg15 = {
    26, "Socket error when trying to connect to the server." };

/* PSO_SEND_ERROR */
pson_MsgStruct pson_Msg16 = {
    27, "Socket error when trying to send a request to the server." };

/* PSO_RECEIVE_ERROR */
pson_MsgStruct pson_Msg17 = {
    28, "Socket error when trying to receive a reply from the server." };

/* PSO_BACKSTORE_FILE_MISSING */
pson_MsgStruct pson_Msg18 = {
    29, "The shared-memory backstore file is missing." };

/* PSO_ERROR_OPENING_MEMORY */
pson_MsgStruct pson_Msg19 = {
    30, "Generic i/o error when attempting to open the shared memory." };

/* PSO_LOGFILE_ERROR */
pson_MsgStruct pson_Msg20 = {
    41, "Error accessing the directory for the log files or error opening the log file itself." };

/* PSO_SESSION_CANNOT_GET_LOCK */
pson_MsgStruct pson_Msg21 = {
    42, "Cannot get a lock on the session (a pthread_mutex or a critical section on Windows)." };

/* PSO_SESSION_IS_TERMINATED */
pson_MsgStruct pson_Msg22 = {
    43, "An attempt was made to use a session object (a session handle) after this session was terminated." };

/* PSO_INVALID_OBJECT_NAME */
pson_MsgStruct pson_Msg23 = {
    51, "Permitted characters for names are alphanumerics, spaces (\' \'), dashes (\'-\') and underlines (\'_\'). The first character must be alphanumeric." };

/* PSO_NO_SUCH_OBJECT */
pson_MsgStruct pson_Msg24 = {
    52, "The object was not found (but its folder does exist)." };

/* PSO_NO_SUCH_FOLDER */
pson_MsgStruct pson_Msg25 = {
    53, "One of the parent folder of an object does not exist." };

/* PSO_OBJECT_ALREADY_PRESENT */
pson_MsgStruct pson_Msg26 = {
    54, "Attempt to create an object which already exists." };

/* PSO_IS_EMPTY */
pson_MsgStruct pson_Msg27 = {
    55, "The object (data container) is empty." };

/* PSO_WRONG_OBJECT_TYPE */
pson_MsgStruct pson_Msg28 = {
    56, "Attempt to create an object of an unknown object type or to open an object of the wrong type." };

/* PSO_OBJECT_CANNOT_GET_LOCK */
pson_MsgStruct pson_Msg29 = {
    57, "Cannot get lock on the object. This might be the result of either a very busy system where unused cpu cycles are rare or a lock might be held by a crashed process." };

/* PSO_REACHED_THE_END */
pson_MsgStruct pson_Msg30 = {
    58, "The search/iteration reached the end without finding a new item/record." };

/* PSO_INVALID_ITERATOR */
pson_MsgStruct pson_Msg31 = {
    59, "An invalid value was used for a psoIteratorType parameter." };

/* PSO_OBJECT_NAME_TOO_LONG */
pson_MsgStruct pson_Msg32 = {
    60, "The name of the object is too long. The maximum length of a name cannot be more than PSO_MAX_NAME_LENGTH (or PSO_MAX_FULL_NAME_LENGTH for the fully qualified name)." };

/* PSO_FOLDER_IS_NOT_EMPTY */
pson_MsgStruct pson_Msg33 = {
    61, "You cannot delete a folder if there are still undeleted objects in it." };

/* PSO_ITEM_ALREADY_PRESENT */
pson_MsgStruct pson_Msg34 = {
    62, "An item with the same key was found." };

/* PSO_NO_SUCH_ITEM */
pson_MsgStruct pson_Msg35 = {
    63, "The item was not found in the hash map." };

/* PSO_OBJECT_IS_DELETED */
pson_MsgStruct pson_Msg36 = {
    64, "The object is scheduled to be deleted soon. Operations on this data container are not permitted at this time." };

/* PSO_OBJECT_NOT_INITIALIZED */
pson_MsgStruct pson_Msg37 = {
    65, "Object must be open first before you can access them." };

/* PSO_ITEM_IS_IN_USE */
pson_MsgStruct pson_Msg38 = {
    66, "The data item is scheduled to be deleted soon or was just created and is not committed. Operations on this data item are not permitted at this time." };

/* PSO_ITEM_IS_DELETED */
pson_MsgStruct pson_Msg39 = {
    67, "The data item is scheduled to be deleted soon. Operations on this data container are not permitted at this time." };

/* PSO_DATA_DEF_UNSUPPORTED */
pson_MsgStruct pson_Msg40 = {
    68, "The data container does not support data definitions for individual data records." };

/* PSO_OBJECT_IS_IN_USE */
pson_MsgStruct pson_Msg41 = {
    69, "The object is scheduled to be deleted soon or was just created and is not committed. Operations on this object are not permitted at this time." };

/* PSO_OBJECT_IS_READ_ONLY */
pson_MsgStruct pson_Msg42 = {
    70, "The object is read-only and update operations on it are not permitted." };

/* PSO_NOT_ALL_EDIT_ARE_CLOSED */
pson_MsgStruct pson_Msg43 = {
    71, "All edits must be closed before a commit." };

/* PSO_A_SINGLE_UPDATER_IS_ALLOWED */
pson_MsgStruct pson_Msg44 = {
    72, "Read-only objects can only have one updater at a time." };

/* PSO_SYSTEM_OBJECT */
pson_MsgStruct pson_Msg45 = {
    73, "You cannot add or remove data if the object is a system object." };

/* PSO_ALREADY_OPEN */
pson_MsgStruct pson_Msg46 = {
    74, "The object is open. You must close it before reopening it." };

/* PSO_INVALID_NUM_FIELDS */
pson_MsgStruct pson_Msg47 = {
    101, "The number of fields in the data definition is invalid." };

/* PSO_INVALID_FIELD_TYPE */
pson_MsgStruct pson_Msg48 = {
    102, "The data type of the field definition is invalid." };

/* PSO_INVALID_FIELD_LENGTH_INT */
pson_MsgStruct pson_Msg49 = {
    103, "The length of an integer field (PSO_INTEGER) is invalid." };

/* PSO_INVALID_FIELD_LENGTH */
pson_MsgStruct pson_Msg50 = {
    104, "The length of a field (string or binary) is invalid." };

/* PSO_INVALID_FIELD_NAME */
pson_MsgStruct pson_Msg51 = {
    105, "The name of the field contains invalid characters." };

/* PSO_DUPLICATE_FIELD_NAME */
pson_MsgStruct pson_Msg52 = {
    106, "The name of the field is already used by another field in the current definition." };

/* PSO_INVALID_PRECISION */
pson_MsgStruct pson_Msg53 = {
    107, "The precision of a PSO_DECIMAL field is invalid." };

/* PSO_INVALID_SCALE */
pson_MsgStruct pson_Msg54 = {
    108, "The scale of a PSO_DECIMAL field is invalid." };

/* PSO_INVALID_KEY_DEF */
pson_MsgStruct pson_Msg55 = {
    109, "The key definition is invalid or missing." };

/* PSO_INVALID_DEF_OPERATION */
pson_MsgStruct pson_Msg56 = {
    110, "An invalid operation was attempted on a definition." };

/* PSO_INVALID_DATA_DEFINITION_TYPE */
pson_MsgStruct pson_Msg57 = {
    121, "An invalid data-definition type was provided." };

/* PSO_XML_READ_ERROR */
pson_MsgStruct pson_Msg58 = {
    201, "Error reading the XML buffer." };

/* PSO_XML_INVALID_ROOT */
pson_MsgStruct pson_Msg59 = {
    202, "The root element is not the expected root." };

/* PSO_XML_NO_SCHEMA_LOCATION */
pson_MsgStruct pson_Msg60 = {
    203, "The attribute xsi:schemaLocation was not found or is invalid." };

/* PSO_XML_PARSER_CONTEXT_FAILED */
pson_MsgStruct pson_Msg61 = {
    204, "The creation of a new schema parser context failed." };

/* PSO_XML_PARSE_SCHEMA_FAILED */
pson_MsgStruct pson_Msg62 = {
    205, "Failure when parsing the XML schema." };

/* PSO_XML_VALID_CONTEXT_FAILED */
pson_MsgStruct pson_Msg63 = {
    206, "The creation of a new XML Schemas validation context failed." };

/* PSO_XML_VALIDATION_FAILED */
pson_MsgStruct pson_Msg64 = {
    207, "Document validation for the xml buffer failed." };

/* PSO_SEM_DESTROY_ERROR */
pson_MsgStruct pson_Msg65 = {
    601, "Abnormal internal error with sem_destroy." };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Array of pointers to previous structs */
pson_MsgStruct * pson_MsgArray[66] = {
    &pson_Msg0,
    &pson_Msg1,
    &pson_Msg2,
    &pson_Msg3,
    &pson_Msg4,
    &pson_Msg5,
    &pson_Msg6,
    &pson_Msg7,
    &pson_Msg8,
    &pson_Msg9,
    &pson_Msg10,
    &pson_Msg11,
    &pson_Msg12,
    &pson_Msg13,
    &pson_Msg14,
    &pson_Msg15,
    &pson_Msg16,
    &pson_Msg17,
    &pson_Msg18,
    &pson_Msg19,
    &pson_Msg20,
    &pson_Msg21,
    &pson_Msg22,
    &pson_Msg23,
    &pson_Msg24,
    &pson_Msg25,
    &pson_Msg26,
    &pson_Msg27,
    &pson_Msg28,
    &pson_Msg29,
    &pson_Msg30,
    &pson_Msg31,
    &pson_Msg32,
    &pson_Msg33,
    &pson_Msg34,
    &pson_Msg35,
    &pson_Msg36,
    &pson_Msg37,
    &pson_Msg38,
    &pson_Msg39,
    &pson_Msg40,
    &pson_Msg41,
    &pson_Msg42,
    &pson_Msg43,
    &pson_Msg44,
    &pson_Msg45,
    &pson_Msg46,
    &pson_Msg47,
    &pson_Msg48,
    &pson_Msg49,
    &pson_Msg50,
    &pson_Msg51,
    &pson_Msg52,
    &pson_Msg53,
    &pson_Msg54,
    &pson_Msg55,
    &pson_Msg56,
    &pson_Msg57,
    &pson_Msg58,
    &pson_Msg59,
    &pson_Msg60,
    &pson_Msg61,
    &pson_Msg62,
    &pson_Msg63,
    &pson_Msg64,
    &pson_Msg65
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

const char * pson_ErrorMessage( int errnum )
{
    int i;

    for ( i = 0; i < 66; ++i ) {
        if ( errnum == pson_MsgArray[i]->errorNumber ) {
            return pson_MsgArray[i]->message;
        }
    }

    return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

