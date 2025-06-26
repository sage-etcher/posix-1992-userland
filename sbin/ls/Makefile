
all: build

build:

ls: ls.o
	${LD} -o $@ $< ${LDFLAGS} ${LDOPTS}

ls.o: ls.c
	${CC} -c -o $@ $< ${CFLAGS} ${COPTS}

clean:
	rm ls.o ls

install:
	install -m 0755 ls ${SBIN}/ls

uninstall:
	rm -f ${SBIN}/ls


.PHONY: build clean install uninstall
# end of file 
