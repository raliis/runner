CC = gcc
CFLAGS = -Wall -g
USBFLAGS = -lusb-1.0 -lpthread

# modify this to set install path
INSTALL_PATH = $(HOME)/.local/bin/

SRCDIR = src
OBJDIR = obj

# https://www.youtube.com/watch?v=CRlqU9XzVr4
SRCS=$(wildcard $(SRCDIR)/*.c)
OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

BINDIR = bin
BINNAME = runner
BIN = $(BINDIR)/$(BINNAME)

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(USBFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

release: CFLAGS = -O2
release: $(SRCDIR)/*.c
	$(CC) $(CFLAGS) $(SRCDIR)/*.c -o $(BIN) $(USBFLAGS)

#$(OBJDIR)/runner.o: $(SRCDIR)/runner.c
#	$(CC) $(CFLAGS) -c $< -o $@

#$(OBJDIR)/util.o: $(SRCDIR)/util.c
#	$(CC) $(CFLAGS) -c $< -o $@
#
clean:
	rm $(BINDIR)/* $(OBJDIR)/*

uninstall:
	rm $(INSTALL_PATH)/$(BINNAME)

install:
	cp $(BIN) $(INSTALL_PATH)
