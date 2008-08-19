/*
 * The original source for this function was taken from the public-domain 
 * timezone package of Arthur David Olson and contributors.
 * (ftp://elsie.nci.nih.gov/pub/)
 * 
 * The code in this file was slightly modified for inclusion into Photon 
 * by D. Prevost, May 2004. Evidently, the modified code is still in the 
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
** A la ISO/IEC 9945-1, ANSI/IEEE Std 1003.1, Second Edition, 1996-07-12.
*/

char *
new_asctime_r(const struct tm *timeptr, char *buf, int buflen )
{
	static const char	wday_name[][3] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char	mon_name[][3] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	register const char *	wn;
	register const char *	mn;

	if (timeptr->tm_wday < 0 || timeptr->tm_wday >= DAYSPERWEEK) {
		wn = "???";
   }
	else	wn = wday_name[timeptr->tm_wday];
	if (timeptr->tm_mon < 0 || timeptr->tm_mon >= MONSPERYEAR) {
		mn = "???";
   }
	else	mn = mon_name[timeptr->tm_mon];
	/*
	** The X3J11-suggested format is
	**	"%.3s %.3s%3d %02.2d:%02.2d:%02.2d %d\n"
	** Since the .2 in 02.2d is ignored, we drop it.
	*/
	(void) sprintf(buf, "%.3s %.3s%3d %02d:%02d:%02d %d\n",
		wn, mn,
		timeptr->tm_mday, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec,
		TM_YEAR_BASE + timeptr->tm_year);
	return buf;
}

