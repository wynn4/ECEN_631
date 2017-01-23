import cv2
import numpy as np


filename = '1.jpg'

img = cv2.imread(filename,0)
cv2.imshow('image',img)
cv2.waitKey(5000)
cv2.destroyAllWindows
