'''
Created on Mar 1, 2013

@author: Josiah
'''
import math

def floor(flt):
    return int(math.floor(flt))

def ceil(flt):
    return int(math.ceil(flt))

def lowPassFilter(scores):
    smoothed_scores = [0.0] * len(scores)
    for i in range(1, len(scores)-1):
        smoothed_scores[i] = (scores[i-1] + scores[i] + scores[i+1]) / 3.0
        
    if smoothed_scores: #edges
        smoothed_scores[0] = scores[0]
        smoothed_scores[-1] = scores[-1]
    return smoothed_scores

