/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
 
#ifndef QUEUE_H
#define QUEUE_H

#include <string>
#include <photon/photon>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Folder separators can either be Unix style or Windows style:
//    "fold\\object" or "fold/object"

const std::string inQueueName  = "QueueExample/InputQueue";
const std::string outQueueName = "QueueExample\\OutputQueue";
const std::string folderName   = "QueueExample";
const std::string controlName  = "QueueExample/QueueControl";

pso::Process process;
pso::Session session;
pso::Queue   inQueue, outQueue;
pso::HashMap control;

// Keys for the control
const char * workProcessKey = "Work Queue";
const char * outProcessKey  = "Out Queue";
const char * shutdownKey    = "Shutdown now";

typedef struct isoStruct
{
   char countryCode[2];
   char description[80];
} isoStruct;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif // QUEUE_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

