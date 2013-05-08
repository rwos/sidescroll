CC=gcc
CFLAGS=-O2 -Wall -Wextra
LD=$(CC)
LDFLAGS=$(shell sdl-config --libs) -lSDL_ttf

OBJS=graphics.o io.o main.o menu.o objects.o profile.o world.o

PROG=sidescroll

all: $(PROG)

debug:
	CFLAGS='-O0 -g' make all

$(PROG): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c *.h
	$(CC) $(CFLAGS) $(shell sdl-config --cflags) $< -c -o $@ 

.PHONY: clean
clean:
	rm -f *.o $(PROG)

