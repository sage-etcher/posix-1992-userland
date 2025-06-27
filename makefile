
PROJECT_ROOT := $(PWD)

all: build

build clean install uninstall debug:
	${MAKE} -C sbin $@ -- "PROJECT_ROOT=$(PROJECT_ROOT)"

test:
	${MAKE} -C test


.PHONY: build clean install uninstall test debug
# vim: noet
# end of file
