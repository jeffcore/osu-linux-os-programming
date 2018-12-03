/* 
/  pipeline.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework5 Problem 2
/  References
/ 		TLPI book
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

void closefd(int fd);

int main(int argc, char **argv)
{	
	char c;	
	int i;
	char *fvalue = NULL; 
	int fds1[2];
	int fds2[2];
	int fds3[2];
	int fds4[2];	
	
	//check for correct number of command line args
	if (argc != 3)
	{
		printf("Proper usage:  pipeline -f <filename>\n");
		exit(EXIT_FAILURE);	
	}
	
	//process command line args
	while((c = getopt(argc, argv, "f:")) != -1)
    {
        switch(c)
        {
            case 'f':				
				fvalue = optarg;
                break;			             
			case '?':
                if (optopt == 'f')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:		
				abort();
        }		
    }
	
	//PIPE1 CREATION
	if(pipe(fds1) == -1)
	{	
		perror("Error Creating Pipes");
		exit(EXIT_FAILURE);
	}
	
	//rev command
	switch(fork()) 
	{
		case -1:
			//error case
			perror("could not create child"); 
			exit(EXIT_FAILURE);
			break;		
		case 0:
			//child case
			//closed unused fd	
			closefd(fds1[0]);			
			
			//PIPE 1 duplicate stdout of write end of pipe 
			if (fds1[1] != STDOUT_FILENO) 
			{
				if (dup2(fds1[1], STDOUT_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}
				//close duplicated descriptor	
				closefd(fds1[1]);								
			}	
			
			execlp("rev", "rev", (char *) NULL);
			perror("Error with rev command");
			exit(EXIT_FAILURE);
					
			break;
		default:
			//parent case	
			break;		
	}
	
	//PIPE2 CREATION
	if(pipe(fds2) == -1)
	{	
		perror("Error Creating Pipes");
		exit(EXIT_FAILURE);
	}
	
	//sort command
	switch(fork()) 
	{ 
		case -1:
			//error case
			perror("could not create child");
			exit(EXIT_FAILURE);
			break;		
		case 0:
			//child case							
			
			//close unused end
			closefd(fds2[0]);				
					
			//PIPE 2 duplicate stdout of write end of pipe 
			if (fds2[1] != STDOUT_FILENO) 
			{
				if (dup2(fds2[1], STDOUT_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}	
				//close duplicated descriptor	
				closefd(fds2[1]);
			}				
			
			//close unused end
			closefd(fds1[1]);	
					
			//PIPE 1 duplicate stdin to read end of pipe 
			if (fds1[0] != STDIN_FILENO) 
			{
				if (dup2(fds1[0], STDIN_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}	
				//close duplicated descriptor	
				closefd(fds1[0]);
			}	
			
			execlp("sort", "sort", (char *) NULL);
			perror("Error with rev command");
			exit(EXIT_FAILURE);			
			
			break;
		default:
			//parent case			
			break;		
	}
	//close parent ends of pipe
	closefd(fds1[0]);
	closefd(fds1[1]);
	
	//PIPE3 CREATION
	if(pipe(fds3) == -1)
	{	
		perror("Error Creating Pipes");
		exit(EXIT_FAILURE);
	}
	//uniq command
	switch(fork()) 
	{ 
		case -1:
			//error case
			perror("could not create child");
			exit(EXIT_FAILURE);
			break;		
		case 0:
			//child case							
			closefd(fds3[0]);	
					
			//PIPE 3 duplicate stdout of write end of pipe 
			if (fds3[1] != STDOUT_FILENO) 
			{
				if (dup2(fds3[1], STDOUT_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}	
				//close duplicated descriptor
				closefd(fds3[1]);					
			}	
		
			//close unused end
			closefd(fds2[1]);
		
			//PIPE 2 duplicate stdin to read end of pipe 
			if (fds2[0] != STDIN_FILENO) 
			{
				if (dup2(fds2[0], STDIN_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}	
				//close duplicated descriptor	
				closefd(fds2[0]);				
			}	
			
			execlp("uniq", "uniq", "-c", (char *) NULL);
			perror("Error with uniq command");
			exit(EXIT_FAILURE);			
			
			break;
		default:
			//parent case			
			break;		
	}
	
	//close parent ends of pipe
	closefd(fds2[0]);
	closefd(fds2[1]);
			
	//PIPE3 CREATION
	if(pipe(fds4) == -1)
	{	
		perror("Error Creating Pipes");
		exit(EXIT_FAILURE);
	}
		
	//tee command
	switch(fork()) 
	{ 
		case -1:
			//error case
			perror("could not create child");
			exit(EXIT_FAILURE);
			break;		
		case 0:
			//child case		
					
			//close unused end
			closefd(fds4[0]);	
					
			//PIPE 4 duplicate stdout of write end of pipe 
			if (fds4[1] != STDOUT_FILENO) 
			{
				if (dup2(fds4[1], STDOUT_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}	
				//close duplicated descriptor
				closefd(fds4[1]);					
			}	
		
			//close unused end
			closefd(fds3[1]);		
		
		
			//PIPE 3 duplicate stdin to read end of pipe 
			if (fds3[0] != STDIN_FILENO) 
			{
				if (dup2(fds3[0], STDIN_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}	
				//close duplicated descriptor	
				closefd(fds3[0]);			
			}	
			
			execlp("tee", "tee", fvalue, (char *) NULL);
			perror("Error with uniq command");
			exit(EXIT_FAILURE);			
			
			break;
		default:
			//parent case			
			break;		
	}	
	
	//close parent ends of pipe
	closefd(fds3[0]);
	closefd(fds3[1]);
		
	//tee command
	switch(fork()) 
	{ 
		case -1:
			//error case
			perror("could not create child");
			exit(EXIT_FAILURE);
			break;		
		case 0:
			//child case							
			
			//close unused end
			closefd(fds4[1]);			
		
			//PIPE 4 duplicate stdin to read end of pipe 
			if (fds4[0] != STDIN_FILENO) 
			{
				if (dup2(fds4[0], STDIN_FILENO) == -1)
				{
					perror("Error Duplication File");
					exit(EXIT_FAILURE);
				}	
				//close duplicated descriptor	
				closefd(fds4[0]);			
			}	
			
			execlp("wc", "wc", (char *) NULL);
			perror("Error with uniq command");
			exit(EXIT_FAILURE);			
			
			break;
		default:
			//parent case			
			break;		
	}	
		
	//close parent ends of pipe
	closefd(fds4[0]);
	closefd(fds4[1]);
	
	//wait and reap all children	
	for(i = 0; i < 5; i++)
	{
		if (wait(NULL) == -1)
		{
			perror("Waiting for second child");
			exit(EXIT_FAILURE);		
		}	
	}
	
    return 0;
}

/**
 *  function used to close file descriptors
 * 		
 *  @param fd file descriptor  * 
 * 
 */
void closefd(int fd)
{
	if(close(fd) == -1)	
	{			
		perror("Error Closing unused file descriptor");
		exit(EXIT_FAILURE);						
	}	
}


