PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

CC = cc
LD = $(CC)

CFLAGS += -std=c99 -pedantic -Wall -Os
LDFLAGS += -lxcb
