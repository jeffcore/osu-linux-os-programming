#Problem4.py
#Question 4
#Jeff Rix 
#rixj@onid.oregonstate.edu
#CS311-400
#Homework 2
import sys

def main():
    #reteive number from command line argument
    
    if len(sys.argv) == 2:
        number = int(sys.argv[1])
    else:
        sys.exit('You did not supply a number as a command line argument')

    #first prime number
    start_prime = 2
    #count the number of primes found
    start_num = 0
    #used to store the current prime number
    the_prime_number = 2

    #loop until we have found the correct number of primes based on user input
    #when a prime is found the start_num variable is incremented
    while(start_num < number):
        if is_prime(start_prime):
            start_num += 1
            the_prime_number = start_prime
        start_prime += 1

    print '%.2f' %(the_prime_number)

def is_prime(number):
    '''(number) -> boolean
    
    Return the True or False based on if number is prime
    
    >>> is_prime(5)
    returns True
    '''
    is_prime = True
    start_num = 2
      
    multiply_num = int(number ** (.5)) + 1
  
    #set up first loop for the number being multiplied by the array contents(index)
    for i in range(start_num, multiply_num):
        #set up second loop to loop through array starting at index position 2
        if number % i == 0:
            is_prime = False

    return is_prime

#if we run this as a program this function will be called
if __name__ == '__main__':
    main()