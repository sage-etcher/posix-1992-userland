
SUBDIRS=    add_child filter snprintlu snprintlu_dup lu_len strdup map \
            dir_content

LSPATH		:=	$(PROJECT_ROOT)/sbin/ls/core
LSLIB		:=	$(LSPATH)/libls.a
_LDFLAGS 	:= 	$(LDFLAGS) -L$(LSPATH) -lls -lm
_CFLAGS		:= 	$(CFLAGS)  -I$(LSPATH)

EXTERN	:=	"PROJECT_ROOT=$(PROJECT_ROOT)"
EXTERN	+=	"CFLAGS=$(CFLAGS) $(_CFLAGS)"
EXTERN	+=	"LDFLAGS=$(LDFLAGS) $(_LDFLAGS)"
EXTERN  +=  "LSPATH=$(LSPATH)"
EXTERN  +=  "LSLIB=$(LSLIB)"
EXTERN  +=  "TAPPATH=$(TAPPATH)"

all: build

build clean run depend debug:
	@for d in $(SUBDIRS); do \
	    $(MAKE) -C $$d $@ -- $(EXTERN) || exit; \
	done;

.PHONY: build clean run
# end of file
