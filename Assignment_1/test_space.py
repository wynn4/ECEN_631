#sup dog
#Jesse Wynn
#ECEN 631 Robotic Vision
#Assignment 1
import cv2
import numpy as np

#create a capture object
cap = cv2.VideoCapture(0)
ret, current_frame = cap.read()
previous_frame = current_frame

#Create VideoWriter object
size = (int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH)),
        int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT)))
fps = 30
fourcc = cv2.cv.CV_FOURCC('x', 'v', 'i', 'd')
out = cv2.VideoWriter()
success = out.open('wynn.avi',fourcc,fps,size,False)

#Plain Grayscale image
while(True):
    #capture frame-by-frame
    ret, frame = cap.read() #comment out for DIFFERENCING

    #here we do operations on the frame (convert to grayscale here)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    out.write(gray)
    cv2.imshow('WYNN', gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
#Binarized Thresholding
while(True):
    #capture frame-by-frame
    ret, frame = cap.read() #comment out for DIFFERENCING

    #here we do operations on the frame (convert to grayscale here)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    ret, frame_thresh = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)
    out.write(frame_thresh)
    cv2.imshow('WYNN', frame_thresh)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
#Canny Edges
while(True):
    #capture frame-by-frame
    ret, frame = cap.read() #comment out for DIFFERENCING

    #here we do operations on the frame (convert to grayscale here)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    frame_canny = cv2.Canny(gray,100,200)
    out.write(frame_canny)
    cv2.imshow('WYNN', frame_canny)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
#Harris Corners
while(True):
    #capture frame-by-frame
    ret, frame = cap.read() #comment out for DIFFERENCING

    #here we do operations on the frame (convert to grayscale here)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = np.float32(gray)
    dst = cv2.cornerHarris(gray,2,3,0.04)
    dst = cv2.dilate(dst,None)
    ret, dst = cv2.threshold(dst,0.01*dst.max(),225,0)
    dst = np.uint8(dst)
    out.write(dst)
    cv2.imshow('WYNN', dst)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
#Hough Lines
while(True):
    #capture frame-by-frame
    ret, frame = cap.read() #comment out for DIFFERENCING

    #here we do operations on the frame (convert to grayscale here)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    edges = cv2.Canny(gray,50,150,apertureSize = 3)
    minLineLength = 50
    maxLineGap = 20
    lines = cv2.HoughLinesP(edges,1,np.pi/180,15,minLineLength,maxLineGap)
    for x in range(0, len(lines)):
        for x1,y1,x2,y2 in lines[x]:
            cv2.line(frame,(x1,y1),(x2,y2),(0,0,255),2)
    gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    out.write(gray_frame)
    cv2.imshow('WYNN', gray_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
#Differencing
while(True):
    current_frame_gray = cv2.cvtColor(current_frame, cv2.COLOR_BGR2GRAY)
    previous_frame_gray = cv2.cvtColor(previous_frame, cv2.COLOR_BGR2GRAY)

    frame_diff = cv2.absdiff(current_frame_gray,previous_frame_gray)

    previous_frame = current_frame.copy()
    ret, current_frame = cap.read()
    out.write(frame_diff)
    cv2.imshow('WYNN', frame_diff)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cap.release()
out.release()
cv2.destroyAllWindows()
