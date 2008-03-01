# Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
# 
# This code is in the public domain.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

LDFLAGS += -L../API/.libs -lvdsf
CFLAGS += -g -I../include -Wall

TARGETS = HashMap     \
          HashMapLoop \
          QueueIn     \
          QueueWork   \
          QueueOut    

all: $(TARGETS)

#HashMap.o: HashMap.c iso_3166.h
#	$(CC) -g -c HashMap.c

#HashMap: HashMap.o
#	$(LD) -o HashMap HashMap.o -lvdsf

clean:
	rm -f *.o $(TARGETS) *~ core
