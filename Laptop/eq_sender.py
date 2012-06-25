#!/usr/bin/python
import pygst
pygst.require("0.10")
import gst, gobject
import math, time, thread
import serial
import binascii

from numpy import interp

# inspired by http://pzwart3.wdka.hro.nl/wiki/Spectrum.py

class Eq:

	def __init__(self):
		
		# initialise the serial connection
		self.usbSerial = serial.Serial('/dev/ttyUSB0',9600)
		
		# configure values corresponding to the grid of strips
		self.unitCols = 8 # number of columns of RGB units (units per strip)
		self.unitRows = 8 # number of rows of RGB units (number of strips)
		self.lowerDb = -70 # the volume corresponding with MIN (zero) on eq display
		self.upperDb = -35 # the volume corresponding with MAX (unitCols) on eq display

		# connect to the primary soundcard to do fast fourier analysis of frequencies
		# other sources are available, this samples every 0.04secs (written in nanoseconds)
		# and assigns frequencies to 16 bands, (only the first 8 of which are later sent onto the display)
		listener_desc = 'pulsesrc ! spectrum interval=40000000 bands=16 threshold=-70 ! fakesink'
		self.listener = gst.parse_launch(listener_desc)
		bus = self.listener.get_bus()
		bus.add_signal_watch()
		bus.connect("message", self.on_message) # get callbacks to on_message(...) from the spectrum element
				
	# this is called when there's a new set of frequency interval amplitude data
	def on_message (self, bus, message):
		s = message.structure
		if s and s.get_name() == "spectrum":
			offset=4
			#keep only some of the 16 bands, (from 4 to 12)
			bandMagnitudes = s['magnitude'][ offset : self.unitCols+offset ]
			# traverse the columns constructing the serial message for the display
			magnitudeBytes = bytearray([])
			for unitCol in range(0,self.unitCols):
				bandMagnitude = bandMagnitudes[unitCol]
				bandRow = interp(bandMagnitude,[self.lowerDb,self.upperDb],[0,self.unitRows], 0, self.unitRows) # remap amplitude into range 0-7
				bandRow = min(self.unitRows,max(0,int(math.floor(bandRow))))
				magnitudeBytes.extend([bandRow])
			# write the new values to the display
			self.usbSerial.flushOutput()			
			self.usbSerial.write(magnitudeBytes + bytearray('\n'))
		return True
 
 	# this is called to trigger the gstreamer pipeline
	# and keeps a user thread running a while loop so 
	# the application stays alive
	def start(self):
		self.listener.set_state(gst.STATE_PLAYING)
		while True:
			time.sleep(1)

# Configures Eq, launches it in its own thread
# and starts GStreamer pipelines running
eq = Eq()
thread.start_new_thread(eq.start, ())
gobject.threads_init()
loop = gobject.MainLoop()
loop.run()
