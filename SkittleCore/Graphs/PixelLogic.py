'''
Created on Dec 5, 2012
@author: Josiah
'''
from random import randint
import SkittleCore.models as SkittleRequestPackets
import math


blankColor = None

colorPalettes = {
"COLORBLINDSAFE" : 
        {'A': (255, 102, 0),
        'C': (153, 0, 0),
        'G': (51, 255, 51),
        'T': (0, 153, 204),
        'N': ( 200, 200, 200)},#not sequenced

"BETTERCBSAFE" : 
        {'A': (0, 204, 204),
        'C': (153, 255, 0),
        'G': (204, 0, 102),
        'T': (255, 102, 0),
        'N': ( 200, 200, 200)},#not sequenced
    
"DARK" : 
        {'A': (152, 147, 173),
        'C': (84, 40, 59),
        'G': (13, 94, 58),
        'T': (40, 75, 163),
        'N': ( 200, 200, 200)},#not sequenced
    
"DRUMS" : 
        {'A': (80, 80, 255),
        'C': (224, 0, 0),
        'G': (0, 192, 0),
        'T': (230, 230, 0),
        'N': ( 200, 200, 200)},#not sequenced
    
"BLUES" : 
        {'A': (141, 0, 74),
        'C': (82, 0, 124),
        'G': (17, 69, 134),
        'T': (14, 112, 118),
        'N': ( 200, 200, 200)},#not sequenced
"REDS" : 
        {'A': (141, 0, 74),
        'C': (159, 0, 0),
        'G': (196, 90, 6),
        'T': (218, 186, 8),
        'N': ( 200, 200, 200)},#not sequenced
    
"Classic" : 
        {'A': (0, 0, 0),
        'C': (255, 0, 0),
        'G': (0, 255, 0),
        'T': (0, 0, 255),
        'N': ( 200, 200, 200)}#not sequenced
    }
'''Safe accessor for nucleotide colors'''
def getColor(state, character):
    assert isinstance(state, SkittleRequestPackets.RequestPacket)
    defaultColor = (0,0,0)
    a = colorPalettes.get(state.colorPalette, None)
    if a is None: 
        print "Palette not found"
        return defaultColor
    b = a.get(character, defaultColor)
    return b

'''Returns a color tuple with one rgb channel set to a random value'''
def randomColor():#only changes one channel right now
    colorT = [0,0,0]
    randomChannel = randint(0,2)
    colorT[randomChannel] = randint(0,255)
    return tuple(colorT)

def drawBar(size, filler_size, barColor, rightJustified):
    filler = [ None ] * max(0,filler_size)
    bar = [barColor] * size
    line = []
    if rightJustified :
        line += filler
        line += bar
    else:
        line += bar
        line += filler
    return line   

def drawJustifiedBar(barSizes, colorSeries, max_bar_width ):
    assert len(barSizes) == len(colorSeries)
    line = []
    for position in range(len(barSizes)):
        size = barSizes[position]
        modulo = position % 2
        filler_size = 0
        rightJustified = (modulo == 0)
        
#        switch(position)//this is the order in which the nucleotides are displayed
#        {//the two pointing outwards need twice the margin of the inner two because
#        //the inner two can overlap each other.
        if modulo == 0:
            if position == 0:
                filler_size = max_bar_width - size
        elif modulo == 1:
            filler_size = max_bar_width - size 
            if position+1 < len(barSizes):
                filler_size -= barSizes[position+1] #overflow from the next letter
            
        barColor = colorSeries[position]
        line += drawBar(size, filler_size, barColor, rightJustified)
    assert len(line) == max_bar_width * math.ceil( len(barSizes)/2.0), "Result %i shoulld be %i." % (len(line), max_bar_width * math.ceil( len(barSizes)/2.0))
    return line

def hasDepth(listLike):
    try:
        return len(listLike) > 0 and not isinstance(listLike, (str,dict, tuple, type(u"unicode string"))) and hasattr(listLike[0], "__getitem__") 
    except:
        return False

def twoSidedSpectrumColoring(floatList, midpoint = 0.0):
    if hasDepth(floatList):
        return map(lambda x: twoSidedSpectrumColoring(x, midpoint), floatList)
    pixels = []
    for score in floatList:
        if score is None:
            pixels.append((0,0,0))
        elif score > midpoint:
            distance = (score-midpoint) / (1.0 - midpoint)
            pixels.append((int(255*distance),0,0))
        else:
#            score -0.8; midpoint = 0.5
            distance = (score - midpoint) / (-1.0 - midpoint)
            pixels.append((0,0, int(255*abs(distance))))
        
    return pixels