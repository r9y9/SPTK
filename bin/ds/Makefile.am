## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.5 2012/07/02 11:36:52 mataki Exp $
INCLUDES = -I$(top_srcdir)/include
lpcoefdir = $(datarootdir)/SPTK

bin_PROGRAMS = ds
ds_SOURCES = ds.c 
ds_LDADD = $(top_builddir)/lib/libSPTK.a
ds_CPPFLAGS = -DLIB=\"$(lpcoefdir)\"

dist_lpcoef_DATA = lpfcoef.2to1 \
	lpfcoef.3to2 \
	lpfcoef.4to3 \
	lpfcoef.5to2dn \
	lpfcoef.5to2up 
