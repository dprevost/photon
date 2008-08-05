/*
 * This file was generated by the program errorParser
 * using the input file wdErrors.xml.
 * Date: Mon Aug  4 23:16:00 2008.
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
#include "wdErrorHandler.h"

struct vdsw_MsgStruct
{
    int  errorNumber;
    const char * message;
};

typedef struct vdsw_MsgStruct vdsw_MsgStruct;

/* VDSW_OK */
vdsw_MsgStruct vdsw_Msg0 = {
    0, "No error..." };

/* VDSW_NOT_ENOUGH_HEAP_MEMORY */
vdsw_MsgStruct vdsw_Msg1 = {
    1, "Not enough heap memory (non-VDS memory)." };

/* VDSW_CFG_BCK_LOCATION_TOO_LONG */
vdsw_MsgStruct vdsw_Msg2 = {
    2, "Backup path is too long." };

/* VDSW_NO_VERIFICATION_POSSIBLE */
vdsw_MsgStruct vdsw_Msg3 = {
    3, "Cannot verify the VDS." };

/* VDSW_MKDIR_FAILURE */
vdsw_MsgStruct vdsw_Msg4 = {
    4, "Cannot create the directory for the VDS." };

/* VDSW_FILE_NOT_ACCESSIBLE */
vdsw_MsgStruct vdsw_Msg5 = {
    5, "Cannot access the backstore file for the VDS." };

/* VDSW_COPY_BCK_FAILURE */
vdsw_MsgStruct vdsw_Msg6 = {
    6, "Error while creating a copy of the backstore file for the VDS." };

/* VDSW_CREATE_BACKSTORE_FAILURE */
vdsw_MsgStruct vdsw_Msg7 = {
    7, "Error creating the backstore file for the VDS." };

/* VDSW_OPEN_BACKSTORE_FAILURE */
vdsw_MsgStruct vdsw_Msg8 = {
    8, "Error opening the backstore file for the VDS." };

/* VDSW_INCOMPATIBLE_VERSIONS */
vdsw_MsgStruct vdsw_Msg9 = {
    9, "Memory-file version mismatch." };

/* VDSW_BACKSTORE_FILE_MISSING */
vdsw_MsgStruct vdsw_Msg10 = {
    10, "The vds backstore file is missing." };

/* VDSW_ERROR_OPENING_VDS */
vdsw_MsgStruct vdsw_Msg11 = {
    11, "Generic i/o error when attempting to open the vds." };

/* VDSW_XML_CONFIG_NOT_OPEN */
vdsw_MsgStruct vdsw_Msg12 = {
    100, "Error opening the XML config file." };

/* VDSW_XML_READ_ERROR */
vdsw_MsgStruct vdsw_Msg13 = {
    101, "Error reading the XML config file." };

/* VDSW_XML_NO_ROOT */
vdsw_MsgStruct vdsw_Msg14 = {
    102, "No root element in the XML document." };

/* VDSW_XML_INVALID_ROOT */
vdsw_MsgStruct vdsw_Msg15 = {
    103, "The root element is not the expected root." };

/* VDSW_XML_NO_SCHEMA_LOCATION */
vdsw_MsgStruct vdsw_Msg16 = {
    104, "The attribute xsi:schemaLocation was not found." };

/* VDSW_XML_MALFORMED_SCHEMA_LOCATION */
vdsw_MsgStruct vdsw_Msg17 = {
    105, "The attribute xsi:schemaLocation is not well formed." };

/* VDSW_XML_PARSER_CONTEXT_FAILED */
vdsw_MsgStruct vdsw_Msg18 = {
    106, "The creation of a new schema parser context failed." };

/* VDSW_XML_PARSE_SCHEMA_FAILED */
vdsw_MsgStruct vdsw_Msg19 = {
    107, "Failure when parsing the XML schema for config." };

/* VDSW_XML_VALID_CONTEXT_FAILED */
vdsw_MsgStruct vdsw_Msg20 = {
    108, "The creation of a new XML Schemas validation context failed." };

/* VDSW_XML_VALIDATION_FAILED */
vdsw_MsgStruct vdsw_Msg21 = {
    109, "Document validation for the config file failed." };

/* VDSW_CFG_VDS_LOCATION_TOO_LONG */
vdsw_MsgStruct vdsw_Msg22 = {
    110, "Config: vds_location is too long." };

/* VDSW_CFG_VDS_LOCATION_IS_MISSING */
vdsw_MsgStruct vdsw_Msg23 = {
    111, "Config: vds_location is missing." };

/* VDSW_CFG_MEM_SIZE_IS_MISSING */
vdsw_MsgStruct vdsw_Msg24 = {
    112, "Config: mem_size is missing." };

/* VDSW_CFG_SIZE_IS_MISSING */
vdsw_MsgStruct vdsw_Msg25 = {
    113, "Config: mem_size:size is missing." };

/* VDSW_CFG_UNITS_IS_MISSING */
vdsw_MsgStruct vdsw_Msg26 = {
    114, "Config: mem_size:units is missing." };

/* VDSW_CFG_WATCHDOG_ADDRESS_IS_MISSING */
vdsw_MsgStruct vdsw_Msg27 = {
    115, "Config: watchdog_address is missing." };

/* VDSW_CFG_FILE_ACCESS_IS_MISSING */
vdsw_MsgStruct vdsw_Msg28 = {
    116, "Config: file_access is missing." };

/* VDSW_CFG_ACCESS_IS_MISSING */
vdsw_MsgStruct vdsw_Msg29 = {
    117, "Config: file_access:access is missing." };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Array of pointers to previous structs */
vdsw_MsgStruct * vdsw_MsgArray[30] = {
    &vdsw_Msg0,
    &vdsw_Msg1,
    &vdsw_Msg2,
    &vdsw_Msg3,
    &vdsw_Msg4,
    &vdsw_Msg5,
    &vdsw_Msg6,
    &vdsw_Msg7,
    &vdsw_Msg8,
    &vdsw_Msg9,
    &vdsw_Msg10,
    &vdsw_Msg11,
    &vdsw_Msg12,
    &vdsw_Msg13,
    &vdsw_Msg14,
    &vdsw_Msg15,
    &vdsw_Msg16,
    &vdsw_Msg17,
    &vdsw_Msg18,
    &vdsw_Msg19,
    &vdsw_Msg20,
    &vdsw_Msg21,
    &vdsw_Msg22,
    &vdsw_Msg23,
    &vdsw_Msg24,
    &vdsw_Msg25,
    &vdsw_Msg26,
    &vdsw_Msg27,
    &vdsw_Msg28,
    &vdsw_Msg29
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

const char * vdsw_ErrorMessage( int errnum )
{
    int i;

    for ( i = 0; i < 30; ++i ) {
        if ( errnum == vdsw_MsgArray[i]->errorNumber ) {
            return vdsw_MsgArray[i]->message;
        }
    }

    return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

