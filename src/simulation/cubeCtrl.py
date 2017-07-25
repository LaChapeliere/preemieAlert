#
# Companion Ctrl Simulation
# author: anneclairebrld
#

#please install click with pip install Click
import sys, click 

@click.command()
def choose_simulation() :
	simulate = True
	while(simulate) :
		click.echo('1: Low BPM, 2: High BPM, 3: Low SP02, 4: High SP02, 5: Low Temp, 6: High Temp, 9: STOP SIMULATION')
		simulation = click.prompt('please choose a simulation', type=int) 
		simulate = keypressed(simulation)


#Chosen simulation
def keypressed(event) :
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


def main(): 
	choose_simulation()
	
	
if __name__ == '__main__':
	main()