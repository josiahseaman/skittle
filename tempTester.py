#!/usr/bin/env python
import os
import sys
  
if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    
    from DNAStorage.StorageRequestHandler import *
        
    if HasFastaFile("hg19", "chrY-sample"):
        print "YES!"
    print GetChromosomeLength("hg19", "chrY-sample")
    #GetFastaFilePath("hg19", "chrY-sample", 1)
    #GetPngFilePath("hg19", "chrY-sample", 'n', 1, scale=1)
