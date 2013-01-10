from django.test import TestCase
import FastaFiles
from models import StatePacket

class TestServerLaunch(TestCase):
    def test_index(self):
        resp = self.client.get('/')
        self.assertEqual(resp.status_code, 301)
        
    def test_browse(self):
        resp = self.client.get('/browse/')
        self.assertEqual(resp.status_code, 200)


import os
class FastaTest(TestCase):
    def test(self):
        state = StatePacket()
        print os.getcwd()
        print state.filePath
    #    state.filePath = 'bogus'
        f = FastaFiles.readFile(state)
        print f        
