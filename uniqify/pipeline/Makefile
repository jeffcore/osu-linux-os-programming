#  Jeff Rix
#  rixj@onid.oregonstate.edu
#  CS311-400
#  Homework 5 Problem 2
#  pipeline.c

CC=gcc
DEBUG=-g
C99=-std=c99
WARNINGS= -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations \
	-Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
CFLAGS=$(DEBUG) $(C99) $(WARNINGS)
FILENAME1=pipeline
PROGS=pipeline


all: $(PROGS)

$(PROGS): $(FILENAME1).o
	$(CC) $(CFLAGS) -o $(PROGS) $(FILENAME1).o

$(PROGS).o: $(FILENAME1).c
	$(CC) $(CFLAGS) -c $(FILENAME1).c
	
testsimple1:
	rm -f outfile*.txt
	bash pipeline.bash -f outfile1b.txt < simple1.txt > outfile2b.txt
	pipeline -f outfile1p.txt < simple1.txt > outfile2p.txt
	diff outfile1b.txt outfile1p.txt 
	diff outfile2b.txt outfile2p.txt 
	cat outfile2b.txt outfile2p.txt 
testsimple2:
	rm -f outfile*.txt
	bash pipeline.bash -f outfile1b.txt < simple2.txt > outfile2b.txt
	pipeline -f outfile1p.txt < simple2.txt > outfile2p.txt
	diff outfile1b.txt outfile1p.txt 
	diff outfile2b.txt outfile2p.txt 
	cat outfile2b.txt outfile2p.txt 	
testwords:
	rm -f outfile*.txt
	bash pipeline.bash -f outfile1b.txt < /usr/local/classes/eecs/winter2014/cs311-400/src/Homework5/Problem2/words.txt > outfile2b.txt
	pipeline -f outfile1p.txt < /usr/local/classes/eecs/winter2014/cs311-400/src/Homework5/Problem2/words.txt > outfile2p.txt
	diff outfile1b.txt outfile1p.txt
	diff outfile2b.txt outfile2p.txt	
testall: testsimple1 testsimple2 testwords

test:
	rm -f outfile*
	bash pipeline.bash -f outfile2b < /usr/local/classes/eecs/winter2014/cs311-400/src/Homework5/Problem2/words.txt > outfile1b
	pipeline -f outfile2p < /usr/local/classes/eecs/winter2014/cs311-400/src/Homework5/Problem2/words.txt > outfile1p
	diff outfile1b outfile1p

clean:
	rm -f $(PROGS) *.o *~ outfile*

