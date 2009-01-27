/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
 
#ifndef QUEUE_H
#define QUEUE_H

#include <photon/photon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * Folder separators can either be Unix style or Windows style:
 *   "fold\\object" or "fold/object"
 */
const char * inQueueName  = "QueueExample/InputQueue";
const char * outQueueName = "QueueExample\\OutputQueue";
const char * folderName   = "QueueExample";
const char * controlName  = "QueueExample/QueueControl";

PSO_HANDLE session = NULL, inQueue = NULL, outQueue = NULL, control = NULL;

/* Keys for the control */
const char * workProcessKey = "Work Queue";
const char * outProcessKey  = "Out Queue";
const char * shutdownKey    = "Shutdown now";

typedef struct isoStruct
{
   char countryCode[2];
   char description[80];
} isoStruct;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* QUEUE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
