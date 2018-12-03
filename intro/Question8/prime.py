#prime.py
#Question 8
#Jeff Rix 
#rixj@onid.oregonstate.edu
#CS311-400
#Homework 1

def prime_numbers(value):
    '''(number) -> number, array
    
    Return the total number of primes and an array of primes
    based on numbers below value provided
        
    >>> prime_numbers(5)
    returns total number of primes - 2
            array of the primes - [2,3]        
    '''
    #create the array to hold the initial number associated to the index
    #  of an array, with setting all indexes to True
    num_array = [True] * value
    #set index 0 and 1 to False since they are not primes
    num_array[0] = num_array[1] = False
    #start number use to set start index when preforming multiplication
    start_num = 2
    #get max number that will be multiplied by index numbers in array
    multiply_num = int(value ** (.5)) + 1
    print str(multiply_num)
  
    #set up first loop for the number being multiplied by the array contents(index)
    for i in range(start_num, multiply_num):
        #set up second loop to loop through array starting at index position 2
        for x in range(start_num, len(num_array)):
            #multiply index position by mutiply number ( i )
            if i*x <= (len(num_array) - 1):
                #if number is in the valid range change index to False (not prime)
                if num_array[i*x]:
                    num_array[i*x] = False

    #create new array to hold the prime numbers and use in return statement
    prime_array = []
    #loop through prime number array and add actual values of array to new prime_array
    for i in range(0, len(num_array)):
        #if index posoition is false add index number to new array
        if num_array[i]:
            prime_array.append(i)
   
    return (len(prime_array), prime_array)

print 'Show a list of prime numbers under the entered value'
input = int(raw_input('Enter a number (3 or greater): '))

prime_total, prime_array = prime_numbers(input)

print 'There are ' +  str(prime_total) + ' prime number(s) under ' + str(input)
print 'List of the Prime(s): ' + str(prime_array)
