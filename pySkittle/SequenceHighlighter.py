'''
Created on Dec 6, 2012

@author: Josiah
'''
from SkittleStatePackets import StatePacket, HighlighterState, SequenceEntry
from SkittleGraphTransforms import reverseComplement
import copy


def measureSequenceMatches(state, sequenceEntry):
    assert isinstance(sequenceEntry, SequenceEntry)
    scores = []
    findSize = len(sequenceEntry.seq)
    seq = sequenceEntry.seq
    start = state.start
    maxMismatches = int(findSize - float(findSize) * sequenceEntry.minimumPercentage + .999)
    #at 50%   1 = 0,  2 = 1, 3 = 1
    for  i in range(min( state.length, len(state.seq) - state.start - (findSize-1))) :
        mismatches = 0
        start_i = start + i
        L = 0
        while mismatches <= maxMismatches and L < findSize:
            if state.seq[start_i + L] != seq[L]:#this is the innermost loop.  This line takes the most time
                mismatches += 1
            L += 1
        scores.append(float(L - mismatches) / len(seq))
    return scores
    
def colorCombinedResults(highlighterState, results ):
    
    return 'combination'

def calculateOutputPixels(state, highlighterState):
    assert isinstance(highlighterState, HighlighterState)
    results = [] #2D array containing a screen full of scores per targetSequence #vector<vector<int> > results;
    for i in range(len( highlighterState.targetSequenceEntries )):
        if len( highlighterState.targetSequenceEntries[i].seq) != 0 :
            results.append( measureSequenceMatches(state, highlighterState.targetSequenceEntries[i] ) )
            if highlighterState.searchReverseComplement:
                current = copy.deepcopy(highlighterState.targetSequenceEntries[i])
                reverseSettings = SequenceEntry(reverseComplement(current.seq), current.minimumPercentage, current.color)
                results.append( measureSequenceMatches(state, reverseSettings ) )
#    synthesis = colorCombinedResults(highlighterState, results )
#    return synthesis
    return results

if __name__ == '__main__':
    state = StatePacket()
    state.seq = 'AAAAGGGG'
    highlighterState = HighlighterState()
    print calculateOutputPixels(state, highlighterState)
    