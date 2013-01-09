from django.test import TestCase

class TestServerLaunch(TestCase):
    def test_index(self):
        resp = self.client.get('/')
        self.assertEqual(resp.status_code, 200)
        
    def test_browse(self):
        resp = self.client.get('/browse/')
        self.assertEqual(resp.status_code, 200)
