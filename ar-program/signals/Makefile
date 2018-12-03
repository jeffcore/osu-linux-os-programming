#  Jeff Rix
#  rixj@onid.oregonstate.edu
#  CS311-400
#  Homework 4 Problem 2

CC=gcc
DEBUG=-g
C99=-std=c99
CFLAGS=$(DEBUG) $(C99) -Wall
FILENAME1=sig_demo
PROGS=sig_demo


all: $(PROGS)

$(PROGS): $(FILENAME1).o
	$(CC) $(CFLAGS) -o $(FILENAME1) $(FILENAME1).o

$(PROGS).o: $(FILENAME1).c
	$(CC) $(CFLAGS) -c $(FILENAME1).c

	
clean:
	rm -f $(PROGS) *.o *~ 
