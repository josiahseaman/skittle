'''
Created on Dec 21, 2012
@author: Josiah
'''
import io
import sys
import png
from collections import namedtuple
from time import sleep

'''The set of availableGraphs is populated by the individual graph modules who are responsible for 
registering with the request Handler using the 'registerGraph' function below. '''
availableGraphs = set()
GraphDescription = namedtuple('GraphDescription', ['symbol', 'name', 'moduleReference', 'rasterGraph','colorPalletteDependant', 'widthTolerance'])

def registerGraph(symbol, name, moduleName, rasterGraph = False, colorPalletteDependant = False, widthTolerance=0.15):
    moduleReference = sys.modules[moduleName]
    availableGraphs.add(GraphDescription(symbol, name, moduleReference, rasterGraph, colorPalletteDependant, widthTolerance))
    
from SkittleCore.models import RequestPacket, ProcessQueue
import SkittleCore.FastaFiles as FastaFiles
import Graphs.AnnotationDisplay
import Graphs.NucleotideDisplay
import Graphs.NucleotideBias
import Graphs.RepeatMap
import Graphs.OligomerUsage
import Graphs.SequenceHighlighter
import Graphs.SimilarityHeatMap
import Graphs.ThreeMerDetector
import Graphs.SNPdisplay
import Graphs.RepeatOverview
from Graphs.SkittleGraphTransforms import countDepth
from PngConversionHelper import convertToPng
import DNAStorage.StorageRequestHandler as StorageRequestHandler
'''Finally, X = __import__('X') works like import X, with the difference that you 
1) pass the module name as a string, and 2) explicitly assign it to a variable in your current namespace.'''

#print __name__, " Printing Available Graphs: "
#for graph in availableGraphs:
#    print graph 

def calculatePixels(state):    
    graphData = getGraphDescription(state)
    name, graphModule = graphData[1], graphData[2]
#    activeSet = state.getActiveGraphs()
    settings = None #activeSet[name]
    results = []
    print "Calling ", name
    if settings is not None:
        results = graphModule.calculateOutputPixels(state, settings)    
    else:
        results = graphModule.calculateOutputPixels(state)
    return results

'''The main entry point for the whole Python logic SkittleCore module and Graphs.'''
def handleRequest(state):
    assert isinstance(state, RequestPacket)
    #Check to see if PNG exists
    png = None
    if state.requestedGraph not in ['h', ]:
        png = tryGetGraphPNG(state)
    #If it doesn't: grab pixel calculations
    if png is None and not isBeingProcessed(state):
        beginProcess(state)
        pixels = calculatePixels(state)
#        print pixels[:10]
        print "Saving to width =", state.width
        png = convertToPng(state, pixels, isRasterGraph(state))
        finishProcess(state)
    elif isBeingProcessed(state):
        sleepTime = 1
        sleep(sleepTime) #This extra sleep command is here to prevent hammering the IsBeingProcessed database
        while isBeingProcessed(state):
            sleepTime = sleepTime * 2
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

class ServerSideGraphDescription():
    def __init__(self, Name, IsRaster, colorSensitive, widthTolerance):
        self.name = Name
        self.rasterGraph = IsRaster
        self.colorPaletteSensitive = colorSensitive
        self.widthTolerance = widthTolerance
    
def generateGraphListForServer():
    graphs = {}
    for description in availableGraphs:
        graphs[description[0]] = ServerSideGraphDescription(description[1], description[3], description[4], description[5]).__dict__
    return graphs
        
def isBeingProcessed(request):
    assert isinstance(request, RequestPacket)
    specimen, chromosome, graph, start, scale, charsPerLine = request.specimen, request.chromosome, request.requestedGraph, request.start, request.scale, request.width
        
    process = ProcessQueue.objects.filter(Specimen = specimen, Chromosome = chromosome, Graph = graph, Start = start, Scale = scale, CharsPerLine = charsPerLine)[:1]
        
    if process:
        return True
    else:
        return False
            
def beginProcess(request):
    if not IsBeingProcessed(request):
        process = ProcessQueue(Specimen = request.specimen, Chromosome = request.chromosome, Graph = request.requestedGraph, Start = request.start, Scale = request.scale, CharsPerLine = request.width)
        process.save()
        return True
    else:
        return False
        
def finishProcess(request):
    if IsBeingProcessed(request):
        process = ProcessQueue.objects.filter(Specimen = specimen, Chromosome = chromosome, Graph = graph, Start = start, Scale = scale, CharsPerLine = charsPerLine).delete()
        return True
    else:
        return False
        
    
