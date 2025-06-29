
CC	?= clang
CFLAGS	?= -O2 -pipe -Wall -Wextra -Wpedantic -std=c89
LFLAGS	?= -ggdb

DESTDIR	?=
PREFIX	?= /usr/local
BINDIR	?= $(DESTDIR)/$(PREFIX)/bin
SBINDIR	?= $(DESTDIR)/$(PREFIX)/sbin
LIBDIR	?= $(DESTDIR)/$(PREFIX)/lib
MANDIR	?= $(DESTDIR)/$(PREFIX)/share/man

# vim: noet
