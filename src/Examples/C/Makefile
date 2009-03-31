# Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
# 
# This code is in the public domain.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# The additional library path is for photon developers (who may not have
# installed photon).
LDFLAGS += -L../../API/.libs -lphoton
# The additional include path is for photon developers (who may not have
# installed photon).
CFLAGS += -g -I../../include -Wall

TARGETS = FastPerf      \
          FastPerfByKey \
          HashMap       \
          HashMapLoop   \
          HashPerf      \
          QueueIn       \
          QueueWork     \
          QueueOut      

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) *~ core
