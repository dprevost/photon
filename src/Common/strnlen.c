/*
 * Taken from http://unixpapa.com/incnote/string.html
 *
 * This code is in the public domain.
 */

#include "Common.h"

#ifndef HAVE_STRNLEN

size_t strnlen( const char * s, size_t maxlen )
{
	size_t i;
   
	for ( i = 0; i < maxlen && *s != '\0'; i++, s++) ;

	return i;
}

#endif
