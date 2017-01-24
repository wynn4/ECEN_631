import cv2
import glob
import numpy as np

#create blob detector parameter object
params = cv2.SimpleBlobDetector_Params()
#filter by size
params.filterByArea = True
params.minArea = 100
#filter by circularity
params.filterByCircularity = True
params.minCircularity = 0.1
#filter by convexity
params.filterByConvexity = True
params.minConvexity = 0.87
#filter by inertia
params.filterByInertia = True
params.minInertiaRatio = 0.35
#create blob detector with params object
blob_detect = cv2.SimpleBlobDetector(params)

number = 1
first_image = cv2.imread('1.jpg',0)

#Create VideoWriter object
#fps = 30
#fourcc = cv2.cv.CV_FOURCC('x', 'v', 'i', 'd')
#out = cv2.VideoWriter()
#success = out.open('wynn_task_3.avi',fourcc,fps,(640,480),True)

while number <= 36:
    num = str(number)
    filename = num + '.jpg'
    img_original = cv2.imread(filename,0)

    #absolute difference it first
    abs_diff = cv2.absdiff(img_original,first_image)
    #then blur it to reduce noise
    img = cv2.GaussianBlur(abs_diff,(7,7),1.5,1.5)
    #then threshold it
    ret, img = cv2.threshold(img,11,255, cv2.THRESH_BINARY_INV)
    #then use simple blob detector with params
    keypoints = blob_detect.detect(img)
    #draw keypoints on original image
    img_w_keypoints = cv2.drawKeypoints(img_original,keypoints,np.array([]),(0,0,255),cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
    #show the images
    #out.write(img_w_keypoints)
    cv2.imshow('wynn',img_w_keypoints)
    number = number + 1
    cv2.waitKey(33)
#out.release()
cv2.destroyAllWindows
