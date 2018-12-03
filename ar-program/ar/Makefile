#  Jeff Rix
#  rixj@onid.oregonstate.edu
#  CS311-400
#  Homework 4 Problem 4

CC=gcc
DEBUG=-g

CFLAGS=$(DEBUG) -Wall
FILENAME1=myar
PROGS=myar

#test macros
ARQ12345=ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
ARQ135=ar q ar135.ar 1-s.txt 3-s.txt 5-s.txt
ARQ24=ar q ar24.ar 2-s.txt 4-s.txt 
MYARQ12345=myar -q myar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
MYARQ135=myar -q myar135.ar 1-s.txt 3-s.txt 5-s.txt
MYARQ24=myar -q myar24.ar 2-s.txt 4-s.txt 


all: $(PROGS)

$(PROGS): $(FILENAME1).o
	$(CC) $(CFLAGS) -o $(FILENAME1) $(FILENAME1).o

$(PROGS).o: $(FILENAME1).c ar.h
	$(CC) $(CFLAGS) -c $(FILENAME1).c

	
testq12345:
	rm -f ar12345.ar myar12345.ar 
	$(ARQ12345)
	$(MYARQ12345)
	diff ar12345.ar myar12345.ar
testq135:
	rm -f ar135.ar myar135.ar 
	$(ARQ135)
	$(MYARQ135)
	diff ar135.ar myar135.ar
testq24:
	rm -f ar24.ar myar24.ar 
	$(ARQ24)
	$(MYARQ24)
	diff ar24.ar myar24.ar
testq: testq12345 testq135 testq24
testt12345:
	rm -f ar12345.ar
	$(ARQ12345)
	ar t ar12345.ar > ar-ctoc.txt 
	myar -t ar12345.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt
testt135:
	rm -f ar135.ar
	$(ARQ135)
	ar t ar135.ar > ar-ctoc.txt 
	myar -t ar135.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt
testt24:
	rm -f ar24.ar
	$(ARQ24)
	ar t ar24.ar > ar-ctoc.txt 
	myar -t ar24.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt
testt: testt12345 testt135 testt24
testv12345:
	rm -f ar12345.ar
	$(ARQ12345)
	ar tv ar12345.ar > ar-vtoc.txt 
	myar -v ar12345.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt
testv135:
	rm -f ar135.ar
	$(ARQ135)
	ar tv ar135.ar > ar-vtoc.txt 
	myar -v ar135.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt
testv24:
	rm -f ar24.ar
	$(ARQ24)
	ar tv ar24.ar > ar-vtoc.txt 
	myar -v ar24.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt
testv: testv12345 testv135 testv24
tests: testq testt testv
#mytests
testxall:
	rm -f 1.ar
	myar -q 1.ar 1.txt 2.txt	
	pico 1.txt
	myar -q 1.ar 1.txt 
	pico 1.ar
	myar -x 1.ar 1.txt
	pico 1.txt
	pico 2.txt
testx2:
	rm -f 1.ar
	myar -q 1.ar 1.txt 2.txt	
	pico 1.txt		
	pico 2.txt
	myar -x 1.ar 2.txt	
	pico 1.txt
	pico 2.txt
testd1:
	rm -f 1.ar dopt1.txt dopt2.txt
	myar -q 1.ar 1.txt	
	less 1.ar > dopt1.txt
	myar -q 1.ar 2.txt		
	myar -d 1.ar 2.txt
	less 1.ar > dopt2.txt
	diff dopt1.txt dopt2.txt	
testd2:
	rm -f 1.ar dopt1.txt dopt2.txt
	myar -q 1.ar 2.txt 1.txt	
	less 1.ar > dopt1.txt
	rm -f 1.ar
	myar -q 1.ar 1.txt 2.txt 1.txt		
	myar -d 1.ar 1.txt
	less 1.ar > dopt2.txt
	diff dopt1.txt dopt2.txt	
testdm1:
	rm -f 1.ar
	myar -q 1.ar 1.txt 2.txt 1.txt	
	less 1.ar
	myar -d 1.ar 1.txt
	pico 1.ar
copy:
	cp myar test/	
clean:
	rm -f $(PROGS) *.o *~ *.ar *toc.txt