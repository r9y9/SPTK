#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT(SPTK, 3.7, http://sourceforge.net/projects/sp-tk/, SPTK)
AC_CONFIG_AUX_DIR([config])
AC_COPYRIGHT(Copyright 1984-2007 Tokyo Institute of Technology)
AC_COPYRIGHT(Copyright 1996-2013 Nagoya Institute of Technology)
AM_INIT_AUTOMAKE([nostdinc])
AC_SUBST(PACKAGE_NAME)
AC_SUBST(PACKAGE_VERSION)

AC_PREFIX_DEFAULT(/usr/local/SPTK)
AC_CONFIG_SRCDIR([bin/acep/acep.c])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AN_MAKEVAR([AR], [AC_PROG_AR])
AN_PROGRAM([ar], [AC_PROG_AR])
AC_DEFUN([AC_PROG_AR], [AC_CHECK_TOOL(AR, ar, :)])
AC_PROG_AR
AC_PATH_PROG(CSH,csh,csh)

# Checks for libraries.
AC_CHECK_LIB([m],[log],,AC_MSG_ERROR(Cannot find libm))

# Checks for X Window Systems
AC_PATH_X
AC_PATH_XTRA

if test x$no_x = "x"; then
	AM_CONDITIONAL( USEXGR, true)
else
	AM_CONDITIONAL( USEXGR, false)
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h strings.h sys/ioctl.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# use host info
AC_CANONICAL_HOST

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNC(pow,,AC_MSG_ERROR("Cannot find pow()"))
AC_CHECK_FUNC(sqrt,,AC_MSG_ERROR("Cannot find sqrt()"))

find_memset=no
find_strrchr=no
AC_CHECK_FUNCS([bzero memset],find_memset="yes",)
AC_CHECK_FUNCS([strrchr rindex],find_strrchr="yes",)

if test "$find_memset" != "yes"; then
	AC_MSG_ERROR("Cannot find memset() or bzero()")
fi
if test "$find_strrchr" != "yes"; then
	AC_MSG_ERROR("Cannot find strrchr() or rindex()")
fi

# Checks for double 
use_double=no
IO_SYMBOL='f'
IO_FORMAT='float'
AC_ARG_ENABLE(double,
	      [  --enable-double         deal with data in double (default=no)],
	      [ if test "$enable_double" = "yes"; then
			use_double=yes
			IO_SYMBOL='d'
			IO_FORMAT='double'
			AC_DEFINE(DOUBLE)
		fi])
AC_SUBST(IO_SYMBOL)
AC_SUBST(IO_FORMAT)

if test "$use_double" = "yes"; then
	AC_DEFINE(FORMAT,"double")
else
	AC_DEFINE(FORMAT,"float")
fi
		
# Checks for da command
use_da=yes
AC_ARG_ENABLE(da,
	      [  --enable-da             use audio output (default=yes)],
	      [ if test "$enable_da" = "no"; then
	       		use_da=no
		fi])

# Checks for audio device
if test "$use_da" = "yes"; then
AC_MSG_CHECKING(audio device)
	case $host in
	*-sunos*)
		AC_MSG_RESULT([sunos])
		AC_CHECK_HEADER([sys/audioio.h],
				[AC_DEFINE(SOLARIS)],
				[AC_MSG_ERROR([sys/audioio.h is not found])])
		AM_CONDITIONAL( USEDA, true)
		;;
	*-solaris*)
		AC_MSG_RESULT([solaris])
		AC_CHECK_HEADER([sys/audioio.h],
				[AC_DEFINE(SOLARIS)],
				[AC_MSG_ERROR([sys/audioio.h is not found])])
		AM_CONDITIONAL( USEDA, true)
		;;
	*-linux*)
		AC_MSG_RESULT([linux])
		AC_CHECK_HEADER([sys/soundcard.h],
				[AC_DEFINE(LINUX)],
				[AC_MSG_ERROR([sys/soundcard.h is not found])])
		AM_CONDITIONAL( USEDA, true)
		;;
	*-freebsd*)
		AC_MSG_RESULT([freebsd])
		AC_CHECK_HEADER([sys/soundcard.h],
				[AC_DEFINE(FreeBSD)],
				[AC_MSG_ERROR([sys/soundcard.h is not found])])
		AM_CONDITIONAL( USEDA, true)
		;;
	*)
		AC_MSG_WARN([not found])
		AM_CONDITIONAL( USEDA, false)
		;;
	esac
else
	AM_CONDITIONAL( USEDA, false)
fi

# check solaris & gcc
case $host in
	 *-solaris*)
	 	if test "$GCC" = "yes"; then
			CFLAGS="-std=c99 $CFLAGS"
		fi
	;;
esac

AC_CONFIG_FILES([Makefile
                 bin/Makefile
                 bin/acep/Makefile
                 bin/acorr/Makefile
                 bin/agcep/Makefile
                 bin/amcep/Makefile
                 bin/average/Makefile
                 bin/b2mc/Makefile
                 bin/bcp/Makefile
                 bin/bcut/Makefile
                 bin/c2acr/Makefile
                 bin/c2ir/Makefile
                 bin/c2sp/Makefile
                 bin/cat2/Makefile
                 bin/cdist/Makefile
                 bin/clip/Makefile
                 bin/da/Makefile
                 bin/dct/Makefile
                 bin/decimate/Makefile
                 bin/delay/Makefile
                 bin/delta/Makefile
                 bin/df2/Makefile
                 bin/dfs/Makefile
                 bin/dmp/Makefile
                 bin/ds/Makefile
                 bin/dtw/Makefile
                 bin/echo2/Makefile
                 bin/excite/Makefile
                 bin/extract/Makefile
                 bin/fd/Makefile
                 bin/fft/Makefile
                 bin/fft2/Makefile
                 bin/fftcep/Makefile
                 bin/fftr/Makefile
                 bin/fftr2/Makefile
                 bin/fig+fdrw/Makefile
                 bin/frame/Makefile
                 bin/freqt/Makefile
                 bin/gc2gc/Makefile
                 bin/gcep/Makefile
                 bin/glsadf/Makefile
                 bin/gmm/Makefile
                 bin/gnorm/Makefile
                 bin/grpdelay/Makefile
                 bin/histogram/Makefile
                 bin/idct/Makefile
                 bin/ifft/Makefile
                 bin/ifftr/Makefile
                 bin/ifft2/Makefile
                 bin/ignorm/Makefile
                 bin/impulse/Makefile
                 bin/imsvq/Makefile
                 bin/interpolate/Makefile
                 bin/ivq/Makefile
                 bin/lbg/Makefile
                 bin/levdur/Makefile
                 bin/linear_intpl/Makefile
                 bin/lmadf/Makefile
                 bin/lpc/Makefile
                 bin/lpc2c/Makefile
                 bin/lpc2lsp/Makefile
                 bin/lpc2par/Makefile
                 bin/lsp2lpc/Makefile
                 bin/lsp2sp/Makefile
                 bin/lspcheck/Makefile
                 bin/lspdf/Makefile
                 bin/ltcdf/Makefile
                 bin/mc2b/Makefile
                 bin/mcep/Makefile
                 bin/merge/Makefile
                 bin/mfcc/Makefile
                 bin/mgc2mgc/Makefile
                 bin/mgc2sp/Makefile
                 bin/mgcep/Makefile
                 bin/mglsadf/Makefile
                 bin/minmax/Makefile
                 bin/mlpg/Makefile
                 bin/mlsacheck/Makefile
                 bin/mlsadf/Makefile
                 bin/msvq/Makefile
                 bin/nan/Makefile
                 bin/norm0/Makefile
                 bin/nrand/Makefile
                 bin/par2lpc/Makefile
                 bin/pca/Makefile
                 bin/phase/Makefile
                 bin/pitch/Makefile
                 bin/poledf/Makefile
                 bin/psgr/Makefile
                 bin/ramp/Makefile
                 bin/rawtowav/Makefile
                 bin/reverse/Makefile
                 bin/rmse/Makefile
                 bin/root_pol/Makefile
                 bin/sin/Makefile
                 bin/smcep/Makefile
                 bin/snr/Makefile
                 bin/sopr/Makefile
                 bin/spec/Makefile
                 bin/step/Makefile
                 bin/swab/Makefile
                 bin/symmetrize/Makefile
                 bin/train/Makefile
                 bin/transpose/Makefile
                 bin/uels/Makefile
                 bin/ulaw/Makefile
                 bin/us/Makefile
                 bin/vc/Makefile 
                 bin/vopr/Makefile
                 bin/vq/Makefile
                 bin/vstat/Makefile
                 bin/vsum/Makefile
                 bin/window/Makefile
                 bin/x2x/Makefile
                 bin/xgr/Makefile
                 bin/zcross/Makefile
                 bin/zerodf/Makefile
                 include/Makefile
                 lib/Makefile
                 script/Makefile])
AC_OUTPUT

