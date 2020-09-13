OUTFILE = helloWorld
OUTDIR = $(HOME)/cmpt433/public/myApps

all:
	arm-linux-gnueabihf-gcc -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror helloWorld.c -o helloWorld
	cp helloWorld $(OUTDIR)

clean:
	rm $(OUTDIR)/$(OUTFILE)
