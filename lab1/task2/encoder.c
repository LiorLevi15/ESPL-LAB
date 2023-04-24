#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int hexToInt(char hex);
void task2a(int debug, FILE* in, FILE* out);
void task2b(int debug, int key, FILE* in, FILE* out);

int main(int argc, char **argv) {
	int debug = 0; int key = 0; int counter = 0; int counter2 = 0;
	char hex = 0;
	char* input; char* output;
	FILE* inFile = stdin; FILE* outFile = stdout;
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
			inFile = fopen(input, "r");
		}
		if(strncmp(argv[i], "-o", 2)==0) {//{'-', 'i', 'f', 'i', 'l', 'e', '\0'}
			while(argv[i][counter2+2] != '\0') counter2++;
			output = malloc(counter2);
			for(int j=0; j < counter2; j++) output[j] = argv[i][j+2];
			outFile = fopen(output, "w+");
		}	
	}
	printf("key: %i\ndebug: %i\n",key, debug);
	if(key) task2b(debug, key, inFile, outFile);
	else task2a(debug, inFile, outFile);
	return 0;
}


void task2a(int debug, FILE* in, FILE* out) {
	printf("2a\n");
	char c;
	int counter = 0;
 	while((c=fgetc(in))!=EOF) {
		if('A' <= c && c <= 'Z') {
			if(debug) fprintf(stderr,"%i %i\n",c,'.');
			fputc('.', out);
			counter++;
		} else if(c == '\n') {
			if(debug) fprintf(stderr,"the number of letter: %i\n", counter);
			fputc(c, out);
			counter = 0;
		} else {
			if(debug) fprintf(stderr,"%i %i\n",c,c);
			fputc(c, out);
		}
	}
	
}

void task2b(int debug, int key, FILE* in, FILE* out) {
	printf("2b\n");
	char c, first;
	int countFirst = 1-key;
	printf("countFirst: %i\n", countFirst);
	int newLine = 1;
 	while((c=fgetc(in))!=EOF) {
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
					fputc(first, out);
					if(debug) fprintf(stderr,"  %i\n", first);
				}
			}
			fputc(c, out);
		} else if(debug && key < 0 && countFirst > 0) fprintf(stderr, "%i  \n", c);
		else {
			if(debug) fprintf(stderr,"%i %i\n",c,c);
			fputc(c, out);
		}
	}
}

int hexToInt(char hex) {
	if(hex && '0' <= hex && hex <= '9') return (hex-'0');
	else /*if (hex && 'A' <= hex && hex <= 'F')*/ return (10 + hex-'A');
}













