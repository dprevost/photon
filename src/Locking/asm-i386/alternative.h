#ifndef _I386_ALTERNATIVE_H
#define _I386_ALTERNATIVE_H

/*
 * Alternative instructions for different CPU types or capabilities.
 *
 * This allows to use optimized instructions even on generic binary
 * kernels.
 *
 * length of oldinstr must be longer or equal the length of newinstr
 * It can be padded with nops as needed.
 *
 * For non barrier like inlines please define new variants
 * without volatile and memory clobber.
 */
#define alternative(oldinstr, newinstr, feature)			\
	asm volatile ("661:\n\t" oldinstr "\n662:\n" 			\
		      ".section .altinstructions,\"a\"\n"		\
		      "  .align 4\n"					\
		      "  .long 661b\n"            /* label */		\
		      "  .long 663f\n"		  /* new instruction */	\
		      "  .byte %c0\n"             /* feature bit */	\
		      "  .byte 662b-661b\n"       /* sourcelen */	\
		      "  .byte 664f-663f\n"       /* replacementlen */	\
		      ".previous\n"					\
		      ".section .altinstr_replacement,\"ax\"\n"		\
		      "663:\n\t" newinstr "\n664:\n"   /* replacement */\
		      ".previous" :: "i" (feature) : "memory")

#define LOCK_PREFIX \
		".section .smp_locks,\"a\"\n"	\
		"  .align 4\n"			\
		"  .long 661f\n" /* address */	\
		".previous\n"			\
	       	"661:\n\tlock; "

#endif /* _I386_ALTERNATIVE_H */

