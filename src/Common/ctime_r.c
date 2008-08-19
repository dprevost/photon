/*
 * The original source for this function was taken from the public-domain 
 * timezone package of Arthur David Olson and contributors.
 * (ftp://elsie.nci.nih.gov/pub/)
 * 
 * The code in this file was modified for inclusion into Photon by
 * D. Prevost, May 2004. Evidently, the modified code is still in the 
 * public domain.
 *
 */

/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson (arthur_david_olson@nih.gov).
*/

#include "private.h"
#include "tzfile.h"

/*
** Section 4.12.3.2 of X3.159-1989 requires that
**	The ctime function converts the calendar time pointed to by timer
**	to local time in the form of a string.  It is equivalent to
**		asctime(localtime(timer))
*/

extern char *new_asctime_r( const struct tm *, char *, int );
extern struct tm *
localtime_r( const time_t * const timep,
             struct tm *          tm );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

char *
new_ctime_r (const time_t *timep, char *buf, int buflen)
{
	struct tm	tm;
   
	buf = new_asctime_r(localtime_r(timep, &tm), buf, buflen );
   return buf;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

