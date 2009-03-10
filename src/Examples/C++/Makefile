# Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
# 
# This code is in the public domain.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# The additional library path is for photon developers (who may not have
# installed photon).
LDFLAGS += -L../../C++/.libs -lphotoncpp

CPPFLAGS += -g -I../../include -Wall

TARGETS = HashMap     \
          HashMapLoop \
          QueueIn     \
          QueueWork   \
          QueueOut    

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) *~ core
