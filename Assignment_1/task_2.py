import cv2
import numpy as np


#create a capture object
#0 for video device 0
cap = cv2.VideoCapture(0)
ret, current_frame = cap.read()
previous_frame = current_frame


while(True):
    #capture frame-by-frame
    ret, frame = cap.read() #comment out for DIFFERENCING

    #here we do operations on the frame (convert to grayscale here)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  #comment out for DIFFERENCING

    #Here are the different image processing functions
    #ret, frame_thresh = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)
    frame_canny = cv2.Canny(gray,100,200)

    #FIND HARRIS CORNERS
    #gray = np.float32(gray)
    #dst = cv2.cornerHarris(gray,2,3,0.04)
    #dst = cv2.dilate(dst,None)
    #ret, dst = cv2.threshold(dst,0.01*dst.max(),225,0)
    #dst = np.uint8(dst)

    #FIND LINES using HoughLines()
    #edges = cv2.Canny(gray,50,150,apertureSize = 3)
    #minLineLength = 50
    #maxLineGap = 20
    #lines = cv2.HoughLinesP(edges,1,np.pi/180,15,minLineLength,maxLineGap)
    #for x in range(0, len(lines)):
    #    for x1,y1,x2,y2 in lines[x]:
    #        cv2.line(frame,(x1,y1),(x2,y2),(255,0,0),2)

    #DIFFERENCING
    current_frame_gray = cv2.cvtColor(current_frame, cv2.COLOR_BGR2GRAY)
    previous_frame_gray = cv2.cvtColor(previous_frame, cv2.COLOR_BGR2GRAY)

    frame_diff = cv2.absdiff(current_frame_gray,previous_frame_gray)

    previous_frame = current_frame.copy()
    ret, current_frame = cap.read()


    cv2.imshow('WYNN', frame_canny)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break


cap.release()
cv2.destroyAllWindows()
