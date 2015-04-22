all: cproxy sproxy

cproxy: cproxy.c
	gcc -o cproxy cproxy.c

sproxy: sproxy.c
	gcc -o sproxy sproxy.c

clean:
	rm -f *.o core cproxy sproxy


