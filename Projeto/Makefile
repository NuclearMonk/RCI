CC = gcc
flags = -std=c99 -g -O3 -Wall -Wextra -Werror=pedantic -Werror=format=2 -pedantic-errors -Wno-unknown-pragmas
ofiles = data.h
files = data.c


ring: main.o
	${CC} ${flags} -o ring main.o ${ofiles} ;
	make clean;

main.o: main.c
	${CC} -c  ${flags} main.c

tests: tests.o ${ofiles}
	rm -f tests
	${CC} ${flags} -o tests tests.o ${ofiles};
	make clean;
	clear;
	./tests;
	
tests.o: tests.c
	${CC} -c  ${flags} tests.c

.PHONY : clean
clean:
	rm -f *.o