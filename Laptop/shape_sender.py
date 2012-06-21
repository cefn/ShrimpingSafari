#!/usr/bin/python
import serial
import time

# illuminate the lower triangular matrix of RGB Leds
# repeatedly sends frames of 8 bytes describing the height 
# of the 'equalizer bars' so as to display a triangular 
# image on the strips, like this
# 
# *
# * *
# * * *
# * * * *
# * * * * *
# * * * * * *
# * * * * * * *
# * * * * * * * *

usbSerial = serial.Serial('/dev/ttyUSB0',9600)
while 1 :
	usbSerial.write(bytearray([0,1,2,3,4,5,6,7,'\n']))
	time.sleep(0.04)
