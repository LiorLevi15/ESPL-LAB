#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void handler(int signum);

int main(int argc, char **argv){ 

	printf("Starting the program\n");
	signal(SIGTSTP, handler);
	signal(SIGCONT, handler);
	signal(SIGINT, handler);


	while(1) {
		sleep(2);
	}

	return 0;
}

void handler(int signum){
	printf("signal was resived: %s\n", strsignal(signum));
	if(signum == SIGTSTP){printf("Stoping the process\n");}
	if(signum == SIGCONT){printf("Continuting the process\n");}
	if(signum == SIGINT){printf("Interrupt the process\n");}
	signal(signum,SIG_DFL);
	raise(signum);
	if(signum == SIGTSTP){signal(SIGCONT,handler);}
	if(signum == SIGCONT){signal(SIGTSTP,handler);}
}