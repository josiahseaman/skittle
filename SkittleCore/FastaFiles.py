'''
Created on Dec 21, 2012
@author: Josiah
'''
from SkittleCore.models import RequestPacket
import DNAStorage.StorageRequestHandler as StorageRequestHandler

class FastaFile(str):
    def __init__(self, sequence = ''):
        if sequence is None:
            sequence = ''
        super(FastaFile, self).__init__(sequence)
    def __str__(self, *args, **kwargs):
        return super.__str__(self, *args, **kwargs)[:1000]



def readFile(state):
    assert isinstance(state, RequestPacket)
    seq = ''
    collection = []
    '''Id be happy to eliminate the need for accumulating this collection of lines'''
    try:
        filePath = StorageRequestHandler.GetFastaFilePath(state.specimen, state.chromosome, state.start)
        print "opening file " , filePath
        if not filePath:
            return None
        rawFile = open(filePath, 'r')
        print 'Opened File'

        for line in rawFile: # 1MB chunks at a time
            collection.append( line )
            
        seq = ''.join(collection)
        return seq #FastaFile(seq)
    
    except IOError:
        print "Couldn't open file.  Maybe it doesn't exist."
        return None
 

if __name__ == '__main__':
    state = RequestPacket()
#    state.filePath = 'bogus'
    f = readFile(state)
    print f
    
    