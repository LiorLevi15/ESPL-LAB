
all: menu

menu: menu_map.o
	gcc -g -m32 -Wall -o menu menu_map.c

menu_map.o :  menu_map.c
	gcc -g -Wall -m32 -c -o menu_map.o menu_map.c

.PHONY: clean

# Clean the build directory
clean:
	rm -f *.o menu

