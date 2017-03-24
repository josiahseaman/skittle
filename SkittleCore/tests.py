from __future__ import print_function
from django.test import TestCase
import FastaFiles
from models import RequestPacket



class TestServerLaunch(TestCase):
    def test_index(self):
        resp = self.client.get('/')
        self.assertEqual(resp.status_code, 200)

    def test_browse(self):
        resp = self.client.get('/browse/')
        self.assertEqual(resp.status_code, 200)


import os


class FastaTest(TestCase):
    def test(self):
        state = RequestPacket()
        print (os.getcwd())
        print (state.getFastaFilePath())
        f = FastaFiles.readFileOrNone(state)
        print (f)


import GraphRequestHandler


class GraphImport(TestCase):
    def test(self):
        print ("Available Graph Descriptions: ")
        desc = GraphRequestHandler.generateGraphListForServer()
        print (desc)
        for i in desc.items():
            print (i)
