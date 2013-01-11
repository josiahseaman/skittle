'''
Created on Dec 21, 2012
@author: Josiah
'''
import io
import sys
import png

'''The set of availableGraphs is populated by the individual graph modules who are responsible for 
registering with the request Handler using the 'registerGraph' function below. '''
availableGraphs = set()

def registerGraph(name, moduleName, rasterGraph = True):
    moduleReference = sys.modules[moduleName]
    availableGraphs.add((name, moduleReference, rasterGraph))
    
from SkittleCore.models import StatePacket
import SkittleCore.FastaFiles as FastaFiles
import Graphs.NucleotideDisplay
import Graphs.NucleotideBias
import Graphs.RepeatMap
import Graphs.OligomerUsage
import Graphs.SimilarityHeatMap
import Graphs.ThreeMerDetector
import Graphs.SequenceHighlighter
'''Finally, X = __import__('X') works like import X, with the difference that you 
1) pass the module name as a string, and 2) explicitly assign it to a variable in your current namespace.'''


print __name__, " Printing Available Graphs: "
for graph in availableGraphs:
    print graph 

def calculatePixels(state):
    sequence = FastaFiles.readFile(state)
    if sequence is None:
        raise IOError('Cannot proceed without sequence')
    state.seq = sequence
    name, graphModule = parseActiveGraphString(state)
#    activeSet = state.getActiveGraphs()
    settings = None #activeSet[name]
    results = []
    print "Calling ", name
    if settings is not None:
        results = graphModule.calculateOutputPixels(state, settings)    
    else:
        results = graphModule.calculateOutputPixels(state)
    return results

def convertToPng(state, pixels):
    greyscale = False
    if greyscale:
        f = open('output.png', 'wb')      # binary mode is important
        w = png.Writer(255, 1, greyscale=True)
        w.write(f, [range(256)])
    else:
        p = []
        newline = []
        for color in pixels:
            newline += color
            if len(newline) >= 1024 * 3: #assumes 3 channels
                p.append(newline)
                newline = []

        f = open(state.getPngFilePath(), 'wb')
        w = png.Writer(1024, 64)
        w.write(f, p)
    f.close()
    f = open(state.getPngFilePath(), 'rb').read() #return the binary contents of the file
    return f


'''The main entry point for the whole Python logic SkittleCore module and Graphs.'''
def handleRequest(state):
    state.start -= 1 #FIRST THING: start index at zero instead of one
    #Check to see if PNG exists
    png = tryGetGraphPNG(state)
    #If it doesn't: grab pixel calculations
    if png is None:
        pixels = calculatePixels(state)
        png = convertToPng(state, pixels)
    return png

    
def parseActiveGraphString(state):
    characterAliases = {
        'n':("Nucleotide Display", Graphs.NucleotideDisplay),
        'b':("Nucleotide Bias", Graphs.NucleotideBias),
        'm':("Repeat Map", Graphs.RepeatMap),
#        'c':"Alignment Cylinder",
#        'r':"Repeat Overview",
        'o':("Oligomer Usage", Graphs.OligomerUsage),
        's':("Similarity Heatmap", Graphs.SimilarityHeatMap),
        't':("Threemer Detector", Graphs.ThreeMerDetector),
        'h':("Sequence Highlighter", Graphs.SequenceHighlighter)}
    name, graphModule = characterAliases[state.requestedGraph]
    return name, graphModule
    
    
    
def tryGetGraphPNG(state):
    fileName = state.getPngFilePath()
    print "Looking for cache: ", fileName
    try:
        data = open(fileName, 'rb').read()
        print "Found cached file: ", fileName
        return data
    except:
        return None
    
    
