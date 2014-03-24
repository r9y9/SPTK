## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.3 2013/11/15 06:23:01 mataki Exp $
AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = fig fdrw
fig_SOURCES = fig.c fig.h fig0.c fig1.c plot.c plot.h plsub.c 
fdrw_SOURCES = fdrw.c plot.c plot.h plsub.c 
fig_LDADD = $(top_builddir)/lib/libSPTK.a
fdrw_LDADD = $(top_builddir)/lib/libSPTK.a
