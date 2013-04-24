'''Created on Apr 17, 2013
@author: Josiah'''
from models import RequestPacket, chunkSize
import FastaFiles


def getSearchSequenceFromRequestPacket(state, searchStart, searchStop):
    assert isinstance(state, RequestPacket)
    #    searchStop = min( searchStop, searchStart + 30)
    chunkStart = int((searchStart - 1) / chunkSize) * chunkSize + 1 #this rounds down to the nearest chunk boundary
    chunkStop = int((searchStop - 1) / chunkSize) * chunkSize + 1
    #    print chunkStart, " : ", chunkStop,

    state.start = chunkStart
    state.seq = FastaFiles.readFile(state)
    if chunkStop != state.start:
        state.readAndAppendNextChunk()

    searchSeq = ''
    try:
        searchSeq = state.seq[searchStart - state.start: searchStop - state.start]
    except:
        pass
    return searchSeq
    
