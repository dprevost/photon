
# The definition of ARCH is from the main Makefile of the
# linux kernel.

ARCH := $(shell uname -m | sed -e s/i.86/i386/ -e s/sun4u/sparc64/ \
				  -e s/arm.*/arm/ -e s/sa110/arm/ \
				  -e s/s390x/s390/ -e s/parisc64/parisc/ \
				  -e s/ppc.*/powerpc/ -e s/mips.*/mips/ )

all: asm

asm:
	ln -fsn ./asm-$(ARCH) asm
   
distclean: clean

clean:
	rm -f asm

check: asm

