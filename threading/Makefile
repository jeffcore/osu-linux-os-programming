  
#  Jeff Rix
#  rixj@onid.oregonstate.edu
#  CS311-400
#  Homework 6 

CC=gcc
DEBUG=-g
C99=-std=c99
MATH=-lm
WARNINGS=-Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
CFLAGS=$(DEBUG) $(C99) $(MATH) $(WARNINGS) 
FILENAME1=primeMProc
FILENAME2=primePThread
PROGS=primeMProc primePThread


all: $(PROGS)

$(FILENAME1): $(FILENAME1).o
	$(CC) $(CFLAGS) -lrt -o $(FILENAME1) $(FILENAME1).o

$(FILENAME1).o: $(FILENAME1).c
	$(CC) $(CFLAGS) -lrt -c $(FILENAME1).c
	

$(FILENAME2): $(FILENAME2).o
	$(CC) $(CFLAGS) -pthread -o $(FILENAME2) $(FILENAME2).o

$(FILENAME2).o: $(FILENAME2).c
	$(CC) $(CFLAGS) -pthread -c $(FILENAME2).c

	
test5000_1td:
	rm -f mytest5000*.txt
	primePThread -q -m 5000 -c 1 > mytest5000_1td.txt
	diff mytest5000_1td.txt twin5000.txt
	rm -f mytest5000*.txt
	primeMProc -q -m 5000 -c 1 > mytest5000_1td.txt
	diff mytest5000_1td.txt twin5000.txt
test5000_2td:
	rm -f mytest5000*.txt
	primePThread -q -m 5000 -c 2 > mytest5000_2td.txt
	diff mytest5000_2td.txt twin5000.txt
	rm -f mytest5000*.txt
	primeMProc -q -m 5000 -c 2 > mytest5000_2td.txt
	diff mytest5000_2td.txt twin5000.txt
test5000_5td:
	rm -f mytest5000*.txt
	primePThread -q -m 5000 -c 5 > mytest5000_5td.txt
	diff mytest5000_5td.txt twin5000.txt
	rm -f mytest5000*.txt
	primeMProc -q -m 5000 -c 5 > mytest5000_5td.txt
	diff mytest5000_5td.txt twin5000.txt
test5000_9td:
	rm -f mytest5000*.txt
	primePThread -q -m 5000 -c 9 > mytest5000_9td.txt
	diff mytest5000_9td.txt twin5000.txt
	rm -f mytest5000*.txt
	primeMProc -q -m 5000 -c 9 > mytest5000_9td.txt
	diff mytest5000_9td.txt twin5000.txt

test5000_10td:
	rm -f mytest5000*.txt
	primePThread -q -m 5000 -c 10 > mytest5000_10d.txt
	diff mytest5000_10d.txt twin5000.txt
	rm -f mytest5000*.txt
	primeMProc -q -m 5000 -c 10 > mytest5000_10d.txt
	diff mytest5000_10d.txt twin5000.txt
test: test5000_1td test5000_2td test5000_5td test5000_9td test5000_10td
test3583:
	rm -f mytest5000*.txt
	primePThread -q -m 5000 -c 10 > mytest5000_10d.txt
	diff mytest5000_10d.txt twin5000.txt
	rm -f mytest5000*.txt
	primeMProc -q -m 5000 -c 10 > mytest5000_10d.txt
	diff mytest5000_10d.txt twin5000.txt

clean:
	rm -f $(PROGS) *.o *~ 
