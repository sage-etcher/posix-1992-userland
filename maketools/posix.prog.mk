
include $(PROJECT_ROOT)/maketools/posix.macros.mk
include makefile.depend

USE_SBIN ?= false
USE_BINDIR = `$(USE_SBIN) && echo "$(SBINDIR)" || echo "$(BINDIR)"`
MANNUM = `echo "$(MAN)" |rev |cut -c 1`
FULL_MANDIR := $(MANDIR)/man$(MANNUM)

CATEGORIES 	?= none
LOCALES 	?= en

CFLAGS	+= '-DDOMAIN_NAME="$(PROG)"'
CFLAGS	+= '-DDOMAIN_DIR="$(LOCDIR)"'

DOMAIN_NAME	?=	$(PROG)

all: build

build: .gitignore build_mo $(PROG) $(MAN).gz

clean:
	rm -f $(OBJS)
	rm -f $(PROG)
	rm -f $(MAN).gz
	rm -f `find locale -name '*.mo'`
	rm -f `find locale -name '*.pot'`
	rm -f .locale_done

.gitignore: makefile
	rm -f $@.tmp
	-cat $@ >$@.tmp
	echo "$@.tmp" >>$@.tmp
	echo "$(PROG)" >>$@.tmp
	cat $@.tmp |sort |uniq >$@

install: build
	install -d -D -m 0755 $(USE_BINDIR)
	install -m 0755 -t $(USE_BINDIR) $(PROG)
	test -z "$(MAN)" || \
		(install -d -D -m 0755 $(FULL_MANDIR) && \
		install -m 0644 -t $(FULL_MANDIR) $(MAN).gz); \
	test -z "$(USE_LOCALES)" || for i in $(LOCALES); do \
		for j in $(CATEGORIES); do \
			install -d -D -m 0755 $(LOCDIR)/$$i/$$j; \
			install -m 0644 ./locale/$$i/$$j/$(PROG).mo $(LOCDIR)/$$i/$$j/; \
		done; \
	done

uninstall:
	rm -f $(USE_BINDIR)/$(PROG)
	rm -f $(FULL_MANDIR)/$(MAN).gz
	rm -f `find $(LOCDIR) -name $(PROG).mo`

depend: makefile.depend

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $< $(CFLAGS)

$(MAN).gz: $(MAN)
	test -z "$(MAN)" || gzip -k9f $(MAN)

makefile.depend:
	cc -M $(SRCS) $(CFLAGS) >$@

include $(PROJECT_ROOT)/maketools/posix.locale.mk

.PHONY: build clean install uninstall depend debug

# vim: noet
# end of file
