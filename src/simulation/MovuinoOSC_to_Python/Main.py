import sys
import time
import numpy as np
import OSC_communication as osc
import math
import random

def main(args = None):
	# Start OSC server (to receive message)
	print "Starting OSCServer"
	osc_server = osc.OSCserver('192.168.0.100', 7400) # Init the OSC server with YOUR IP and use the same port number as the sender (here Movuino on port 7400)
	osc_server.addListener("movuinOSC") # add listener on address "movuinOSC"
	time.sleep(0.1)

	d1 = 0.0
	d2 = 0.0

	# Start OSC client (to send message)
	osc_client = osc.OSCclient('192.168.0.101', 7401) # Init the OSC client with the IP of the host, for Movuino you need to check the address on the Arduino serial monitor when the connection is set

	timer0 = time.time()
	timer1 = timer0
	while (timer1-timer0 < 10):
		timer1 = time.time()
		time.sleep(1)

		
		# SEND MESSAGE TO MOVUINO
		randNumber = random.randint(0,4)
		print randNumber
		osc_client.sendOSCMessage('filter', randNumber) # send random int between 0 and 5

		print "---------------"

	osc_server.closeServer() # ERROR MESSAGE but close the OSC server without killing the app

if __name__ == '__main__':
    sys.exit(main())
