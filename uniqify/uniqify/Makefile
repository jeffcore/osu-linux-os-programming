#  Jeff Rix
#  rixj@onid.oregonstate.edu
#  CS311-400
#  Homework 5 Problem 3
#  uniqify

CC=gcc
DEBUG=-g
C99=-std=c99
WARNINGS= -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations \
	 -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
CFLAGS=$(DEBUG) $(C99) $(WARNINGS)
FILENAME1=uniqify
PROGS=uniqify


all: $(PROGS)

$(PROGS): $(FILENAME1).o
	$(CC) $(CFLAGS) -o $(PROGS) $(FILENAME1).o

$(PROGS).o: $(FILENAME1).c
	$(CC) $(CFLAGS) -c $(FILENAME1).c
	
	
testsimple:
	rm -f myu.txt bashu.txt
	uniqify -n 10 < simple.txt > myu.txt
	bash uniquify.bash < simple.txt > bashu.txt
	diff myu.txt bashu.txt	
cat:
	cat myu.txt bashu.txt	
	make 
testdecl:
	rm -f myu.txt bashu.txt
	uniqify -n 10 < decl-indep.txt > myu.txt
	bash uniquify.bash < decl-indep.txt > bashu.txt
	diff myu.txt bashu.txt	
testiliad:
	rm -f myu.txt bashu.txt
	uniqify -n 10 < iliad.txt > myu.txt
	bash uniquify.bash < iliad.txt > bashu.txt
	diff myu.txt bashu.txt	
testwords:
	rm -f myu.txt bashu.txt
	uniqify -n 10 < words.txt > myu.txt
	bash uniquify.bash < words.txt > bashu.txt
	diff myu.txt bashu.txt
test:
	rm -f myu.txt bashu.txt
	uniqify -n 5 < websters.txt > myu.txt
	bash uniquify.bash < websters.txt > bashu.txt
	diff myu.txt bashu.txt	

testall: testsimple testiliad testdecl testwords test
	rm -f myu.txt bashu.txt
testsmall: testsimple testiliad testdecl testwords 
	rm -f myu.txt bashu.txt
clean:
	rm -f $(PROGS) *.o *~ 

