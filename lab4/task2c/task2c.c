#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUF_SIZE 8192


typedef struct linux_dirent {
    int inode;
    int offset;
    short len;
    char buf[1];
}linux_dirent;

char *types[]= {"0","DT_FIFO","DT_CHR","3","DT_DIR","5","DT_BLK","7","DT_REG","9",
		"DT_LNK","11","DT_SOCK","13","DT_WHT"};

void debugger(int ID, int return_code);
void errorCheck(int num);
extern int system_call();
extern void code_start();
extern void code_end();
extern void infection();
extern void infector();

int main (int argc , char* argv[], char* envp[])
{

	int fd, nread, bpos, d_type, i, error; int debug = 0; int prefix = 0; int add = 0;
	char buf[BUF_SIZE];
	linux_dirent *dirent;
	char *prefixVal;

	for (i=1; i<argc; i++){
		if(strcmp(argv[i], "-D")==0) debug=1;
	}

	for (i=1; i<argc; i++){
		if (strncmp(argv[i], "-p", 2)==0) {
			prefixVal = argv[i]+2;
			prefix = 1;
		}
		else if (strncmp(argv[i], "-a", 2)==0) {
			prefixVal = argv[i]+2;
			prefix = 1;
			add = 1;
		}
	}

	fd = system_call(SYS_OPEN, ".", 0, 0);
	errorCheck(fd);
	if (debug) debugger(SYS_OPEN, fd);
	nread = system_call(SYS_GETDENTS, fd, buf, BUF_SIZE);
        errorCheck(nread);
	if (debug) debugger(SYS_GETDENTS, nread);
        for (bpos = 0; bpos < nread;) {
            	dirent = (linux_dirent *) (buf + bpos);
		if (prefix == 0 || strncmp(dirent->buf, prefixVal, strlen(prefixVal)) == 0) {
			error = system_call(SYS_WRITE, STDOUT, "Name: ", 6);
			errorCheck(error);
			if (debug) debugger(SYS_WRITE, error);
             		error = system_call(SYS_WRITE, STDOUT, dirent->buf, strlen(dirent->buf));
			errorCheck(error);
			if (debug) debugger(SYS_WRITE, error);
			error = system_call(SYS_WRITE,STDOUT,"\n",1);
			errorCheck(error);
			if (debug) debugger(SYS_WRITE, error);
		}
		if (prefix && strncmp(dirent->buf, prefixVal, strlen(prefixVal)) == 0) {
            		d_type = (int)(*(buf + bpos + (dirent->len - 1)));
			error = system_call(SYS_WRITE,STDOUT," Type: ",7);
			errorCheck(error);
			if (debug) debugger(SYS_WRITE, error);
			error = system_call(SYS_WRITE, STDOUT, types[d_type], strlen(types[d_type]));
			errorCheck(error);
			if (debug) debugger(SYS_WRITE, error);
			error = system_call(SYS_WRITE,STDOUT,"\n",1);
			errorCheck(error);
			if (debug) debugger(SYS_WRITE, error);
            		
		}
		if (add && strncmp(dirent->buf, prefixVal, strlen(prefixVal)) == 0) {
			infector(dirent->buf);
		}
		bpos += dirent->len;
        }
	return 0;
}


void debugger(int ID, int return_code) {
	system_call(SYS_WRITE,STDERR,"ID: ", 4);
	system_call(SYS_WRITE,STDERR,itoa(ID), 1);
	system_call(SYS_WRITE,STDERR,"\n", 1);
	system_call(SYS_WRITE,STDERR,"return code: ", 13);
	system_call(SYS_WRITE,STDERR,itoa(return_code), 1);
	system_call(SYS_WRITE,STDERR,"\n", 1);
}


void errorCheck(int num) {
	if (num < 0) system_call(SYS_EXIT, 0x55, 1, 1);
}
