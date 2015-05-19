#!/usr/bin/env python
import os
import sys
from multiprocessing import Pool


def lockAndWriteFile(start, answerTuple):
    succeeded = False
    while not succeeded:
        try:
            f = open('results.csv', 'a')
            data = str(answerTuple).replace('(', '').replace(')', '').replace('[', '').replace(']', '')
            f.write(str(start) + ',' + data + '\n')
            f.close()
            succeeded = True
        except:
            succeeded = False


def outputThreemerNormalization(request):
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    from SkittleCore.Graphs import ThreeMerDetector
    from SkittleCore.Graphs.SkittleGraphTransforms import getChunkStart

    specimen = request[0]
    chromosomes = ['chr2']
    for chromosome in chromosomes:
        chunks = [getChunkStart(x) for x in range(7077889, 11610965, 2 ** 16)] #range(16518287, 20987087, 2**16)
        for targetIndex in range(len(chunks)):
            start = chunks[targetIndex]
            widths = range(10, 500, 20)
            for widthIndex in range(request[2], len(widths), request[1]):
                width = widths[widthIndex]
                state = makeRequestPacket(specimen, chromosome, start)
                state.width = width
                state.requestedGraph = 't'
                print "Computing: ", state.specimen, state.chromosome, state.start, state.width
                #                state.readAndAppendNextChunk()
                #                GraphRequestHandler.handleRequest(state)
                answerTuple = ThreeMerDetector.calculateOutputPixels(state)
                lockAndWriteFile(state.start, answerTuple)
                print "Done computing ", state.specimen, state.chromosome, state.start, state.width


def precomputeRepeatMap(request):
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    from SkittleCore import GraphRequestHandler
    from DNAStorage import StorageRequestHandler

    specimen = request[0]
    dbSpecimen = StorageRequestHandler.GetSpecimen(specimen)
    chromosomes = StorageRequestHandler.GetRelatedChromosomes(dbSpecimen)
    for chromosome in chromosomes:
        length = StorageRequestHandler.GetChromosomeLength(specimen, chromosome)
        chunks = range(1, length + 1, 2 ** 16)
        for targetIndex in range(request[2], len(chunks), request[1]):
            start = chunks[targetIndex]
            state = makeRequestPacket(specimen, chromosome, start, graphSymbol='m')

            print "Computing: ", state.specimen, state.chromosome, state.start
            GraphRequestHandler.handleRequest(state)
            print "Done computing ", state.specimen, state.chromosome, state.start


def makeRequestPacket(specimen, chromosome, start, graphSymbol='m', scale=1):
    from SkittleCore import models
    from SkittleCore.Graphs.models import RepeatMapState

    state = models.RequestPacket()
    state.specimen = specimen
    state.chromosome = chromosome
    state.start = start
    state.scale = scale
    state.width = RepeatMapState.skixelsPerSample
    state.requestedGraph = graphSymbol
    return state


def startThreemer(specimen, nProcessors):
    requests = [(specimen, nProcessors, PID) for PID in range(nProcessors)]
    processors = Pool(nProcessors)
    processors.map(outputThreemerNormalization, requests)


def startRepeatMap(specimen, nProcessors):
#    ProcessorRequest = namedtuple('ProcessorRequest', ['specimen', 'nProcessors', 'PID'])
    requests = [(specimen, nProcessors, PID) for PID in range(nProcessors)]
    processors = Pool(nProcessors)
    processors.map(precomputeRepeatMap, requests)


def benchmarkHere(request):
    precomputeAnyGraph(request)


def precomputeAnyGraph(request):
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    from SkittleCore import GraphRequestHandler
    from DNAStorage import StorageRequestHandler

    specimen = request[2]

    #    chromosomes = StorageRequestHandler.GetRelatedChromosomes(specimen)
    for chromosome in ['chrY']: #chromosomes:
        length = StorageRequestHandler.GetChromosomeLength(specimen, chromosome)
        chunks = [1] #range(1, length+1, 2**16)[-7:]
        for targetIndex in range(request[1], len(chunks),
                                 request[0]):#this loop divies up the jobs by PID according to modulo nProcessors
            start = chunks[targetIndex]
            state = makeRequestPacket(specimen, chromosome, start, request[3], request[4])

            print "Computing: ", state.specimen, state.chromosome, state.start
            GraphRequestHandler.handleRequest(state)
            print "Done computing ", state.specimen, state.chromosome, state.start


def allGraphs(specimen, nProcessors):
    processors = Pool(nProcessors)
    for graphSymbol in ['n', 'm', 'r', 'o', 'b', 'h', 't']:
        for scale in [1, 16]:#scales we'd like to test
            requests = [(nProcessors, PID, specimen, graphSymbol, scale) for PID in range(nProcessors)]
            processors.map(benchmarkHere, requests)


if __name__ == "__main__":
    if len(sys.argv) >= 3:
        nProcessors = int(sys.argv[2])
    else:
        nProcessors = 3
    specimen = sys.argv[1]
    print "Specimen: ", specimen, "Processors: ", str(nProcessors)


    #    startThreemer(specimen, nProcessors)
    startRepeatMap(specimen, nProcessors)
#    allGraphs(specimen, nProcessors)
    
    
    
            
            
            
            
            
            
            
            
            
            
            
            
            
    