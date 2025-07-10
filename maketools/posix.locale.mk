
POT=	locale/$(DOMAIN_NAME).pot
PO=		locale/$$loc/LC_MESSAGES/$(DOMAIN_NAME).po
MO=		locale/$$loc/LC_MESSAGES/$(DOMAIN_NAME).mo

$(POT): $(SRCS)
	test -z "$(USE_LOCALES)" || xgettext $(SRCS) -o $(POT) -k_

build_po: $(POT)
	test -z "$(USE_LOCALES)" || for loc in $(LOCALES); do \
		test -e "$(PO)" && \
				msgmerge $(PO) $(POT) -o $(PO) || \
				msginit $(PO) -i $(POT) -l $$loc --no-translator; \
	done

build_mo: .locale_done

.locale_done:
	cat /dev/null > .locale_done
	test -z "$(USE_LOCALES)" || for loc in $(LOCALES); do \
		msgfmt $(PO) -o $(MO); \
	done
	
locale: build_po

.PHONY: build_po build_mo locale

# end of file
