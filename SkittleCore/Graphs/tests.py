"""
This file demonstrates writing tests using the unittest module. These will pass
when you run "manage.py test".

Replace this with more appropriate tests for your application.
"""

from django.test import TestCase
from SkittleCore.models import RequestPacket
from models import *


class SimpleTest(TestCase):
    def test_basic_addition(self):
        """
        Tests that 1 + 1 always equals 2.
        """
        self.assertEqual(1 + 1, 2)

def TestPacket():
    state = RequestPacket()
    
    state.specimen = 'hg18'
    state.chromosome = 'chrY-sample'
    state.seq = ''#ACGTAAAACCCCGGGGTTTTACGTACGTACGTACGTACGTACGTACGTACGTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTACGTACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'
    state.colorPalette = 'Classic'
    state.width = 24
    state.scale = 2
    state.start = 1
#    state.requestedGraph = 'n'
    return state

import SkittleGraphTransforms
import SkittleCore.FastaFiles
import DNAStorage.StorageRequestHandler

class FastaTest(TestCase):
    def testFilePath(self):
        path = DNAStorage.StorageRequestHandler.GetFastaFilePath('hg18', 'chrY-sample', 1)
        print path
        self.assertNotEqual(None, path, "Didn't return a path")

class TransformTest(TestCase):
    def testModelSeqDepth(self):
        state = TestPacket()
        self.assertFalse(type(state.seq) == object)
        self.assertFalse(isinstance(['ACGTACGTAAAACCCCGGGGTTTT','AAAACGCCGTN', 'AGTGGGG'], type(state.seq)), type(state.seq))
        self.assertTrue(SkittleGraphTransforms.hasDepth(['ACGTACGTAAAACCCCGGGGTTTT','AAAACGCCGTN', 'AGTGGGG']), type(state.seq))
        self.assertFalse(SkittleGraphTransforms.hasDepth(SkittleCore.FastaFiles.FastaFile('AGCT')))
        self.assertFalse(SkittleGraphTransforms.hasDepth(state.seq), "The Graph transforms will break with the current sequence data type:" + str(type(state.seq)))
        print len(state.seq), " ",
        state.seq = 'ACGT'
        self.assertTrue(0 < len(state.seq))
        print len(state.seq)
    def testChunks(self):
        state = TestPacket()
        self.assertEqual(len(state.seq[1:5]), len(str('ACGCGCTCTATCA')[1:5]))
        
    def testCountNucleotides(self):
        counts = SkittleGraphTransforms.countNucleotides(['ACGTACGTAAAACCCCGGGGTTTT','AAAACGCCGTN', 'AGTGGGG'])
        print counts


import AnnotationDisplay
class AnnotationDisplayTest(TestCase):
    def test(self):
        print 'Annotation Display Test case'
        state = TestPacket()
        state.length = 300
        annotationState = AnnotationDisplay.AnnotationTrackState()  #this is not a model state.  This is only here because gtfReader has not been implemented
        print AnnotationDisplay.calculateOutputPixels(state, annotationState)
        
import NucleotideBias        
class NucleotideBiasTest(TestCase):
    def test(self):
        print 'Nucleotide Bias test case'
        state = TestPacket()
#        state.seq = 'ACGTACGTAAAACCCCGGGGTTTT'
        print NucleotideBias.calculateOutputPixels(state)
        
import NucleotideDisplay        
class NucleotideDisplayTest(TestCase):
    def test(self):
        print 'Nucleotide Display test case'
        state = TestPacket()
    #    state.scale = 1
        print NucleotideDisplay.calculateOutputPixels(state)
        
import OligomerUsage
class OligomerUsageTest(TestCase):
    def test(self):
        print 'OligomerUsage test case'
        state = TestPacket()
        extraState = OligomerUsageState()
        print OligomerUsage.calculateOutputPixels(state, extraState)#
        
import RepeatMap
class RepeatMapTest(TestCase):
    def test(self):
        print 'RepeatMap test case'
        state = TestPacket()
        repeatMapState = RepeatMapState()
        print RepeatMap.calculateOutputPixels(state, repeatMapState)
        
import SequenceHighlighter
class SequenceHighlighterTest(TestCase):#TODO: currently blank
    def test(self):
        print 'SequenceHighlighter test case'
        state = TestPacket()
#        state.seq = 'AAAAGGGGTATATATATATATGGGATAAAGCCCCC'
        print SequenceHighlighter.calculateOutputPixels(state, HighlighterState())
    
import SimilarityHeatMap
class SimilarityHeatMapTest(TestCase):
    def test(self):
        print 'SimilarityHeatMap test case'
        state = TestPacket()
        state.width = 200
        heatMap = SimilarityHeatMap.calculateOutputPixels(state, heatMapState = SimilarityHeatMapState())
        SimilarityHeatMap.prettyPrint(heatMap[:5])
        
import ThreeMerDetector
class ThreeMerDetectorTest(TestCase):
    def test(self):
        print 'ThreeMerDetector test case'
        state = TestPacket()
        extra = ThreeMerDetectorState()
        print ThreeMerDetector.calculateOutputPixels(state, extra)

import SkittleCore.Graphs.RepeatOverview
from PixelLogic import spectrum
class RepeatOverviewTest(TestCase):
    def test(self):
        pixels = []
        for y in range(64):
            for i in range(1024):
                pixels.append(spectrum(i / 1024.0))
        return pixels
    
#import grph
#class grphTest(TestCase):
#    def test(self):
#        print 'grph test case'
#        state = TestPacket()
#        #
#        print grph.calculateOutputPixels(state)#
