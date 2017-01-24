import cv2
import glob
import numpy as np

#create blob detector parameter object
params = cv2.SimpleBlobDetector_Params()
#threshold
#params.thresholdStep = 2
params.minThreshold = 10
params.maxThreshold = 255
#filter by size
params.filterByArea = True
params.minArea = 100
#filter by circularity
params.filterByCircularity = True
params.minCircularity = 0.8
#params.maxCircularity = 1.0
#filter by convexity
params.filterByConvexity = True
params.minConvexity = 0.87
#filter by inertia
params.filterByInertia = True
params.minInertiaRatio = 0.4
#create blob detector with params object
blob_detect = cv2.SimpleBlobDetector(params)

number = 1
first_image = cv2.imread('1.jpg',0)

while number <= 36:
    num = str(number)
    filename = num + '.jpg'
    img = cv2.imread(filename,0)

    #absolute difference it first
    abs_diff = cv2.absdiff(img,first_image)
    #thn blur it to reduce noise
    img = cv2.GaussianBlur(abs_diff,(7,7),1.5,1.5)
    #then threshold it
    ret, img = cv2.threshold(img,10,255, cv2.THRESH_BINARY_INV)
    #then use simple blob detector with size and circularity

    keypoints = blob_detect.detect(img)
    print keypoints
    #draw keypoints
    img_w_keypoints = cv2.drawKeypoints(img,keypoints,np.array([]),(0,0,255))
    #then draw the keypoints returned by simple blob detector
    #then draw the keypoints on the original frame
    #ret, img = cv2.threshold(img, 30,255, cv2.THRESH_BINARY)
    cv2.imshow('image',img_w_keypoints)
    number = number + 1
    cv2.waitKey(33)
cv2.waitKey(3000)
cv2.destroyAllWindows
