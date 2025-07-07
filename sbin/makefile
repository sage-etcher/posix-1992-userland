
SUBDIRS=	ls

USE_SBIN=	true
EXTERN :=	"PROJECT_ROOT=$(PROJECT_ROOT)" \
			"USE_SBIN=$(USE_SBIN)"

all: build

build clean locale install uninstall depend debug:
	@for d in $(SUBDIRS); do \
		$(MAKE) -C $$d $@ -- $(EXTERN); \
	done

.PHONY: build clean locale install uninstall depend debug
# end of file 
