'''
Created on Jan 2, 2013

@author: Josiah
'''
from SkittleCore.models import StatePacket
from SkittleRequestHandler import handleRequest

if __name__ == '__main__':
    print 'Request Handler Test'
    state = StatePacket()
    handleRequest(state)