#!/usr/bin/python
import os
import sys
from multiprocessing import Pool

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
from SkittleCore import GraphRequestHandler
from DNAStorage import StorageRequestHandler
from SkittleCore import models  
from SkittleCore.Graphs import RepeatMap


def computeRequest(state):
    print "Computing: ", state.specimen, state.chromosome, state.start    
    GraphRequestHandler.handleRequest(state)
    print "Done computing ", state.specimen, state.chromosome, state.start

def makePacket(specimen, chromosome, start):
    state = models.RequestPacket()
    state.specimen = specimen
    state.chromosome = chromosome
    state.start = start
    state.scale = 1
    state.width = RepeatMap.skixelsPerSample
    state.requestedGraph = 'm'
    return state

if __name__ == "__main__":
        
    specimen = sys.argv[1]
    chromosomes = StorageRequestHandler.GetRelatedChromosomes(specimen)
    for chromosome in chromosomes:
        length = StorageRequestHandler.GetChromosomeLength(specimen, chromosome)
        chunks = range(1, length+1, models.chunkSize)
        requests = map(lambda start: makePacket(specimen, chromosome, start), chunks)
        processors = Pool(6)
        
        processors.map(computeRequest, requests)