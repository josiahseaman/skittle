#!/usr/bin/env python
import os
import sys

def importFasta():
    from DNAStorage.ProcessFasta    import run
    run()
    
if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")

    from django.core.management import execute_from_command_line

    if "ImportFasta" in sys.argv:
        importFasta()
    else:
        execute_from_command_line(sys.argv)
