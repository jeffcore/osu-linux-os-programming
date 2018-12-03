/* 
/  uniqify.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework5 Problem 3
/  References	
/ 		tolower() - http://www.cplusplus.com/reference/cctype/tolower/
/		word parsing - CS261 Concordance project
/		stdin definition -  http://www.cplusplus.com/reference/cstdio/stdin
/		dyamically sized arrays for msg queues - http://web.cs.swarthmore.edu/~newhall/unixhelp/C_arrays.html
/		tsearch man page
/		strcmp - http://www.cplusplus.com/reference/cstring/strcmp/
*/
#define _GNU_SOURCE
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
#include <ctype.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <mqueue.h>
#include <signal.h>
#include <sys/stat.h>
#include <search.h>

#define DEFAULT_NUM_PROC 10;
#define QUEUE_PERMS 0666
#define MAX_MTEXT 100
#define WORD_TYPE 2
#define TERMINATOR_TYPE 2

typedef struct message
{
	long mtype;
	int mcount;
	char mtext[MAX_MTEXT]; 
} mess_t;

typedef struct word {
	int count;
	char word[MAX_MTEXT];
} word_t;

static key_t mq_key = (key_t) 0;
static int parent_queue = (int) 0;
static int num_subproc = (int) 0;
static int *msg_keys;
static int *msg_queues;
static int child_q;

//prototypes
void create_child_keys(void);
void create_child_queues(void);
void combination_process(void);
void create_subprocesses(void);
void sort_words_par_queue(void) ;
int compare(const void *word1, const void *word2);
void action(const void *nodep, const VISIT which, const int depth);
void add_words_to_queue(int mssq_id, char *word, int count);
word_t *get_word_from_queue(int mssq_id);
char* getWord(FILE *file);
void send_terminator(int mssq_id);
void sig_int(int sig);
void kill_all_queues(void);

int main(int argc, char **argv)
{	
	char c;	
	char *word;
	int i;
	
	//set default number of processes if n is not specified
	if (argc <= 2)
	{
		num_subproc = DEFAULT_NUM_PROC;
	}
	else
	{
		//process command line args
		while((c = getopt(argc, argv, "n:")) != -1)
		{
			switch(c)
			{
				case 'n':				
					num_subproc = atoi(optarg);
					if (num_subproc > 50)
					{
						num_subproc = 50;
					}				
					break;			             
				case '?':
					if (optopt == 'n')
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
	}
	
	//create message queues key
	mq_key = ftok(__FILE__, 'p');
	create_child_keys();	
		
	//create message queues
	parent_queue = msgget(mq_key, QUEUE_PERMS | IPC_CREAT);
	fflush(stdout);
	create_child_queues();
	signal(SIGINT, sig_int);
	create_subprocesses();	
	//combination process
	switch(fork()) 
	{
		case 0: 
			//child
				signal(SIGINT, sig_int);
				combination_process();
			break;
		case -1:
			perror("Error - creating sub process");
			exit(EXIT_FAILURE);
		default:
			break;		
	}
	
	//fill message queue
	while((word = getWord(stdin)))
	{
		add_words_to_queue(parent_queue, word, 1);
	}			
	
	//add terminating messages into message queue
	for(i = 0; i < num_subproc; i++)	
	{
		send_terminator(parent_queue);
	}

	//reap all children	
	for (i = 0; i < num_subproc + 1; i++)
	{		
		wait(NULL);
	}
	
	kill_all_queues();
	
	return 0;
}


/**
 *  create child keys for message queue creation used in subprocesses
 */
void create_child_keys(void)
{
	int i;
	
	msg_keys = (int *)malloc(sizeof(int)*(num_subproc));  
	//create message queue keys
	for(i = 0; i < num_subproc; i++) 
	{
		msg_keys[i] = ftok(__FILE__, i);
	}	
	
	fflush(stdout);
	return;
}

/**
 *  create child message queues used in subprocesses
 */
void create_child_queues(void)
{	
	int i;
	
	msg_queues = (int *)malloc(sizeof(int)*(num_subproc));  
	//create message queue keys
	for(i = 0; i < num_subproc; i++) 
	{
		msg_queues[i] = msgget(msg_keys[i], QUEUE_PERMS | IPC_CREAT);
	}	
		
	fflush(stdout);	
	return;
}

/**
 *  COMBINER Phase: the process that receives messages from child queues prints to to stdout
 *		this proc also combines duplicates and prints in alpha order
 */
void combination_process(void)
{
	word_t *words[num_subproc];   //pointer array to hold the current top word from each child process
	word_t *cur_word;			  // stores the current lowests word from top child queues
	int i, j, k;
	
	int found_word = 0;			  //used to determine when a valid word is found in child queues, due to terminator messages
		
	int num_terminators = 0;	  //count number of terminator messages from child queues, tells us that queues are empty
	int cur_word_index = 0;		  //index position of current word
	
	//premil filling of words array with the top word from each queue
	for(i = 0; i < num_subproc; i++)	
	{
		words[i] = get_word_from_queue(msg_queues[i]);
	}
	
	//forever loop to grab, process and combine and print all words in all queues
	for(;;)
	{	
		//get smallest word from word array - skip if equal words or terminator words - count number of terminators
		for(j = 0; j < num_subproc; j++)	
		{	
			//skip if terminator word
			if(words[j]->count > 0)
			{	
				//once a non-terminator word is found initial cur_word is created
				if(found_word == 0)
				{
					cur_word = words[j];
					cur_word_index = j;
					found_word = 1;
				}
				else
				{
					//compare current word to next word in array - if greater replace current word with this word
					if(strcmp(cur_word->word,  words[j]->word) > 0)
					{
						cur_word = words[j];
						cur_word_index = j;
					}
				}
			}
			else
			{
				//count number of terminator messages
				num_terminators++;
			}		
		}
		found_word = 0;   //reset word found
			
		//break loop if terminator messages found in all queues				
		if(num_terminators == num_subproc)
		{			
			break;
		}
		else
		{
			num_terminators = 0;
		}
	
		//replace queue index with new word, where current word was pulled from 
		words[cur_word_index] = get_word_from_queue(msg_queues[cur_word_index]);
			
		//check queue for duplicates of current word
		//now the fun begins - I am proud as shit of this code 
		//first step is to loop through each index in the words array		
		for(k = 0; k < num_subproc; k++)	
		{	
			//skip over terminator message
			if(words[k]->count > 0)
			{	
				//MATCH FOUND - first we increment the count, second replace position with new word
				//   then keep repeating in this index position until there is  not a match
				//	 this is sick, I am going to frame this section of code
				while(strcmp(cur_word->word,  words[k]->word) == 0)
				{
					cur_word->count += words[k]->count;
					words[k] = get_word_from_queue(msg_queues[k]);					
				} 
			}
		}
		
		//print to screen
		printf("%7d %s\n", cur_word->count, cur_word->word);			
	}
	
	return;
}

/**
 *  CHILD SUBPROCESS CREATION -- create n number of child process to sort words from parent message queue	
 */
void create_subprocesses(void)
{	
	int i;
	for(i = 0; i < num_subproc; i++)	
	{
		switch(fork()) 
		{
			case 0: 
				//child						
				signal(SIGINT, sig_int);				
				child_q = msg_queues[i];				
				sort_words_par_queue();
				send_terminator(child_q);				
				exit(EXIT_SUCCESS);
				break;
			case -1:
				perror("ERROR - creating child sub process");
				exit(EXIT_FAILURE);
			default:
				//parent
				break;		
		}
	}
	return;
}

/**
 *  SORT WORDS From Parent Message Queue 
 *  using tsearch
 */
void sort_words_par_queue(void)
{
	word_t *word;	
	void *root = NULL;
	void *sort_word;
	
	for(;;)
	{
		word = get_word_from_queue(parent_queue);
		
		if(word->count == 0)
		{
			break;
		}
		else
		{
			//see if word is already in tree
			sort_word = tfind((void*)word, &root, compare);
			
			//if it is in tree increment count
			if(sort_word != NULL)
			{
				(*(word_t **)sort_word)->count++;
			}
			else  //if not add it to bst
			{
				tsearch((void*)word, &root, compare);
			}
		}
	}
		
	//walk the tree calls function action
	twalk(root, action);
	
	fflush(stdout);					
	return;
}

/**
 *  compare function used in tsearch
 *  @param word1  pointer to word1
 *  @param word1  pointer to word2
 *  @return  > 0 if greater, 0 equal, < 0 if less than
 */
int compare(const void *word1, const void *word2) {
	int result;
	word_t *cmp1;
	word_t *cmp2;
	
	cmp1 = (word_t*)word1;
	cmp2 = (word_t*)word2;
	
	result = strcmp(cmp1->word, cmp2->word);
	
	return result;
}

/**
 *  action function for twalk function, does a post order walk
 *     adds words to child message queues
 */
void action(const void *nodep, const VISIT which, const int depth) {
	word_t *word;
	
	switch (which)
	{
		case postorder:
			word = *(word_t **) nodep;
			add_words_to_queue(child_q, word->word, word->count);
			break;
		case leaf:
			word = *(word_t **) nodep;
			add_words_to_queue(child_q, word->word, word->count);
			break;
		default:
			break;
	}
	return;
}

/**
 *  adds words to any message queue specified by mssq_id parameter
 *  @param mssq_id  id of message queue
 *  @param word  pointer to word
 *  @param word  count of word occurance 
 */
void add_words_to_queue(int mssq_id, char *word, int count)
{
	mess_t msg;
	int rc = 0;
	
	fflush(stdout);
	//type is always 1 for words - 2 for terminators
	msg.mtype = (long)WORD_TYPE;
	msg.mcount = count;
	memcpy(msg.mtext, word, strlen(word) + 1);
	
	rc = msgsnd(mssq_id, &msg, MAX_MTEXT, MSG_NOERROR);	
	if (rc == -1)
	{
		perror("ERROR - adding words to message queue");
		exit(EXIT_FAILURE);
	}	
	return;
}

/**
 *  grab word from queue return word_t struct
 *  @param mssq_id  id of message queue
 *  @return returns word_t struct 
 */
word_t *get_word_from_queue(int mssq_id)
{
	word_t *word = (word_t*)malloc(sizeof(word_t));
	mess_t rcv;
	int rc = 0;		
	
	rc = msgrcv(mssq_id, &rcv, MAX_MTEXT, 0, MSG_NOERROR);			
	if (rc == -1)
	{
		perror("ERROR - grabing word from message queue");
		exit(EXIT_FAILURE);
	}

	//fill word_t struct
	word->count = (int)rcv.mcount;
	memcpy(word->word, rcv.mtext, strlen(rcv.mtext)+1);
	
	return word; 
}

/**
 *  process word file do all requirements from assignment sheet
 *  @param file  pointer to file or stdin to process
 *  @return returns word
 */
char* getWord(FILE *file)
{
	int length = 0;
	int maxLength = 16;
	char character;
    
	char* word = (char*)malloc(sizeof(char) * maxLength);
	
	while( (character = fgetc(stdin)) != EOF)
	{
		if((length+1) > maxLength)
		{
			maxLength *= 2;
			word = (char*)realloc(word, maxLength);
		}
		if((character >= 'A' && character <= 'Z') || /*or an uppercase letter*/
		   (character >= 'a' && character <= 'z')) /*or a lowercase letter*/		     
		{			
			word[length] = tolower(character);
			length++;
		}
		else if(length > 0)
			break;
	}
    
	if(length == 0)
	{
		free(word);
		return NULL;
	}
	word[length] = '\0';

	return word;
}

/**
 *  add terminator to any message queue, used to detect that queue is empty
 *  @param mssq_id  id of message queue 
 */
void send_terminator(int mssq_id) 
{
	int rc = 0;
	mess_t msg;
	
	fflush(stdout);
	
	msg.mtype = (long)TERMINATOR_TYPE;
	msg.mcount = 0;
	memcpy(msg.mtext, "end", 4);		
	
	rc = msgsnd(mssq_id, &msg, sizeof(mess_t) - sizeof(long), MSG_NOERROR);
	if (rc == -1) {
		perror("ERROR -Sending Terminator Message To Queue");
		exit(EXIT_FAILURE);
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
	fflush(stdout);
  	
	//reap all child processes
	for (i = 0; i < num_subproc + 1; i++)
	{
		wait(NULL);
	}
	
	kill_all_queues();
	
  	exit(EXIT_SUCCESS);
	return;
}


/**
 *  kill all the queues 
 */
void kill_all_queues(void)
{	
	int i;
	for(i = 0; i < num_subproc; i++) 
	{			
		msgctl(msg_queues[i], IPC_RMID, NULL);
	}	
		
	msgctl(parent_queue, IPC_RMID, NULL);
	return;
}