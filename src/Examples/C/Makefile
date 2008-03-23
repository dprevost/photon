# Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
# 
# This code is in the public domain.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# The additional library path is for vdsf developers (who may not have
# installed vdsf).
LDFLAGS += -L../../API/.libs -lvdsf
# The additional include path is for vdsf developers (who may not have
# installed vdsf).
CFLAGS += -g -I../../include -Wall

TARGETS = HashMap     \
          HashMapLoop \
          QueueIn     \
          QueueWork   \
          QueueOut    

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) *~ core
