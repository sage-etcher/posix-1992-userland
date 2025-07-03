
PROJECT_ROOT = `pwd`

all: build

build clean locale install uninstall depend debug:
	${MAKE} -C sbin $@ -- "PROJECT_ROOT=$(PROJECT_ROOT)"

test:
	${MAKE} -C test

.PHONY: build clean locale install uninstall test depend debug
# vim: noet
# end of file
