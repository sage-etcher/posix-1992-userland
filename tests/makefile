
SUBDIRS=	sbin

CFLAGS	:=	$(CFLAGS) -O0 -ggdb
LDFLAGS :=	$(LDFLAGS)

EXTERN	:=	"PROJECT_ROOT=$(PROJECT_ROOT)"
EXTERN  +=  "CFLAGS=$(CFLAGS)"
EXTERN  +=  "LDFLAGS=$(LDFLAGS)"

all: build

build clean run depend debug:
	@for d in $(SUBDIRS); do \
	    $(MAKE) -C $$d $@ -- $(EXTERN) || exit; \
	done

locale install uninstall:

.PHONY: build clean locale install uninstall depend debug
# end of file
