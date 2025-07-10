
MANNUM = `echo "$(MAN)" |rev |cut -c 1`
FULL_MANDIR := $(MANDIR)/man$(MANNUM)

$(MAN).gz: $(MAN)
	test -z "$(MAN)" || gzip -k9f $(MAN)

build_man: $(MAN).gz

.PHONY: build_manpage

# vim: noet
# end of file
