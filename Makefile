PROGRAM = tetriks
CC      = gcc 
CFLAGS  = -g -ansi -std=c11 -Wall -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib -no-pie 
LDLIBS  = -lglut -lGLU -lGL -lm  

$(PROGRAM): image.o drawTetris.o
	$(CC) $(LDFLAGS) -o $(PROGRAM) drawTetris.o image.o main.c $(LDLIBS)

.PHONY: clean dist

clean:
	-rm *.o $(PROGRAM) *core

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)
