'''
Created on Dec 19, 2012

@author: Josiah
'''
from SkittleCore.models import RequestPacket
from PixelLogic import randomColor, blankColor
from SkittleCore.GraphRequestHandler import registerGraph
from models import AnnotationDisplayState

registerGraph('a',"Annotation Display", __name__, False, helpText='Annotation Display is linked to an annotation file with start and stop positions for tracks.  It aligns these start and stop positions along with the rest of the graphs, expanding to accommodate overlapping annotations as necessary.  The user can select individual annotations and see the full text associated with that annotation.')

class Annotation():
    def getStartingLine(self, state):
        assert isinstance(state, RequestPacket)
        return self.start / state.nucleotidesPerLine()

    def lengthIndices(self, state):
        indices = range(self.start / state.nucleotidesPerLine(), self.stop / state.nucleotidesPerLine() +1 )
        return indices

    def __init__(self, Start, End):
        self.start = Start
        self.stop = End
        self.color = randomColor()

class AnnotationTrackState():
    def getSortedAnnotation(self):
        return sorted(self.annotations, key=lambda anno: anno.start)
    
    def __init__(self):
        self.currentAnnotation = 'testAnnotation.gtf'
        self.annotations = [Annotation(5,12), Annotation(3,5)]#start and stop positions

def padColumn(pixels):
    for line in pixels:
        line.append(blankColor)

def findEmptyColumnSlot(state, pixels, annotation):
    for columnIndex in range(len(pixels[0])):
        if pixels[annotation.getStartingLine(state)][columnIndex] == blankColor:
            return columnIndex
    return len(pixels[0]) #no empty slots = index off the edge

def layoutParallelTracks(state, annotations):
    pixels = [ [] for x in range(state.height())]
    for current in annotations:
        columnIndex = findEmptyColumnSlot(state, pixels, current)
        while columnIndex >= len(pixels[0]):
            padColumn(pixels)
        for length in current.lengthIndices(state):#end - begin
            if length < len(pixels):
                pixels[length][columnIndex] = current.color
        
    return pixels

def calculateOutputPixels(state, annotationState = AnnotationTrackState()):
    #get annotations from state
    annotations = annotationState.getSortedAnnotation()    #annotations should already be sorted
    #layout annotations considering overlap
    pixels = layoutParallelTracks(state, annotations)
    
    return pixels
