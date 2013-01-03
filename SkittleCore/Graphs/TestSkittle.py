'''
Created on Dec 7, 2012

@author: Josiah
'''

def handleFloatList(floatList):
    if not isinstance(floatList, dict) and len(floatList) > 0 and hasattr(floatList[0], "__iter__"):
        return map(lambda x: handleFloatList(x), floatList)
    return map(lambda x: x*2, floatList)


def countNucleotides(seq):
    if len(seq) > 0 and not isinstance(seq, (str,dict)) and hasattr(seq[0], "__getitem__"):
        print 'recursing'
        return map(lambda x: countNucleotides(x), seq)
    counts = {}
    for c in seq:
        counts[c] = 1 + counts.get(c,0) #defaults to 0 
    return counts

def test(a='a'):
    print 'method ', a
        
if __name__ == '__main__':
    a = [ 1, 2, 3]
    b = [-1,-2,-3]
    c = [[1,2,3],[4,5,6],[7,8,9]]
    test(a)
    test()
    print handleFloatList(a)
    print
    print handleFloatList(c)
    print handleFloatList([])
    print handleFloatList([(2,3), (4,5)])
    print handleFloatList({'A':5, 'C':2, 'G':3})
    
    print handleFloatList('hello world')
    
    seq = ['AAGTTC', 'GT', 'ATA', 'AGGA', 'CC']
    print countNucleotides(seq)
    print reduce(max, map(lambda x: len(x), seq))