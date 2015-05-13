"""This is a simple test Brendan made when we were discussing chunk reading performance.  It is not used by
the program, but may come in handy in the future."""

import time
 
sample_chunk = "A" * 65536
numChunks = 60000
 
 
def nonesAndJoin():
    print("Method 1: list of strings, with \"\".join()")
    start = time.time()
 
 
    partialSequences = [None] * numChunks
 
    for index in range(numChunks):
        partialSequences[index] = sample_chunk
 
    seq = "".join(partialSequences)    
 
 
    elapsed = time.time() - start
    print(elapsed, len(seq))
    return elapsed


def testAppend():
    print("Method 2: str +=")
    start = time.time()
 
 
    seq = ""
 
    for index in range(numChunks):
        seq += sample_chunk
 
 
    elapsed = time.time() - start
    print(elapsed)
    return elapsed
 

def testNoNones():
    print("Method 3: ''.join([].append())")
    start = time.time()

    partialSequences = []

    for index in range(numChunks):
        partialSequences.append(sample_chunk)

    seq = "".join(partialSequences)

    elapsed = time.time() - start
    print(elapsed)
    return elapsed


joinTime = nonesAndJoin()
# appendTime = testAppend()
noNoneTime = testNoNones()
import sys
print (sys.version) 
# print ("Append is slower by a factor of", appendTime/joinTime)
print ("No Nones is faster by a factor of", joinTime/noNoneTime)
