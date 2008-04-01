/*
 * This file was generated by the program errorParser
 * using the input file ../../src/include/vdsf/vdsErrors.h.
 * (The code generator is copyrighted by Daniel Prevost 
 * <dprevost@users.sourceforge.net>)
 *
 * This file is free software; as a special exception the author gives
 * unlimited permission to copy and/or distribute it, with or without
 * modifications, as long as this notice is preserved.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef VDSERRORHANDLER_H
#define VDSERRORHANDLER_H

/* Just in case we get included by C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#define VDSEERR_MAX_ENTRIES  42
#define VDSEERR_MAX_COMMENTS 248

/*
 * Use this function to access the error messages (generated from the 
 * comments in the input header file).
 * 
 * Parameters:
 *   - errnum    The error number
 *   - msg       User-supplied buffer where the message will be copied
 *   - msgLength The length of the message.
 *
 * [Tip: you can safely use VDSEERR_MAX_COMMENTS to define the length of 
 * your buffer.]
 *
 * Return values:
 *    0  if errnum is valid (exists)
 *   -1  otherwise
 */

int vdseErrGetErrMessage( int errnum, char *msg, unsigned int msgLength );

#ifdef __cplusplus
} /* End of extern C */
#endif

#endif /* VDSERRORHANDLER_H */
