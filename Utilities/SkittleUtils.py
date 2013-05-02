from django.conf import settings
import math

def GetRoundedIndex(index):
    return int(math.floor(int(index) / settings.CHUNK_SIZE) * settings.CHUNK_SIZE) + 1
