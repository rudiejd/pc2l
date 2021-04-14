#   -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#---------------------------------------------------------------------
#  ____ 
# |  _ \    This file is part of  PC2L:  A Parallel & Cloud Computing 
# | |_) |   Library <http://www.pc2lab.cec.miamioh.edu/pc2l>. PC2L is 
# |  __/    free software: you can  redistribute it and/or  modify it
# |_|       under the terms of the GNU  General Public License  (GPL)
#           as published  by  the   Free  Software Foundation, either
#           version 3 (GPL v3), or  (at your option) a later version.
#    
#   ____    PC2L  is distributed in the hope that it will  be useful,
#  / ___|   but   WITHOUT  ANY  WARRANTY;  without  even  the IMPLIED
# | |       WARRANTY of  MERCHANTABILITY  or FITNESS FOR A PARTICULAR
# | |___    PURPOSE.
#  \____| 
#            Miami University and  the PC2Lab development team make no
#            representations  or  warranties  about the suitability of
#  ____      the software,  either  express  or implied, including but
# |___ \     not limited to the implied warranties of merchantability,
#   __) |    fitness  for a  particular  purpose, or non-infringement.
#  / __/     Miami  University and  its affiliates shall not be liable
# |_____|    for any damages  suffered by the  licensee as a result of
#            using, modifying,  or distributing  this software  or its
#            derivatives.
#
#  _         By using or  copying  this  Software,  Licensee  agree to
# | |        abide  by the intellectual  property laws,  and all other
# | |        applicable  laws of  the U.S.,  and the terms of the  GNU
# | |___     General  Public  License  (version 3).  You  should  have
# |_____|    received a  copy of the  GNU General Public License along
#            with MUSE.  If not,  you may  download  copies  of GPL V3
#            from <http://www.gnu.org/licenses/>.
#
# --------------------------------------------------------------------
# Authors:   Dhananjai M. Rao          raodm@miamioh.edu
#---------------------------------------------------------------------



AC_PREREQ(2.69)
AC_INIT([muse],[0.1.0],[raodm@miamioh.edu])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_REQUIRE_CPP

# The macros above automatically fill in some CXXFLAGS so we'll undo it.
# CXX=mpicxx
CFLAGS=""
CXXFLAGS=""
CPPFLAGS=""

# Define MPI library (http://www.open-mpi.org)
AC_DEFUN([AC_PROG_MPI],
[
    AC_ARG_WITH(mpi, AC_HELP_STRING(--with-mpi=PATH,path to mpicc),
        [ if test "$withval" != "yes"; then
              AC_MSG_CHECKING([for MPI])
	      if test -x "$withval" ; then
	          MPI="$withval"
	          AC_MSG_RESULT([$MPI])
	      else
	          MPI="no"
	          AC_MSG_RESULT(no)
	      fi
          fi
     ])
     
     if test -z "$MPI"; then
         AC_PATH_PROG([MPI], [mpicxx], [notFound])
     fi
     
     # have_mpi is set here and used in AC_CHECK_MPICC
     have_mpi=0

     if test $MPI = notFound; then
        AC_MSG_ERROR([MPI could no be found on your system.
                      To compile without MPI use --without-mpi option.
		      You can download MPI for free from
		      http://www.open-mpi.org
                      Please check that MPI is installed on your system
		      and your path includes the top-level install directory
		      (example: /usr/include/openmpi), or specify
		      --with-mpi=PATH option when running configure.])
     elif test $MPI = no; then
          AC_MSG_WARN([You have chosen to compile MUSE without MPI.
                       This is OK and MUSE will run in single-process mode
                       (MPI is needed to run in parallel using multiple
                       processes to fully utilize computational power of
                       supercomputing clusters). You can download MPI for
                       free from http://www.open-mpi.org])
     else
        CXX=mpicxx
        have_mpi=1
        AC_SUBST(MPI)
        AC_DEFINE([HAVE_LIBMPI], [1], [Using MPI to compile MUSE])
     fi
])

# A custom helper to check to ensure C++ version of MPI is operational
# Call AC_PROG_MPI before calling this check
AC_DEFUN([AC_CHECK_MPICC],
[
    if test -n "$have_mpi" && test $have_mpi = 1; then
        AC_MSG_CHECKING([whether $CXX is operational])
	AC_LANG([C++])
     
	AC_LINK_IFELSE(
            [AC_LANG_PROGRAM([[#include "mpi.h"
                               #include <cstdlib>
                             ]],
                             [[int argc=0; char **argv = 0;
                               MPI_Init(&argc, &argv); MPI_Finalize();
                             ]])],
	    [AC_MSG_RESULT([yes])],
            [AC_MSG_RESULT([no])
  	        AC_MSG_ERROR([
	            There is a serious configuration issue on this
	            machine.  It claims MPI is installed and mpicc is
	            your path. However, mpicc is not operational and
	            can't even compile a simple MPI program. This is
	            not a problem with MUSE. Most likely no MPI
	            program will compile on this machine.  Ensure you
	            are loading the correct version of MPI (possibly
	            using the 'module' command (example: module load
	            openmpi-x86_64) in ~/.bashrc])
        ])
    fi
])

# A custom helper macro check if NUMA development libraries are
# available and operational.  Maybe some of our software will be NUMA-aware
# and relies on Linux's NUMA libraries for its's operation.
AC_DEFUN([AC_PROG_NUMA],
[
     # Let user provide configure flag to enable/disable NUMA
     AC_ARG_WITH([numa],
         AS_HELP_STRING([--with-numa],[Enable NUMA aware memory management]),
         [ numa="$withval" ])

     # If user has not explicitly enabled/disabled NUMA then use numa
     # if libraries are found to be operational.
     if test -z "$numa"; then
         AC_MSG_CHECKING([whether NUMA library is operational])
         AC_LANG([C++])
         old_libs="$LIBS"
         LIBS="$LIBS -lnuma"
         AC_RUN_IFELSE(
             [AC_LANG_PROGRAM([[#include <numa.h>
                                #include <numaif.h>
                                #include <iostream>
                              ]],
                              [[return (numa_available() == -1 ? 1 : 0)]])],
	     [ AC_MSG_RESULT([yes])
               numa="yes" ],
             [ AC_MSG_RESULT([no])
               numa="no"
               LIBS="$old_libs"
             ]
         )
     fi

     # Now enable/disable compiler macro in config.h
     if test "$numa" != "yes"; then
         AC_DEFINE_UNQUOTED([USE_NUMA], 0, [Define to 1 if NUMA is usable.])
     else
         AC_DEFINE_UNQUOTED([USE_NUMA], 1, [Define to 1 if NUMA is usable.])
     fi
])

# Check if mpicxx requires -lstdc++ option (needed on redhawk)
AC_DEFUN([AC_LINK_STDCPP],
[
     AC_MSG_CHECKING([if $CXX needs stdc++])
     AC_LANG([C++])
     
     AC_LINK_IFELSE(
         [AC_LANG_PROGRAM([[#include "mpi.h"]],[[std::cout << "Hello";]])],
         [AC_MSG_RESULT([no])
	 ],
	 [AC_MSG_RESULT([yes])
	     LDFLAGS="-lstdc++ $LDFLAGS"
	 ])

])

# Check if we are using icc to supress a couple of useless remarks
AC_DEFUN([AC_PROG_ICC],
[
    AC_ARG_WITH([icc],
            AS_HELP_STRING([--with-icc],[Code will be compiled with icc]),
    [ if test "$withval" != "yes"; then
	  icc="yes"
      else
	  icc="no"
      fi
     ])

     if test -z "$icc"; then
         AC_MSG_CHECKING([for icc])
         icc=`$CXX --version | grep -iw "icc"`
	 if test -n "$icc"; then
	     icc="yes"
	 else
	     icc="no"
	 fi
         AC_MSG_RESULT([$icc])
     fi

     if test "yes" == "$icc"; then
          USING_ICC="-DICC"
          # -wd981: Supresses values evalutaed in differen order
          # -wd383: Supresses value copied to temporary for use
          # -wd424: Supresses extra ";" at end warning
          # -wd654: Supresses partial overload warnings
          ICC_NO_WARN_SOME="-wd981 -wd383 -wd424 -wd654"
	  ICC_NO_WARN_FOR_CONFIGURE="-wd981 -wd383 -wd1419"
     else
          USING_ICC=""
          ICC_NO_WARN_SOME=""
	  ICC_NO_WARN_FOR_CONFIGURE=""
     fi

     AC_SUBST(USING_ICC)
     AC_SUBST(ICC_NO_WARN_SOME)
])

# A custom helper macro check if OpenCL development libraries are
# available and operational.  MUSE's heterogeneous compute
# capabilities relies on OpenCL libraries for its's operation.
AC_DEFUN([AC_LIB_OPENCL],
[
    AC_ARG_WITH(opencl, AC_HELP_STRING(--with-opencl=PATH,path to OpenCL),
        [ if test "$withval" != "no"; then
              if test "$withval" != "yes"; then
                  AC_MSG_NOTICE([Checking for OpenCL in $withval])
                  OPEN_CL_PATH="$withval"
                  have_opencl="maybe"
              else
                  AC_MSG_NOTICE([Checking for OpenCL])
                  # Assume OpenCL is in default path
                  OPEN_CL_PATH=""
                  have_opencl="maybe"
              fi
          else
              # ignore and don't use OpenCL
              have_opencl="ignore"
          fi
        ], [
            # User has not specified any options. Ignore OpenCL by default
            have_opencl="ignore"
        ]
     )

     # Save current flags to restore after test below.
     OLD_CFLAGS="$CFLAGS"
     OLD_CPPFLAGS="$CPPFLAGS"
     LDFLAGS="$OLD_LDFLAGS"
     AC_LANG_PUSH([C++])

     # Check if we are able to compile a OpenCL headers
     if test "$have_opencl" = "maybe"; then
         # Add OpenCL path for use.
         CFLAGS="$CFLAGS -I$OPEN_CL_PATH/include"
         CPPFLAGS="$CPPFLAGS -I$OPEN_CL_PATH/include"
         AC_MSG_CHECKING([if OpenCL headers are operational])

         AC_COMPILE_IFELSE(
             [AC_LANG_PROGRAM([[#ifdef __APPLE__
                                #include <OpenCL/cl.hpp>
                                #else
                                #include "CL/cl.hpp"
                                #endif
                              ]],
                              [[cl::Platform::getDefault()]])],
             [AC_MSG_RESULT([yes])],
             [AC_MSG_RESULT([no])
              have_opencl="no"
             ])
     fi

     # Now that the header seems fine next check path to find library
     # and link against it.
     OPEN_CL_LDPATH=""     
     if test "$have_opencl" = "maybe"; then
         AC_MSG_CHECKING([for OpenCL library path])
         for subdir in "lib64" "lib/x86_64"; do
             ldpath="$OPEN_CL_PATH/$subdir"
             found="no"
             LDFLAGS="$OLD_LDFLAGS -L$ldpath -lOpenCL"
             AC_LINK_IFELSE(
                 [AC_LANG_PROGRAM([[#ifdef __APPLE__
                                    #include <OpenCL/cl.hpp>
                                    #else
                                    #include "CL/cl.hpp"
                                    #endif
                                  ]],  [[cl::Platform::getDefault()]])],
                  [found="yes"],  [found="no"])
             # If the library worked, we have found a path
             if test "$found" = "yes"; then
                 have_opencl="yes"
                 OPEN_CL_LDPATH="$ldpath"
                 break
             fi
         done
         if test "$have_opencl" = "yes"; then
             AC_MSG_RESULT([$OPEN_CL_LDPATH])
         else
             AC_MSG_RESULT([none])
             have_opencl="no"
         fi
     fi

     # Restore the search flags.
     AC_LANG_POP([C++])
     CFLAGS="$OLD_CFLAGS"
     CPPFLAGS="$OLD_CPPFLAGS"
     LDFLAGS="$OLD_LDFLAGS" 

     if test "$have_opencl" = "no"; then
        AC_MSG_ERROR([OpenCL headers were not found on your system.
                    Specify --with-opencl=PATH option when running configure.
                    To compile without OpenCL use --without-opencl option.])
     elif test "$have_opencl" = "ignore"; then
          AC_MSG_WARN([You have chosen to compile MUSE without OpenCL.
                    This is OK and MUSE will not include heterogeneous
                    computing capabilities])
     else
        OPEN_CL_PATH="-I$OPEN_CL_PATH/include"
        OPEN_CL_LDPATH="-L$OPEN_CL_LDPATH -lOpenCL"
        AC_SUBST(OPEN_CL_PATH)
        AC_SUBST(OPEN_CL_LDPATH)
        AC_DEFINE([HAVE_OPEN_CL], [1], [Using OpenCL in MUSE])
     fi
     # Setup conditional variable used in kernel/Makefile.am to
     # conditionally compile openCL components.
     AM_CONDITIONAL([COND_USE_OPENCL], [test "x$have_opencl" == "xyes"])
])

# A convenience macro to check if the compiler accepts a specific
# compiler flag or argument (and does not generate an error)
AC_DEFUN([AC_CHECK_ARGS],
[
    AC_MSG_CHECKING([whether _AC_LANG compiler accepts $1])
    if test -z "[$4]"; then
        AC_LANG_CONFTEST([AC_LANG_PROGRAM([[#include <iostream>]],
				          [[std::cout << "ac_check_args"]])])
        src=`ls conftest.c*`
        output=`$CXX $1 $CXXFLAGS $LIBS $ICC_NO_WARN_FOR_CONFIGURE $src 2>&1`
	flag_check=`expr match "$output" ".*$1.*"`
        if test "$flag_check" == "0"; then
            AC_MSG_RESULT([yes])
            m4_default([$2], :)
            if test -z "[$5]"; then 
                CPPFLAGS+=" [$1]"
            fi
        else
	    AC_MSG_RESULT([no])
	    m4_default([$3], :)
        fi
    else
        AC_MSG_RESULT([skipped (because "$4" overrides)])
	m4_default([$3], :)
    fi
])

# Check if we are using correct version of gcc
AC_DEFUN([AC_CHECK_GCC_VERSION],
[
    AC_MSG_CHECKING([whether gcc is sufficiently new])
    # Get version from GCC version header in the form:
    # gcc (GCC) 6.x.x ...
    version=`g++ --version 2>&1 | head -1 | cut -d' ' -f3`
    majorVer=`echo $version | cut -d'.' -f1`
    minorVer=`echo $version | cut -d'.' -f2`
    patchLevel=`echo $version| cut -d'.' -f3`

    good="yes"
    # Check to ensure gcc is at least 4.0.1
    if test $majorVer -lt 6; then
       good="no"
    elif test $majorVer -eq 6; then
	 if test $minorVer -lt 1 && test $patchLevel -lt 3; then
 	     good="no"
	 fi
    fi

    if test $good = no; then
        AC_MSG_RESULT([no])
	AC_MSG_ERROR([
	  MUSE has been developed using the latest C++ standards.
	  It requires at least GCC Version 6.3.0 to compile. The gcc
	  version on your machine is $version which is not
	  sufficiently recent to compile MUSE. You need upgrade
	  gcc. If you are on a cluster environment contact your
	  system adminstrator to determine how to load a more recent
	  gcc module by default.
	 ])
    else
        AC_MSG_RESULT([yes, $version])
    fi
])

# Check if we are using gcc to supress a couple of useless remarks
AC_DEFUN([AC_PROG_GCC],
[
    AC_ARG_WITH(gcc,
        AC_HELP_STRING([--with-gcc],[Code will be compiled with gcc]),
        [ if test "$withval" == "yes"; then
	  	 gcc="yes"
          else
		gcc="no"
          fi
        ])

     if test -z "$gcc"; then
         AC_MSG_CHECKING([for gcc])
         gcc=`$CXX --version 2>&1 | grep -i -w "GCC"`
	 if test -n "$gcc"; then
	     gcc="yes"
	 else
	     gcc="no"
	 fi
         AC_MSG_RESULT([$gcc])
     fi

     if test "yes" == "$gcc"; then
         USING_GCC="-DGCC"
	 AC_CHECK_GCC_VERSION
     else
     	 USING_GCC=""
     fi

     AC_SUBST(USING_GCC) 
])

# Checks for programs.
AC_PROG_MPI
AC_CHECK_MPICC
AC_MSG_CHECKING([properties of C++ compiler])
AC_MSG_RESULT([$CXX])
AC_PROG_CXX
AC_LINK_STDCPP
AC_PROG_ICC
AC_PROG_GCC
AC_PROG_RANLIB
AC_PROG_NUMA

# Currently we are not using OpenCL
# AC_LIB_OPENCL

# Checks for suitable archiver to be used
AM_PROG_AR

# Check and set some default compiler flags for the build
AC_CHECK_ARGS([-Minform=warn], [INFORM_WARN="-Minform=warn"], [INFORM_WARN=""])
AC_SUBST(INFORM_WARN)
AC_CHECK_ARGS([-Wall], [WALL="-Wall"], [WALL=""], [])
AC_SUBST(WALL)
AC_CHECK_ARGS([-Wextra], [WEXTRA="-Wextra"], [WEXTRA=""], [])
AC_SUBST(WEXTRA)
AC_CHECK_ARGS([-pipe], [PIPE="-pipe"], [PIPE=""], [])
AC_SUBST(PIPE)

# The -fast flag for ICC causes libraries to break due to inter-procedural
# call optimizations. So sadly it cannot be used for libraries. 
# AC_CHECK_ARGS([-fast], [FAST="-fast"], [FAST=""], [])
# AC_SUBST(FAST)

# The -O3 flag is now accepted by gcc, icc, & clang.
AC_CHECK_ARGS([-O3], [OPT_O3="-O3"], [OPT_O3=""], [$OPT_O3], ["no"])

# Override -O3 based on developer assertions flags as needed.
DEV_ASSERT=""
AC_ARG_ENABLE([assertions],
    AS_HELP_STRING([--enable-assertions], [Enable developer assertion checks for sanity checking]),
    [ if test "$enableval" == "yes"; then
          AC_MSG_NOTICE([Enabling developer assertion compiler flag])    
          DEV_ASSERT="-DDEVELOPER_ASSERTIONS"
      fi
    ])
# override full debugging & optimization flags as needed.
AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Setup all compile in debugging mode]),
    [ if test "$enableval" == "yes"; then
          AC_MSG_NOTICE([Enabling debug flags & developer assertions])
          OPT_O3="-O0"
          DEV_ASSERT="-DDEVELOPER_ASSERTIONS"
      fi
    ])

# override full debugging & optimization flags as needed.
AC_ARG_ENABLE([full-debug],
    AS_HELP_STRING([--enable-full-debug], [Setup all compile with debugging outputs]),
    [ if test "$enableval" == "yes"; then
          AC_MSG_NOTICE([Enabling full debug flags & developer assertions])    
          OPT_O3="-O0"
          DEV_ASSERT="-DDEVELOPER_ASSERTIONS -DDEBUG_OUTPUT"
      fi
    ])

# Substitute values of variables in makefile(s)
AC_SUBST(OPT_O3)
AC_SUBST(DEV_ASSERT)

# Check and start using new C++14 standards
AC_CHECK_ARGS([-std=c++14], [CPP14="-std=c++14"], [CPP14=""], [])
if test "x$CPP14" = "x"; then
    AC_MSG_ERROR([PC2L requires C++14 or newer support. Your compiler does not
                 seem to accept -std=c++14 flag suggesting it does not support
                 c++14 standard. Please check your compiler settings (you may
                 need to load a different module) and try again.])
fi

# Put object files in corresponding directories even if make is not
# running directly in the source directory.
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([m],[asin],,,[-lstdc++])
# AC_CHECK_FUNCS([MPI_Init])
AC_SEARCH_LIBS([initscr],[curses],,,[$LINK_STD_CPP])
AC_CHECK_LIB([readline],[readline],,,[$LINK_STD_CPP])
AC_CHECK_LIB([pthread], [pthread_create],,,[$LINK_STD_CPP])
AC_SEARCH_LIBS([gethostname], [c,nsl],,,[$LINK_STD_CPP])
AC_SEARCH_LIBS([bind],[c,nsl,socket],,,[$LINK_STD_CPP])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([alloca.h cstdlib string pthread.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE
AC_C_RESTRICT
AC_STRUCT_TM


# Checks for library functions.
# AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor memset mkdir pow pthread_getattr_np])

AC_CONFIG_FILES([Makefile\
        include/Makefile\
	src/Makefile\
        examples/Makefile\
        examples/big_vec/Makefile])

# Check for doxygen available to generate API documentation.
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
# Add doxygen file to the set of config files if doxygen is available.
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Makefile])])


AC_OUTPUT

# End of script
