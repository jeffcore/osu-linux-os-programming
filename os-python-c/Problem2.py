#Jeff Rix
#rixj@onid.oregonstate.edu
#CS311-400
#Homework2 Question 2
#Problem2.py
import os
import sys
import getopt

def main():
    #attempt to retrieve and parse command line arguments
    try:
        opts, arg = getopt.getopt(sys.argv[1:], "t:c:",["term=","class="])
    except getopt.GetoptError as err:
        print 'Error with Command Line Arguments: ' + str(err)
        sys.exit(0)

    #variable to store and build directory path
    start_path = os.getenv("HOME") + '/'
    #double check to make sure we have two command line arguments
    if len(opts) == 2:
        #loop through command line argument array to retrieve term and class
        term_dir = ''
        class_dir = ''

        for data in opts:
            if data[0] in ('-t','--term'):
                term_dir = data[1] + '/'
            elif data[0] in ('-c','--class'):
                class_dir = data[1] + '/'
        #build path and start creating directories
        if term_dir and class_dir:	
	    start_path += term_dir
            if os.path.exists(start_path) == False:
                os.mkdir(start_path)
	    else:
		print 'Directory ' + term_dir + ' already exists. It was not created.'

	    start_path += class_dir    
            if os.path.exists(start_path) == False:
                os.mkdir(start_path)
            else:
                print 'Directory ' + class_dir + ' already exists. It was not created.'
        else:
            sys.exit('Error with Command Line Arguments')

        #array to hold directories to create
        directories = ['assignments', 'examples', 'exams', 'lecture_notes', 'submissions']
        #create directories
        for dir in directories:
            if(os.path.exists(start_path + dir)) == False:
                os.mkdir(start_path + dir)
            else:
		print 'Directory ' + dir + ' already exists. It was not created.'


        #change path to directory inside /term/course/
        os.chdir(start_path)
        #create symbolic links
	try:
            os.symlink('/usr/local/classes/eecs/winter2014/cs311-400/src/README', 'README')
        except:
	    print 'Error creating symlink README. It might already exist'
	try:
       	    os.symlink('/usr/local/classes/eecs/winter2014/cs311-400/src', 'src_class')
	except:
	    print 'Error creating symlink src_class. It might already exist'

    else:
        sys.exit('Error Did Not Submit Proper Amount of Arguments')

    print 'Program is done running'

#if we run this as a program this function will be called
if __name__ == '__main__':
    main()
