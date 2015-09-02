# -*- Autoconf -*-

# Process this file with autoconf to produce a configure script.

#
#  configure.ac.tpl is a template used by bootstrap.py script
#  to produce an actual configure.ac
#


# ====================================================================
#               *********  INITIALIZATION **********
# ====================================================================

AC_PREREQ(2.60)
AC_INIT(${NAME}, ${VERSION}, ${EMAIL})

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_MACRO_DIR([m4])
m4_pattern_allow([AC_MSG_FAILURE])

# Check for a system
AC_CANONICAL_TARGET

# We define the target_os_base variable
case x${target_os} in
  *darwin*) target_os_base='MacOSX' ;;
  *linux*)  target_os_base='GNU/Linux' ;;
  *) AC_MSG_ERROR([Unsupported OS ${target_os}]) ;;
esac
echo "System: ${target_os_base}"

AM_CONDITIONAL( [OSDARWIN], [test x${target_os_base} = x'MacOSX'] )
AM_CONDITIONAL( [OSLINUX],  [test x${target_os_base} = x'GNU/Linux'] )

AM_INIT_AUTOMAKE

LT_INIT([disable-static])
AC_SUBST(LT_SHELL)

# ====================================================================
#            *********  COMPILERS AND UTILITIES **********
# ====================================================================

AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_INSTALL

# checking for cxx11 complience
AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])
dnl AS_IF( [ test "x${HAVE_CXX11}" = x1 ], [AC_MSG_NOTICE([YES])],
dnl        [AC_MSG_ERROR([no C++11 standard supported. On MacOSX try to use CC=clang CXX=clang++])] )

AM_CONDITIONAL( [ISGCC], [test x${GCC} = xyes] )

# ====================================================================
#                  *********  MAKEFILES *********
# ====================================================================


AC_CONFIG_FILES([Makefile
                 src/Makefile
                 deps/Makefile
                 deps/shapelib/Makefile])

# ====================================================================
#                *********  ENABLE OPTIONS **********
# ====================================================================

# --------------------------------------------------------------------
# Enabling profiling with different profilers (TODO: tau)
# --------------------------------------------------------------------

AC_ARG_ENABLE( [profile],
  [AS_HELP_STRING( [--enable-profile],
                   [turn on profiling compilation (for developers)] )],
  [case "${enableval}" in
   yes) profile=true  ;;
    no) profile=false ;;
     *) AC_MSG_ERROR( [bad value ${enableval} for --enable-profile]) ;;
   esac],
  [profile=false])

AM_CONDITIONAL( [PROFILE], [test x$profile = xtrue] )

# --------------------------------------------------------------------
# Check for float point operation correctness, no NaNs allowed
# --------------------------------------------------------------------

AC_ARG_ENABLE( [fp-check],
   [AS_HELP_STRING( [--enable-fp-check],
                    [turn on float point NaN check,
                     if enabled will stop at first NaN or inf] )],
   [case "${enableval}" in
    yes) fpcheck=true  ;;
     no) fpcheck=false ;;
      *) AC_MSG_ERROR( [bad value ${enableval}
                        for --enable-fp-check]) ;;
    esac],
   [fpcheck=false] )

AM_CONDITIONAL( [FPCHECK], [test x$fpcheck = xtrue] )

# --------------------------------------------------------------------
# Enabling output additional configuration info after configure
# --------------------------------------------------------------------

AC_ARG_ENABLE( [flags-output],
  [AS_HELP_STRING( [--enable-flags-output],
                   [output some configurated flags (for developers)] )],
  [case "${enableval}" in
   yes) flagsout=true  ;;
    no) flagsout=false ;;
     *) AC_MSG_ERROR( [bad value ${enableval} for --enable-flags-output]) ;;
   esac],
  [flagsout=false])

# ====================================================================
#              *********  WITH OPTIONS **********
# ====================================================================

# ====================================================================
#          *********  HEADERS AND PROGRAMS CHECK **********
# ====================================================================

# Checks for header files.
AC_HEADER_STDC
AX_FUNC_GETOPT_LONG

# Checking for Python (using python.m4)
PC_INIT([3.0])
PC_PYTHON_CHECK_VERSION
PC_PYTHON_CHECK_HEADERS
PC_PYTHON_CHECK_LIBS
PC_PYTHON_CHECK_CFLAGS
PC_PYTHON_CHECK_LDFLAGS

# correcting PYTHON_CXXFLAGS (should this work in no-bash?)
PYTHON_CXXFLAGS=${PYTHON_CFLAGS//'-Wstrict-prototypes'/}
AC_SUBST(PYTHON_CXXFLAGS)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AX_BOOST_BASE([1.53], [],
             [echo "No Boost library found";
              echo "You can get Boost at";
              echo "http://www.boost.org";
              exit 1] )
AX_BOOST_DATE_TIME
AX_BOOST_THREAD

# CGAL

#AC_CHECK_LIB(gmp, __gmpz_init, , [AC_MSG_ERROR([GNU MP not found, see https://gmplib.org/])])

#AC_CHECK_LIB(mpfr, mpfr_add, , [AC_MSG_ERROR([Need MPFR either from GNU MP 4 or separate MPFR package.
#      See http://www.mpfr.org or http://swox.com/gmp])])

#AX_LIB_CGAL_CORE([], [AC_MSG_ERROR([No CGAL library found, please, install CGAL])] )

# ====================================================================
#             *********  LIBRARIES CHECK **********
# ====================================================================

# Checks for libraries.
AC_CHECK_LIB([m], [cos])

# GSL
AC_CHECK_LIB([gslcblas],[cblas_dgemm])
AC_CHECK_LIB([gsl],[gsl_blas_dgemm])

#AC_CHECK_LIB([dl], [dlopen])  # for lua -- we should not need those.
AC_CHECK_LIB([hdf5], [H5Fcreate], [],
             [echo "No HDF5 found. Check it at http://www.hdfgroup.org/HDF5";
              exit 1] )

AC_CHECK_LIB([netcdf], [nc_open], [],
             [echo "No NetCDF found"; exit 1] )

# ====================================================================
#              *********  EXTENTIONS CHECK **********
# ====================================================================

# check for mmx or sse support
AX_EXT

# ====================================================================
#             *********  FLAGS SUBSTITUTION **********
# ====================================================================

# ====================================================================
#                *********  DIAGNOSTICS **********
# ====================================================================

AC_OUTPUT

AC_MSG_NOTICE( [ ] )
AC_MSG_NOTICE( [----------------------------------------------------- ] )
AC_MSG_NOTICE( [System:                              ${target_os_base}] )
AC_MSG_NOTICE( [Output gmon.out for profiling:       ${profile}  ] )
AC_MSG_NOTICE( [Float point correctness check:       ${fpcheck}  ] )
AC_MSG_NOTICE( [Python version:                      ${PYTHON_VERSION}] )
AC_MSG_NOTICE( [----------------------------------------------------- ] )
AC_MSG_NOTICE( [ ] )

if test x$flagsout = xtrue
then

# Additional configuration variables check
AC_MSG_NOTICE( [--------------PYTHON CONFIGURATION------------------- ] )
AC_MSG_NOTICE( [PYTHON_CFLAGS: ${PYTHON_CFLAGS}] )
AC_MSG_NOTICE( [PYTHON_CXXFLAGS: ${PYTHON_CXXFLAGS}] )
AC_MSG_NOTICE( [PYTHON_LDFLAGS: ${PYTHON_LDFLAGS}] )
AC_MSG_NOTICE( [----------------------------------------------------- ] )

fi
