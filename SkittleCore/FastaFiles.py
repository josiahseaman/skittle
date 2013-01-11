'''
Created on Dec 21, 2012

@author: Josiah
'''
from SkittleCore.models import StatePacket

class FastaFile(str):
    def __init__(self, sequence = ''):
        if sequence is None:
            sequence = ''
        super(FastaFile, self).__init__(sequence)
    def __str__(self, *args, **kwargs):
        return super.__str__(self, *args, **kwargs)[:1000]



def readFile(state):
    assert isinstance(state, StatePacket)
    seq = ''
    collection = []
    '''Id be happy to eliminate the need for accumulating this collection of lines'''
    try:
        state.calculateFilePath()
        print "opening file " , state.filePath
        rawFile = open(state.filePath, 'r')
        print 'Opened File'

        for line in rawFile: # 1MB chunks at a time
            collection.append( line )
            
        seq = ''.join(collection)
        return seq #FastaFile(seq)
    
    except IOError:
        print "Couldn't open file.  Maybe it doesn't exist."
        return None
 

if __name__ == '__main__':
    state = StatePacket()
#    state.filePath = 'bogus'
    f = readFile(state)
    print f
    
    