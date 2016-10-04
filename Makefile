include config.mk

HDR = arg.h util.h
SRC =           \
	wew.c   \
	chwb2.c \
	wname.c \
	xmmv.c \
	xmrs.c

OBJ = $(SRC:.c=.o)
BIN = $(SRC:.c=)

.POSIX:

all: binutils

binutils: $(BIN)

manpages:
	cd man; $(MAKE) $(MFLAGS)

$(OBJ): $(HDR) util.o

.o:
	@echo "LD $@"
	@$(LD) $< -o $@ $(LDFLAGS) util.o

.c.o:
	@echo "CC $<"
	@$(CC) -c $< -o $@ $(CFLAGS)

install: $(BIN)
	mkdir -p $(DESTDIR)$(PREFIX)/bin/
	cp -f $(BIN) $(DESTDIR)$(PREFIX)/bin/
	cd man; $(MAKE) $(MFLAGS) install

uninstall:
	@echo "uninstalling binaries"
	@for util in $(BIN); do \
		rm -f $(DESTDIR)$(PREFIX)/bin/$$util; \
	done
	cd man; $(MAKE) $(MFLAGS) uninstall

clean:
	rm -f $(OBJ) $(BIN) util.o
