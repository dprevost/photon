/*
 * The original source for this function was taken from the public-domain 
 * timezone package of Arthur David Olson and contributors.
 * (ftp://elsie.nci.nih.gov/pub/)
 * 
 * The code in this file was modified for inclusion into Photon by
 * D. Prevost, May 2004. The modified code is still in the public domain.
 *
 */

/*
** This file is in the public domain, so clarified as of
** 1996-06-05 by Arthur David Olson (arthur_david_olson@nih.gov).
*/
/*
** Leap second handling from Bradley White (bww@k.gp.cs.cmu.edu).
** POSIX-style TZ environment variable handling from Guy Harris
** (guy@auspex.com).
*/

#include "Common/Common.h"
#include "Common/private.h"
#include "Common/tzfile.h"

#ifndef WILDABBR
/*
** Someone might make incorrect use of a time zone abbreviation:
**	1.	They might reference tzname[0] before calling tzset (explicitly
**		or implicitly).
**	2.	They might reference tzname[1] before calling tzset (explicitly
**		or implicitly).
**	3.	They might reference tzname[1] after setting to a time zone
**		in which Daylight Saving Time is never observed.
**	4.	They might reference tzname[0] after setting to a time zone
**		in which Standard Time is never observed.
**	5.	They might reference tm.TM_ZONE after calling offtime.
** What's best to do in the above cases is open to debate;
** for now, we just set things up so that in any of the five cases
** WILDABBR is used.  Another possibility:  initialize tzname[0] to the
** string "tzname[0] used before set", and similarly for the other cases.
** And another:  initialize tzname[0] to "ERA", with an explanation in the
** manual page of what this "time zone abbreviation" means (doing this so
** that tzname[0] has the "normal" length of three characters).
*/
#define WILDABBR	"   "
#endif /* !defined WILDABBR */

static char		wildabbr[] = "WILDABBR";

static const char	gmt[] = "GMT";

struct ttinfo {			/* time type information */
	long		tt_gmtoff;	/* UTC offset in seconds */
	int		tt_isdst;	/* used to set tm_isdst */
	int		tt_abbrind;	/* abbreviation list index */
	int		tt_ttisstd;	/* TRUE if transition is std time */
	int		tt_ttisgmt;	/* TRUE if transition is UTC */
};

struct lsinfo {				/* leap second information */
	time_t		ls_trans;	/* transition time */
	long		ls_corr;	      /* correction to apply */
};

#define BIGGEST(a, b)	(((a) > (b)) ? (a) : (b))

#ifdef TZNAME_MAX
#define MY_TZNAME_MAX	TZNAME_MAX
#endif /* defined TZNAME_MAX */
#ifndef TZNAME_MAX
#define MY_TZNAME_MAX	255
#endif /* !defined TZNAME_MAX */

struct state {
	int		leapcnt;
	int		timecnt;
	int		typecnt;
	int		charcnt;
	time_t		ats[TZ_MAX_TIMES];
	unsigned char	types[TZ_MAX_TIMES];
	struct ttinfo	ttis[TZ_MAX_TYPES];
	char		chars[BIGGEST(BIGGEST(TZ_MAX_CHARS + 1, sizeof gmt),
				(2 * (MY_TZNAME_MAX + 1)))];
	struct lsinfo	lsis[TZ_MAX_LEAPS];
};

/*
** Prototypes for static functions.
*/

static void 
timesub( const time_t * const		timep,
         const long			offset,
         const struct state * const	sp,
         struct tm * const		tmp );

static struct state	lclmem;
#define lclptr		(&lclmem)

#ifndef TZ_STRLEN_MAX
#define TZ_STRLEN_MAX 255
#endif /* !defined TZ_STRLEN_MAX */

static char		lcl_TZname[TZ_STRLEN_MAX + 1];
static int		lcl_is_set;
static int		gmt_is_set;

/* Already defined by the VC++ crt library */
#if ! defined(WIN32)
char * tzname[2] = {
   wildabbr,
   wildabbr
};
#endif

/*
** Section 4.12.3 of X3.159-1989 requires that
**	Except for the strftime function, these functions [asctime,
**	ctime, gmtime, localtime] return values in one of two static
**	objects: a broken-down time structure and an array of char.
** Thanks to Paul Eggert (eggert@twinsun.com) for noting this.
*/

#ifdef USG_COMPAT
time_t			timezone = 0;
int			daylight = 0;
#endif /* defined USG_COMPAT */

#ifdef ALTZONE
time_t			altzone = 0;
#endif /* defined ALTZONE */

static const int	mon_lengths[2][MONSPERYEAR] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const int	year_lengths[2] = {
	DAYSPERNYEAR, DAYSPERLYEAR
};

/*
** The easy way to behave "as if no library function calls" localtime
** is to not call it--so we drop its guts into "localsub", which can be
** freely called.  (And no, the PANS doesn't require the above behavior--
** but it *is* desirable.)
**
** The unused offset argument is for the benefit of mktime variants.
*/

/*ARGSUSED*/
static void
localsub( const time_t * const	timep,
          const long		offset,
          struct tm * const	tmp )
{
	register struct state *		sp;
	register const struct ttinfo *	ttisp;
	register int			i;
	const time_t			t = *timep;
        long useless;
	
	useless = offset; /* Removes a warning */
	sp = lclptr;
	if (sp->timecnt == 0 || t < sp->ats[0]) {
		i = 0;
		while (sp->ttis[i].tt_isdst) {
			if (++i >= sp->typecnt) {
				i = 0;
				break;
			}
      }
	} 
   else {
		for (i = 1; i < sp->timecnt; ++i) {
			if (t < sp->ats[i]) break;
      }
		i = sp->types[i - 1];
	}
	ttisp = &sp->ttis[i];
	/*
	** To get (wrong) behavior that's compatible with System V Release 2.0
	** you'd replace the statement below with
	**	t += ttisp->tt_gmtoff;
	**	timesub(&t, 0L, sp, tmp);
	*/
	timesub(&t, ttisp->tt_gmtoff, sp, tmp);
	tmp->tm_isdst = ttisp->tt_isdst;
	tzname[tmp->tm_isdst] = &sp->chars[ttisp->tt_abbrind];
#ifdef TM_ZONE
	tmp->TM_ZONE = &sp->chars[ttisp->tt_abbrind];
#endif /* defined TM_ZONE */
}

/*
** Re-entrant version of localtime.
*/

PHOTON_COMMON_EXPORT
struct tm *
localtime_r( const time_t * 	timep,
             struct tm *	tm )
{
	localsub(timep, 0L, tm);
	return tm;
}

static void
timesub( const time_t * const		timep,
         const long			offset,
         const struct state * const	sp,
         struct tm * const		tmp )
{
	register const struct lsinfo *	lp;
	register long			days;
	register long			rem;
	register int			y;
	register int			yleap;
	register const int *		ip;
	register long			corr;
	register int			hit;
	register int			i;

	corr = 0;
	hit = 0;
	i = sp->leapcnt;
	while (--i >= 0) {
		lp = &sp->lsis[i];
		if (*timep >= lp->ls_trans) {
			if (*timep == lp->ls_trans) {
				hit = ((i == 0 && lp->ls_corr > 0) ||
					lp->ls_corr > sp->lsis[i - 1].ls_corr);
				if (hit) {
					while (i > 0 &&
						sp->lsis[i].ls_trans ==
						sp->lsis[i - 1].ls_trans + 1 &&
						sp->lsis[i].ls_corr ==
						sp->lsis[i - 1].ls_corr + 1) {
							++hit;
							--i;
					}
            }
			}
			corr = lp->ls_corr;
			break;
		}
	}
	days = (long)*timep / SECSPERDAY;
	rem  = (long)*timep % SECSPERDAY;
#ifdef mc68k
	if (*timep == 0x80000000) {
		/*
		** A 3B1 muffs the division on the most negative number.
		*/
		days = -24855;
		rem = -11648;
	}
#endif /* defined mc68k */
	rem += (offset - corr);
	while (rem < 0) {
		rem += SECSPERDAY;
		--days;
	}
	while (rem >= SECSPERDAY) {
		rem -= SECSPERDAY;
		++days;
	}
	tmp->tm_hour = (int) (rem / SECSPERHOUR);
	rem = rem % SECSPERHOUR;
	tmp->tm_min = (int) (rem / SECSPERMIN);
	/*
	** A positive leap second requires a special
	** representation.  This uses "... ??:59:60" et seq.
	*/
	tmp->tm_sec = (int) (rem % SECSPERMIN) + hit;
	tmp->tm_wday = (int) ((EPOCH_WDAY + days) % DAYSPERWEEK);
	if (tmp->tm_wday < 0) {
		tmp->tm_wday += DAYSPERWEEK;
   }
	y = EPOCH_YEAR;
#define LEAPS_THRU_END_OF(y)	((y) / 4 - (y) / 100 + (y) / 400)
	while (days < 0 || days >= (long) year_lengths[yleap = isleap(y)]) {
		register int	newy;

		newy = y + days / DAYSPERNYEAR;
		if (days < 0) --newy;
		days -= (newy - y) * DAYSPERNYEAR +
			LEAPS_THRU_END_OF(newy - 1) -
			LEAPS_THRU_END_OF(y - 1);
		y = newy;
	}
	tmp->tm_year = y - TM_YEAR_BASE;
	tmp->tm_yday = (int) days;
	ip = mon_lengths[yleap];
	for (tmp->tm_mon = 0; days >= (long) ip[tmp->tm_mon]; ++(tmp->tm_mon)) {
		days = days - (long) ip[tmp->tm_mon];
   }
	tmp->tm_mday = (int) (days + 1);
	tmp->tm_isdst = 0;
#ifdef TM_GMTOFF
	tmp->TM_GMTOFF = offset;
#endif /* defined TM_GMTOFF */
}

