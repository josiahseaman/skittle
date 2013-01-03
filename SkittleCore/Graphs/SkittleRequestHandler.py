'''
Created on Dec 21, 2012

@author: Josiah
'''
import io
import sys

'''The set of availableGraphs is populated by the individual graph modules who are responsible for 
registering with the request Handler using the 'registerGraph' function below. '''
availableGraphs = set()

def registerGraph(name, moduleName):
    moduleReference = sys.modules[moduleName]
    availableGraphs.add((name, moduleReference))
    
from SkittleStatePackets import StatePacket
import FastaFiles
import NucleotideDisplay
import NucleotideBias
import RepeatMap
import OligomerUsage
import SimilarityHeatMap
import ThreeMerDetector
import SequenceHighlighter

print __name__, " Printing Available Graphs: "
for graph in availableGraphs:
    print graph 


def handleRequest(state):
    graphCollage = []
    sequence = FastaFiles.readFile(state)
    if sequence is None:
        raise IOError('Cannot proceed without sequence')
    
    activeSet = filter(lambda x: x[0] in state.activeGraphs, availableGraphs)
    print "Active Set: ", activeSet
    
    for name, graphModule in activeSet:
        print "Executing ", name
        #get settings from state
        settings = state.activeGraphs[name]
        if settings is not None:
            graphModule.calculateOutputPixels(state, settings)    
        else:
            graphModule.calculateOutputPixels(state)
    
    
    
def parseActiveGraphString(state):
    characterAliases = {
        'n':("Nucleotide Display", NucleotideDisplay),
        'b':("Nucleotide Bias", NucleotideBias),
        'm':("Repeat Map", RepeatMap),
#        'c':"Alignment Cylinder",
#        'r':"Repeat Overview",
        'o':("Oligomer Usage", OligomerUsage),
        's':("Similarity Heatmap", SimilarityHeatMap),
        't':("Threemer Detector", ThreeMerDetector),
        'h':("Sequence Highlighter", SequenceHighlighter)}
    #TODO insert State URL parser here    
    
    
    