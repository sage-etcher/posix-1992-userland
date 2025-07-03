
CC       ?= clang
CFLAGS   ?= -O0 -pipe -Wall -Wextra -Wpedantic -pedantic-errors -std=c89
LFLAGS   ?= -ggdb

DESTDIR  ?=
PREFIX   ?= /usr/local
BINDIR   ?= $(DESTDIR)/$(PREFIX)/bin
SBINDIR  ?= $(DESTDIR)/$(PREFIX)/sbin
LIBDIR   ?= $(DESTDIR)/$(PREFIX)/lib
SHAREDIR ?= $(DESTDIR)/$(PREFIX)/share
MANDIR   ?= $(DESTDIR)/$(PREFIX)/share/man
LOCDIR   ?= $(DESTDIR)/$(PREFIX)/share/locale

# vim: noet
