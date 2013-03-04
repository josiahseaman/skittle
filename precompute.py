#!/usr/bin/env python
import os
import sys
from collections import namedtuple
from multiprocessing import Pool

def lockAndWriteFile(w,a,m):
    succeeded = False
    while not succeeded:
        try:
            f = open('results.csv', 'a')
            f.write(str(w) + ',' + str(a)+ ',' + str(m) + '\n')
            f.close()
            succeeded = True
        except: succeeded = False

def outputThreemerNormalization(request):
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    from SkittleCore import GraphRequestHandler
    from SkittleCore.Graphs import ThreeMerDetector
    from SkittleCore.Graphs.SkittleGraphTransforms import getChunkStart
    
    specimen = request[0]
    chromosomes = ['chr2']
    for chromosome in chromosomes:
        chunks = [getChunkStart(x) for x in range(16518287, 20987087, 2**16)] 
        for targetIndex in range(len(chunks)):
            start = chunks[targetIndex]
            widths = range(10,500,20)
            for widthIndex in range(request[2], len(widths), request[1]):
                width = widths[widthIndex]
                state = makeRequestPacket(specimen, chromosome, start)
                state.width = width
                state.requestedGraph = 't'
                print "Computing: ", state.specimen, state.chromosome, state.start, state.width    
#                state.readAndAppendNextChunk()
                GraphRequestHandler.handleRequest(state)
#                (w, a, m) = ThreeMerDetector.calculateOutputPixels(state)
#                lockAndWriteFile(w,a,m)
                print "Done computing ", state.specimen, state.chromosome, state.start, state.width



def precomputeRepeatMap(request):
    
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
            state = makeRequestPacket(specimen, chromosome, start)
            
            print "Computing: ", state.specimen, state.chromosome, state.start    
            GraphRequestHandler.handleRequest(state)
            print "Done computing ", state.specimen, state.chromosome, state.start

def makeRequestPacket(specimen, chromosome, start):
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
    if len(sys.argv) >= 3: nProcessors = int(sys.argv[2])     
    else: nProcessors = 6
    specimen = sys.argv[1]
#    ProcessorRequest = namedtuple('ProcessorRequest', ['specimen', 'nProcessors', 'PID'])
    requests = [(specimen, nProcessors, PID) for PID in range(nProcessors)]
    
    processors = Pool(nProcessors)
    processors.map(precomputeRepeatMap, requests)
#    processors.map(outputThreemerNormalization, requests)
#    outputThreemerNormalization( requests[0])
    