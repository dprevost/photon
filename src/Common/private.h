/*
 * The original source for this function was taken from the public-domain 
 * timezone package of Arthur David Olson and contributors.
 * (ftp://elsie.nci.nih.gov/pub/)
 * 
 * The code in this file was modified for inclusion into Photon by
 * D. Prevost, May 2004. The modified code is still in the public domain.
 *
 */

#ifndef PSOC_PRIVATE_H
#define PSOC_PRIVATE_H

/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson (arthur_david_olson@nih.gov).
*/

/*
** This header is for use ONLY with the time conversion code.
** There is no guarantee that it will remain unchanged,
** or that it will remain at all.
** Do NOT copy it to any system include directory.
** Thank you!
*/


/*
** Nested includes
*/

#include "sys/types.h"	/* for time_t */
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "limits.h"	/* for CHAR_BIT */
#include "time.h"
#include "stdlib.h"

/*
** Workarounds for compilers/systems.
*/


/*
** Some ancient errno.h implementations don't declare errno.
** But some newer errno.h implementations define it as a macro.
** Fix the former without affecting the latter.
*/
#ifndef errno
extern int errno;
#endif /* !defined errno */


/*
** UNIX was a registered trademark of The Open Group in 2003.
*/

#endif /* !defined PSOC_PRIVATE_H */
