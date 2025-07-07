
SUBDIRS=	function

LSPATH		:=	$(PROJECT_ROOT)/sbin/ls
_LDFLAGS 	:= 	$(LDFLAGS) -L$(LSPATH) -lls
_CFLAGS		:= 	$(CFLAGS)  -I$(LSPATH)

EXTERN	:=	"PROJECT_ROOT=$(PROJECT_ROOT)"
EXTERN	+=	"CFLAGS=$(CFLAGS) $(_CFLAGS)"
EXTERN	+=	"LDFLAGS=$(LDFLAGS) $(_LDFLAGS)"
EXTERN  +=  "LSPATH=$(LSPATH)"

all: build

build clean run depend debug:
	@for d in $(SUBDIRS); do \
	    $(MAKE) -C $$d $@ -- $(EXTERN) || exit; \
	done;

debug:
	echo 'test-ls-function: $(EXTERN)'

.PHONY: build clean run
# end of file
