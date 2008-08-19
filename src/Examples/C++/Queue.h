/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <photon/vds>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Folder separators can either be Unix style or Windows style:
//    "fold\\object" or "fold/object"

const std::string inQueueName  = "Queue Example/Input Queue";
const std::string outQueueName = "Queue Example\\Output Queue";
const std::string folderName   = "Queue Example";
const std::string controlName  = "Queue Example/Queue Control";

psoProcess process;
psoSession session;
psoQueue   inQueue(session), outQueue(session);
psoHashMap control(session);

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

