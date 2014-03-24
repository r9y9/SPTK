## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.6 2013/11/15 06:23:04 mataki Exp $
AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = pitch
pitch_SOURCES = pitch.c $(srcdir)/snack/jkGetF0.c $(srcdir)/snack/sigproc.c $(srcdir)/snack/jkGetF0.h \
	$(srcdir)/swipe/swipe.c $(srcdir)/swipe/vector.c $(srcdir)/swipe/vector.h
pitch_LDADD = $(top_builddir)/lib/libSPTK.a
