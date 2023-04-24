#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int hexToInt(char hex);
void task1a();
void task1b();
void task1c(int debug, int key);
void task1db(int debug, char* input);
void task1dc(int debug, int key, char* input);

int main(int argc, char **argv) {
	int debug = 0; int key = 0; int counter = 0;
	char hex = 0;
	char* input; //256];
	for (int i=1; i<argc; i++){
		if(strcmp(argv[i], "-D")==0) debug=1;
		if((strncmp(argv[i], "-e", 2)==0)) {
			hex = argv[i][2]; //good assamption?
			if(hex) key = -(hexToInt(hex));
		}
		if(strncmp(argv[i], "+e", 2)==0) {
			hex = argv[i][2];
			if(hex) key = hexToInt(hex);
		}
		if(strncmp(argv[i], "-i", 2)==0) {//{'-', 'i', 'f', 'i', 'l', 'e', '\0'}
			while(argv[i][counter+2] != '\0') counter++;
			input = malloc(counter);
			for(int j=0; j < counter; j++) input[j] = argv[i][j+2];
		}
	}
	printf("key: %i\ndebug: %i\n",key, debug);
	if(counter && key) task1dc(debug, key, input);
	else if(counter) task1db(debug, input);
	else if(key) task1c(debug, key);
	else if (debug) task1b();
	else task1a();
	return 0;
}

int hexToInt(char hex) {
	if(hex && '0' <= hex && hex <= '9') return (hex-'0');
	else /*if (hex && 'A' <= hex && hex <= 'F')*/ return (10 + hex-'A');
}

void task1a() {
	printf("task1a\n");
	char c;
	while((c=fgetc(stdin))!=EOF) {
		if('A' <= c && c <= 'Z') printf(".");
		else printf("%c", c);
	}
}

void task1b() {
	printf("task1b\n");
	char c;
	int counter = 0;
 	while((c=fgetc(stdin))!=EOF) {
		if('A' <= c && c <= 'Z') {
			fprintf(stderr,"%i %i\n",c,'.');
			printf(".");
			counter++;
		} else if(c == '\n') {
			fprintf(stderr,"the number of letter: %i\n", counter);
			printf("%c", c);
			counter = 0;
		} else {
			fprintf(stderr,"%i %i\n",c,c);
			printf("%c", c);
		}
	}
}

void task1c(int debug, int key) {
	printf("task1c\nkey: %i\ndebug: %i\n", key, debug);
	char c, first;
	int countFirst = 1-key;
	printf("countFirst: %i\n", countFirst);
	int newLine = 1;
 	while((c=fgetc(stdin))!=EOF) {
		countFirst--;
		if (countFirst > 0) {
			if(debug) fprintf(stderr,"%i  \n", c);			
			continue;
		}
		if(newLine) {
			first = c;
			newLine = 0;
		}
		if(c == '\n') {
			newLine = 1;
			countFirst = 1-key;
			if(key > 0) {
				for(int i=0; i<key; i++) {
					printf("%c", first);
					if(debug) fprintf(stderr,"  %i\n", first);
				}
			}
			printf("%c", c);
		} else if(debug && key < 0 && countFirst > 0) fprintf(stderr, "%i  \n", c);
		else {
			if(debug) fprintf(stderr,"%i %i\n",c,c);
			printf("%c", c);
		}
	}
}

void task1db(int debug, char* input) {
	FILE* file = fopen(input, "r"); 
	char c;
	int counter = 0;
 	while((c=fgetc(file))!=EOF) {
		if('A' <= c && c <= 'Z') {
			if(debug) fprintf(stderr,"%i %i\n",c,'.');
			printf(".");
			counter++;
		} else if(c == '\n') {
			if(debug) fprintf(stderr,"the number of letter: %i\n", counter);
			printf("%c", c);
			counter = 0;
		} else {
			if(debug) fprintf(stderr,"%i %i\n",c,c);
			printf("%c", c);
		}
	}
}

void task1dc(int debug, int key, char* input) {
	//printf("task1d\nkey: %i\ndebug: %i\ninput: %s\n", key, debug, input);
	FILE* file = fopen(input, "r"); 
	char c, first;
	int countFirst = 1-key;
	printf("countFirst: %i\n", countFirst);
	int newLine = 1;
 	while((c=fgetc(file))!=EOF) {
		countFirst--;
		if (countFirst > 0) {
			if(debug)fprintf(stderr,"%i  \n", c);			
			continue;
		}
		if(newLine) {
			first = c;
			newLine = 0;
		}
		if(c == '\n') {
			newLine = 1;
			countFirst = 1-key;
			if(key > 0) {
				for(int i=0; i<key; i++) {
					printf("%c", first);
					if(debug) fprintf(stderr,"  %i\n", first);
				}
			}
			printf("%c", c);
		} else if(debug && key < 0 && countFirst > 0) fprintf(stderr, "%i  \n", c);
		else {
			if(debug) fprintf(stderr,"%i %i\n",c,c);
			printf("%c", c);
		}
	}
}
	












