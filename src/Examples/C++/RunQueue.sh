#!/bin/sh

# Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
# 
# This code is in the public domain.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

if [ "$#" != 5 ] ; then
   echo "usage: $0 iso_data_file quasar_address number_of_iterations millisecs iterations_per_cycle"
   exit 1
fi

iso_file=$1
wd_addr=$2
num_iterations=$3
ms=$4
cycle=$5

./QueueIn $iso_file $wd_addr $num_iterations $ms $cycle &
sleep 1
./QueueWork $wd_addr &
./QueueOut  $wd_addr &

# if [ "$?" != 0 ] ; then