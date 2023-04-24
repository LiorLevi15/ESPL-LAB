#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define STDIN 0
#define STDOUT 1
#define STDERR 2


extern int system_call();
int isLexChar(char chr);
void debugger(int ID, int return_code);
void debuggerIO(char* type, char* toPrint);

int main (int argc , char* argv[], char* envp[])
{
	int debug = 0; char chr[1]; int count = 0;
	int inWord = 0; int ret = 0; int i = 0;
	int input = STDIN; int output = STDOUT;
	char * inputPath = "stdin"; char * outputPath = "stdout";
	
	for (i=1; i<argc; i++){
		if(strcmp(argv[i], "-D")==0) debug=1;
	}
	for (i=1; i<argc; i++){
		if (strncmp(argv[i], "-i", 2)==0) {
			input = system_call(SYS_OPEN, argv[i]+2, 2, 0777);
			inputPath = argv[i]+2;
			if (debug) debugger(SYS_OPEN, input);
		}
		else if(strncmp(argv[i], "-o", 2)==0) {
			output = system_call(SYS_OPEN, argv[i]+2, 2 | 64, 0777);
			outputPath = argv[i]+2;
			if (debug) debugger(SYS_OPEN, input);
		}
	}
	if (debug) debuggerIO("input", inputPath);
	if (debug) debuggerIO("output", outputPath);
	while (*chr != 0x0a) {
		ret = system_call(SYS_READ, input, chr, 1);
		if (debug) debugger(SYS_READ, ret);
		if (inWord == 0 && isLexChar(*chr) == 1) {
			count++;
			inWord = 1;
		}
		else if ( isLexChar(*chr) == 0 ) inWord = 0;
	}
	ret = system_call(SYS_WRITE,output,itoa(count), count);
	if (debug) debugger(SYS_WRITE, ret);
	ret = system_call(SYS_WRITE,output,"\n", 1);
	if (debug) debugger(SYS_WRITE, ret);
  return 0;
}

int isLexChar(char chr) { 
	if( ('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z') ) return 1;
	return 0;
}

void debuggerIO(char* type, char* toPrint) {
	system_call(SYS_WRITE, STDERR, type, strlen(type));
	system_call(SYS_WRITE,STDERR,": ", 2);
	system_call(SYS_WRITE,STDERR,toPrint, strlen(toPrint));
	system_call(SYS_WRITE,STDERR,"\n", 1);
}

void debugger(int ID, int return_code) {
	system_call(SYS_WRITE,STDERR,"ID: ", 4);
	system_call(SYS_WRITE,STDERR,itoa(ID), 1);
	system_call(SYS_WRITE,STDERR,"\n", 1);
	system_call(SYS_WRITE,STDERR,"return code: ", 13);
	system_call(SYS_WRITE,STDERR,itoa(return_code), 1);
	system_call(SYS_WRITE,STDERR,"\n", 1);
}
