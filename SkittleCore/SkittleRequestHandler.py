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

def registerGraph(name, moduleName):
    moduleReference = sys.modules[moduleName]
    availableGraphs.add((name, moduleReference))
    
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
    print sequence
    state.seq = sequence
    state.start -= 1 #start index at zero instead of one
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

def convertStateToFileName(state):
    return None#TODO: implement

def convertToPng(pixels):
    greyscale = False
    if greyscale:
        f = open('output.png', 'wb')      # binary mode is important
        w = png.Writer(255, 1, greyscale=True)
        w.write(f, [range(256)])
    else:
        p = []
#        map(lambda x: (*x), pixels) #[ pixels[x]* for x in ]
        
        for line in pixels:
            newline = []
            for color in line:
                newline += color
            p.append(newline)  
        f = open('output.png', 'wb')
        w = png.Writer(1024, 64)
        w.write(f, p)
    f.close()
    f = open('output.png', 'rb').read() #return the binary contents of the file
    return f

def handleRequest(state):
    #Check to see if PNG exists
    png = tryGetGraphPNG(state)
    #If it doesn't: grab pixel calculations
    if not png:
        pixels = calculatePixels(state)
        #convert to PNG
        png = convertToPng(pixels)
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
    fileName = convertStateToFileName(state)
    try:
        return open(fileName)
    except:
        return None
    
    
