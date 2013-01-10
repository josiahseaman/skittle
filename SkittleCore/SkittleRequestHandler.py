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
    
from SkittleCore.models import StatePacket
import SkittleCore.FastaFiles as FastaFiles
import Graphs.NucleotideDisplay
import Graphs.NucleotideBias
import Graphs.RepeatMap
import Graphs.OligomerUsage
import Graphs.SimilarityHeatMap
import Graphs.ThreeMerDetector
import Graphs.SequenceHighlighter



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
        'n':("Nucleotide Display", Graphs.NucleotideDisplay),
        'b':("Nucleotide Bias", Graphs.NucleotideBias),
        'm':("Repeat Map", Graphs.RepeatMap),
#        'c':"Alignment Cylinder",
#        'r':"Repeat Overview",
        'o':("Oligomer Usage", Graphs.OligomerUsage),
        's':("Similarity Heatmap", Graphs.SimilarityHeatMap),
        't':("Threemer Detector", Graphs.ThreeMerDetector),
        'h':("Sequence Highlighter", Graphs.SequenceHighlighter)}
    #TODO insert State URL parser here    
    
    
    