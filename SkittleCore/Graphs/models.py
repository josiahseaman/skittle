import math

# Create your models here.

class ParentState():
    pass


class AnnotationDisplayState(ParentState):
    annotationPath = ''


class HighlighterState(ParentState):
    def __init__(self):
        self.targetSequenceEntries = []
        self.searchReverseComplement = True
        self.minimumPercentage = .6

    def getTargetSequenceEntries(self):
        '''targetSequenceEntries contains a series of SequenceEntries.'''
        if not self.targetSequenceEntries:
            default = SequenceEntry()
            return [default]
        return self.targetSequenceEntries

    def __str__(self):
        return "Search other strand: " + str(self.searchReverseComplement) + "   Sequences: " + str(
            self.getTargetSequenceEntries()[0].seq)

    def __repr__(self):
        dCopy = self.__dict__
        return str(dCopy)


'''This class contains all the state information specific exclusively to Repeat Map. 
This is the link and definition of the settings tab for Repeat Map.
I'm dubious about where to put this since GraphTransforms depends on it, but otherwise
it would be included in RepeatMap.py'''


class RepeatMapState(ParentState):
    F_width = 11
    F_start = 1
    skixelsPerSample = 24

    def height(self, state, pixels):
        F_height = len(pixels) / state.nucleotidesPerLine() # self.F_width*20
        return F_height

    def oldHeight(self, state, pixels):
    #        assert isinstance(state, SkittleCore.RequestPacket)
        F_height = ((len(pixels)) - (
                                        self.F_start - 1) * state.scale - self.F_width * state.scale ) / state.nucleotidesPerLine()
        #        F_height = max(0, min(400, F_height) )
        return F_height

    def numberOfColumns(self):
        return (self.F_width + 1) * self.skixelsPerSample


class NucleotideDisplayState(ParentState):
    visible = True


class NucleotideBiasState(ParentState):
    pass


class OligomerUsageState(ParentState):
    oligomerSize = 2


class SimilarityHeatMapState(OligomerUsageState):
    useRowColumnCorrelation = False


class ThreeMerDetectorState(ParentState):
    barWidth = 40    #used for display size calculations
    samples = 20

    def height(self, state, seq):
        F_height = int(math.ceil(((len(seq)) - (self.samples * 3) * state.scale ) / state.nucleotidesPerLine()))
        return F_height


class Color(tuple):
    def __init__(self, c):
        pass

    def __repr__(self):
        return '%02x%02x%02x' % (self)


class SequenceEntry():
    def __init__(self):
        self.seq = 'AAAAAAAAAA'
        self.color = Color([0, 255, 0])

    def __repr__(self):
        return str(self.__dict__)


        #h = HighlighterState()
        #h.targetSequenceEntries.append(SequenceEntry())
        #h.__repr__()