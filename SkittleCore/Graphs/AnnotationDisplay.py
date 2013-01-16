'''
Created on Dec 19, 2012

@author: Josiah
'''
from SkittleCore.models import StatePacket
from PixelLogic import randomColor, blankColor
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('a',"Annotation Display", __name__, False)

class Annotation():
    def getStartingLine(self, state):
        assert isinstance(state, StatePacket)
        return self.start / state.width

    def lengthIndices(self, state):
        indices = range(self.start / state.width, self.stop / state.width +1 )
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

def calculateOutputPixels(state, annotationState):
    #get annotations from state
    annotations = annotationState.getSortedAnnotation()    #annotations should already be sorted
    #layout annotations considering overlap
    pixels = layoutParallelTracks(state, annotations)
    
    return pixels

if __name__ == '__main__':
    print 'Annotation Display Test case'
    state = StatePacket()
    annotationState = AnnotationTrackState()  
    print calculateOutputPixels(state, annotationState)