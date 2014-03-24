## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.7 2013/12/13 09:19:05 mataki Exp $
AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = gmm gmmp
gmm_SOURCES = _gmm.c gmm.c
gmm_LDADD = $(top_builddir)/lib/libSPTK.a
gmmp_SOURCES = _gmm.c gmmp.c
gmmp_LDADD = $(top_builddir)/lib/libSPTK.a
