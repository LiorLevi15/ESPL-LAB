#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void printPath();
void execute (cmdLine* p);
int debug = 0;

void checkRedirect(cmdLine* p);
void executeSingleCommand(cmdLine* p);
void executeChildCommand(cmdLine *cmd);
int checkP(cmdLine* p);

int main (int argc , char* argv[], char* envp[])
{
	
	char buf2[2048];
	for (int i=1; i<argc; i++){
		if(strcmp(argv[i], "-d")==0) debug=1;
	}
	while(1){
		printPath();
		fgets(buf2, 2048, stdin);
		cmdLine* commands = parseCmdLines(buf2);
		execute(commands);
		freeCmdLines(commands);
	}
  	return 0;
}

void printPath(){
	char buf1[PATH_MAX]; 
	char* currDir = getcwd(buf1, 2048);
	printf("current directory: %s\n", currDir);
}

void execute (cmdLine* cmd){
    if (checkP(cmd)) return;
    if (cmd->next == NULL) {
        executeSingleCommand(cmd);
        return;
    }
    int c1_pid, c2_pid, p[2];
    pipe(p);
    cmdLine* cmd_next = cmd->next;

    if(debug) printf("parent_process>forking...\n");
    c1_pid = fork();
    if (c1_pid > 0) { //parent
        if(debug) printf("parent_process>created process with id: %i\n", c1_pid);
        if(debug) printf("parent_process>closing the write end of the pipe...\n");
        close(p[1]);
        if(debug) printf("parent_process>forking...\n");
        c2_pid = fork();
        if (c2_pid > 0) { //parent
            if(debug) printf("parent_process>created process with id: %i\n", c2_pid);
            if(debug) printf("parent_process>closing the read end of the pipe...\n");
            close(p[0]);
            if(debug) printf("parent_process>waiting for child processes to terminate...\n");
            if(cmd->blocking) waitpid(c1_pid, NULL, 0);
            if(cmd_next->blocking) waitpid(c2_pid, NULL, 0);
            if(debug) printf("parent_process>exiting...\n");

        } else { //child2
            if(debug) printf("child2>redirecting stdin to the read end of the pipe...\n");
            close(0);
            dup(p[0]);
            close(p[0]);
            if(debug) printf("child2>going to execute cmd: tail -n 2\n");
            checkRedirect(cmd_next);
            executeChildCommand(cmd_next);
        }

    } else { //child1
        if(debug) printf("child1>redirecting stdout to the write end of the pipe...\n");
        close(1);
        dup(p[1]);
        close(p[1]);
        if(debug) printf("child1>going to execute cmd: ls -l\n");
        checkRedirect(cmd);
        executeChildCommand(cmd);
    }
}

void executeChildCommand(cmdLine *cmd) {
    int error = 0;
    error = execvp(cmd->arguments[0], cmd->arguments);
    if(error<0) {
		perror("error");
		_exit(EXIT_FAILURE);
	}
    _exit(EXIT_SUCCESS);
}

void executeSingleCommand(cmdLine* cmd) {
    int error = 0;
	int pid = fork();
	if(!pid){
        checkRedirect(cmd);
		error = execvp(cmd->arguments[0], cmd->arguments);
		if(error<0) {
			perror("error");
			_exit(EXIT_FAILURE);
		}
        _exit(EXIT_SUCCESS);
	}
	if (debug){
			fprintf(stderr, "PID: %d\nExecuting command: %s\n",pid,cmd->arguments[0]);
		}
	if (cmd->blocking) {
		waitpid(pid,NULL,0);
	} 
}

int checkP(cmdLine* p){
	int error = 0;
	int found = 0;
	if(strcmp(p->arguments[0],"quit")==0){
		found = 1;
		_exit(EXIT_SUCCESS);
	} else if(strcmp(p->arguments[0],"cd")==0){
		found = 1;
		error = chdir(p->arguments[1]);
    }
	if(error<0) {
			perror("error");
			exit(EXIT_FAILURE);
	}
	return found;
}

void checkRedirect(cmdLine* p) {
    if(p->inputRedirect) {
        close(0);
        dup(open(p->inputRedirect, O_RDONLY, 0644));
    }
    if(p->outputRedirect) {
        close(1);
        dup(open(p->outputRedirect, O_WRONLY | O_CREAT, 0644));
    }
}


