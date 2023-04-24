# All Targets
all: virusDetector

virusDetector: task2b.o
	gcc -g -Wall -m32 -o virusDetector task2b.o

# Dependancies:
task2b.o: task2b.c
	gcc -g -Wall -m32 -c -o task2b.o task2b.c

.PHONY: clean

# Clean the build directory
clean:
	rm -f *.o virusDetector


