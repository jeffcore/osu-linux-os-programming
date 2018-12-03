#Jeff Rix
#rixj@onid.oregonstate.edu
#CS311-400
#Homework2 Question 5
#Problem5.py

import subprocess
import shlex
import sys


def main():
    command = 'who -bdlprTtu'
    #split the command line args
    command_sequence = shlex.split(command)
    #process the command
    try:
        who = subprocess.Popen(command_sequence, stdout=subprocess.PIPE)
    except:
        sys.exit('Error processing command')

    #save the output and errors
    stdout, stderr = who.communicate(command)
    
    #print results
    if stdout:
        print stdout
    #print error
    if stderr:
        print stderr

#if we run this as a program this function will be called
if __name__ == '__main__':
    main()