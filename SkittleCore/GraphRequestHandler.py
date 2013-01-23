'''
Created on Dec 21, 2012
@author: Josiah
'''
import io
import sys
import png
from collections import namedtuple

'''The set of availableGraphs is populated by the individual graph modules who are responsible for 
registering with the request Handler using the 'registerGraph' function below. '''
availableGraphs = set()
GraphDescription = namedtuple('GraphDescription', ['symbol', 'name', 'moduleReference', 'rasterGraph'])

def registerGraph(symbol, name, moduleName, rasterGraph = False):
    moduleReference = sys.modules[moduleName]
    availableGraphs.add(GraphDescription(symbol, name, moduleReference, rasterGraph))
    
from SkittleCore.models import RequestPacket
import SkittleCore.FastaFiles as FastaFiles
import Graphs.AnnotationDisplay
import Graphs.NucleotideDisplay
import Graphs.NucleotideBias
import Graphs.RepeatMap
import Graphs.OligomerUsage
import Graphs.SequenceHighlighter
import Graphs.SimilarityHeatMap
import Graphs.ThreeMerDetector
from Graphs.SkittleGraphTransforms import hasDepth
from PngConversionHelper import convertToPng
import DNAStorage.StorageRequestHandler as StorageRequestHandler
'''Finally, X = __import__('X') works like import X, with the difference that you 
1) pass the module name as a string, and 2) explicitly assign it to a variable in your current namespace.'''

#print __name__, " Printing Available Graphs: "
#for graph in availableGraphs:
#    print graph 

def calculatePixels(state):
    sequence = FastaFiles.readFile(state)
    if sequence is None:
        raise IOError('Cannot proceed without sequence')
    state.seq = sequence
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
    if state.requestedGraph != 'm':
        png = tryGetGraphPNG(state)
    #If it doesn't: grab pixel calculations
    if png is None:
        pixels = calculatePixels(state)
#        print pixels[:10]
        png = convertToPng(state, pixels, isRasterGraph(state))
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
    def __init__(self, Name, IsRaster):
        self.name = Name
        self.rasterGraph = IsRaster    
    
def generateGraphListForServer():
    graphs = {}
    for description in availableGraphs:
        graphs[description[0]] = ServerSideGraphDescription(description[1], description[3]).__dict__
    return graphs
        
        
        
    
