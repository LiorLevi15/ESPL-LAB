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
void checkRedirect(cmdLine* p);
void executeSingleCommand(cmdLine* p);
void executeChildCommand(cmdLine *cmd);
void executeCommands(cmdLine* cmd,int** pipes);
int *rightPipe(int **pipes, cmdLine *pCmdLine);
int *leftPipe(int **pipes, cmdLine *pCmdLine);
void releasePipes(int **pipes, int nPipes);
int ** createPipes(int nPipes);
int countCommands(cmdLine* cmd);
int checkP(cmdLine* p);

int debug = 0;
char lastCommand[2048];

int main (int argc , char* argv[], char* envp[])
{
	
	char buf2[2048];
	for (int i=1; i<argc; i++){
		if(strcmp(argv[i], "-d")==0) debug=1;
	}
	while(1){
		printPath();
		fgets(buf2, 2048, stdin);
        if(strncmp(buf2, "prtrls", 6) == 0) {
            printf("last command: %s", lastCommand);
            strcpy(lastCommand, buf2);
            continue;
        } else strcpy(lastCommand, buf2);
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
    int nPipes = countCommands(cmd);
    int ** pipes = createPipes(nPipes);
    executeCommands(cmd, pipes);
    releasePipes(pipes, nPipes);
}

void executeCommands(cmdLine* cmd, int** pipes) {
    if(cmd == NULL) {
        if(debug) printf("parent_process>exiting...\n");
        return;
    }
    int* leftP = leftPipe(pipes, cmd);
    int* rightP = rightPipe(pipes, cmd);
    int c_pid = fork();
    if(!c_pid) {
        if(leftP != NULL) {
            if(debug) printf("child%i>redirecting stdin to the read end of the pipe...\n", cmd->idx+1);
            close(0);
            dup(leftP[0]);
            close(leftP[0]);
            if(debug) printf("child%i>going to execute cmd: tail -n 2\n", cmd->idx+1);
        }
        if(rightP != NULL) {
            if(debug) printf("child%i>redirecting stdout to the write end of the pipe...\n", cmd->idx+1);
            close(1);
            dup(rightP[1]);
            close(rightP[1]);
            if(debug) printf("child%i>going to execute cmd: ls -l\n", cmd->idx+1);
        }
        checkRedirect(cmd);
        executeChildCommand(cmd);
    } else {
        if(debug) printf("parent_process>created process with id: %i\n", c_pid);
        if(debug) printf("parent_process>closing the write end of the pipe...\n");
        if(rightP != NULL) close(rightP[1]);
        if(debug) printf("parent_process>forking...\n");
        if(debug) printf("parent_process>created process with id: %i\n", c_pid);
        if(debug) printf("parent_process>closing the read end of the pipe...\n");
        if(leftP != NULL) close(leftP[0]);
        if(debug) printf("parent_process>waiting for child processes to terminate...\n");
        if(cmd->blocking) waitpid(c_pid, NULL, 0);
        executeCommands(cmd->next, pipes);
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

int countCommands(cmdLine* cmd) {
    cmdLine* curr = cmd;
    int count = 0;
    while(curr != NULL) {
        count = curr->idx;
        curr = curr->next;
    }
    return count;
}

int ** createPipes(int nPipes){
    int** pipes;
    pipes=(int**) calloc(nPipes, sizeof(int));

    for (int i=0; i<nPipes;i++){
        pipes[i]=(int*) calloc(2, sizeof(int));
        pipe(pipes[i]);
    }  
    return pipes;

}

void releasePipes(int **pipes, int nPipes){
        for (int i=0; i<nPipes;i++){
            free(pipes[i]);
        
        }  
    free(pipes);
}
int *leftPipe(int **pipes, cmdLine *pCmdLine){
    if (pCmdLine->idx == 0) return NULL;
    return pipes[pCmdLine->idx -1];
}

int *rightPipe(int **pipes, cmdLine *pCmdLine){
    if (pCmdLine->next == NULL) return NULL;
    return pipes[pCmdLine->idx];
}
