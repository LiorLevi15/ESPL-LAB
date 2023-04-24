# All Targets
all: encoder

encoder: encoder.o
	gcc -g -Wall -m32 -o encoder encoder.o

#Dependencies:
encoder.o: encoder.c
	gcc -g -Wall -m32 -c -o encoder.o encoder.c

.PHONY: clean

#Clean
clean:
	rm -f *.o encoder

