.POSIX:

DEBUG ?= 0

CPPFLAGS += -DDEBUG=$(DEBUG)
CFLAGS += -pedantic -Wall -Wextra
LDFLAGS += -s

PREFIX	= /usr/local
BINDIR	= $(PREFIX)/bin

NAME := asmc_fan_control
VERSION = 0.1
TARGET := asmc_fan_control
SRCS = asmc_fan_control.c
OBJS := $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

install: $(TARGET)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp -f $(TARGET) $(DESTDIR)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

distclean: clean
	rm -f $(NAME)-$(VERSION).tar.gz

dist: clean
	mkdir -p $(NAME)-$(VERSION)
	cp -R Makefile $(SRCS) $(NAME)-$(VERSION)
	tar -cf - $(NAME)-$(VERSION) | gzip > $(NAME)-$(VERSION).tar.gz
	rm -rf $(NAME)-$(VERSION)

.PHONY: all install uninstall clean distclean dist
