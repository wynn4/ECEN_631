import numpy as np
import cv2
refPt = []
def click_and_print_pixel_data(event, x, y, flags, param):
    global refPt
    if event == cv2.EVENT_LBUTTONDOWN:
        refPt = [(x,y)]
        print refPt
#create a capture object
#0 for video device 0
cv2.namedWindow('image')
cv2.setMouseCallback('image', click_and_print_pixel_data)
cap = cv2.VideoCapture(1)

while(True):
    #capture frame-by-frame
    ret, frame = cap.read()

    #here we do operations on the frame (convert to grayscale here)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)


    cv2.imshow('image', gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
