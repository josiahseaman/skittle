'''
Created on Dec 21, 2012
@author: Josiah
'''
import sys
from collections import namedtuple
from time import sleep
from PngConversionHelper import convertToPng
from models import chunkSize

'''The set of availableGraphs is populated by the individual graph modules who are responsible for 
registering with the request Handler using the 'registerGraph' function below. '''
availableGraphs = set()
GraphDescription = namedtuple('GraphDescription',
                              ['symbol', 'name', 'moduleReference', 'rasterGraph', 'colorPaletteDependant',
                               'widthTolerance', 'isGrayScale', 'stretchy', 'helpText'])


def registerGraph(symbol, name, moduleName, rasterGraph=False, colorPaletteDependant=False, widthTolerance=0.15,
                  isGrayScale=False, stretchy=True, helpText=None):
    moduleReference = sys.modules[moduleName]
    availableGraphs.add(
        GraphDescription(symbol, name, moduleReference, rasterGraph, colorPaletteDependant, widthTolerance,
                         isGrayScale, stretchy, helpText))


from SkittleCore.models import RequestPacket, ProcessQueue
import DNAStorage.StorageRequestHandler as StorageRequestHandler
from django.db import transaction


def calculatePixels(state, settings=None):
    graphData = getGraphDescription(state)
    name, graphModule = graphData[1], graphData[2]

    results = []
    print "Calling ", name
    if settings is not None:
        results = graphModule.calculateOutputPixels(state, settings)
    else:
        results = graphModule.calculateOutputPixels(state)
    return results


def roundStartPosition(state):
    if (state.start - 1) % chunkSize == 0:
        return
    if (state.start) % chunkSize == 0:
        state.start += 1
        return
    state.start = int(state.start / chunkSize) * chunkSize + 1


'''The main entry point for the whole Python logic SkittleCore module and Graphs.'''


def handleRequest(state, settings=None):
    assert isinstance(state, RequestPacket)
    roundStartPosition(state)
    #Check to see if PNG exists
    png = None
    if state.requestedGraph not in ['h', ]:
        png = tryGetGraphPNG(state)
        #If it doesn't: grab pixel calculations
    if png is None and not isBeingProcessed(state):#TODO: handle same state different "settings" being separate computation
        #TODO: Handle beginProcess and finishProcess possible return of False
        beginProcess(state)
        pixels = calculatePixels(state, settings)
        png = convertToPng(state, pixels, isRasterGraph(state))
        finishProcess(state)
    elif isBeingProcessed(state):
        sleepTime = 2
        sleep(sleepTime) #This extra sleep command is here to prevent hammering the IsBeingProcessed database
        while isBeingProcessed(state):
            sleep(sleepTime)
        return handleRequest(state)
    print 'Done'
    return png


def isRasterGraph(state):
    graphDescription = getGraphDescription(state)
    return graphDescription[3]


def getGraphDescription(state):
    targetGraphTuple = filter(lambda x: state.requestedGraph == x[0], availableGraphs)
    if targetGraphTuple:
        return targetGraphTuple[0] #return the first match
    else:
        return filter(lambda x: 'n' == x[0], availableGraphs)[0]


def tryGetGraphPNG(state):
    fileName = StorageRequestHandler.GetPngFilePath(state)
    try:
        data = open(fileName, 'rb').read()
        print "Found cached file: ", fileName
        return data
    except:
        return None


def isBeingProcessed(request):
    #print "Checking if in process queue..."
    specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width

    process = ProcessQueue.objects.filter(Specimen=specimen, Chromosome=chromosome, Graph=graph, Start=start,
                                          Scale=scale, CharsPerLine=charsPerLine)

    transaction.enter_transaction_management()
    transaction.commit()

    if process:
    #print "We are still processing..."
    #        print process[0].Specimen
        return True
    else:
        #print "Processing is done!"
        return False


def beginProcess(request):
    if not isBeingProcessed(request):
        process = ProcessQueue()
        process.Specimen = request.specimen
        process.Chromosome = request.chromosome
        process.Graph = request.requestedGraph
        process.Start = request.start
        process.Scale = request.scale
        process.CharsPerLine = request.width
        process.save()
        return True
    else:
        return False


def finishProcess(request):
    if isBeingProcessed(request):
        specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width

        ProcessQueue.objects.filter(Specimen=specimen, Chromosome=chromosome, Graph=graph, Start=start, Scale=scale,
                                    CharsPerLine=charsPerLine).delete()
        return True
    else:
        return False


def GetRegisteredGraphsSymbols():
    symbols = []
    for graph in availableGraphs:
        symbols.append(graph[0])
    return symbols


class ServerSideGraphDescription():#TODO: I think this could be replaced with a dictionary
    def __init__(self, Name, IsRaster, colorSensitive, widthTolerance, stretchy, helpText):
        self.name = Name
        self.rasterGraph = IsRaster
        self.colorPaletteSensitive = colorSensitive
        self.widthTolerance = widthTolerance
        self.stretchy = stretchy
        self.helpText = helpText


def generateGraphListForServer():
    graphs = {}
    for description in availableGraphs:
        graphs[description[0]] = ServerSideGraphDescription(description[1], description[3], description[4],
                                                            description[5], description[7], description[8]).__dict__
    return graphs


'''These are here for the purposes of invoking the registerGraph call at the beginning of every graph definition file'''
graphNames = ['NucleotideBias', 'NucleotideDisplay', 'OligomerUsage', 'RawFrequencyMap',
              'RepeatMap', 'RepeatOverview', 'SimilarityHeatMap', 'ThreeMerDetector', 'PhotoGallery']
for name in graphNames:
    filename = 'SkittleCore.Graphs.' + name
    __import__(filename)
assert len(availableGraphs) == len(graphNames), "One or more of the graphs didn't import correctly. " + str(len(availableGraphs))

'''Finally, X = __import__('X') works like import X, with the difference that you
1) pass the module name as a string, and 2) explicitly assign it to a variable in your current namespace.'''
