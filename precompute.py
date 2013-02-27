#!/usr/bin/env python
import os
import sys
from collections import namedtuple
from multiprocessing import Pool



def computeRequest(request):
    
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    from SkittleCore import GraphRequestHandler
    from DNAStorage import StorageRequestHandler
    
    specimen = request[0]
    chromosomes = StorageRequestHandler.GetRelatedChromosomes(specimen)
    for chromosome in chromosomes:
        length = StorageRequestHandler.GetChromosomeLength(specimen, chromosome)
        chunks = range(1, length+1, 2**16)
        for targetIndex in range(request[2], len(chunks), request[1]):
            start = chunks[targetIndex]
            state = makePacket(specimen, chromosome, start)
            
            print "Computing: ", state.specimen, state.chromosome, state.start    
            GraphRequestHandler.handleRequest(state)
            print "Done computing ", state.specimen, state.chromosome, state.start

def makePacket(specimen, chromosome, start):
    from SkittleCore import models  
    from SkittleCore.Graphs import RepeatMap
    
    state = models.RequestPacket()
    state.specimen = specimen
    state.chromosome = chromosome
    state.start = start
    state.scale = 1
    state.width = RepeatMap.skixelsPerSample
    state.requestedGraph = 'm'
    return state

if __name__ == "__main__":
        
    nProcessors = 6
    specimen = sys.argv[1]
#    ProcessorRequest = namedtuple('ProcessorRequest', ['specimen', 'nProcessors', 'PID'])
    requests = [(specimen, nProcessors, PID) for PID in range(nProcessors)]
    processors = Pool(nProcessors)
    processors.map(computeRequest, requests)
    