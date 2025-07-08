
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

all: build

build: locale $(STATIC_LIB) $(SHARED_LIB) $(MAN).gz

clean:
	rm -f $(OBJS)
	rm -f $(SHARED_LIB)
	rm -f $(STATIC_LIB)
	rm -f $(MAN).gz
	rm -f `find locale -name '*.mo'`
	rm -f `find locale -name '*.pot'`
	rm -f .locale_done

locale: .locale_done

.locale_done: $(SRCS)
	cat /dev/null >$@
	test -z "$(USE_LOCALES)" || $(PROJECT_ROOT)/buildtools/generate_locales.py \
		--domainname $(LIB) \
		--inputfiles `echo "$(SRCS)" |sed 's/ \+/,/g'` \
		--locales    `echo "$(LOCALES)" |sed 's/ \+/,/g'` \
		--categories `echo "$(CATEGORIES)" |sed 's/ \+/,/g'`

install: build
	install -d -D -m 0755 $(LIBDIR)
	test -z "$(AS_STATIC)" || install -m 0755 $(STATIC_LIB) $(LIBDIR)/$(STATIC_LIB)
	test -z "$(AS_SHARED)" || \
		(install -m 0755 $(SHARED_LIB) $(LIBDIR)/$(SHARED_LIB).$(SO_VERSION) && \
		ln -sf $(SHARED_LIB).$(SO_VERSION) $(LIBDIR)/$(SHARED_LIB))
	test -z "$(MAN)" || install -d -D -m 0755 $(FULL_MANDIR)
	test -z "$(MAN)" || install -m 0644 -t $(FULL_MANDIR) $(MAN).gz
	test -z "$(USE_LOCALES)" || for i in $(LOCALES); do \
		for j in $(CATEGORIES); do \
			install -d -D -m 0755 $(LOCDIR)/$$i/$$j; \
			install -m 0644 ./locale/$$i/$$j/$(LIB).mo $(LOCDIR)/$$i/$$j/; \
		done; \
	done

uninstall:
	-unlink $(USE_LIBDIR)/$(SHARED_LIB)
	rm -f $(USE_LIBDIR)/$(SHARED_LIB).$(SO_VERSION)
	rm -f $(USE_LIBDIR)/$(STATIC_LIB)
	rm -f $(FULL_MANDIR)/$(MAN).gz
	rm -f `find $(LOCDIR) -name $(LIB).mo`

depend: makefile.depend

debug:
	echo "$(AS_STATIC)"
	echo "$(AS_SHARED)"

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


.PHONY: build clean locale install uninstall depend debug

# vim: noet
# end of file
