/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Options.h"
#include "Common.h"

/** 
 * Unique identifier for the psocInternalOpt struct. 
 * \ingroup psocOptions 
 */
#define PSOC_OPTION_SIGNATURE ((unsigned int)0xd70e9ab8)

/** This struct augments the original struct used to describe the 
 *  supported options (by adding field that indicates where the
 *  the options are (if found) in the list of arguments provided
 *  to the program in argv[]). 
 *
 *  This struct is \b not used by the interface of this module but
 *  hidden away in the implementation (the code).
 *
 *  A value of zero in the two location fields indicates that
 *  the option is absent (since argv[0] is reserved for the
 *  program name).
 * \ingroup psocOptions 
 */
struct psocOptArray
{
   /** Local copy of the description of supported options. */
   struct psocOptStruct opt;
   /** Location of the option (if found) in the argv[] array. */
   int optionLocation;
   /** Location of the argument (if found) in the argv[] array. */
   int argumentLocation;
   
};

typedef struct psocOptArray psocOptArray;

/** 
 *  This struct is \b not used by the interface of this module but
 *  hidden away in the implementation (the code).
 *
 *  To be exact, a pointer to this struct is passed as an opaque handle,
 *  ::psocOptionHandle, throughout the interface. 
 *
 *  The struct itself contains all our "bookkeeping stuff".
 * \ingroup psocOptions 
 */
struct psocInternalOpt
{
   /** Set to PSOC_OPTION_SIGNATURE at initialization. */
   unsigned int initialized;
   
   /** Pointer to the allocated array. */
   psocOptArray * pArray;

   /** Number of supported option (array size). */
   int numOpt;

   /**
    * We keep a copy of the value of argv for psocGetLongOptArgument and 
    * similar to retrieve the argument from the proper argv[].
    */
   char ** argv;

   /** Additional protection - this field is set to "true" (1) only after
    *  the argv[] array is analyzed and validated. */
   int validated;

};

typedef struct psocInternalOpt psocInternalOpt;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * The option for help does not need to be included.
 *
 * A snippet of code, showing how to use this function:
 * \code
 *   psocOptionHandle handle;
 *   bool ok;
 *
 *   struct psocOptStruct opts[5] = 
 *      { '3', "three",   1, "", "repeat the loop three times",
 *        'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server",
 *        'x', "",        1, "DISPLAY", "X display to use",
 *        'v', "verbose", 1, "", "try to explain what is going on",
 *        'z', "zzz",     1, "", "go to sleep..."
 *      };
 *  
 *   ok = psocSetSupportedOptions( 5, opts, &handle );
 \endcode

 * \param[in] numOpts The number of supported options.
 * \param[in] opts    Array of psocOptStruct describing the supported 
 *                    options.
 * \param[out] pHandle A pointer to an opaque handle.
 *
 * \retval true  on success
 * \retval false on error
 *
 * \pre \em numOpts must be greater than zero.
 * \pre \em opts cannot be NULL.
 * \pre \em pHandle cannot be NULL.
 * \pre Both long and short options cannot be both empty.
 * \pre All three strings in the psocOptStruct struct must be NULL terminated.
 * \pre Options (both short and long) must be unique.
 */
bool psocSetSupportedOptions( int                numOpts, 
                              psocOptStruct    * opts,
                              psocOptionHandle * pHandle )
{
   int i, k;
   bool nullTerminatedString;
   psocInternalOpt* optStruct;
   
   /* There is no point in supporting all of this if the list
    * of available options is empty...
    */
   PSO_PRE_CONDITION( numOpts > 0 );
   PSO_PRE_CONDITION( opts != NULL );
   PSO_PRE_CONDITION( pHandle != NULL );

   /* Test the integrity of the options */
   for ( i = 0; i < numOpts; ++i ) {
      /* Both short and long are empty */
      PSO_PRE_CONDITION( 
         ( opts[i].shortOpt   != '\0' && 
           opts[i].shortOpt   != ' '  ) ||
         ( opts[i].longOpt[0] != '\0' && 
           opts[i].longOpt[0] != ' '  ) );

      /* All strings NULL terminated (this test must take place
       * before the "repeated option test" since we use the strcmp() 
       * function in that test).
       */
      nullTerminatedString = false;
      for ( k = 0; k < PSOC_OPT_LONG_OPT_LENGTH; ++k ) {
         if ( opts[i].longOpt[k] == '\0' ) {
            nullTerminatedString = true;
            break;
         }
      }
      PSO_PRE_CONDITION( nullTerminatedString == true );

      nullTerminatedString = false;
      for ( k = 0; k < PSOC_OPT_ARGUMENT_MSG_LENGTH; ++k ) {
         if ( opts[i].argumentMessage[k] == '\0' ) {
            nullTerminatedString = true;
            break;
         }
      }
      PSO_PRE_CONDITION( nullTerminatedString == true );

      nullTerminatedString = false;
      for ( k = 0; k < PSOC_OPT_COMMENT_LENGTH; ++k ) {
         if ( opts[i].comment[k] == '\0' ) {
            nullTerminatedString = true;
            break;
         }
      }
      PSO_PRE_CONDITION( nullTerminatedString == true );
      
      /* Repeated options */
      for ( k = 0; k < i; ++k ) {
         if ( opts[i].shortOpt != '\0' && opts[i].shortOpt != ' ' ) {
            PSO_PRE_CONDITION( opts[i].shortOpt != opts[k].shortOpt );
         }
         if ( opts[i].longOpt[0] != '\0' && opts[i].longOpt[0] != ' ' ) {
            if ( strlen(opts[i].longOpt) == strlen(opts[i].longOpt) ) {
               PSO_PRE_CONDITION( strcmp( opts[i].longOpt, 
                                          opts[k].longOpt ) != 0 );
            }
         }
      }

   } /* end of loop for preconditions */
   
   optStruct = (psocInternalOpt *)malloc( sizeof(psocInternalOpt) );
   if ( optStruct == NULL ) return false;
   
   optStruct->pArray = (psocOptArray *)malloc(numOpts*sizeof(psocOptArray));
   if ( optStruct->pArray == NULL ) {
      free( optStruct );
      return false;
   }

   memset( optStruct->pArray, 0, numOpts*sizeof(psocOptArray) );
   optStruct->numOpt = numOpts;
   optStruct->argv = NULL;
   optStruct->validated = 0;
   
   for ( i = 0; i < numOpts; ++i ) {
      memcpy( &optStruct->pArray[i].opt, &opts[i], sizeof(psocOptStruct) );
   }
   
   *pHandle = (void*) optStruct;

   optStruct->initialized = PSOC_OPTION_SIGNATURE;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * This function will check to see if any mandatory options are 
 * missing or if the mandatory arguments (for options with mandatory 
 * arguments) are present.
 *
 * The function assumes that argv[0] is not an option but the program
 * name (or some other dummy value in some cases). 
 *
 * If the test fails it will print info on the error (on stderr) if 
 * printError is set to 1.
 *
 * \param[in] handle Opaque handle to a psocOptArray struct.
 * \param[in] argc   Size of the array argv[]
 * \param[in] argv   Array of arguments passed in to the program.
 * \param[in] printError A flag. If true, it will print error messages on
 *                       stderr, if needed.
 *
 * \retval 1  Help was requested (-?, -h or --help)
 * \retval 0  Success
 * \retval -1 Error
 *
 * \pre \em handle cannot be NULL.
 * \pre \em argc must be greater than zero.
 * \pre \em argv (and all the member of the array) cannot be NULL.
 *
 * \invariant \em optStruct->initialized must equal 
 *                ::PSOC_OPTION_SIGNATURE.
 */
int psocValidateUserOptions( psocOptionHandle   handle,
                             int                argc, 
                             char             * argv[], 
                             int                printError )
{
   int i = 1, j, returnCode = 0, found;
   size_t len;
   psocInternalOpt* optStruct = (psocInternalOpt*)handle;
   
   PSO_PRE_CONDITION( handle != NULL );
   PSO_INV_CONDITION( optStruct->initialized == PSOC_OPTION_SIGNATURE );
   
   PSO_PRE_CONDITION( argc > 0 );
   PSO_PRE_CONDITION( argv != NULL );
   for ( i = 0; i < argc; ++i ) {
      PSO_PRE_CONDITION( argv[i] != NULL );
   }
   
   if ( optStruct->validated ) {
      /* So we were called before... better clear some stuff before
       * it leads to confusion. */
      for ( i = 0; i < optStruct->numOpt; ++i ) {
         optStruct->pArray[i].optionLocation = 0;
         optStruct->pArray[i].argumentLocation = 0;
      }
   }
   
   i = 1;
   while ( i < argc ) {
      if (argv[i][0] == '-' ) {
         /* We found an option! Test the length to know
          * if it is a long or short one (or a single dash).
          * (don't use strlen - this is directly from the user
          * and we want to make sure there are no buffer overflow).
          */
         
         for ( j = 1, len = 0; j < PSOC_OPT_LONG_OPT_LENGTH; ++j ) {
            if ( argv[i][j] == '\0' ) {
               len = j;
               break;
            }
         }
         
         if ( len < 2 ) {
            if ( printError ) {
               fprintf( stderr, "Unrecognized option: %s\n", argv[i] );
            }
            returnCode = -1;
            break;
         }
         if ( len == 2 ) {
            /* Possibly, a short option */

            if ( argv[i][1] == '?' || argv[i][1] == 'h' ) {
               returnCode = 1;
               break;
            }
            found = 0;
            for ( j = 0; j < optStruct->numOpt; ++j ) {
               /* A long option with no equivalent short option
                * is a possibility (and the other way around)
                */
               if ( optStruct->pArray[j].opt.shortOpt == '\0' ) continue;
               if ( optStruct->pArray[j].opt.shortOpt == ' '  ) continue;

               if ( argv[i][1] == optStruct->pArray[j].opt.shortOpt ) {
                  found = 1;
                  optStruct->pArray[j].optionLocation = i;
                  
                  if ( optStruct->pArray[j].opt.argumentMessage[0] != '\0' &&
                       optStruct->pArray[j].opt.argumentMessage[0] != ' ' ) {
                     if ( i == argc-1 ) {
                        returnCode = -1;
                        break;
                     }
                     if ( argv[i+1][0] == '-' ) {
                        returnCode = -1;
                        break;
                     }

                     optStruct->pArray[j].argumentLocation = i+1;
                     i++;
                     break;
                  }
               }
            }
            if ( returnCode == 0 && found == 1 ) {
               ++i;
               continue;
            }
            
            if ( printError && found == 0 ) {
               fprintf( stderr, "Unrecognized option: %s\n", argv[i] );
            }
            
            if ( printError && found == 1 ) {
               fprintf( stderr, "Missing or illegal argument to option: %s\n", 
                        argv[i] );
            }
            
            returnCode = -1;
            break;
            
         } /* end if - len == 2 */
         else {
            /* A long option then... */

            if ( len == 6 ) {
               if ( memcmp(argv[i], "--help", 6) == 0 ) {
                  returnCode = 1;
                  break;
               }
            }
            
            found = 0;
            for ( j = 0; j < optStruct->numOpt; ++j ) {
               /* A short option with no equivalent long option
                * is a possibility (and the other way around)
                */
               if ( optStruct->pArray[j].opt.longOpt[0] == '\0' ) continue;
               if ( optStruct->pArray[j].opt.longOpt[0] == ' '  ) continue;
               
               if ( len == (strlen(optStruct->pArray[j].opt.longOpt) + 2) ) {
                  if ( memcmp( &argv[i][2], 
                               optStruct->pArray[j].opt.longOpt, 
                               len-2 ) == 0 ) {
                     found = 1;
                     optStruct->pArray[j].optionLocation = i;
                  
                     if ( optStruct->pArray[j].opt.argumentMessage[0] != '\0' &&
                          optStruct->pArray[j].opt.argumentMessage[0] != ' ' ) {
                        if ( i == argc-1 ) {
                           returnCode = -1;
                           break;
                        }
                        if ( argv[i+1][0] == '-' ) {
                           returnCode = -1;
                           break;
                        }
                        
                        optStruct->pArray[j].argumentLocation = i+1;
                        i++;
                        break;
                     }

                  } /* end if - check that the content is equal */
               } /* end if - check that lenghts are equal */
            }
            if ( returnCode == 0 && found == 1 ) {
               ++i;
               continue;
            }   

            if ( printError && found == 0 ) {
               fprintf( stderr, "Unrecognized option: %s\n", argv[i] );
            }
            
            if ( printError && found == 1 ) {
               fprintf( stderr, "Missing or illegal argument to option: %s\n", 
                        argv[i] );
            }
            
            returnCode = -1;
            break;

         } /* end of else - has a long option */
         
      } /* end if - found an option */
      else {
         i++; 
      }
   } /* end of while on argc */
      
   /* Make sure all mandatory options are present */
   if ( returnCode == 0 ) {
      for ( j = 0; j < optStruct->numOpt; ++j ) {
         /* Mandatory? */
         if ( optStruct->pArray[j].opt.isOptionel == 1 ) continue;
         /* Is it there? */
         if ( optStruct->pArray[j].optionLocation > 0 ) continue;
         
         if ( printError ) {
            if ( optStruct->pArray[j].opt.shortOpt == '\0' ||
                 optStruct->pArray[j].opt.shortOpt == ' '  ) {
               fprintf( stderr, "%s: Missing option (--%s)\n",
                        argv[0], 
                        optStruct->pArray[j].opt.longOpt );
            }
            else if ( optStruct->pArray[j].opt.longOpt[0] == '\0' ||
                      optStruct->pArray[j].opt.longOpt[0] == ' '  ) {
               fprintf( stderr, "%s: Missing option (-%c)\n",
                        argv[0], 
                        optStruct->pArray[j].opt.shortOpt );
            }
            else {
               fprintf( stderr, "%s: Missing option (either -%c or --%s)\n",
                        argv[0], 
                        optStruct->pArray[j].opt.shortOpt, 
                        optStruct->pArray[j].opt.longOpt );
            }
         }
         
         returnCode = -1;
         break;
      }
   }
   
   if ( returnCode == 0 ) {
      optStruct->argv = argv;
      optStruct->validated = 1;
   }
   else {
      /* Better clear some stuff before it leads to confusion. */
      for ( i = 0; i < optStruct->numOpt; ++i ) {
         optStruct->pArray[i].optionLocation = 0;
         optStruct->pArray[i].argumentLocation = 0;
      }
   }
   
   return returnCode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * This function uses the short option as the key for the search but 
 * this is independent of the way the option was entered by the 
 * end-user (short or long) if both versions of the option were defined.
 *
 * \note The decision to use two functions instead of a unified one 
 * was done in order to simplify the code. Otherwise the code would 
 * have required multiple additional checks.
 *
 * \param[in] handle Opaque handle to a psocOptArray struct.
 * \param[in] opt    The short option.
 * \param[out] argument The argument associated with this short option, if any.
 *
 * \retval false  If either option or argument are not found. *argument 
 *                is also set to NULL.
 * \retval true   The argument was found.
 *
 * The function can be used in boolean expressions.
 *
 * \pre \em handle cannot be NULL.
 * \pre \em argument cannot be NULL.
 *
 * \invariant \em optStruct->initialized must equal 
 *                ::PSOC_OPTION_SIGNATURE.
 * \invariant \em optStruct->pArray cannot be NULL.
 * \invariant \em optStruct->numOpt must be positive.
 */
bool psocGetShortOptArgument( psocOptionHandle    handle,
                              const char          opt, 
                              char             ** argument )
{
   psocInternalOpt * optStruct = (psocInternalOpt *)handle;
   int i;
   
   PSO_PRE_CONDITION( handle != NULL );
   PSO_INV_CONDITION( optStruct->initialized == PSOC_OPTION_SIGNATURE );

   PSO_INV_CONDITION( optStruct->pArray != NULL );
   PSO_INV_CONDITION( optStruct->numOpt > 0 );
   
   PSO_PRE_CONDITION( argument != NULL );

   *argument = NULL;
   if ( opt == ' ' || opt == '\0' ) return false;
   
   for ( i = 0; i < optStruct->numOpt; ++i ) {
      if ( optStruct->pArray[i].opt.shortOpt == opt ) {
         if ( optStruct->pArray[i].argumentLocation != 0 ) {
            *argument = optStruct->argv[optStruct->pArray[i].argumentLocation];
            return true;
         }
         break;
      }
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * This function uses the long option as the key for the search but 
 * this is independent of the way the option was entered by the 
 * end-user (short or long) if both versions of the option were defined.
 *
 * \note The decision to use two functions instead of a unified one 
 * was done in order to simplify the code. Otherwise the code would 
 * have required multiple additional checks.
 *
 * \param[in] handle Opaque handle to a psocOptArray struct.
 * \param[in] opt    The long option.
 * \param[out] argument The argument associated with this long option, if any.
 *
 * \retval false  If either option or argument are not found. *argument 
 *                is also set to NULL.
 * \retval true   The argument was found.
 *
 * The function can be used in boolean expressions.
 *
 * \pre \em handle cannot be NULL.
 * \pre \em opt cannot be NULL.
 * \pre \em argument cannot be NULL.
 *
 * \invariant \em optStruct->initialized must equal 
 *                ::PSOC_OPTION_SIGNATURE.
 * \invariant \em optStruct->pArray cannot be NULL.
 * \invariant \em optStruct->numOpt must be positive.
 */
bool psocGetLongOptArgument( psocOptionHandle    handle,
                             const char*         opt, 
                             char             ** argument )
{
   psocInternalOpt * optStruct = (psocInternalOpt *)handle;
   int i;
   bool nullTerminatedString;
   
   PSO_PRE_CONDITION( handle != NULL );
   PSO_INV_CONDITION( optStruct->initialized == PSOC_OPTION_SIGNATURE );

   PSO_INV_CONDITION( optStruct->pArray != NULL );
   PSO_INV_CONDITION( optStruct->numOpt > 0 );
   
   PSO_PRE_CONDITION( opt   != NULL );
   PSO_PRE_CONDITION( argument != NULL );

   *argument = NULL;
   if ( opt[0] == ' ' || opt[0] == '\0' ) return false;

   /* Make sure we were passed a null-terminated string */
   nullTerminatedString = false;
   for ( i = 0; i < PSOC_OPT_LONG_OPT_LENGTH; ++i ) {
      if ( opt[i] == '\0' ) {
         nullTerminatedString = true;
         break;
      }
   }
   if ( ! nullTerminatedString ) return false;   

   for ( i = 0; i < optStruct->numOpt; ++i ) {
      if ( strcmp( optStruct->pArray[i].opt.longOpt, opt ) == 0 ) {
         if ( optStruct->pArray[i].argumentLocation != 0 ) {
            *argument = optStruct->argv[optStruct->pArray[i].argumentLocation];
            return true;
         }
         break;
      }
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * This function uses the short option as the key for the search but 
 * this is independent of the way the option was entered by the 
 * end-user (short or long) if both versions of the option were defined.
 *
 * \note The decision to use two functions instead of a unified one 
 * was done in order to simplify the code. Otherwise the code would 
 * have required multiple additional checks.
 *
 * \param[in] handle Opaque handle to a psocOptArray struct.
 * \param[in] opt    The long option.
 *
 * \retval false  If the option was not found.
 * \retval true   The option was found.
 *
 * The function can be used in boolean expressions.
 *
 * \pre \em handle cannot be NULL.
 *
 * \invariant \em optStruct->initialized must equal 
 *                ::PSOC_OPTION_SIGNATURE.
 * \invariant \em optStruct->pArray cannot be NULL.
 * \invariant \em optStruct->numOpt must be positive.
 */
bool psocIsShortOptPresent( psocOptionHandle handle,
                            const char       opt )
{
   psocInternalOpt * optStruct = (psocInternalOpt *)handle;
   int i;
   
   PSO_PRE_CONDITION( handle != NULL );
   PSO_INV_CONDITION( optStruct->initialized == PSOC_OPTION_SIGNATURE );

   PSO_INV_CONDITION( optStruct->pArray != NULL );
   PSO_INV_CONDITION( optStruct->numOpt > 0 );
   
   if ( opt == ' ' || opt == '\0' ) return false;
   
   for ( i = 0; i < optStruct->numOpt; ++i ) {
      if ( optStruct->pArray[i].opt.shortOpt == opt ) {
         if ( optStruct->pArray[i].optionLocation != 0 ) {
            return true;
         }
         break;
      }
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * This function uses the long option as the key for the search but 
 * this is independent of the way the option was entered by the 
 * end-user (short or long) if both versions of the option were defined.
 *
 * \note The decision to use two functions instead of a unified one 
 * was done in order to simplify the code. Otherwise the code would 
 * have required multiple additional checks.
 *
 * \param[in] handle Opaque handle to a psocOptArray struct.
 * \param[in] opt    The long option.
 *
 * \retval false  If the option was not found.
 * \retval true   The option was found.
 *
 * The function can be used in boolean expressions.
 *
 * \pre \em handle cannot be NULL.
 * \pre \em opt cannot be NULL.
 *
 * \invariant \em optStruct->initialized must equal 
 *                ::PSOC_OPTION_SIGNATURE.
 * \invariant \em optStruct->pArray cannot be NULL.
 * \invariant \em optStruct->numOpt must be positive.
 */
bool psocIsLongOptPresent( psocOptionHandle   handle,
                           const char       * opt )
{
   psocInternalOpt * optStruct = (psocInternalOpt *)handle;
   int i;
   bool nullTerminatedString;
   
   PSO_PRE_CONDITION( handle != NULL );
   PSO_INV_CONDITION( optStruct->initialized == PSOC_OPTION_SIGNATURE );

   PSO_INV_CONDITION( optStruct->pArray != NULL );
   PSO_INV_CONDITION( optStruct->numOpt > 0 );
   
   PSO_PRE_CONDITION( opt   != NULL );

   if ( opt[0] == ' ' || opt[0] == '\0' ) return false;

   /* Make sure we were passed a null-terminated string */
   nullTerminatedString = false;
   for ( i = 0; i < PSOC_OPT_LONG_OPT_LENGTH; ++i ) {
      if ( opt[i] == '\0' ) {
         nullTerminatedString = true;
         break;
      }
   }
   if ( ! nullTerminatedString ) return false;   

   for ( i = 0; i < optStruct->numOpt; ++i ) {
      if ( strcmp( optStruct->pArray[i].opt.longOpt, opt ) == 0 ) {
         if ( optStruct->pArray[i].optionLocation != 0 ) {
            return true;
         }
         break;
      }
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*!
 * The output of this function is sent to stderr.
 *
 * \param[in] handle Opaque handle to a psocOptArray struct.
 * \param[in] progName The name of the calling program (which might 
 *                     differ from argv[0], for example if the program 
 *                     was called using the full path).
 * \param[in] addArguments Trailing list of arguments, filenames, etc. For 
 *                         example, "pathname [ pathname ... ]".
 *
 * \pre \em handle cannot be NULL.
 * \pre \em progName cannot be NULL.
 * \pre \em addArguments cannot be NULL.
 *
 * \invariant \em optStruct->initialized must equal 
 *                ::PSOC_OPTION_SIGNATURE.
 * \invariant \em optStruct->pArray cannot be NULL.
 * \invariant \em optStruct->numOpt must be positive.
 *
 */

void psocShowUsage( psocOptionHandle   handle,
                    const char       * progName,
                    const char       * addArguments )
{
   int i, len, max_len;
   char longOpt[PSOC_OPT_LONG_OPT_LENGTH+PSOC_OPT_ARGUMENT_MSG_LENGTH+3];
   char shortOpt[3], openBracket, closeBracket;
   char comment[PSOC_OPT_COMMENT_LENGTH];
   psocInternalOpt * optStruct = (psocInternalOpt *)handle;
   
   PSO_PRE_CONDITION( handle != NULL );
   PSO_INV_CONDITION( optStruct->initialized == PSOC_OPTION_SIGNATURE );
   PSO_INV_CONDITION( optStruct->pArray != NULL );
   PSO_INV_CONDITION( optStruct->numOpt > 0 );
   PSO_PRE_CONDITION( progName     != NULL );
   PSO_PRE_CONDITION( addArguments != NULL );

   max_len = 0;
   for ( i = 0; i < optStruct->numOpt; ++i ) {
      len = strlen( optStruct->pArray[i].opt.longOpt ) + 
         strlen( optStruct->pArray[i].opt.argumentMessage ) + 3;
      if ( len > max_len ) max_len = len;
   }
   
   if ( progName[0] == '\0' && optStruct->argv != NULL ) {
      fprintf( stderr, "usage: %s options %s\n\n", 
               optStruct->argv[0], 
               addArguments );
   }
   else {
      fprintf( stderr, "usage: %s options %s\n\n", progName, addArguments );
   }
   
   for ( i = 0; i < optStruct->numOpt; ++i ) {
      memset( shortOpt, 0, 3 );
      memset( longOpt, 0, 
              PSOC_OPT_LONG_OPT_LENGTH+PSOC_OPT_ARGUMENT_MSG_LENGTH+3 );
      memset( comment, 0, PSOC_OPT_COMMENT_LENGTH );
      openBracket = closeBracket = ' ';
      
      if ( optStruct->pArray[i].opt.isOptionel ) {
         openBracket  = '[';
         closeBracket = ']';
      }
      
      if ( optStruct->pArray[i].opt.shortOpt != '\0' &&
           optStruct->pArray[i].opt.shortOpt != ' '  ) {
         shortOpt[0] = '-';
         shortOpt[1] = optStruct->pArray[i].opt.shortOpt;
      }
      if ( optStruct->pArray[i].opt.longOpt[0] == '\0' ||
           optStruct->pArray[i].opt.longOpt[0] == ' '  ) {
         sprintf( longOpt, "%s", optStruct->pArray[i].opt.argumentMessage );
      }
      else {
         sprintf( longOpt, "--%s %s", 
                  optStruct->pArray[i].opt.longOpt,
                  optStruct->pArray[i].opt.argumentMessage );
      }

      fprintf( stderr, "%c%2.2s %-*.*s%c %s\n", 
               openBracket,
               shortOpt,
               max_len,
               max_len,
               longOpt,
               closeBracket,
               optStruct->pArray[i].opt.comment );
      
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Unset the internal arrays of all supported options (and free
 * the allocated memory, if needed).
 */
void psocUnsetSupportedOptions( psocOptionHandle handle )
{
   psocInternalOpt * optStruct = (psocInternalOpt *)handle;

   PSO_PRE_CONDITION( handle != NULL );
   PSO_INV_CONDITION( optStruct->initialized == PSOC_OPTION_SIGNATURE );

   if ( optStruct->pArray != NULL ) free( optStruct->pArray );
   
   /* We zero the struct, just in case someone tries to reuse
    * the handle (and somehow the preconditions do not catch it).
    */
   memset( optStruct, 0, sizeof(psocInternalOpt) );
   
   free( optStruct );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

