import sys, click, argparse 
import time
import numpy as np
import OSC_communication as osc
import math
import random

osc_client = 0

@click.command()
def choose_simulation() :
	simulate = True
	while(simulate) :
		click.echo('1: Low BPM, 2: High BPM, 3: Low SP02, 4: High SP02, 5: Low Temp, 6: High Temp, 9: STOP SIMULATION')
		simulation = click.prompt('please choose a simulation', type=int) 
		simulate = keypressed(simulation)


#Chosen simulation
def keypressed(event) :
        if event > 0 and event < 7:
                osc_client.sendOSCMessage('filter', (event - 1)) # send random int between 0 and 5
        
	if event == 1 :
		print('Low BPM')
	elif event == 2:
		print('High BPM')
	elif event == 3:
		print('Low SP02')
	elif event == 4:
		print('High SP02')
	elif event == 5:
		print('Low Temp')
	elif event == 6: 
		print('High Temp')
	elif event == 9 :
		return False
	return True

def main(args = None):
        global osc_client
        
	# Start OSC server (to receive message)
	print "Starting OSCServer"
	osc_server = osc.OSCserver('192.168.0.100', 7400) # Init the OSC server with YOUR IP and use the same port number as the sender (here Movuino on port 7400)
	osc_server.addListener("movuinOSC") # add listener on address "movuinOSC"
	time.sleep(0.1)

	d1 = 0.0
	d2 = 0.0

	# Start OSC client (to send message)
	osc_client = osc.OSCclient('192.168.0.101', 7401) # Init the OSC client with the IP of the host, for Movuino you need to check the address on the Arduino serial monitor when the connection is set

        # Start listening for key strokes
        choose_simulation();

	osc_server.closeServer() # ERROR MESSAGE but close the OSC server without killing the app

if __name__ == '__main__':
    sys.exit(main())
