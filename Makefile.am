## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.3 2008/11/12 10:49:01 s_sako Exp $
SUBDIRS = include lib bin script

# Header files to be included in the distribution.
include_HEADERS = include/SPTK.h

# Additional files to be included in the distribution.
EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL NEWS README Makefile.mak \
	     bin/Makefile.mak include/Makefile.mak lib/Makefile.mak

# Files to remove by 'make distclean'
DISTCLEANFILES = *.log *.out *~ bin/*~ bin/*/*~ config/*~ include/*~ lib/*~ scripts/*~

# Autotools-generated files to remove by 'make maintainer-clean'.
MAINTAINERCLEANFILES = aclocal.m4 configure Makefile.in        \
                       config/config.guess config/config.sub   \
                       config/depcomp config/install-sh        \
                       config/missing

