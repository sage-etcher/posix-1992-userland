
PROJECT_ROOT = `pwd`
SUBDIRS=	sbin tests

EXTERN	:=	"PROJECT_ROOT=$(PROJECT_ROOT)"


all: build

build clean locale install uninstall depend debug: 
	@for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@ -- $(EXTERN) || exit; \
	done

tests:
	@$(MAKE) -C tests run -- $(EXTERN) || exit;

.PHONY: build clean locale install uninstall depend debug tests
# vim: noet
# end of file
