# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGETS= main
SRC_C = main.c noworky.c

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror 
# -pg for supporting gprof profiling.
CFLAGS += -pg



# Convert the targets to full-path executables to delete.
OUTPUTS= $(addprefix $(OUTDIR)/, $(TARGETS))

all: $(TARGETS) noworky

$(TARGETS):
	$(CC_C) $(CFLAGS) $@.c -o $(OUTDIR)/$@ -lpthread

noworky:
	$(CC_C) $(CFLAGS) noworky.c -o $(OUTDIR)/noworky

clean:
	rm -f $(OUTPUTS)
