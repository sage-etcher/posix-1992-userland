
include $(PROJECT_ROOT)/maketools/posix.macros.mk
include makefile.depend

MANNUM = `echo "$(MAN)" |rev |cut -c 1`
FULL_MANDIR := $(MANDIR)/man$(MANNUM)

CATEGORIES 	?= none
LOCALES 	?= en

CFLAGS	+= '-DDOMAIN_NAME="$(LIB)"'
CFLAGS	+= '-DDOMAIN_DIR="$(LOCDIR)"'

STATIC_LIB	?=	lib$(LIB).a
SHARED_LIB	?=	lib$(LIB).so

DOMAIN_NAME	?=	$(LIB)

all: build

build: .gitignore build_mo $(STATIC_LIB) $(SHARED_LIB) $(MAN).gz

clean:
	rm -f $(OBJS)
	rm -f $(SHARED_LIB)
	rm -f $(STATIC_LIB)
	rm -f $(MAN).gz
	rm -f `find locale -name '*.mo'`
	rm -f `find locale -name '*.pot'`
	rm -f .locale_done

.gitignore: makefile
	rm -f $@.tmp
	-cat $@ >$@.tmp
	echo "$@.tmp" >>$@.tmp
	echo "$(SHARED_LIB)" >>$@.tmp
	echo "$(STATIC_LIB)" >>$@.tmp
	cat $@.tmp |sort |uniq >$@


install: build
	install -d -D -m 0755 $(LIBDIR)
	test -z "$(AS_STATIC)" || install -m 0755 $(STATIC_LIB) $(LIBDIR)/$(STATIC_LIB)
	test -z "$(AS_SHARED)" || \
		(install -m 0755 $(SHARED_LIB) $(LIBDIR)/$(SHARED_LIB).$(SO_VERSION) && \
		ln -sf $(SHARED_LIB).$(SO_VERSION) $(LIBDIR)/$(SHARED_LIB))
	test -z "$(MAN)" || \
		(install -d -D -m 0755 $(FULL_MANDIR) && \
	 	install -m 0644 -t $(FULL_MANDIR) $(MAN).gz); \
	test -z "$(USE_LOCALES)" || for i in $(LOCALES); do \
		for j in $(CATEGORIES); do \
			install -d -D -m 0755 $(LOCDIR)/$$i/$$j; \
			install -m 0644 ./locale/$$i/$$j/$(LIB).mo $(LOCDIR)/$$i/$$j/; \
		done; \
	done

uninstall:
	-unlink $(LIBDIR)/$(SHARED_LIB)
	rm -f $(LIBDIR)/$(SHARED_LIB).$(SO_VERSION)
	rm -f $(LIBDIR)/$(STATIC_LIB)
	rm -f $(FULL_MANDIR)/$(MAN).gz
	rm -f `find $(LOCDIR) -name $(LIB).mo`

depend: makefile.depend

$(STATIC_LIB): $(OBJS)
	test -z "$(AS_STATIC)" || \
		$(AR) rcs $@ $(OBJS)
	
$(SHARED_LIB): $(OBJS)
	test -z "$(AS_SHARED)" || \
		$(CC) -o $@ $(OBJS) $(LDFLAGS) -shared

.c.o:
	$(CC) -c $< $(CFLAGS) -fPIC

$(MAN).gz: $(MAN)
	test -z "$(MAN)" || gzip -k9f $(MAN)

makefile.depend:
	cc -M $(SRCS) $(CFLAGS) >$@

include $(PROJECT_ROOT)/maketools/posix.locale.mk

.PHONY: build clean install uninstall depend debug

# vim: noet
# end of file
