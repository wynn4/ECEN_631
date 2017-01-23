import cv2
import glob
import numpy as np

number = 1
first_image = cv2.imread('1.jpg',0)
while number <= 36:
    num = str(number)
    filename = num + '.jpg'
    img = cv2.imread(filename,0)

    #absolute difference it first
    abs_diff = cv2.absdiff(img,first_image)
    #then threshold it
    ret, img = cv2.threshold(abs_diff,13,255, cv2.THRESH_BINARY)
    #then use simple blob detector with size and circularity
    #then draw the keypoints returned by simple blob detector
    #then draw the keypoints on the original frame
    #ret, img = cv2.threshold(img, 30,255, cv2.THRESH_BINARY)
    cv2.imshow('image',img)
    number = number + 1
    cv2.waitKey(33)
cv2.destroyAllWindows
