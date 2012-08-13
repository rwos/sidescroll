CC=gcc
CFLAGS=-O2 -Wall -Wextra $(shell sdl-config --cflags)

CXX=g++
CXXFLAGS=$(CFLAGS)

LD=$(CXX)
LDFLAGS=$(CXXFLAGS) $(shell sdl-config --libs) -lSDL_ttf

C_OBJS=graphics.o highscores.o io.o menu.o objects.o profile.o world.o
CXX_SRC=client.cpp main.cpp
SERVER_CXX_SRC=serv.cpp serv_main.cpp

PROG=sidescroll

all: $(PROG) $(PROG)-server

$(PROG): $(C_OBJS) $(CXX_SRC)
	$(LD) $^ $(LDFLAGS) -o $@

$(PROG)-server: CFLAGS+= -DSERVER_BUILD
$(PROG)-server: $(C_OBJS) $(SERVER_CXX_SRC)
	$(LD) $^ $(LDFLAGS) -o $@

$(C_OBJS): %.o: %.c *.h
	$(CC) -c $< $(CFLAGS) -o $@ 

.PHONY: clean
clean:
	rm -f $(C_OBJS) $(PROG) $(PROG)-server

