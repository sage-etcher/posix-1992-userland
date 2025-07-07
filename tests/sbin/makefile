
SUBDIRS=	ls

EXTERN	:=	"PROJECT_ROOT=$(PROJECT_ROOT)"
EXTERN	+=	"CFLAGS=$(CFLAGS)"
EXTERN	+=	"LDFLAGS=$(LDFLAGS)"

all: build

build clean run depend debug:
	@for d in $(SUBDIRS); do \
	    $(MAKE) -C $$d $@ -- $(EXTERN); \
	done

.PHONY: build clean depend debug run
# end of file
