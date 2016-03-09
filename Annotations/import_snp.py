"""
Created on Mar 7, 2013

@author: Josiah
"""
import json

from django.conf import settings

from models import SnpIndexInfo
import SkittleCore.Graphs.SNPdata


def createSnpIndex():
    indexFile = open(settings.BASE_DIR + 'Annotations/snps.index.sorted.txt', 'r')
    index = 0
    for line in indexFile:
        tokens = line.split()
        if len(tokens) < 4:
            continue
        entry, created = SnpIndexInfo.objects.get_or_create(CompactIndex=tokens[0], SnpName=tokens[1],
                                                            Chromosome=tokens[2], Start=tokens[3])
        #        print entry
        if index % 1000 == 0:
            print index / 1047958.0 * 100
        index += 1


import StorageRequestHandler


def createAnnotationsFromCompact(clientName, chromosome, start):
    chunkSnps = {'SNP_' + clientName: {}}

    validChromosomes = SnpIndexInfo.objects.values_list('Chromosome', flat=True).distinct()
    chromosome = StorageRequestHandler.ParseChromosomeName(validChromosomes, chromosome)
    compactString = SkittleCore.Graphs.SNPdata.packedSNPs

    for snp in SnpIndexInfo.objects.filter(Chromosome=chromosome, Start__gte=start,
                                           Start__lt=start + settings.CHUNK_SIZE):
        uniqueID = snp.SnpName
        chunkSnps['SNP_' + clientName][uniqueID] = {"Start": snp.Start, "Allele 1": compactString[snp.CompactIndex * 2],
                                                    "Allele 2": compactString[snp.CompactIndex * 2 + 1]}
        #    assert len(chunkSnps) != 0, "SNP index not loaded"

    return json.dumps(chunkSnps)
