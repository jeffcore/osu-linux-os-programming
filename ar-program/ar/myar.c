/* 
/   myar.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework4 Problem 3
/  References
/    copying args into buffer for concatenation http://stackoverflow.com/questions/15915869/problems-using-mainint-argc-char-argv
/	 get file size http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
*/   
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <utime.h>
#include <dirent.h>
#include "ar.h"

#define BLOCKSIZE 60
#define BLOCKSIZE2 1
#define STR_SIZE sizeof("rwxrwxrwx")
#define DEFAULT_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

#define ARNAMESIZE 16
#define ARDATESIZE 12
#define ARUIDSIZE 6
#define ARGIDSIZE 6
#define ARMODESIZE 8
#define ARSIZESIZE 10
#define ARFMAGSIZE 2

void q_option(char *ar_file, char **argv, int argc, int optind);
void t_option(char *ar_file);
void v_option(char *ar_file);
void x_option(char *ar_file, char **argv, int argc, int optind);
void d_option(char *ar_file, char **argv, int argc, int optind);
void a_option(char *ar_file);
int append_to_ar(int ar_fd, char *arfile);
int validate_ar_format(int ar_fd);
void convert_to_string(char *ar_info, char *temp, int len);
long long int convert_to_llint(char *ar_info, int len);

int main(int argc, char **argv)
{	
	char c;	
			
	while((c = getopt(argc, argv, "q:x:t:v:d:A:w:")) != -1)
    {
        switch(c)
        {
            case 'q':				
				q_option(optarg, argv, argc, optind);
                break; 
			case 'x':					
                x_option(optarg, argv, argc, optind);	
                break; 
			case 't':				
                t_option(optarg);		
                break;
			case 'v':
				v_option(optarg);	
                break; 						
			case 'd':				
                d_option(optarg, argv, argc, optind);				
                break; 
			case 'A':
				a_option(optarg);			
                break; 			
			case '?':
                if (optopt == 'q' || optopt == 'x' || optopt == 't' || optopt == 'd' || optopt == 'A')
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
	
	return 0;	
}

/**
 *  q command line option
 * 		“Quickly” append named files (members) to archive.
 *  @param ar_file name of the archive file
 *  @param argv command line arguments array
 *  @param argc number of command line arguments
 *  @param optind index point of optional arguments (the name of files to be appended)
 * 
 */
void q_option(char *ar_file, char **argv, int argc, int optind)
{
	int index;
	int ar_fd;
	int ar_file_exists = 1;
	int ar_file_end;
	struct stat s;
	
	//check if archive file exists
	if (stat(ar_file, &s) == -1)
		ar_file_exists = 0;
	
	//open or create archive file
	ar_fd = open(ar_file, O_CREAT | O_RDWR, 0666);
			
	if(ar_fd == -1)
	{
		perror("Can't open/create output file");
		exit(-1);
	}
	
	//seek to end of archive file or write ARMAG to top of new archive file 
	if(ar_file_exists)
	{
		ar_file_end = lseek(ar_fd, 0, SEEK_END);		
	}
	else
	{
		write(ar_fd, ARMAG, SARMAG);			
	}
	
	//write contents of files to archive file
	for (index = optind; index < argc; index++)		
	{			
		if(append_to_ar(ar_fd, argv[index]) == 0)
			printf("unable to append file to archive: %s", argv[index] );		
	}	

	if (close(ar_fd) == -1)
		perror("close ar file error");	
}

/**
 *  t command line option
 * 		Print a concise table of contents of the archive.
 *  @param ar_file name of the archive file 
 * 
 */
void t_option(char *ar_file)
{
	int ar_fd;
	size_t current_byte;			
	struct ar_hdr ar2;
	struct stat st;		
	char name[ARNAMESIZE+1];	
	off_t file_size;	
			
	//open archive file		
	ar_fd = open(ar_file, O_RDWR);
			
	if(ar_fd == -1)
	{
		perror("Can't open archive file");
		exit(-1);
	}
	
	//validate format of archive file
	if(validate_ar_format(ar_fd) == 0)
	{
		printf("Archive file invalid format\n");
		exit(-1);
	}
	
	//seek past SARMAG header
	current_byte = lseek(ar_fd, SARMAG, SEEK_SET);
	
	//get file size	of original ar file
	fstat(ar_fd, &st);			
	
	//loop through file archives in archive file	
	while(current_byte < st.st_size){		
	
		//copy file header information to ar_hrd struct
		read(ar_fd, ar2.ar_name, ARNAMESIZE);
		read(ar_fd, ar2.ar_date, ARDATESIZE);
		read(ar_fd, ar2.ar_uid, ARUIDSIZE);
		read(ar_fd, ar2.ar_gid, ARGIDSIZE);
		read(ar_fd, ar2.ar_mode, ARMODESIZE);
		read(ar_fd, ar2.ar_size, ARSIZESIZE);
		read(ar_fd, ar2.ar_fmag, ARFMAGSIZE);
	
		//convert file name to usable null terminated string
		convert_to_string(ar2.ar_name,name,ARNAMESIZE);
		
		//display the file name to screen
		printf("%s\n", name);

		//get file size	
		file_size = convert_to_llint(ar2.ar_size, ARNAMESIZE);
	
		//seek to end of file contents
		lseek(ar_fd, file_size, SEEK_CUR);
		
		//add new line to odd byte count		
		if ((lseek(ar_fd, 0, SEEK_CUR) % 2) == 1) 
		{
			lseek(ar_fd, 1, SEEK_CUR);			
		}
		current_byte = lseek(ar_fd, 0, SEEK_CUR);	
						
	}	
	
	if (close(ar_fd) == -1)
			perror("close input");	
}

/**
 *  v command line option
 * 		Print a verbose table of contents of the archive.
 *  @param ar_file name of the archive file 
 * 
 */
void v_option(char *ar_file)
{
	
	int ar_fd;
	off_t file_size;	
	size_t current_byte;
	struct ar_hdr ar2;
	struct stat st;			
	char uid[ARUIDSIZE+1];
	char gid[ARGIDSIZE+1];
	char name[ARNAMESIZE+1];
	char mode[ARMODESIZE + 1];	
	char size[ARSIZESIZE + 1];	
	static char str[STR_SIZE];	
	time_t t; 
	struct tm *local;
	char time_string[18];
	
	//open archive file		
	ar_fd = open(ar_file, O_RDWR);
			
	if(ar_fd == -1)
	{
		perror("Can't open archive file");
		exit(-1);
	}
	
	//validate format of archive file
	if(validate_ar_format(ar_fd) == 0)
	{
		printf("Archive file invalid format\n");
		exit(-1);
	}
	
	//seek past SARMAG header
	current_byte = lseek(ar_fd, SARMAG, SEEK_SET);
	
	//get file size	of original archive file
	fstat(ar_fd, &st);			
	
	while(current_byte < st.st_size){		
		
		//read contents of file to stat struct
		read(ar_fd, ar2.ar_name, ARNAMESIZE);
		read(ar_fd, ar2.ar_date, ARDATESIZE);
		read(ar_fd, ar2.ar_uid, ARUIDSIZE);
		read(ar_fd, ar2.ar_gid, ARGIDSIZE);
		read(ar_fd, ar2.ar_mode, ARMODESIZE);
		read(ar_fd, ar2.ar_size, ARSIZESIZE);
		read(ar_fd, ar2.ar_fmag, ARFMAGSIZE);
				
		//convert file permissions to string 
		memcpy(mode, ar2.ar_mode,  ARMODESIZE);
		mode[ARMODESIZE] = '\0';
		long int perm = strtol(mode, NULL, 8);
				
		snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
			(perm & S_IRUSR) ? 'r' : '-',
			(perm & S_IWUSR) ? 'w' : '-',
			(perm & S_IXUSR) ? 'x' : '-',					
			(perm & S_IRGRP) ? 'r' : '-',
			(perm & S_IWGRP) ? 'w' : '-',
			(perm & S_IXGRP) ? 'x' : '-',				
			(perm & S_IROTH) ? 'r' : '-', 
			(perm & S_IWOTH) ? 'w' : '-',
			(perm & S_IXOTH) ? 'x' : '-'	 
		);				
		
		convert_to_string(ar2.ar_uid,uid,ARUIDSIZE);
		convert_to_string(ar2.ar_gid,gid,ARGIDSIZE);			
		convert_to_string(ar2.ar_name,name,ARNAMESIZE);
		convert_to_string(ar2.ar_size,size,ARSIZESIZE);
		
		
		file_size = convert_to_llint(ar2.ar_size, ARNAMESIZE);
		
		//seek to end of file contents
		lseek(ar_fd, file_size, SEEK_CUR);
		
		//convert date/time to null terminated string
		char ar_time[ARDATESIZE + 1];			
		memcpy(ar_time, ar2.ar_date,  ARDATESIZE);
		ar_time[ARDATESIZE] = '\0';
		long int file_time = atol(ar_time);   //switch to long long int
		
		//format date
		t = file_time;			
		local = localtime(&t);			
		strftime(time_string, 18, "%b %e %R %Y", local);
		
		//print file information to console
		printf("%s %s/%s %6s %s %s\n", str, uid, gid, size, time_string, name);
		
		//add new line to odd byte count		
		if ((lseek(ar_fd, 0, SEEK_CUR) % 2) == 1) 
		{
			lseek(ar_fd, 1, SEEK_CUR);			
		}
		
		current_byte = lseek(ar_fd, 0, SEEK_CUR);				
	}	
	
	if (close(ar_fd) == -1)
		perror("close ar file error");	
}

/**
 *  x command line option
 * 		Extract named members. 
 *  @param ar_file name of the archive file
 *  @param argv command line arguments array
 *  @param argc number of command line arguments
 *  @param optind index point of optional arguments (the name of files to be appended)
 * 
 */
void x_option(char *ar_file, char **argv, int argc, int optind)
{					
	///x clean up
	///x make a function
	///x only extract first file found
	///x copy uid and gid to file
	///unmask permissionsf
	
	int ar_fd;
	int file_fd;	
	off_t file_size;
	unsigned long file_mode;
	long int file_time;
	ssize_t num_read = 0;
	int file_match;		
	size_t current_byte;
	struct utimbuf tstamp;
	struct ar_hdr ar2;
	struct stat st;		
	char uid[ARUIDSIZE+1];
	char gid[ARGIDSIZE+1];
	char name[ARNAMESIZE+1];
	char mode[ARMODESIZE + 1];		
	char ar_time[ARDATESIZE + 1];		
	char buf2[BLOCKSIZE2];
	
	//open archive file		
	ar_fd = open(ar_file, O_RDONLY);
			
	if(ar_fd < 0)
	{
		perror("Can't open archive file");
		exit(-1);
	}
	
	//validate format of archive file
	if(validate_ar_format(ar_fd) == 0)
	{
		printf("Archive file invalid format\n");
		exit(-1);
	}	
	
	//loop through file to extract
	current_byte = lseek(ar_fd, SARMAG, SEEK_SET);
	
	//get file size	of original ar file
	fstat(ar_fd, &st);		
	
	//  first file name matched code - variable length array used to keep track of files (from command line args)
	//	 that already match a file in the archive
	int j;
	int a[argc];
	for (j = 0; j < argc; j++)
	{
		a[j] = 0;			
	}	
	
	while(current_byte < st.st_size)
	{				
		file_match = 0;
		
		//read contents of file to stat struct
		read(ar_fd, ar2.ar_name, ARNAMESIZE);
		read(ar_fd, ar2.ar_date, ARDATESIZE);
		read(ar_fd, ar2.ar_uid, ARUIDSIZE);
		read(ar_fd, ar2.ar_gid, ARGIDSIZE);
		read(ar_fd, ar2.ar_mode, ARMODESIZE);
		read(ar_fd, ar2.ar_size, ARSIZESIZE);
		read(ar_fd, ar2.ar_fmag, ARFMAGSIZE);			

		//convert file info to strings							
		convert_to_string(ar2.ar_uid,uid,ARUIDSIZE);
		convert_to_string(ar2.ar_gid,gid,ARGIDSIZE);			
		convert_to_string(ar2.ar_name,name,ARNAMESIZE);
		
		//convert file mode to unsigned long 
		memcpy(mode, ar2.ar_mode,  ARMODESIZE);
		mode[ARMODESIZE] = '\0';
		file_mode = strtoul(mode, NULL, 8);   
		
		//convert file size to ll int							
		file_size = convert_to_llint(ar2.ar_size, ARNAMESIZE);
		
		//loop through file names to be deleted from command line args			
		int i;
		if (argc > 3)
		{							
			for (i = optind; i < argc; i++)					
			{				
				//  more first file name matched code
				if((a[i] == 0) && (strcmp(argv[i], name) == 0))
				{	
					file_match = 1;
					a[i] = 1;															
					break;
				}
			}				
		}
		else
		{			
			file_match = 1;
		}
		
		//if file match extract contents to file 
		if(file_match)
		{
			umask(DEFAULT_PERMS);
			
			file_fd = creat(name, file_mode);
			if(file_fd < 0)
			{
				perror("Can't create file");	
				exit(-1);				
			}		
			
			//write contents to file
			while(num_read < file_size)
			{				
				read(ar_fd, buf2, BLOCKSIZE2);
				num_read += write(file_fd, buf2, BLOCKSIZE2);	
			}	
			
			if (close(file_fd) == -1)
				perror("file close error");		

			//copy time stamps to extracted file
			memcpy(ar_time, ar2.ar_date,  ARDATESIZE);
			ar_time[ARDATESIZE] = '\0';
			file_time = atol(ar_time);   //switch to long long int
			
			tstamp.actime = file_time;
			tstamp.modtime = file_time;
			
			if(utime(name, &tstamp) == -1)
			{
				perror("Problem Assigning Timestamp");
			}
			
			//copy user id and group id to  extracted file
			chown(name, (long)uid, (long)gid);
				
		}
		else //no file match skip to end of file contents
		{
			lseek(ar_fd, file_size, SEEK_CUR);			
		}
		
		//compensate for new line to odd byte count				
		if ((lseek(ar_fd, 0, SEEK_CUR) % 2) == 1) 
		{				
			lseek(ar_fd, 1, SEEK_CUR);					
		}
		//record current byte
		current_byte = lseek(ar_fd, 0, SEEK_CUR);	
	}	

	if (close(ar_fd) == -1)
		perror("Closing archive file error");	
}

/**
 *  d command line option
 * 		Extract named members. 
 *  @param ar_file name of the archive file
 *  @param argv command line arguments array
 *  @param argc number of command line arguments
 *  @param optind index point of optional arguments (the name of files to be appended)
 * 
 */
void d_option(char *ar_file, char **argv, int argc, int optind)
{			
	if (argc > 3)
	{		
		int ar_fd;
		int new_ar_fd;
		int file_match;
		int file_size;
		ssize_t num_read;
		size_t current_byte;					
		struct ar_hdr ar2;
		struct stat st;
		char uid[ARUIDSIZE+1];
		char gid[ARGIDSIZE+1];
		char name[ARNAMESIZE+1];
		char size[ARSIZESIZE + 1];	
		char buf2[BLOCKSIZE2];
		
		// open main ar file and unlink it
		ar_fd = open(ar_file, O_RDWR);
				
		if(ar_fd == -1)
		{
			perror("Can't open archive file");
			exit(-1);
		}
		
		//validate format of archive file
		if(validate_ar_format(ar_fd) == 0)
		{
			printf("Archive file invalid format\n");
			exit(-1);
		}
		
		unlink(ar_file);
		
		//create new ar file that will replace the previous one
		new_ar_fd = open(ar_file, O_CREAT | O_RDWR, 0666);
				
		if(new_ar_fd == -1)
		{
			perror("Can't open/create output file");
			exit(-1);
		}
		
		//write ARMAG tag to new ar file
		write(new_ar_fd, ARMAG, SARMAG);			
		
		
		//seek past ARMAG tag
		current_byte = lseek(ar_fd, SARMAG, SEEK_SET);
		
		if (current_byte == -1)
		{
			perror("Badly formatted ar file.");
			exit(-1);
		}
		
		//get file size	of original ar file
		fstat(ar_fd, &st);					
		
		//  first file name matched code - variable length array used to keep track of files (from command line args)
		//	 that already match a file in the archive
		int j;
		int a[argc];
		for (j = 0; j < argc; j++)
		{
			a[j] = 0;			
		}	
					
		while((int)current_byte < st.st_size)
		{	
			file_match = 0;
			
			//read contents of file to stat struct
			read(ar_fd, ar2.ar_name, ARNAMESIZE);
			read(ar_fd, ar2.ar_date, ARDATESIZE);
			read(ar_fd, ar2.ar_uid, ARUIDSIZE);
			read(ar_fd, ar2.ar_gid, ARGIDSIZE);
			read(ar_fd, ar2.ar_mode, ARMODESIZE);
			read(ar_fd, ar2.ar_size, ARSIZESIZE);
			read(ar_fd, ar2.ar_fmag, ARFMAGSIZE);
			
			//convert file info to strings			
			convert_to_string(ar2.ar_uid,uid,ARUIDSIZE);
			convert_to_string(ar2.ar_gid,gid,ARGIDSIZE);			
			convert_to_string(ar2.ar_name,name,ARNAMESIZE);
			
			//convert file size to int
			memcpy(size, ar2.ar_size,  ARSIZESIZE);
			size[ARSIZESIZE] = '\0';
			file_size = atoi(size);   //switch to long long int
			
			//loop through file names to be deleted from command line args
			int i;				
			for (i = optind; i < argc; i++)					
			{
				// more first file name matched code
				if((a[i] == 0) && (strcmp(argv[i], name) == 0))
				{							
					{
						file_match = 1;
						a[i] = 1;
						break;
					}				
				}
			}
			
			//if file is matched seek to end of file contents OR write header and contents to new archive file
			if (file_match == 1)   
			{					
				lseek(ar_fd, file_size, SEEK_CUR);					
			}		
			else
			{
				//write file header contents
				write(new_ar_fd, &ar2, 60);
				
				//write contents of file
				num_read = 0;
				while(num_read < file_size)
				{				
					read(ar_fd, buf2, BLOCKSIZE2);
					num_read += write(new_ar_fd, buf2, BLOCKSIZE2);	
				}					
			}
			
			
			//add new line to odd byte count	
			if ((lseek(ar_fd, 0, SEEK_CUR) % 2) == 1) 
			{				
				lseek(ar_fd, 1, SEEK_CUR);
			}
			
			
			//get current offset of file
			current_byte = lseek(ar_fd, 0, SEEK_CUR);	
			
		}			
		if (close(ar_fd) == -1)
			perror("close ar file error");	
		
		if (close(new_ar_fd) == -1)
			perror("close ar file error");	 
	}
	else
	{
		 printf("No files where supplied to deleted from archive\n");
	}				
}


/**
 *  A command line option
 * 		Quickly append all “regular” files in the current directory (Except the archive itself)
 *  @param ar_file name of the archive file 
 * 
 */
void a_option(char *ar_file)
{
	//Resources	- Linux Programming Interface - Chapter 18 
	int ar_fd;
	int ar_file_end;
	struct stat st;	
	struct dirent *entry;
	DIR *dp;	
	
	//open archive file		
	ar_fd = open(ar_file, O_RDWR);
			
	if(ar_fd == -1)
	{
		perror("File Does not exist or is unable to be opened.");
		exit(-1);
	}
	
	//validate format of archive file
	if(validate_ar_format(ar_fd) == 0)
	{
		printf("Archive file invalid format\n");
		exit(-1);
	}
	
	
	//seek to the end of the file
	ar_file_end = lseek(ar_fd, 0, SEEK_END);					
	
	//open the current working directory 
	dp = opendir("./");
	//loop through all contents of the directory
	while((entry = readdir(dp)))
	{			
		//validate file can be opened, retrieve stat information of file
		if(stat(entry->d_name, &st) == 0)
		{			
			//check for only regular files (exclude root or parent directory and archive file)
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ar_file) != 0 && S_ISREG(st.st_mode))
			{
				//append file to archive
				if(append_to_ar(ar_fd, entry->d_name) == 0)
				{
					printf("unable to append file to archive: %s", entry->d_name );							
				}
				else
				{
					printf("%s added to archive\n", entry->d_name);
				}				
			}
		}					
	}
	
	closedir(dp);	
	
	if (close(ar_fd) == -1)
		perror("close file input error");			
}

/**
 * appends file to archive file
 * 
 *  @param ar_fd file descriptor for archive file 
 *  @param file name of file to be appended to archive file
 *  @return 1 - it worked , 0 - it failed, unable to open file
 */
int append_to_ar(int ar_fd, char *file)
{
	struct stat s;	
	int file_fd;
	ssize_t num_read;
	char file_name[ARNAMESIZE + 1];
	char buf[BLOCKSIZE];
	char buf2[BLOCKSIZE2];
	
	if(stat(file, &s) == 0)
	{			
		//open file that will be appended to archive file
		file_fd = open(file, O_RDONLY, 0777);
		if(file_fd == -1)
		{
			perror("Can't open/create output file");
			exit(-1);
		}
		
		//concatenate front slash to file name
		strcpy(file_name, file);		
		strcat(file_name, "/");

		//write archive header file information to buffer
		sprintf(buf, "%-16s%-12ld%-6ld%-6ld%-8lo%-10lld", file_name, (long)s.st_mtime,(long)s.st_uid,(long)s.st_gid, (unsigned long)s.st_mode, (long long)s.st_size);	
		
		//write archive header information to archive file
		write(ar_fd, buf, 58);	
		write(ar_fd, &ARFMAG, ARFMAGSIZE);
				
		//write contents of file to archive file
		while((num_read = read(file_fd, buf2, BLOCKSIZE2)) > 0)
		{				
			write(ar_fd, buf2, BLOCKSIZE2);	
		}
		
		//add new line to odd byte count
		if ((lseek(ar_fd, 0, SEEK_CUR) % 2) == 1) 
		{
			if (write(ar_fd, "\n", sizeof(char)) == -1) 
			{		
				perror("Error writing to archive file");
			}
		}		
		
		if (close(file_fd) == -1)
			perror("close file input error");				
		
		return 1;
	}	
	else
	{
		return 0;
	}	
}

/**
 *  validates header of archive file
 * 		also removes front slash in file name
 *  @param ar_file name of the archive file
 *  @param temp char pointer that result will be stored into 
 */
int validate_ar_format(int ar_fd)
{	
	char header[SARMAG];
	int result;
	
	lseek(ar_fd, 0, SEEK_SET);
	
	read(ar_fd, header, SARMAG);
	
	if(memcmp(header, ARMAG, SARMAG) == 0)
	{	
		result = 1;		
	}
	else
	{	
		result = 0;
	}
	
	lseek(ar_fd, 0, SEEK_SET);
	return result;
}

/**
 *  converts text from struct ar_hdr to a null terminated string
 * 		also removes front slash in file name
 *  @param ar_file name of the archive file
 *  @param temp char pointer that result will be stored into 
 *  @return  1 valid 0 not valid
 */
void convert_to_string(char *ar_info, char *temp, int len)
{	
	memset(temp, '\0', len + 1);
	memcpy(temp, ar_info, len);

	while ((temp[strlen(temp) - 1] == ' ') || (temp[strlen(temp) - 1] == '/')) {
		temp[strlen(temp) - 1] = '\0';
	}
}

/**
 *  converts text from struct ar_hdr to a long long int
 * 		also removes front slash in file name
 *  @param ar_file name of the archive file
 *  @param temp char pointer that result will be stored into 
 *  @return  1 valid 0 not valid
 */
long long int convert_to_llint(char *ar_info, int len)
{	
	char temp[len + 1];	
	memcpy(temp, ar_info,  len);
	temp[ARSIZESIZE] = '\0';
	
	return atoll(temp);  
}





