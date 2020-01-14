import cv2
import numpy as np 

print("Started security app")

camPort = 1
cam = cv2.VideoCapture(camPort)
fgbg = cv2.BackgroundSubtractorMOG2()


while True:
    retval, frame = cam.read()
    if retval != True:
        raise ValueError("Can't read frame")

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    print("will apply")
        
    try:
        fgmask = fgbg.apply(frame)
    except expression as identifier:
        print(identifier)

    print("applied")

    # Display the resulting frame
    # cv2.imshow('frame',frame)
    cv2.imshow('fg', fgmask)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()