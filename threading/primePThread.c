/* 
/  primePTread.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework6 
/  References 
/ 		shortended sieve http://www.programminglogic.com/the-sieve-of-eratosthenes-implemented-in-c/
/       bitarray http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
*/		
#include <pthread.h>  
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

static int *A;	//bit array
static pthread_mutex_t mutex_bit_array;

void *check_prime(void *arg);
void *check_twinprime(void *arg);
void *check_prime_no_threads(void *arg); 

//struct to store information to be sent to threads
struct data{	
	unsigned int max_value;
	int num_threads;
	unsigned int num_bit_arrays;
	int offset;	
};

int main(int argc, char **argv)
{	
	char c;	
	int qvalue = 0; 
	unsigned int mvalue = UINT_MAX; 
	int cvalue = 1; 
	int size_bit_array = 0;
	unsigned int j;
	int i;
	pthread_t *thread;
	struct data *d;
	
	//process command line args
	while((c = getopt(argc, argv, "qm:c:")) != -1)
    {
        switch(c)
        {
            case 'q':				
				qvalue = 1;
                break;			
			case 'm':								
				if ((mvalue = strtoul(optarg, NULL, 0)) < 2)
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
	size_bit_array = mvalue / NUM_BITS_INT + 1;
	A = (int *)malloc(sizeof(int)*(size_bit_array));  
		
	// Clear the bit array
	for (j = 0; j < size_bit_array; j++ )
	{			
		A[j] = 0; 
	}
	
	//preset 0 and 1 
	SetBit( A, 0 );               // Set 3 bits
    SetBit( A, 1 );
	
	//create mutex for marking twin primes
	pthread_mutex_init(&mutex_bit_array, NULL);
	
	//create threads
	//check if max prime to find is worth creating threads
	//   if max prime is to small based on number of threads I am unable to split up 
	//	 bit array to guarantee concurrency
	if(size_bit_array/cvalue)
	{
		thread = (pthread_t*)malloc(sizeof(pthread_t)*cvalue);
		for (i = 0; i < cvalue; ++i)
		{	
			 d = malloc(sizeof(struct data));
			 d->max_value = mvalue;
			 d->num_threads = cvalue;	
			 d->num_bit_arrays = size_bit_array;
			 d->offset = i;			 
			 if (0 != pthread_create(&thread[i], NULL, check_prime, (void*)d))
			 {
			 	perror("bad thread");
				exit(EXIT_FAILURE);
			 }		
		}	 
	}
	else
	{
		thread = (pthread_t*)malloc(sizeof(pthread_t)*1);
		d = malloc(sizeof(struct data));
		d->max_value = mvalue;
		d->num_threads = 1;	
		d->num_bit_arrays = size_bit_array;
		d->offset = 0;			 
		if (0 != pthread_create(&thread[0], NULL, check_prime_no_threads, (void*)d))
		{
			perror("bad thread");
			exit(EXIT_FAILURE);
		}
	}
	
	//kill threads by joining them 
	for(i = 0; i < cvalue; ++i)
	{
		pthread_join(thread[i], NULL);
	}
	
	//create threads for twin prime checking step
	thread = (pthread_t*)malloc(sizeof(pthread_t)*cvalue);
		
	for (i = 0; i < cvalue; ++i)
	{	
		d = malloc(sizeof(struct data));
		d->max_value = mvalue;
		d->num_threads = cvalue;	
		d->offset = i;	
		pthread_create(&thread[i], NULL, check_twinprime, (void*)d);		
	}
		
	//kill threads by joining them 
	for(i = 0; i < cvalue; ++i)
	{
		pthread_join(thread[i], NULL);
	}
	
	pthread_mutex_destroy(&mutex_bit_array);

	//loop through bit array to find set values
	
	if(qvalue)
	{
		for (j = 2; j < mvalue - 1; j++ )
		{			
			if (!TestBit(A, j))
			{
				fprintf(stdout, "%u %u\n", j, (j + 2));				
			}		
		}	
	}
	free(A);
	return 0;
}

/**
 *  finds primes in a range of numbers without using threads
 *  @param struct with program information
 */
void *check_prime_no_threads(void *arg)
{
	unsigned int times_two = 2;
	unsigned int factor = 2;
	unsigned int multiply_num;
    unsigned int max_num;	
	unsigned int j;	
	struct data *d = (struct data*)arg;
	
	max_num = d->max_value;
	multiply_num = sqrt(max_num); 
	
	if (max_num == UINT_MAX)
	{
		max_num-=2;
	}	
	
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
					if((UINT_MAX - multiply_num) < factor)
					{						
						break;
					}
				}					
			 }
		 }
		 
		factor++;
	}	
	return NULL;
}

/**
 *  checks if a number is prime using threads
 *  @param struct with program information
 */
void *check_prime(void *arg)
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

	struct data *d = (struct data*)arg;
	x = d->max_value;
	
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
	//get min  and max numbers based on bit array splits	
	start_num = start * NUM_BITS_INT;
	if (x == UINT_MAX)
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
	
	return NULL;
}

/**
 *  checks for twin prime
 *  @param struct with program information
 */
void *check_twinprime(void *arg)
{
	unsigned int j;
	unsigned int start;
	unsigned int end;
	unsigned int length;
	unsigned int x; 
	
	//get min and max values that thread will check for twin primes
	struct data *d = (struct data*)arg;
	x = d->max_value;
	length = (x / (unsigned int)d->num_threads)+1;
	start = (unsigned int)d->offset * length;
	end = start + (unsigned int)length;

	//set the max value for last thread, due to integer division inaccuracies
	if((d->num_threads - 1) == d->offset)
		end = x - 2;
	else
		end = start + (unsigned int)length;
			
	//loop through bit array to find set values
	for (j = start; j <= end; j++ )
	{	
		if (!TestBit(A, j) && TestBit(A, (j+2)))
	 	{
	 		pthread_mutex_lock(&mutex_bit_array);
			SetBit(A, j);
			pthread_mutex_unlock(&mutex_bit_array);											
		}
	}

	return NULL;
}
































