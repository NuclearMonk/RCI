CC = gcc
flags = -std=gnu99 -g -O3 -Wall -Wextra -Werror=pedantic -Werror=format=2 -pedantic-errors -Wno-unknown-pragmas
ofiles = client.o  ring.o node_data.o main.o message.o


ring: ${ofiles}
	${CC} ${flags} -o ring ${ofiles};
	make clean;
main.o: main.c
	${CC} -c  ${flags} main.c

tests: tests.o ${ofiles}
	rm -f tests
	${CC} ${flags} -o tests tests.o ${ofiles};
	make clean;
	./tests;
	
tests.o: tests.c
	${CC} ${flags} -c  tests.c


client.o: client.c client.h
	${CC} -c  ${flags} client.c client.h

ring.o: ring.c ring.h
	${CC} -c  ${flags} ring.c ring.h

node_data.o: node_data.c node_data.h
	${CC} -c  ${flags} node_data.c node_data.h

message.o: message.c message.h
	${CC} -c  ${flags} message.c message.h



.PHONY : clean
clean:
	rm -f *.o *.gch