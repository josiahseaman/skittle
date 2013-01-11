"""
This file demonstrates writing tests using the unittest module. These will pass
when you run "manage.py test".

Replace this with more appropriate tests for your application.
"""

from django.test import TestCase
from SkittleCore.models import StatePacket
from models import *


class SimpleTest(TestCase):
    def test_basic_addition(self):
        """
        Tests that 1 + 1 always equals 2.
        """
        self.assertEqual(1 + 1, 2)

import SkittleGraphTransforms
import SkittleCore.FastaFiles
class TransformTest(TestCase):
    def testModelSeqDepth(self):
        state = StatePacket()
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
        state = StatePacket()
        self.assertEqual(len(state.seq[1:5]), len(str('ACGCGCTCTATCA')[1:5]))
        
    def testCountNucleotides(self):
        counts = SkittleGraphTransforms.countNucleotides(['ACGTACGTAAAACCCCGGGGTTTT','AAAACGCCGTN', 'AGTGGGG'])
        print counts


import AnnotationDisplay
class AnnotationDisplayTest(TestCase):
    def test(self):
        print 'Annotation Display Test case'
        state = StatePacket()
        state.length = 300
        annotationState = AnnotationDisplay.AnnotationTrackState()  #this is not a model state.  This is only here because gtfReader has not been implemented
        print AnnotationDisplay.calculateOutputPixels(state, annotationState)
        
import NucleotideBias        
class NucleotideBiasTest(TestCase):
    def test(self):
        print 'Nucleotide Bias test case'
        state = StatePacket()
        state.seq = 'ACGTACGTAAAACCCCGGGGTTTT'
        print NucleotideBias.calculateOutputPixels(state)
        
import NucleotideDisplay        
class NucleotideDisplayTest(TestCase):
    def test(self):
        print 'Nucleotide Display test case'
        state = StatePacket()
    #    state.scale = 1
        print NucleotideDisplay.calculateOutputPixels(state)
        
import OligomerUsage
class OligomerUsageTest(TestCase):
    def test(self):
        print 'OligomerUsage test case'
        state = StatePacket()
        extraState = OligomerUsageState()
        print OligomerUsage.calculateOutputPixels(state, extraState)#
        
import RepeatMap
class RepeatMapTest(TestCase):
    def test(self):
        print 'RepeatMap test case'
        state = StatePacket()
        repeatMapState = RepeatMapState()
        print RepeatMap.calculateOutputPixels(state, repeatMapState)
        
import SequenceHighlighter
class SequenceHighlighterTest(TestCase):#TODO: currently blank
    def test(self):
        print 'SequenceHighlighter test case'
        state = StatePacket()
        state.seq = 'AAAAGGGGTATATATATATATGGGATAAAGCCCCC'
        print SequenceHighlighter.calculateOutputPixels(state, HighlighterState())
    
import SimilarityHeatMap
class SimilarityHeatMapTest(TestCase):
    def test(self):
        print 'SimilarityHeatMap test case'
        state = StatePacket()
        state.width = 30
        heatMap = SimilarityHeatMap.calculateOutputPixels(state, heatMapState = SimilarityHeatMapState())
        SimilarityHeatMap.prettyPrint(heatMap)
        
import ThreeMerDetector
class ThreeMerDetectorTest(TestCase):
    def test(self):
        print 'ThreeMerDetector test case'
        state = StatePacket()
        extra = ThreeMerDetectorState()
        print ThreeMerDetector.calculateOutputPixels(state, extra)
        
#import grph
#class grphTest(TestCase):
#    def test(self):
#        print 'grph test case'
#        state = StatePacket()
#        #
#        print grph.calculateOutputPixels(state)#
