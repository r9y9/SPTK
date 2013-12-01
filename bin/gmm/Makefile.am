## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.5 2012/06/06 12:10:19 mataki Exp $
INCLUDES = -I$(top_srcdir)/include
bin_PROGRAMS = gmm gmmp
gmm_SOURCES = _gmm.c gmm.c gmm.h
gmm_LDADD = $(top_builddir)/lib/libSPTK.a
gmmp_SOURCES = _gmm.c gmmp.c gmm.h
gmmp_LDADD = $(top_builddir)/lib/libSPTK.a
