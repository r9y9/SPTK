## Process this file with automake to produce Makefile.in
##
## $Id: Makefile.am,v 1.12 2013/11/26 02:06:19 mataki Exp $
bin_SCRIPTS = bell.sh gseries.sh glogsp.sh grlogsp.sh gwave.sh mgc2mgclsp.sh mgclsp2mgc.sh raw2wav.sh wav2raw.sh
EXTRA_DIST = bell.in gseries.in glogsp.in grlogsp.in gwave.in mgc2mgclsp.in mgclsp2mgc.in raw2wav.in wav2raw.in

CLEANFILES = $(bin_SCRIPTS)

IO_SYMBOL = @IO_SYMBOL@
IO_FORMAT = @IO_FORMAT@

replace_path  = sed -e 's,@binpath@,$(DESTDIR)$(bindir),g' \
	            -e 's,@libpath@,$(DESTDIR)$(libdir),g' \
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
