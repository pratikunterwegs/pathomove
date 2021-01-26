#### code to summarise simulation landscapes with Moran's I ####

#### importing libraries and paths
# check python path
import sys

# should yield python 3.7 file path
for p in sys.path:
    print(p)

import pandas as pd  # similar to dplyr! yay!
import os  # has list dir functions etc
import numpy as np  # some matrix functions
from scipy import misc
import matplotlib.pyplot as plt

# check the current working directory
os.getcwd()
currentWd = p  # os.path.dirname(os.path.abspath(__file__)) #os.getcwd()

# check again
print(currentWd)

# gather image output
outputFolder = os.path.join(currentWd, "bin/settings")  # os.path.abspath("output")
# check for the right folder
if "bin" not in outputFolder:
    raise Exception('seems like the wrong output folder...')

#### list files and filter by name
# gather contents of the folder
imgFiles = list()
for root, directories, filenames in os.walk(outputFolder):
    for filename in filenames:
        imgFiles.append(os.path.join(root, filename))

# filter filenames to match foodlandscape
imgFiles = list(filter(lambda x: "foodlandscape" in x, imgFiles))


# function to get image generation and rep number
def funcImgNames (x):
    assert "str" in str(type(x)), "input doesn't seem to be a filepath"
    assert "foodlandscape" in x, "input is not a foodlandscape"
    names = ((x.split("foodlandscape")[1]).split(".")[0]).split("sim")
    return names


# get the image identity to match to parameters later
imgId = list(map(funcImgNames, imgFiles))
# make a pd df
imgId = pd.DataFrame(imgId, columns=['gen','sim'])
# make gen integer
imgId['gen'] = pd.to_numeric(imgId['gen'])
# identify numbers where image corresponds to gen > 400
imgId['listnumber'] = np.arange(0, imgId.shape[0], 1)
# imgId = imgId.query('gen < 1000')

# subset images to process
imgFilesToProcess = [imgFiles[i] for i in imgId.listnumber]

# test import by showing the n/2th landscape
plt.imshow(misc.imread(imgFilesToProcess[27*3])[:,:,1], cmap="inferno")


#### read the images in using a function and access the second channel (green)
import pysal.lib
from pysal.explore.esda.moran import Moran

# get image size, assuming square
landsize = (512)  # this should be set manually
# create a spatial weights matrix
w = pysal.lib.weights.lat2W(landsize, landsize)


# function to read image and calculate Moran I
def funcReadAndMoran (x):
    assert "str" in str(type(x)), "input doesn't seem to be a filepath"
    image = misc.imread(x)
    image = image[:, :, 1]  # selects the second channel which is green
    assert "array" in str(type(image)), "input doesn't seem to be an array"
    assert len(image.shape) == 2, "non 2-d array, input must be a 2d array"
    mi = Moran(image, w)
    del image
    return mi.I


# read in images and do Moran I
imgMoran = list(map(funcReadAndMoran, imgFilesToProcess))

# get as numpy array
dfarray = np.asarray(imgMoran)

# add array as pd df
imgId['morani'] = dfarray

# write to csv
imgId.to_csv(path_or_buf="landMoranVals_no_k.csv")

# ends here