#  Jeff Rix
#  rixj@onid.oregonstate.edu
#  CS311-400
#  Homework 2 problem 7
#

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall
PROGS=problem7

all: $(PROGS)

problem7: problem7.o
	$(CC) $(CFLAGS) -o problem7 problem7.o

problem7.o: problem7.c
	$(CC) $(CFLAGS) -c problem7.c

clean:
	rm -f $(PROGS) *.o *~

