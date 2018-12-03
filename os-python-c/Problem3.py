#Jeff Rix
#rixj@onid.oregonstate.edu
#CS311-400
#Homework2 Question 3
#Problem3.py
import os
import sys
import urllib2
import getopt

def main():
    #attempt to retrieve and parse command line arguments
    if len(sys.argv) == 3:
        url = sys.argv[1]
        file_name = sys.argv[2]
        #add http:// to url if omitted
	if url[:7]!='http://':
            url = 'http://' + url      
           
	#open url 
        try:
            url_contents = urllib2.urlopen(url)
        except:
            sys.exit('Error retrieving file')

	#copy contents url url to file        
        file = open(file_name, 'w')
        for row in url_contents:
            file.write(row)
     
        print 'Content of ' + url + ' copied to ' + file_name

    else:
        print "Error: Command line arguments are wrong. Enter url and file name"



#if we run this as a program this function will be called
if __name__ == '__main__':
    main()
