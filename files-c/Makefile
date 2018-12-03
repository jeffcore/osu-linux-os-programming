#  Jeff Rix
#  rixj@onid.oregonstate.edu
#  CS311-400
#  Homework 3

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall
FILENAME=homework3
PROGS=homework3

all: $(PROGS)

$(FILENAME): $(FILENAME).o
	$(CC) $(CFLAGS) -o $(FILENAME) $(FILENAME).o

$(FILENAME).o: $(FILENAME).c
	$(CC) $(CFLAGS) -c $(FILENAME).c

clean:
	rm -f $(PROGS) *.o *~