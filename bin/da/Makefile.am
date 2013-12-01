## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.7 2012/07/02 11:36:51 mataki Exp $
if USEDA
INCLUDES = -I$(top_srcdir)/include
bin_PROGRAMS = dawrite
bin_SCRIPTS = da.sh
dawrite_SOURCES = da.h dawrite.c winplay.c winplay.h
dawrite_LDADD = $(top_builddir)/lib/libSPTK.a
endif

EXTRA_DIST = da.in
CLEANFILES = $(bin_PROGRAMS) $(bin_SCRIPTS)

replace_path  = sed -e 's,@binpath@,$(DESTDIR)$(bindir),g' \
	       -e 's,@libpath@,$(datarootdir)/SPTK,g' \
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
	    $(binSCRIPT_INSTALL) "$$d$$p" "$(DESTDIR)$(bindir)/$$f"; \
	  else :; fi; \
	  p2=`basename $$p .sh`; \
	  rm -f $(DESTDIR)$(bindir)/$$p2; \
	  (cd $(DESTDIR)$(bindir); $(LN_S) $$p  $$p2) \
	done

uninstall-binSCRIPTS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_SCRIPTS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's|^.*/||;$(transform)'`; \
	  echo " rm -f '$(DESTDIR)$(bindir)/$$f'"; \
	  rm -f "$(DESTDIR)$(bindir)/$$f"; \
	  p2=`basename $$p .sh`; \
	  rm -f "$(DESTDIR)$(bindir)/$$p2"; \
	done
