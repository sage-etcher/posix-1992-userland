
CC       ?= clang
CFLAGS   := -Wall -Wextra -Wpedantic -pedantic-errors -std=c89 $(CFLAGS)
COPTS    ?= -O0

LDFLAGS  := $(LDFLAGS)
LDADD    := $(LDADD)
LDOPTS   ?= -ggdb


DESTDIR  ?=
PREFIX   ?= /usr/local
BINDIR   ?= $(DESTDIR)/$(PREFIX)/bin
SBINDIR  ?= $(DESTDIR)/$(PREFIX)/sbin
LIBDIR   ?= $(DESTDIR)/$(PREFIX)/lib
SHAREDIR ?= $(DESTDIR)/$(PREFIX)/share
MANDIR   ?= $(DESTDIR)/$(PREFIX)/share/man
LOCDIR   ?= $(DESTDIR)/$(PREFIX)/share/locale

.SUFFIXES: .c .o

# vim: noet
