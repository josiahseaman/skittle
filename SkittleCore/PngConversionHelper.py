'''
Functions related to PNG moved from GraphRequestHandler
Created on Jan 17, 2013
@author: Josiah
'''
from functools import reduce
import os
import tempfile
import copy

import png
from DNAStorage.StorageRequestHandler import StorePng
import SkittleCore.GraphRequestHandler #import getGraphDescription, GraphDescription


def checkForGreyscale(state):
    desc = SkittleCore.GraphRequestHandler.getGraphDescription(state)
    assert isinstance(desc, SkittleCore.GraphRequestHandler.GraphDescription)
    return desc.isGrayScale


def convertToPng(state, pixels, isRaster=False):
    targetWidth = 1024
    greyscale = checkForGreyscale(state)
    f = tempfile.mktemp()
    # open up tempFile
    f = open(f, 'wb')
    if greyscale:
        if not isRaster:
            p = pixels
            # p = multiplyGreyscale(pixels, 255)
            w = png.Writer(len(p[0]), len(p), greyscale=True)
        else:  # grey scale raster e.g. Tag Vision
            p = flattenImage(pixels, targetWidth, isColored=False)
            w = png.Writer(targetWidth, len(p), greyscale=True, alpha=False)
    else:
        if not isRaster:  # Nucleotide Bias
            p = flattenImage(pixels, len(pixels[0]), True, 4)
            w = png.Writer(len(p[0]) / 4, len(p), greyscale=False, alpha=True)
        else:  # raster, color graphs
            p = flattenImage(pixels, targetWidth)
            w = png.Writer(targetWidth, len(p))
    w.write(f, p)
    f.close()
    f = open(f.name, 'rb')  # return the binary contents of the file
    data = f.read()
    StorePng(state, f)
    f.close()
    os.unlink(f.name)
    return data


def capRange(color):
    newColor = ()
    for part in color:
        newColor += (int(min(255, max(0, part))),)
    return newColor


def flattenImage(pixels, targetWidth, isColored=True, nChannels=3):
    pixels = squishImage(pixels)
    if not isColored:
        return [pixels[start: start + targetWidth] for start in range(0, len(pixels), targetWidth)]

    p = []
    newline = []
    for color in pixels:
        if color is None:
            newline += (0,) * nChannels
        elif nChannels == 4:
            pix = capRange(color)
            newline += pix
            newline += (255,) #alpha
        else:
            pix = capRange(color)
            newline += pix

        if len(newline) >= targetWidth * nChannels:
            p.append(newline)
            newline = []
    if newline and newline[0] != []:
        while len(newline) < targetWidth * nChannels:
            newline.append(0)  # pad with zeros for a partial line at the end of the chromosome
        p.append(newline)  # append a partial line if there is one
    return p


def multiplyGreyscale(pixelLines, greyMax=255):
    for index, line in enumerate(pixelLines):
        pixelLines[index] = map(lambda x: int(min(255, max(x, 0) * greyMax)), line)
    return pixelLines


def squishImage(pixels):
    if isinstance(pixels, list) and pixels: #not empty
        if isinstance(pixels[0], list):
            return reduce(lambda x, y: x + squishImage(y), pixels, [])
        else:
            return pixels
    else:
        return pixels