
all: build

build clean install uninstall:
	${MAKE} -C sbin $@

test:
	${MAKE} -C test


.PHONY: build clean install uninstall test
# end of file
