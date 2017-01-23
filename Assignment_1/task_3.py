import cv2
import glob
import numpy as np

number = 1
while number <= 36:
    num = str(number)
    filename = num + '.jpg'
    img = cv2.imread(filename,0)
    #difference it first
    #then threshold it
    #then use simple blob detector with size and circularity
    #then draw the keypoints returned by simple blob detector
    #then draw the keypoints on the original frame
    #ret, img = cv2.threshold(img, 30,255, cv2.THRESH_BINARY)
    cv2.imshow('image',img)
    number = number + 1
    cv2.waitKey(33)
cv2.destroyAllWindows
