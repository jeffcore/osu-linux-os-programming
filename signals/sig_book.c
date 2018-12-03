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

static void
sigHandler(int sig)
{
	
	if(sig == SIGINT)
	{
		printf("interup");
	}
	else
	{
		printf("quit call");
	}
	
	printf("ouch\n");
}


int main(int argc, const char * argv[])
{
	int j;
	
	if(signal(SIGINT, sigHandler) == SIG_ERR)
		perror("signal");
   
	
	if(signal(SIGQUIT, sigHandler) == SIG_ERR)
		perror("signal");
   
   
    for (j = 0; ; j++)
	{
		printf("%d\n", j);
		sleep(3);
	
	}
   
    return 0;
}
