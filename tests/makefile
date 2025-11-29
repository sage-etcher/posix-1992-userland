SUBDIRS=    tap_suite sbin

TAPPATH     :=  $(PROJECT_ROOT)/tests/tap_suite
_CFLAGS     :=  $(CFLAGS) -O0 -ggdb -I$(TAPPATH)
_LDFLAGS    :=  $(LDFLAGS) -L$(TAPPATH) -ltap

EXTERN  :=  "PROJECT_ROOT=$(PROJECT_ROOT)"
EXTERN  +=  "CFLAGS=$(CFLAGS) $(_CFLAGS)"
EXTERN  +=  "LDFLAGS=$(LDFLAGS) $(_LDFLAGS)"
EXTERN  +=  "TAPPATH=$(TAPPATH)"

all: build

build clean run depend debug:
	@for d in $(SUBDIRS); do \
	    $(MAKE) -C $$d $@ -- $(EXTERN) || exit; \
	done

locale install uninstall:

.PHONY: build clean locale install uninstall depend debug
