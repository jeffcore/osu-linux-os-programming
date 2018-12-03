/*
/  homework3.c
/
/  #Jeff Rix
/  #rixj@onid.oregonstate.edu
/  #CS311-400
/  #Homework3
/  References
/      atio() http://stackoverflow.com/questions/4796662/how-to-take-integers-as-command-line-arguments
/      displaying c string to write() function
/	   http://stackoverflow.com/questions/3866217/how-can-i-make-the-system-call-write-print-to-the-screen
*/
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define BLOCKSIZE 1

void write_to_console(int fd, int start_point, int end_point);

int main(int argc, char **argv)
{
	int fd;					/* file descriptor */
	char c;					/* used to store results from getopt() command line parser */
    /* variables that store the results of the command line args */
	int cla_count = 0;
	int vflag = 0;
	char *fvalue = NULL;
  int lvalue = 0;
	int o1value = 0;
	int o2value = 0;
	int evalue = 0;
  /*text dividers for bytes displayed */
	char msg1[] = "<offset 1>-----------------------------------------------------------------\n";
	char msg2[] = "<offset 2>-----------------------------------------------------------------\n";
	char msg3[] = "<end bytes>----------------------------------------------------------------\n";
	char msg4[] = "---------------------------------------------------------------------------\n";
	/*variables used in seek functions to display bytes from file*/
	int offset1;
	int offset2;
	int elen;
	int copy_to;

    /* retrieve the command line arguments and process them */
    while((c = getopt(argc, argv, "vl:o:O:e:f:")) != -1)
    {
      switch(c)
      {
        case 'v':
          vflag = 1;
						cla_count--;
          break;
				case 'f':
          fvalue = optarg;
          break;
	      case 'l':
	          lvalue = atoi(optarg);
	          break;
				case 'o':
            o1value = atoi(optarg);
            break;
				case 'O':
	          o2value = atoi(optarg);
	          break;
				case 'e':
            evalue = atoi(optarg);
            break;
        case '?':
          if (optopt == 'f' || optopt == 'e' || optopt == 'o' || optopt == 'O' || optopt == 'l')
          	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
          	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
            fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
          return 1;
        default:
				cla_count--;
				abort();
      }
			cla_count++;
    }
   	/*verify the correct number of valid cl args found */
    if(cla_count != 5)
    {
			printf("Incorrect Usage: %s -f <file name>, -l <length>, -o <offset1>, -O <offset2>, -e <elen>, [-v]\n", argv[0]);
			exit(1);
    }

    /*validate offest1 is not 0 or less */
	if(o1value<=0)
	{
		printf("-o offset must be greater than 0\n");
		exit(1);
	}
	/*validate lvalue(len) is not 0 or less */
	if(lvalue<=0)
	{
		printf("-l offset must be greater than 0\n");
		exit(1);
	}

  /* display v option domain */
  if(vflag)
  {
    printf ("%s -f %s -l %d -o %d -O %d -e %d \n",  argv[0], fvalue, lvalue, o1value, o2value, evalue);
  }

	/* open file from command line argument  */
	fd = open(fvalue, O_RDONLY);
	if(fd == -1)
	{
		perror("Can't open input file");
		exit(-1);
	}
	else
	{
		if(vflag)
		{
			printf("file opened successfully\n\n");
		}
	}

	/* write out divider */
	write(STDOUT_FILENO, msg1, sizeof(msg1)-1);

	/* set file offset to offset1 from command line arg*/
    offset1 = lseek(fd, o1value, SEEK_SET);
	copy_to = offset1 + lvalue;

	/* read contents of file  to console from offset1 plus number of bytes in len specified in command line arg 	 */
	write_to_console(fd, offset1, copy_to);
	/* write out divider */
	write(STDOUT_FILENO, msg2, sizeof(msg2)-1);

	/* set file offset to offset2 from command line arg*/
	offset2 = lseek(fd, o2value, SEEK_CUR);
	copy_to = offset2 + lvalue;

	/* read contents of file to console from offset2 plus plus number of bytes in len specified in command line arg  */
	write_to_console(fd, offset2, copy_to);
	/* write out divider */
	write(STDOUT_FILENO, msg3, sizeof(msg3)-1);

	/* set file offset to endlength(elen) from end of file foward */
	elen = lseek(fd, evalue * -1, SEEK_END );

	/*get the byte number one before the end of the file, we will copy bytes to this point */
	copy_to = elen + evalue - 1;

	/*  read contents of file to console from end of file forward to bytes in elen specified in command line args*/
	write_to_console(fd, elen, copy_to);
	/* write out divider */
	write(STDOUT_FILENO, msg4, sizeof(msg4)-1);

  return 0;
}

/*function used to read contents of a file and write it to console */
void write_to_console(int fd, int start_point, int end_point)
{
	char buf[BLOCKSIZE];
	int num_read;
	int num_written;
	while(start_point < end_point)
	{
		num_read = read(fd, buf, BLOCKSIZE);
		num_written = write(STDOUT_FILENO, buf, BLOCKSIZE);

		start_point += num_written;
	}
	write(STDOUT_FILENO, "\n", 1);
}
