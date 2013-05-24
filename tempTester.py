#!/usr/bin/env python
import os

if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "SkittleTree.settings")

    #from Annotations.import_gff import ImportGFF

    #ImportGFF("ZmB73", "./Annotations/history/ZmB73_5b_FGS.gff")

    from SkittleCore.models import SkittleUserManager
    thingy = SkittleUserManager()
    thingy.create_superuser("marshall@newlinetechnicalinnovations.com", "I<3$Connie", "Marshall", "Smith")
