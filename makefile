
PROJECT_ROOT = `pwd`
SUBDIRS=	sbin tests

EXTERN	:=	"PROJECT_ROOT=$(PROJECT_ROOT)"

include config.mk

all: build

build clean locale install uninstall depend debug: 
	@for d in $(SUBDIRS); do \
		test -z "$(ENABLE_TESTS)" && test "$$d" = "tests" && \
				echo "WARNING: skipping the test suite, to build with them use 'make ... -- ENABLE_TESTS=true'" && \
				continue; \
		$(MAKE) -C $$d $@ -- $(EXTERN) || exit; \
	done

tests:
	@$(MAKE) -C tests run -- $(EXTERN) || exit;

.PHONY: build clean locale install uninstall depend debug tests
# vim: noet
# end of file
