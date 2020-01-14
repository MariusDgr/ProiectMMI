import cv2
import numpy as np 
import time 
import serial 
import urllib.request

print("Started security app")
# App settings
arduinoEnabled = True
serverEnabled = True


if arduinoEnabled:
    serCam = serial.Serial('COM3', baudrate=9600)
    if not serCam.isOpen():
        serCam.open()
    print("Started serial")

camPort = 1
cam = cv2.VideoCapture(camPort)
fgbg = cv2.createBackgroundSubtractorMOG2()

# Server 
url = "http://127.0.0.1:8080/securitymodule/intruder"

# Intruder detection settings
intruderThreshold = 3000000 # smaller value means higher sensitivity
intruderCooldownTime = 5
lastTime = time.time()

while True:
    retval, frame = cam.read()
    if retval != True:
        raise ValueError("Can't read frame")
    if frame.size == 0:
         continue

    cv2.imshow('frame',frame)
    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    fgmask = fgbg.apply(frame)
    
    # Display the resulting frame
    cv2.imshow('fg', fgmask)
    # print(fgmask.sum())

    if fgmask.sum() > intruderThreshold:
        if time.time() - lastTime > intruderCooldownTime:
            print("Detected intruder!")
            print(fgmask.sum())
            lastTime = time.time()

            # Send to arduino
            if arduinoEnabled:
                try:
                    serCam.write(b'1')
                except Exception as e:
                    print("Arduino serial might not be started")

            # Send to server
            if serverEnabled:
                try:
                    values = {}
                    data = urllib.parse.urlencode(values).encode("utf-8")
                    req = urllib.request.Request(url)
                    with urllib.request.urlopen(req,data=data) as f:
                        resp = f.read()
                        print(resp)
                        print()
                except Exception as e:
                    print("Server might be down")

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cam.release()
cv2.destroyAllWindows()