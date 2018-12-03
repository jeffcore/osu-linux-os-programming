/* 
/  sig_demo.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework4 Problem 2
/  References
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


//way to print signals, similar to perror
extern void psignal(int sig, const char *msg);
extern const char *const sys_siglist[];

void intr_sig(int signal)
{ 
    psignal(signal, "Caught it");
    printf("caught signal %d: %s\n", signal, sys_siglist[signal]);
    return;
}


int main(int argc, const char * argv[])
{
    struct sigaction s;
    struct sigaction t;
    
    s.sa_handler = intr_sig;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
  
    sigaction(SIGINT, &s , &t);
    int i;
    for(i= 0;;++i){
        printf("i = %d\n", i);
        sleep(3);
    }
    
    
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}


