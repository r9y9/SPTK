## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.3 2013/11/15 06:23:05 mataki Exp $
if USEXGR
bin_PROGRAMS = xgr
endif

AM_CPPFLAGS = -I$(top_srcdir)/include
xgr_SOURCES = config.h \
	gcdata.h \
	plot.c \
	window.c \
	xgr.c \
	xgr.h \
	marks/mark0.dat \
	marks/mark1.dat \
	marks/mark10.dat \
	marks/mark11.dat \
	marks/mark12.dat \
	marks/mark13.dat \
	marks/mark14.dat \
	marks/mark15.dat \
	marks/mark2.dat \
	marks/mark3.dat \
	marks/mark4.dat \
	marks/mark5.dat \
	marks/mark6.dat \
	marks/mark7.dat \
	marks/mark8.dat \
	marks/mark9.dat \
	tills/till0.dat \
	tills/till1.dat \
	tills/till10.dat \
	tills/till11.dat \
	tills/till12.dat \
	tills/till13.dat \
	tills/till14.dat \
	tills/till15.dat \
	tills/till16.dat \
	tills/till2.dat \
	tills/till3.dat \
	tills/till4.dat \
	tills/till5.dat \
	tills/till6.dat \
	tills/till7.dat \
	tills/till8.dat \
	tills/till9.dat

xgr_CFLAGS = $(X_CFLAGS)
xgr_LDFLAGS = $(X_LIBS)
xgr_LDADD = $(top_builddir)/lib/libSPTK.a -lX11
