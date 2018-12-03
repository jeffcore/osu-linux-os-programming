//
//  Problem7.c
//  CS311Problem7
//
//  #Jeff Rix
//  #rixj@onid.oregonstate.edu
//  #CS311-400
//  #Homework2 Question 7
//  #used example code from 
//   http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt

#include <stdio.h>
#include <getopt.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char c;
    int hflag = 0;
    int tflag = 0;
    char *fvalue = NULL;
    struct utsname uname_pointer;
    time_t time_raw_format;
    struct stat s;
     
    //retrieve the command line arguments and process them
    if(argc == 1)
    {	
	printf("No command line agruments supplied:  -h show base hostname ");
	printf("-t  show current time ");
	printf("-f <file name>   show size of a file\n");

	exit(1);
    }    

    while((c = getopt(argc, argv, "htf:")) != -1)
    {
        switch(c)
        {
            case 'h':
                hflag = 1;
                break;
            case 't':
                tflag = 1;
                break;
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
		exit(1);
        }
    }
    
    //printf ("aflag = %d, bflag = %d, cvalue = %s\n", hflag, tflag, fvalue);
    
    //display h option domain
    if(hflag)
    {
        uname(&uname_pointer);
        printf("Hostname = %s \n", uname_pointer.nodename);
    }
    
    //display t option for local time
    if(tflag)
    {
        time(&time_raw_format);
        printf("The current local time: %s\n", ctime(&time_raw_format));
    }
    
    //display f option for size of file
    if (fvalue != NULL) {
        if (stat(fvalue, &s) == 0) {
            printf("size of file '%s' is %d bytes\n", fvalue, (int) s.st_size);
        } else {
            printf("file '%s' not found\n", fvalue);
        }
    }
   
    return 0;
}

