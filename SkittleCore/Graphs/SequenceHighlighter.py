'''Created on Dec 6, 2012
@author: Josiah'''

from SkittleCore.models import RequestPacket, chunkSize
from models import HighlighterState, SequenceEntry
from SkittleGraphTransforms import reverseComplement, calculatePerCharacterMatch
import PixelLogic
import SkittleCore.FastaFiles as FastaFiles 
import copy
from SkittleCore.GraphRequestHandler import registerGraph

registerGraph('h', "Sequence Highlighter", __name__, True, helpText='''Use the Select Tool and click on a line you would like to search for. 
 You can either click on Nucleotide Display or Sequence Highlighter to pick a sequence.  
 Given a search sequence, the Highlighter checks every start position on the screen.  
 The grayscale pixels are start positions that didn't make the cut.  
 Light pixels are near misses.  
 Example: When the Highlighter finds another sequence that is at least 70\% the same,
 it highlights each of the matching nucleotides in bright green.''')

def measureSequenceMatches(state, highlighterState, sequenceEntry):
    assert isinstance(sequenceEntry, SequenceEntry)
    scores = []
    findSize = len(sequenceEntry.seq)
    searchSeq = sequenceEntry.seq
    maxMismatches = int(findSize - float(findSize) * highlighterState.minimumPercentage + .999)
    #at 50%   1 = 0,  2 = 1, 3 = 1
    for start_i in range(min( chunkSize, len(state.seq)  - (findSize-1))) :
        mismatches = 0
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
        return entries[entryNumber-2].color or (0,255,0)
    else:
        grey = int(entryNumber*255)
        return (grey,grey,grey)
    
def colorCombinedResults(state, highlighterState, results, entries = None ):
    results = ensureEqualLengths2D(results)
    if not results: return results
    hitCanvas = [0] * len(results[0])#create a large blank canvas to paint on
    if entries is None:
        entries = highlighterState.getTargetSequenceEntries()
    for sequenceEntryIndex, searchPageResults in enumerate(results):
#        if highlighterState.searchReverseComplement:
#            sequenceEntryIndex /= 2
        searchSeq = entries[sequenceEntryIndex].seq
        for index, startScore in enumerate(searchPageResults):
            hitCanvas[index] = max(startScore, hitCanvas[index])#sets the grey pixels
            if startScore >= highlighterState.minimumPercentage:
                splatter = calculatePerCharacterMatch(state.seq[index:index+len(searchSeq)], searchSeq)
                splatter = map(lambda x: x*(sequenceEntryIndex+2), splatter)#this tags each character hit with WHICH search sequence it came from, +1 because of 0*0
                for localIndex, hit in enumerate(splatter):
                    if index + localIndex < len(hitCanvas):
                        hitCanvas[index+localIndex] = max(hit, hitCanvas[index+localIndex])
                    
    hitColors = map(lambda entryNumber: getMatchColor(entryNumber, entries), hitCanvas)
    
    return hitColors

def calculateOutputPixels(state, highlighterState = HighlighterState()):
    if state.scale != 1:
        return PixelLogic.blackSquare()

    assert isinstance(highlighterState, HighlighterState)
    state.readFastaChunks()
    results = [] #2D array containing a screen full of scores per targetSequence
     
    #TODO: temporary workaround of settings  
    targetSequenceEntries = highlighterState.getTargetSequenceEntries()
    if state.searchStop != 1:
        print "WARNING: SearchStop is still being defined"
        
        
        
        
    print "Searching for", len(targetSequenceEntries), "unique entries"
    if highlighterState.searchReverseComplement:
        startingSize = len(targetSequenceEntries)
        for i in range(startingSize):
            original = targetSequenceEntries[i]
            newEntry = copy.deepcopy(original)
            newEntry.seq = reverseComplement(original.seq)
            targetSequenceEntries.append(newEntry)

    for i in range(len( targetSequenceEntries )):
        if len( targetSequenceEntries[i].seq) != 0 :
            results.append( measureSequenceMatches(state, highlighterState, targetSequenceEntries[i] ) )
    synthesis = colorCombinedResults(state, highlighterState, results, targetSequenceEntries )
    return synthesis

    