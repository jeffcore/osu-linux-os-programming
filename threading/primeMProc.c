/* 
/  primeMProc.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework6 
/  References 
/ 		
*/
#define _BSD_SOURCE 

#include <sys/mman.h>  
#include <sys/stat.h>   
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
#include <limits.h>
#include <math.h>  

#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )            
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) )
#define NUM_BITS_INT 32

void check_prime(void *arg);
void check_prime_no_threads(void *arg); 
void sig_int(int sig);

static int num_processes = 0;

//struct to store information to be sent to threads
struct data{	
	unsigned int max_value;
	int num_threads;
	unsigned int num_bit_arrays;
	int offset;	
	unsigned int *A;	
};


int main(int argc, char **argv)
{	
	char c;	
	int qvalue = 0; 
	unsigned int mvalue = UINT_MAX; 
	int cvalue = 1; 
	
	unsigned int *A;	
	unsigned int size_bit_array = 0;
	unsigned int bitmap_size;
	unsigned int object_size;
	unsigned int *bitmap;
	void *addr;
	int shmem_fd;
	
	unsigned int j;
	int i;
	int one_process = 0;
	struct data *d;
	pid_t pid;
	
	
	//process command line args
	while((c = getopt(argc, argv, "qm:c:")) != -1)
    {
        switch(c)
        {
            case 'q':				
				qvalue = 1;
                break;			
			case 'm':								
				if ((mvalue = atoi(optarg)) < 2)
					mvalue = UINT_MAX;
                break;			
			case 'c':			
				cvalue = atoi(optarg);
				if (cvalue <= 0)
					cvalue = 1;
                break;			
			case '?':
                if (optopt == 'q' || optopt == 'm' || optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:		
				break;
        }		
    }
	
	//set size of up bit array	
	size_bit_array = (int)(mvalue / NUM_BITS_INT + 1);
	bitmap_size = size_bit_array * sizeof(unsigned int);
	object_size = bitmap_size + sizeof(unsigned int);
	
	shmem_fd = shm_open("/rixj_memory", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(shmem_fd == -1)
	{
		fprintf(stdout, "Failed to create mem object");
		exit(EXIT_FAILURE);
	}
	
	//default to a size of zero so you have to resize memory object and use ftruncate	
	if(ftruncate(shmem_fd, object_size) == -1)
	{
		fprintf(stdout, "Failed to resize shared mem object");
		exit(EXIT_FAILURE);
	}
	//have to map it into address space in order to do anything with it with mmap
	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	
	if (addr == MAP_FAILED)
	{
		fprintf(stdout, "Failed to map shared mem object");
		exit(EXIT_FAILURE);
	}
		
	bitmap = (unsigned int*)addr;
	A = (unsigned int*)(bitmap + size_bit_array);

	// Clear the bit array
	for (j = 0; j < size_bit_array; j++ )
	{	
		bitmap[j] = 0; 
	}
	
	//preset 0 and 1 
	SetBit( bitmap, 0 );              
    SetBit( bitmap, 1 );
  
	//check if max prime to find is worth creating threads
	//   if max prime is to small based on number of threads I am unable to split up 
	//	 bit array to guarantee concurrency
	if(!size_bit_array/cvalue)
	{
    	cvalue = 1;
    	one_process = 1;
    }
  
	//code for sig handler
	num_processes = cvalue;
	signal(SIGINT, sig_int);
	
	  
    for(i = 0; i < cvalue; i++)
    {
    	switch(pid = fork())
    	{
    		case -1:    		
    			break;
    		case 0:    		
    			 d = malloc(sizeof(struct data));
				 d->max_value = mvalue;
				 d->num_threads = cvalue;	
				 d->num_bit_arrays = size_bit_array;
				 d->offset = i;	
				 d->A = bitmap;	
    			
				if(one_process == 1)
					check_prime_no_threads(d);					 
				else
					check_prime(d);
				if(munmap(addr, object_size) != 0)
					perror("unable to destory shared memory");						
				
				exit(EXIT_SUCCESS);
    			break;
    		default:
    			break;
    	}
    }
    
	//kill processes
    for(i = 0; i < cvalue; i++)
	{
		if (wait(NULL) == -1)
		{
			perror("Waiting for second child");
			exit(EXIT_FAILURE);		
		}	
	}  
	
	//mark twin primes
	for (j = 2; j < mvalue-1; j++ )
	{						
		if (!TestBit(bitmap, j) && TestBit(bitmap, (j+2)))
	 	{	
			SetBit(bitmap, j);			
		}	
	}	
	
	//display twin primes
	if(qvalue)
	{
		for (j = 2; j < mvalue - 1; j++ )
		{			
			if (!TestBit(bitmap, j))
			{
				fprintf(stdout, "%u %u\n", j, (j + 2));				
			}		
		}	
	}
	//kill memory and file descriptor
	if(munmap(addr, object_size) != 0)
		perror("unable to destory shared memory");
	
	if (shm_unlink("/rixj_memory") != 0)
		perror("unable to destory shared memory");
	
	if (close(shmem_fd) == -1)
		perror("close shared memory file descriptor error");	
	
	return 0;
}

/**
 *  finds primes in a range of numbers without using threads
 *  @param struct with program information
 */
void check_prime_no_threads(void *arg)
{
	unsigned int times_two = 2;
	unsigned int factor = 2;
	unsigned int multiply_num;
    unsigned int max_num;	
	unsigned int j;	
	struct data *d = (struct data*)arg;
	unsigned int *A;
	
	max_num = d->max_value;
	multiply_num = sqrt(max_num); 
	A = d->A;	
	// Sieve of Eratosthenes algorithm
	while(factor <= multiply_num)
	{
		if (TestBit(A, factor) == 0)
		{
		    for (j = times_two*factor; j <= max_num; j += factor)
			{				
				if (TestBit(A, j) == 0)
				{
					SetBit(A, j);
				}
				//manual check for overflow
				if((d->num_threads - 1) == d->offset)
				{													
					if((UINT_MAX - j) < factor)
					{						
						break;
					}
				}					
			 }
		 }
		 
		factor++;
	}	
	
	return;
}

/**
 *  checks if a number is prime using threads
 *  @param struct with program information
 */
void check_prime(void *arg)
{
	unsigned int times_two = 2;
	unsigned int factor = 2;
	unsigned int multiply_num;
    unsigned int x;	
	unsigned int j;
	unsigned int start;
	unsigned int end;
	unsigned int start_num;
	unsigned int end_num;
	unsigned int length;
	unsigned int *A;
	
	struct data *d = (struct data*)arg;
	x = d->max_value;
	A = d->A;
	//code that gets min and max number for thread to sieve through
	//	first step is getting the min max of bit arrays
	//	this splits the min max numbers based on bit array sizes of 32
	//		this is done to guarantee that we are not writing to the same bit array
	length = ceil((float)d->num_bit_arrays / d->num_threads);
	start = d->offset * length;
	end = start + length;
 	
 	//reset end thread to last bit array
	if (end > d->num_bit_arrays)
		end = d->num_bit_arrays;
	
	//get min  and max numbers based on bit array splits	
	start_num = start * NUM_BITS_INT;
	//prevent overflow
	if (d->max_value == UINT_MAX)
	{
		end_num = UINT_MAX;
	}
	else
	{
		end_num = end * NUM_BITS_INT - 1;
	}
	
	//fix max number based on it being UINT_MAX
	if (end_num > x)
	{
		end_num = x;
	}

	//get square root max number for thread
	multiply_num = sqrt(end_num) + 1; 
	
	// Sieve of Eratosthenes algorithm
	// 		if first offset run different sieve
	while(factor <= multiply_num)
	{
		if(d->offset == 0)
		{ 		   
		    for (j = times_two*factor; j <= end_num; j += factor)
			{				
				if (TestBit(A, j) == 0)
				{					
					SetBit(A, j);
				}
				//manual check for overflow
				if((d->num_threads - 1) == d->offset)
				{													
					 if((UINT_MAX - j) < factor)
					 {						
						break;
					 }
				}	
			 }		
		}
		else  //sieve through range of numbers
		{
			for (j = ((start_num/factor) + 1)*factor; j <= end_num; j += factor)
			{								
				if (TestBit(A, j) == 0)
				{
					SetBit(A, j);
				}
				//manual check for overflow
				if((d->num_threads - 1) == d->offset)
				{													
					 if((UINT_MAX - j) < factor)
					 {
						break;
					 }			
				}					
			}
		} 
		factor++; 	
	}
	
	return;
}

/**
 *  sighandler for program
 *  @param mssq_id  id of message queue 
 */
void sig_int(int sig)
{
	int i;	
  	
	//reap all child processes
	for (i = 0; i < num_processes; i++)
	{
		wait(NULL);
	}
	
  	exit(EXIT_SUCCESS);
	return;
}