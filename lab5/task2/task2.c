#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <sys/wait.h>
#include <time.h>
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0


typedef struct process{
	cmdLine* cmd; /* the parsed command line*/
 	pid_t pid; /* the process id that is
	running the command*/
 	int status; /* status of the process:
	RUNNING/SUSPENDED/TERMINATED */
 	struct process *next; /* next process in chain */
} process;

void addProcess(process** process_list, cmdLine* cmd, pid_t pid);
void printPath();
void execute (cmdLine* p);
int checkP(cmdLine* p);
process* add_last(process* _process, cmdLine* cmd, pid_t pid);
void printProcessList(process** process_list);
char* getStatusString(int status);
void freeProcessList(process* process_list);
void free_list(process* process_list);
void updateProcessList(process **process_list);
void updateProcessStatus(process* process_list, int pid, int status);
process* deleteProcesses(process* process_list);
process* deleteProcess(process* pToDelete);
void nap(int time, int pidToNap);
void stop(int pidToKill);
int debug = 0;
process* Gprocess_list = NULL;


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
	}
  	return 0;
}

void printPath(){
	char buf1[PATH_MAX]; 
	char* currDir = getcwd(buf1, 2048);
	printf("current directory: %s\n", currDir);
}

void execute (cmdLine* p){
	if (checkP(p)) return;
	int pid = fork();
	if(!pid){
		int error = execvp(p->arguments[0], p->arguments);
		if(error<0) {
			perror("error");
			_exit(EXIT_FAILURE);
		}
	}
	if(pid != -1) addProcess(&Gprocess_list, p, pid);
	if (debug){
			fprintf(stderr, "PID: %d\nExecuting command: %s\n",pid,p->arguments[0]);
		}
	if (p->blocking) {
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
		printf("%d\t\t%s\t%s\n",Gprocess_list->pid,Gprocess_list->cmd->arguments[0],getStatusString(Gprocess_list->status));
		error = chdir(p->arguments[1]);
		
	} else if(strcmp(p->arguments[0],"s")==0){
		found = 1;
		printProcessList(&Gprocess_list);

	} else if(strcmp(p->arguments[0],"nap")==0){
		found = 1;
		nap(atoi(p->arguments[1]), atoi(p->arguments[2]));

	} else if(strcmp(p->arguments[0],"stop")==0){
		found = 1;
		stop(atoi(p->arguments[1]));
	}

	if(error<0) {
			perror("error");
			exit(EXIT_FAILURE);
	}
	return found;
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
	(*process_list)=add_last((*process_list),cmd, pid);
}

process* add_last(process* _process, cmdLine* cmd, pid_t pid){
    if(_process==NULL){
		_process = malloc(sizeof(struct process));
    	_process->cmd=cmd;
    	_process->pid=pid;
    	_process->status=RUNNING;
    	_process->next=NULL;
    }
    else
        _process->next=add_last(_process->next,cmd, pid);
    return _process;
}

void printProcessList(process** process_list){
    updateProcessList(process_list);
    printf("PID\t\tCommand\t\tSTATUS\n");
	process* curr_process = (*process_list);
    while(curr_process != NULL){
        printf("%d\t\t%s\t%s\n",curr_process->pid,curr_process->cmd->arguments[0],getStatusString(curr_process->status));
        curr_process=curr_process->next;
    }
    (*process_list)=deleteProcesses((*process_list));
}


char* getStatusString(int status){
    if(status == TERMINATED)
      return "Terminated";
    else if(status == RUNNING)
      return "Running";
    else         
      return "Suspended";
}

void freeProcessList(process* process_list) {
	free_list(process_list);
}

void free_list(process* process_list) {
	process* curr_process=process_list;
    if(curr_process!=NULL){
        free_list(curr_process->next);
        freeCmdLines(curr_process->cmd);
        free(curr_process->cmd);
        free(curr_process);
    }
}

void updateProcessList(process **process_list) {
	process *currProcess_list = *(process_list);
	while(currProcess_list != NULL) {
		int status;
		int updatedS = RUNNING;
		int waitRes = waitpid(currProcess_list->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
		if(waitRes) {
			if(WIFEXITED(status) || WIFSIGNALED(status)) { updatedS = TERMINATED; }
			else if(WIFSTOPPED(status)) { updatedS = SUSPENDED; }
			//else if(WIFCONTINUED(status)) { updatedS = RUNNING; }
			updateProcessStatus(currProcess_list, currProcess_list->pid, updatedS);
		}
		currProcess_list = currProcess_list->next;
	}
}

void updateProcessStatus(process* process_list, int pid, int status) {
	process *currProcess_list = process_list;
	while(currProcess_list != NULL && currProcess_list->pid != pid) { currProcess_list = currProcess_list->next; }
	if(currProcess_list != NULL) { currProcess_list->status = status; }
}

process* deleteProcesses(process* process_list) {
	if(process_list == NULL) return process_list;
	else if(process_list->status == TERMINATED) {
		process_list = deleteProcess(process_list);
		return deleteProcesses(process_list);
	}
	else {
		process_list->next = deleteProcesses(process_list->next);
		return process_list;
	}
}

process* deleteProcess(process* pToDelete) {
	process* ret = pToDelete->next;
	freeCmdLines(pToDelete->cmd);
    pToDelete->cmd=NULL;
    free(pToDelete);
	return ret;
}

void nap(int time, int pidToNap) {
	int pid = fork();
	if(!pid) {
		kill(pidToNap, SIGTSTP);
		updateProcessStatus(Gprocess_list, pidToNap, SUSPENDED);
		sleep(time);
		kill(pidToNap, SIGCONT);
		updateProcessStatus(Gprocess_list, pidToNap, RUNNING);
		_Exit(EXIT_SUCCESS);
	} 
}

void stop(int pidToKill) {
	kill(pidToKill, SIGINT);
	updateProcessStatus(Gprocess_list, pidToKill, TERMINATED);
}

