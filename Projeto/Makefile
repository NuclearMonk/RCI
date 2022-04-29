CC = gcc
flags = -Wall -g
ofiles = client.o  ring.o node_data.o main.o message.o wait_list.o
testofiles = node_data.o wait_list.o


ring: ${ofiles}
	${CC} ${flags} -o ring ${ofiles};
	make clean;
main.o: main.c
	${CC} -c  ${flags} main.c

tests: tests.o ${testofiles}
	rm -f tests
	${CC} ${flags} -o tests tests.o ${testofiles};
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

wait_list.o: wait_list.c wait_list.h
	${CC} -c  ${flags} wait_list.c wait_list.h



.PHONY : clean
clean:
	rm -f *.o *.gch