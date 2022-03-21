CC = gcc
flags = -std=gnu99 -g -O3 -Wall -Wextra -Werror=pedantic -Werror=format=2 -pedantic-errors -Wno-unknown-pragmas
hfiles = data.h
ofiles = client.o  server.o node.o ring.o


ring: ${ofiles}
	${CC} ${flags} -o ring ${ofiles} ;
	make clean;

ring.o: ring.c
	${CC} -c  ${flags} ring.c

tests: tests.o ${ofiles}
	rm -f tests
	${CC} ${flags} -o tests tests.o ${ofiles};
	make clean;
	./tests;
	
tests.o: tests.c
	${CC} ${flags} -c  tests.c


client.o: client.c client.h
	${CC} -c  ${flags} client.c client.h

server.o: server.c server.h
	${CC} -c  ${flags} server.c server.h

node.o: node.c node.h
	${CC} -c  ${flags} node.c node.h



.PHONY : clean
clean:
	rm -f *.o