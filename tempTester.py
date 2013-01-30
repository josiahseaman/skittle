#!/usr/bin/env python
import os
import sys
  
if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    
    from DNAStorage.StorageRequestHandler import *
        
    print GetTreeList()
