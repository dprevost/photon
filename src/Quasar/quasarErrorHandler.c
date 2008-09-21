/*
 * This file was generated by the program errorParser
 * using the input file quasarErrors.xml.
 * Date: Sat Sep 20 10:18:57 2008.
 *
 * The version of this interface is 0.3.
 *
 * Copyright (C) 2006-2008 Daniel Prevost
 *
 * This file is part of the Photon framework (photonsoftware.org).
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
#include "quasarErrorHandler.h"

struct psoq_MsgStruct
{
    int  errorNumber;
    const char * message;
};

typedef struct psoq_MsgStruct psoq_MsgStruct;

/* PSOQ_OK */
psoq_MsgStruct psoq_Msg0 = {
    0, "No error..." };

/* PSOQ_NOT_ENOUGH_HEAP_MEMORY */
psoq_MsgStruct psoq_Msg1 = {
    1, "Not enough heap memory (non-VDS memory)." };

/* PSOQ_CFG_BCK_LOCATION_TOO_LONG */
psoq_MsgStruct psoq_Msg2 = {
    2, "Backup path is too long." };

/* PSOQ_NO_VERIFICATION_POSSIBLE */
psoq_MsgStruct psoq_Msg3 = {
    3, "Cannot verify the shared memory." };

/* PSOQ_MKDIR_FAILURE */
psoq_MsgStruct psoq_Msg4 = {
    4, "Cannot create the directory for the shared memory." };

/* PSOQ_FILE_NOT_ACCESSIBLE */
psoq_MsgStruct psoq_Msg5 = {
    5, "Cannot access the backstore file for the shared memory." };

/* PSOQ_COPY_BCK_FAILURE */
psoq_MsgStruct psoq_Msg6 = {
    6, "Error while creating a copy of the backstore file for the shared memory." };

/* PSOQ_CREATE_BACKSTORE_FAILURE */
psoq_MsgStruct psoq_Msg7 = {
    7, "Error creating the backstore file for the shared memory." };

/* PSOQ_OPEN_BACKSTORE_FAILURE */
psoq_MsgStruct psoq_Msg8 = {
    8, "Error opening the backstore file for the shared memory." };

/* PSOQ_INCOMPATIBLE_VERSIONS */
psoq_MsgStruct psoq_Msg9 = {
    9, "Memory-file version mismatch." };

/* PSOQ_BACKSTORE_FILE_MISSING */
psoq_MsgStruct psoq_Msg10 = {
    10, "The shared-memory backstore file is missing." };

/* PSOQ_ERROR_OPENING_MEMORY */
psoq_MsgStruct psoq_Msg11 = {
    11, "Generic i/o error when attempting to open the shared memory." };

/* PSOQ_XML_CONFIG_NOT_OPEN */
psoq_MsgStruct psoq_Msg12 = {
    100, "Error opening the XML config file." };

/* PSOQ_XML_READ_ERROR */
psoq_MsgStruct psoq_Msg13 = {
    101, "Error reading the XML config file." };

/* PSOQ_XML_NO_ROOT */
psoq_MsgStruct psoq_Msg14 = {
    102, "No root element in the XML document." };

/* PSOQ_XML_INVALID_ROOT */
psoq_MsgStruct psoq_Msg15 = {
    103, "The root element is not the expected root." };

/* PSOQ_XML_NO_SCHEMA_LOCATION */
psoq_MsgStruct psoq_Msg16 = {
    104, "The attribute xsi:schemaLocation was not found." };

/* PSOQ_XML_MALFORMED_SCHEMA_LOCATION */
psoq_MsgStruct psoq_Msg17 = {
    105, "The attribute xsi:schemaLocation is not well formed." };

/* PSOQ_XML_PARSER_CONTEXT_FAILED */
psoq_MsgStruct psoq_Msg18 = {
    106, "The creation of a new schema parser context failed." };

/* PSOQ_XML_PARSE_SCHEMA_FAILED */
psoq_MsgStruct psoq_Msg19 = {
    107, "Failure when parsing the XML schema for config." };

/* PSOQ_XML_VALID_CONTEXT_FAILED */
psoq_MsgStruct psoq_Msg20 = {
    108, "The creation of a new XML Schemas validation context failed." };

/* PSOQ_XML_VALIDATION_FAILED */
psoq_MsgStruct psoq_Msg21 = {
    109, "Document validation for the config file failed." };

/* PSOQ_CFG_PSO_LOCATION_TOO_LONG */
psoq_MsgStruct psoq_Msg22 = {
    110, "Config: mem_location is too long." };

/* PSOQ_CFG_PSO_LOCATION_IS_MISSING */
psoq_MsgStruct psoq_Msg23 = {
    111, "Config: mem_location is missing." };

/* PSOQ_CFG_MEM_SIZE_IS_MISSING */
psoq_MsgStruct psoq_Msg24 = {
    112, "Config: mem_size is missing." };

/* PSOQ_CFG_SIZE_IS_MISSING */
psoq_MsgStruct psoq_Msg25 = {
    113, "Config: mem_size:size is missing." };

/* PSOQ_CFG_UNITS_IS_MISSING */
psoq_MsgStruct psoq_Msg26 = {
    114, "Config: mem_size:units is missing." };

/* PSOQ_CFG_WATCHDOG_ADDRESS_IS_MISSING */
psoq_MsgStruct psoq_Msg27 = {
    115, "Config: quasar_address is missing." };

/* PSOQ_CFG_FILE_ACCESS_IS_MISSING */
psoq_MsgStruct psoq_Msg28 = {
    116, "Config: file_access is missing." };

/* PSOQ_CFG_ACCESS_IS_MISSING */
psoq_MsgStruct psoq_Msg29 = {
    117, "Config: file_access:access is missing." };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Array of pointers to previous structs */
psoq_MsgStruct * psoq_MsgArray[30] = {
    &psoq_Msg0,
    &psoq_Msg1,
    &psoq_Msg2,
    &psoq_Msg3,
    &psoq_Msg4,
    &psoq_Msg5,
    &psoq_Msg6,
    &psoq_Msg7,
    &psoq_Msg8,
    &psoq_Msg9,
    &psoq_Msg10,
    &psoq_Msg11,
    &psoq_Msg12,
    &psoq_Msg13,
    &psoq_Msg14,
    &psoq_Msg15,
    &psoq_Msg16,
    &psoq_Msg17,
    &psoq_Msg18,
    &psoq_Msg19,
    &psoq_Msg20,
    &psoq_Msg21,
    &psoq_Msg22,
    &psoq_Msg23,
    &psoq_Msg24,
    &psoq_Msg25,
    &psoq_Msg26,
    &psoq_Msg27,
    &psoq_Msg28,
    &psoq_Msg29
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

const char * psoq_ErrorMessage( int errnum )
{
    int i;

    for ( i = 0; i < 30; ++i ) {
        if ( errnum == psoq_MsgArray[i]->errorNumber ) {
            return psoq_MsgArray[i]->message;
        }
    }

    return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

