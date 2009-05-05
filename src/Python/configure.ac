#############################################################################
#                                                                           #
# Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>           #
#                                                                           #
# This file is free software; as a special exception the author gives       #
# unlimited permission to copy and/or distribute it, with or without        #
# modifications, as long as this notice is preserved.                       #
#                                                                           #
# This program is distributed in the hope that it will be useful, but       #
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the    #
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  #
#                                                                           #
#############################################################################

#############################################################################
#                                                                           #
# To do:                                                                    #
#                                                                           #
#  - java, python, etc. APIS as optional (specially if we cannot find the   #
#    package, JAVA_HOME or similar.                                         #
#                                                                           #
#############################################################################

AC_INIT([Python Photon Software],
        [0.5.0],
        [Daniel Prevost <dprevost@photonsoftware.org>],
        [photon])

AC_PREREQ(2.57)
#AC_CONFIG_AUX_DIR(Config)

AC_CONFIG_HEADER(src/config.h)

##################################
#                                #
# SECTION - checks for programs  #
#                                #
##################################

if test -z "$PYTHON"; then
AC_SUBST([PYTHON], [python2.6])
fi
AC_ARG_VAR([PYTHON], [The python executable])

# Not sure  which version is needed to run the tests.
#AM_PATH_PYTHON(,, AC_MSG_ERROR([python is required for the test suite]))
#AC_DEFINE( PYTHONHAVE_ASCTIME_R_OK, 1

# Tell our makefiles that we can use errorParser to regenerate the error
# handling code, as needed.
#AC_CHECK_PROG([ERRORPARSER_IS_PRESENT], [errorParser], [yes], [no])
#AM_CONDITIONAL([COND_USE_ERRORPARSER], [test "$ERRORPARSER_IS_PRESENT" = yes])

##################################
#                                #
# SECTION - checks for libraries #
#                                #
##################################

#####################################
#                                   #
# SECTION - checks for header files #
#                                   #
#####################################

AC_HEADER_STDC 

##############################
#                            #
# Section - checks for types #
#                            #
##############################

AC_CHECK_SIZEOF(void *)

###################################
#                                 #
# Section - checks for structures #
# (do they miss data members?)    #
#                                 #
###################################

#################################################
#                                               #
# Section - checks for compiler characteristics #
#                                               #
#################################################

##################################
#                                #
# Section - checks for functions #
#                                #
##################################

########################################
#                                      #
# Section - checks for system services #
#                                      #
########################################

###################################
#                                 #
# Section - automake conditionals #
#                                 #
###################################

####################
#                  #
# Section - output #
#                  #
####################

AC_CONFIG_FILES([
   Makefile
])

AC_OUTPUT

