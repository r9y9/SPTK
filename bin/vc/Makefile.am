## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.3 2013/12/14 11:30:08 mataki Exp $

AM_CPPFLAGS = -I$(srcdir)/hts_engine_API \
              -I$(top_srcdir)/include
bin_PROGRAMS = vc
vc_SOURCES = vc.c _vc.c \
            $(srcdir)/hts_engine_API/HTS_misc.c \
            $(srcdir)/hts_engine_API/HTS_pstream.c \
            $(srcdir)/hts_engine_API/HTS_sstream.c \
            $(srcdir)/hts_engine_API/HTS_hidden.h \
            $(srcdir)/hts_engine_API/HTS_engine.h
vc_LDADD = $(top_builddir)/lib/libSPTK.a
