'''
Created on Dec 21, 2012

@author: Josiah
'''
from SkittleCore.models import StatePacket

class FastaFile(str):
    def __init__(self, sequence = ''):
        if sequence is None:
            sequence = ''
        self.seq = sequence
    def __str__(self, *args, **kwargs):
        return self.seq[:1000]



def readFile(state):
    assert isinstance(state, StatePacket)
    seq = ''
    collection = [] 
    '''Id be happy to eliminate the need for accumulating this collection of lines'''
    try:
        rawFile = open(state.filePath, 'r')
        print 'Opened File'
        rawFile.readline()#skip first line because it is not part of the sequence
        for line in rawFile: # 1MB chunks at a time
            collection.append( line .rstrip() .upper() )
            
        seq = ''.join(collection)
        return FastaFile(seq)
    
    except IOError:
        print "Couldn't open file.  Maybe it doesn't exist."
        return None
 

if __name__ == '__main__':
    state = StatePacket()
#    state.filePath = 'bogus'
    f = readFile(state)
    print f
    
    