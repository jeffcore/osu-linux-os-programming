/*
  Jeff Rix
  rixj@onid.oregonstate.edu
  CS311-400
  Homework 1
  $RCSfile$
  $Revision$
  $Author$
  $Date$
  $Log$

  from:
   http://www.programmingsimplified.com/c-program-read-file
   
  source for clearing trailing new line from filename input
    http://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
    2nd post
*/

#include "read_file.h"


int main()
{
   char ch, file_name[25];
   //FILE *fp;
   int fd;
   int num_read;
   int len;

   printf("Enter the name of file you wish to see\n");  
   fgets(file_name, 25, stdin);

   len = strlen(file_name);
   if(file_name[len - 1] == '\n')
     file_name[len - 1] = '\0';

 
   fd = open(file_name, O_RDONLY);
   if (fd < 0)
     {
       perror("Error while opening the file.\n");
       exit(EXIT_FAILURE);
     }
   printf("The contents of %s file are :\n", file_name);

   for (;;)
     {
       num_read = read(fd, &ch, sizeof(ch));
       if (num_read < 0)
	 {
	   perror("Error while reading the file.\n");
	   exit(EXIT_FAILURE);
	 }
       if (num_read == 0)
	 {
	   break;
	 }
       write(STDOUT_FILENO, &ch, sizeof(ch));
     }
   return 0;
}
