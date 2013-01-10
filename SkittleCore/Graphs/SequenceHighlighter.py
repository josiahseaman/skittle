'''
Created on Dec 6, 2012

@author: Josiah
'''
from SkittleCore.models import StatePacket 
from models import HighlighterState, SequenceEntry
from SkittleGraphTransforms import reverseComplement, calculatePerCharacterMatch
import copy
import SkittleRequestHandler

SkittleRequestHandler.registerGraph("Sequence Highlighter", __name__)

def measureSequenceMatches(state, sequenceEntry):
    assert isinstance(sequenceEntry, SequenceEntry)
    scores = []
    findSize = len(sequenceEntry.seq)
    searchSeq = sequenceEntry.seq
    start = state.start
    maxMismatches = int(findSize - float(findSize) * sequenceEntry.minimumPercentage + .999)
    #at 50%   1 = 0,  2 = 1, 3 = 1
    for  i in range(min( state.length, len(state.seq) - state.start - (findSize-1))) :
        mismatches = 0
        start_i = start + i
        L = 0
        while mismatches <= maxMismatches and L < findSize:
            if state.seq[start_i + L] != searchSeq[L]:#this is the innermost loop.  This line takes the most time
                mismatches += 1
            L += 1
        scores.append(float(L - mismatches) / len(searchSeq))
    return scores
    
def ensureEqualLengths2D(results, defaultValue = 0.0):
    if not results: return results
    targetLength = reduce(max, map(lambda x: len(x), results))#find the longest length
    for seq in results:
        for i in range(targetLength - len(seq)):#the difference in lengths
            seq.append( defaultValue) #append defaults
    return results
    
def getMatchColor(entryNumber, entries):
    if entryNumber > 1:
        return entries[entryNumber-2].color
    else:
        grey = int(entryNumber*255)
        return (grey,grey,grey)
    
def colorCombinedResults(state, highlighterState, results ):
    results = ensureEqualLengths2D(results)
    if not results: return results
    hitCanvas = [0] * len(results[0])#create a large blank canvas to paint on
    entries = highlighterState.targetSequenceEntries
    for sequenceEntryIndex, searchPageResults in enumerate(results):
        if highlighterState.searchReverseComplement:
            sequenceEntryIndex /= 2
        searchSeq = entries[sequenceEntryIndex].seq
        for index, startScore in enumerate(searchPageResults):
            hitCanvas[index] = max(startScore, hitCanvas[index])#sets the grey pixels
            if startScore >= entries[sequenceEntryIndex].minimumPercentage:
                splatter = calculatePerCharacterMatch(state.seq[index:index+len(searchSeq)], searchSeq)
                splatter = map(lambda x: x*(sequenceEntryIndex+2), splatter)#this tags each character hit with WHICH search sequence it came from, +1 because of 0*0
                for localIndex, hit in enumerate(splatter):
                    if index + localIndex < len(hitCanvas):
                        hitCanvas[index+localIndex] = max(hit, hitCanvas[index+localIndex])
                    
    hitColors = map(lambda entryNumber: getMatchColor(entryNumber, entries), hitCanvas)
                    
    return hitColors

def calculateOutputPixels(state, highlighterState):
    assert isinstance(highlighterState, HighlighterState)
    results = [] #2D array containing a screen full of scores per targetSequence 
    for i in range(len( highlighterState.getTargetSequenceEntries() )):
        if len( highlighterState.targetSequenceEntries[i].seq) != 0 :
            results.append( measureSequenceMatches(state, highlighterState.targetSequenceEntries[i] ) )
            if highlighterState.searchReverseComplement:
                current = copy.deepcopy(highlighterState.targetSequenceEntries[i])
                reverseSettings = SequenceEntry(reverseComplement(current.seq), current.minimumPercentage, current.color)
                results.append( measureSequenceMatches(state, reverseSettings ) )
    synthesis = colorCombinedResults(state, highlighterState, results )
    return synthesis

if __name__ == '__main__':
    state = StatePacket()
    state.seq = 'AAAAGGGGTATATATATATATGGGATAAAGCCCCC'
    highlighterState = HighlighterState()
    print calculateOutputPixels(state, highlighterState)
    