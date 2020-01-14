import serial
import socket
import time

# Initialize serial streams
# serDrink = serial.Serial('COM5', baudrate=9600, timeout=1)
serCam = serial.Serial('COM3', baudrate=9600)
# if not serCam.isOpen():
#     serCam.open()

time.sleep(3)
serCam.write(b"1")

