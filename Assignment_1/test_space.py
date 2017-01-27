# Standard imports
#import cv2
#import numpy as np;

# Read image
#im = cv2.imread("blobs.jpg", cv2.IMREAD_GRAYSCALE)

# Set up the detector with default parameters.
#detector = cv2.SimpleBlobDetector()

# Detect blobs.
#keypoints = detector.detect(im)
#print keypoints

# Draw detected blobs as red circles.
# cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS ensures the size of the circle corresponds to the size of blob
#im_with_keypoints = cv2.drawKeypoints(im, keypoints, np.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

# Show keypoints
#cv2.imshow("Keypoints", im_with_keypoints)
#cv2.waitKey(0)

import numpy as np
import cv2

# Create a black image
img = np.zeros((512,512,3), np.uint8)

# Draw a blue line with thickness of 5 px
cv2.line(img,(15,20),(70,50),(255,0,0),5)

#Display the image
cv2.imshow("img",img)

cv2.waitKey(0)
