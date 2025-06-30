
include $(PROJECT_ROOT)/maketools/posix.macros.mk
include makefile.depend

MANNUM = `echo "$(MAN)" |rev |cut -c 1`
FULL_MANDIR := $(MANDIR)/man$(MANNUM)

all: build

build: $(PROG) $(MAN).gz

clean:
	rm -f $(OBJS)
	rm -f $(PROG)
	rm -f $(MAN).gz

install: build
	install -d -D -m 0755 $(SBINDIR)
	install -m 0755 -t $(SBINDIR) $(PROG)
	test -z "$(MAN)" || install -d -D -m 0755 $(FULL_MANDIR)
	test -z "$(MAN)" || install -m 0644 -t $(FULL_MANDIR) $(MAN).gz

uninstall:
	rm -f $(SBINDIR)/$(PROG)
	rm -f $(FULL_MANDIR)/$(MAN).gz

depend: makefile.depend

debug:
	echo "$(FULL_MANDIR)"

$(PROG): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(MAN).gz: $(MAN)
	-gzip -k9f $(MAN)

makefile.depend:
	cc -M $(SRCS) $(CFLAGS) >$@


.PHONY: build clean install uninstall depend debug

# vim: noet
# end of file
