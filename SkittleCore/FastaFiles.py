'''
Created on Dec 21, 2012
@author: Josiah
'''
# from SkittleCore.models import RequestPacket
import DNAStorage.StorageRequestHandler as StorageRequestHandler


class FastaFile(str):
    def __init__(self, sequence=''):
        if sequence is None:
            sequence = ''
        super(FastaFile, self).__init__(sequence)

    def __str__(self, *args, **kwargs):
        return super.__str__(self, *args, **kwargs)[:1000]


def readFileOrNone(state):
    # assert isinstance(state, RequestPacket)
    try:
        filePath = StorageRequestHandler.GetFastaFilePath(state.specimen, state.chromosome, state.start)
        if state.scale < 100000:  # TODO: this is set really high right now for testing. Originally 10
            print "opening file ", filePath, " for ", state.specimen, state.chromosome, str(state.start)
        if not filePath:
            return None

        with open(filePath, 'r') as rawFile:
            result = rawFile.read()
            return result

    except IOError:
        print "Couldn't open file.  Maybe it doesn't exist."
        return None
