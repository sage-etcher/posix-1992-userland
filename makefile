
PROJECT_ROOT := $(PWD)

all: build

build clean install uninstall depend debug:
	${MAKE} -C sbin $@ -- "PROJECT_ROOT=$(PROJECT_ROOT)"

test:
	${MAKE} -C test


.PHONY: build clean install uninstall test depend debug
# vim: noet
# end of file
