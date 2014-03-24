## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.11 2013/12/14 17:30:41 mataki Exp $
if USEDA
AM_CPPFLAGS = -I$(top_srcdir)/include
bin_PROGRAMS = dawrite
bin_SCRIPTS = da.sh
dawrite_SOURCES = da.h dawrite.c winplay.c winplay.h
dawrite_LDADD = $(top_builddir)/lib/libSPTK.a
endif

EXTRA_DIST = da.in
CLEANFILES = $(bin_PROGRAMS) $(bin_SCRIPTS)

replace_path  = sed -e 's,@binpath@,$(DESTDIR)$(bindir),g' \
	       -e 's,@libpath@,$(datarootdir)/SPTK,g' \
	       -e 's,@sharepath@,$(datarootdir)/SPTK,g' \
	       -e 's,@sptk_version@,$(PACKAGE_VERSION),g' \
	       -e 's,@cshpath@,$(CSH),' \
	       -e 's,@io_symbol@,$(IO_SYMBOL),g' \
	       -e 's,@io_format@,$(IO_FORMAT),g'
	       

.in.sh:
	$(replace_path) < $< > $@
	chmod +x $@


install-binSCRIPTS: $(bin_SCRIPTS)
	@$(NORMAL_INSTALL)
	test -z "$(bindir)" || $(mkdir_p) "$(DESTDIR)$(bindir)"
	@list='$(bin_SCRIPTS)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  if test -f $$d$$p; then \
	    f=`echo "$$p" | sed 's|^.*/||;$(transform)'`; \
	    echo " $(binSCRIPT_INSTALL) '$$d$$p' '$(DESTDIR)$(bindir)/$$f'"; \
	    $(binSCRIPT_INSTALL) "$$d$$p" "$(datarootdir)/SPTK/$$f"; \
	  else :; fi; \
	  p2=`basename $$p .sh`; \
	  rm -f "$(DESTDIR)$(bindir)/$$p2"; \
	  (cd $(DESTDIR)$(bindir); $(LN_S) "$(datarootdir)/SPTK/$$p"  $$p2) \
	done

uninstall-binSCRIPTS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_SCRIPTS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's|^.*/||;$(transform)'`; \
	  echo " rm -f '$(datarootdir)/SPTK/$$f'"; \
	  rm -f "$(datarootdir)/SPTK/$$f"; \
	  p2=`basename $$p .sh`; \
	  rm -f "$(DESTDIR)$(bindir)/$$p2"; \
	done

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
