/* 
/  sig_demo.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework4 Problem 2
/ 
*/ 
#define _POSIX_SOURCE
#define _BSD_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

static void sigHandlerUSR1(int sig)
{
	printf("SIGUSR1 has been caught\n");
}

static void sigHandlerUSR2(int sig)
{	
	printf("SIGUSR2 has been caught\n");
}

static void sigHandlerINT(int sig)
{	
	printf("SIGINT has been caught, terminating the program\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, const char * argv[])
{
	int pid = getpid();

	if(signal(SIGUSR1, sigHandlerUSR1) == SIG_ERR)
		exit(EXIT_FAILURE);
		
	if(signal(SIGUSR2, sigHandlerUSR2) == SIG_ERR)
		exit(EXIT_FAILURE);
		
	if(signal(SIGINT, sigHandlerINT) == SIG_ERR)
		exit(EXIT_FAILURE);
   
    kill(pid, SIGUSR1);
	kill(pid, SIGUSR2);
	kill(pid, SIGINT);
	       
    return 0;
}
