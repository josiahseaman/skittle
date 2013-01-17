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
    
from SkittleCore.models import StatePacket
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

def checkForGreyscale(state):
    grayGraph = ['m', 'o', 's']
    return state.requestedGraph in grayGraph

def convertToPng(state, pixels):
    targetWidth = 1024
    greyscale = checkForGreyscale(state)
    print "GreyScale: ", greyscale
    f = open(state.getPngFilePath(), 'wb')
    if greyscale:
        p = multiplyGreyscale(pixels, 255)
        w = png.Writer(len(p[0]), len(p), greyscale=True)
    elif not isRasterGraph(state):   #Nucleotide Bias
        p = flattenImage(pixels, len(pixels[0]), True, 4)
        w = png.Writer(len(p[0])/4, len(p), greyscale=False, alpha=True)
    else: #raster, color graphs
        p = flattenImage(pixels, targetWidth)
        w = png.Writer(targetWidth, 64)
    w.write(f, p)
    f.close()
    data = open(state.getPngFilePath(), 'rb').read() #return the binary contents of the file
    return data

def flattenImage(pixels, targetWidth, isColored = True, nChannels = 3, depth = 0):
#    if isinstance(pixels, list) and len(pixels) != 0:
##        return map(lambda x: flattenImage(x, targetWidth, isColored, nChannels, depth+1), pixels) 
#        if isinstance(pixels[:1], list):#TODO: something about hasDepth
#            return reduce(lambda x,y: x + flattenImage(y, targetWidth, isColored, nChannels), pixels, [])
#        else:
#            return pixels
#    else:
#        return pixels
    pixels = squishImage(pixels)
    
    p = []
    newline = []
    for color in pixels:
        if color is None:
            newline += (0,) * nChannels
        elif nChannels == 4:
            newline += color 
            newline += (255,) #alpha
        else: 
            newline += color
        if len(newline) >= targetWidth * nChannels: 
            p.append(newline)
            newline = []
    return p

def squishImage(pixels):
    if isinstance(pixels, list):
        if isinstance(pixels[0], list):
            return reduce(lambda x,y: x + squishImage(y), pixels, [])
        else:
            return pixels
    else:
        return pixels

'''The main entry point for the whole Python logic SkittleCore module and Graphs.'''
def handleRequest(state):
    assert isinstance(state, StatePacket)
    state.start -= 1 #FIRST THING: start index at zero instead of one
    #Check to see if PNG exists
    png = None
#    if state.requestedGraph == 'n':
#        png = tryGetGraphPNG(state)
    #If it doesn't: grab pixel calculations
    if png is None:
        pixels = calculatePixels(state)
#        print pixels[:10]
        png = convertToPng(state, pixels)
    return png

def isRasterGraph(state):
    graphDescription = getGraphDescription(state)
    return graphDescription[3]

def multiplyGreyscale(p, greyMax = 255):
    for index, line in enumerate(p):
        p[index] = map(lambda x: int(max(x,0.0) * greyMax), line)
    return p
    
def getGraphDescription(state):
    targetGraphTuple = filter(lambda x: state.requestedGraph == x[0], availableGraphs)
    if targetGraphTuple:
        return targetGraphTuple[0] #return the first match
    else:
        return filter(lambda x: 'n' == x[0], availableGraphs)[0]
    
def tryGetGraphPNG(state):
    fileName = state.getPngFilePath()
    try:
        data = open(fileName, 'rb').read()
        print "Found cached file: ", fileName
        return data
    except:
        return None
    
def generateGraphListForServer():
    graphs = {}
    ServerDescription = namedtuple('ServerGraphDescription', ['name', 'rasterGraph'])
    for description in availableGraphs:
        graphs[description[0]] = ServerDescription(description[1], description[3])
    return graphs
        
        
        
    
