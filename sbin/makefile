
USE_SBIN=	true
EXTERN :=	"PROJECT_ROOT=$(PROJECT_ROOT)" \
			"USE_SBIN=$(USE_SBIN)"

all: build

build clean locale install uninstall depend debug:
	${MAKE} -C ls $@ -- $(EXTERN)


.PHONY: build clean locale install uninstall depend debug
# end of file 
