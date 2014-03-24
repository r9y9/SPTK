## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.5 2013/11/26 02:06:19 mataki Exp $
AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = rawtowav
rawtowav_SOURCES = rawtowav.c 
rawtowav_LDADD = $(top_builddir)/lib/libSPTK.a

install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	test -z "$(bindir)" || $(MKDIR_P) "$(datarootdir)/SPTK"
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	  ; then \
	    f=`echo "$$p1" | sed 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) '$$p' '$(datarootdir)/SPTK/$$f'"; \
	   $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) "$$p" "$(datarootdir)/SPTK/$$f" || exit 1; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f '$(datarootdir)/SPTK/$$f'"; \
	  rm -f "$(datarootdir)/SPTK/$$f"; \
	done
