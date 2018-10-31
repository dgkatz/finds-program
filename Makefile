all: finds

finds: finds.o pathalloc.o error.o
	gcc -o finds finds.o pathalloc.o error.o

finds.o: finds.c
	gcc -c finds.c

pathalloc.o: pathalloc.c
	gcc -c pathalloc.c

error.o: error.c
	gcc -c error.c

clean: 
	rm -rf finds finds.o pathalloc.o error.o finds.c~ Makefile~ 
