CC=gcc
CFLAGS=-O2 -Wall -Wextra $(shell sdl-config --cflags)
LD=$(CC)
LDFLAGS=$(CXXFLAGS) $(shell sdl-config --libs) -lSDL_ttf

OBJS=graphics.o highscores.o io.o main.o menu.o objects.o profile.o world.o

PROG=sidescroll

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

%.o: %.c *.h
	$(CC) $< -c $(CFLAGS) -o $@ 

.PHONY: clean
clean:
	rm -f *.o $(PROG)

