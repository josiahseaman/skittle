'''
Created on Mar 7, 2013

@author: Josiah
'''
from models import SnpIndexInfo, Annotation
import SkittleCore.Graphs.SNPdata
import json
from django.conf import settings

def createSnpIndex():
    indexFile = open(settings.SKITTLE_TREE_LOC+'Annotations/snps.index.sorted.txt', 'r')
    index = 0
    for line  in indexFile:
        tokens = line.split()
        if len(tokens) < 4: continue
        entry, created = SnpIndexInfo.objects.get_or_create(Start=tokens[0], SnpName=tokens[1], Chromosome=tokens[2], CompactIndex=tokens[3])
#        print entry
        if index % 1000 == 0:
            print index / 1047958.0 * 100
        index += 1
        
def createAnnotationsFromCompact(clientName, chromosome, start):
    chunkSnps = {'SNP_' + clientName: dict()}
#    f = open(clientGenotypeFilepath, 'r')
#    compactString = f.read()
#    f.close()
    compactString =SkittleCore.Graphs.SNPdata.packedSNPs 
    print "Received SNP request"
    for snp in SnpIndexInfo.objects.filter(Chromosome=chromosome, CompactIndex__gte=start, CompactIndex__lt=start+settings.CHUNK_SIZE):
        uniqueID = snp.SnpName
        chunkSnps['SNP_' + clientName][uniqueID] = {"Start": snp.CompactIndex, "Allele 1":compactString[snp.Start*2], "Allele 2":compactString[snp.Start*2+1]}
        print uniqueID,
    assert len(snp) != 0, "SNP index not loaded"
    
    return json.dumps(chunkSnps)
