
include $(PROJECT_ROOT)/maketools/posix.macros.mk
include makefile.depend

MANNUM = `echo "$(MAN)" |rev |cut -c 1`
FULL_MANDIR := $(MANDIR)/man$(MANNUM)

build: $(PROG) $(MAN).gz

clean:
	rm -f $(PROG)
	rm -f $(MAN).gz

install: build
	install -d -D -m 0755 $(BINDIR)
	install -m 0755 -t $(BINDIR) $(PROG)
	test -z "$(MAN)" || install -d -D -m 0755 $(FULL_MANDIR)
	test -z "$(MAN)" || install -m 0644 -t $(FULL_MANDIR) $(MAN).gz

uninstall:
	rm -f $(BINDIR)/$(PROG)
	rm -f $(FULL_MANDIR)/$(MAN).gz

depend: makefile.depend

debug:
	echo "$(FULL_MANDIR)"

$(PROG): $(SRCS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(SRCS)

$(MAN).gz: $(MAN)
	-gzip -k9 $(MAN)

makefile.depend:
	cc -M $(SRCS) $(CFLAGS) >$@


.PHONY: build clean install uninstall depend debug

# vim: noet
# end of file
