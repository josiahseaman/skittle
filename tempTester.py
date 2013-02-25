#!/usr/bin/env python
import os
import sys
  
if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")
    
    from Annotations.import_gff import ImportGFF
    
    ImportGFF("ZmB73", "./Annotations/ZmB73_5b_FGS.gff")
