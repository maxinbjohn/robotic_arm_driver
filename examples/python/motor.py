""" An example program to move different parts of USB Robotic Arm """

import os
import fnmatch
import sys
import time

""" Locate the sysfs entry corresponding to USB Robotic ARM """
def find_usb_device():
	for file in os.listdir('/sys/bus/usb/drivers/robotic_arm/'):
                if fnmatch.fnmatch(file, '*:*'):
                        return file

""" To roate the motor in clockwise direction """
def move_clockwise(motor):
	fd= open(motor, "w")
	fd.write("1")
	fd.close()

""" To roate the motor in anti-clockwise direction """
def move_anti_clockwise(motor):
	fd= open(motor, "w")
	fd.write("2")
	fd.close()


""" To stop the current activity """
def stop(device):
	fd= open(device, "w")
	fd.write("0")
	fd.close()
	
""" To switch on LED in Robotic ARM """
def led_on(led):
	fd= open(led, "w")
	fd.write("1")
	fd.close()

if __name__ == '__main__':

	usb_dev_name = find_usb_device()
	
	if ( usb_dev_name == None):
		print "Please ensure that robotic_arm module is loaded "
		print " Also ensure that you have connected the robotic arm "
		print " and switched on the Robotic ARM device"
                sys.exit(-1)

	# Path for the robotic arm sysfs entries
	robotic_arm_path= "/sys/bus/usb/drivers/robotic_arm/"+ usb_dev_name + "/"

	# Switch on and off the LED in Robotic arm
	print "LED control"
	led= robotic_arm_path+"led"
	led_on(led)
	time.sleep(1)
	stop(led)

	# Move the base 
	print "Base Motor control"
	base= robotic_arm_path+"basemotor"
	move_clockwise(base)
	time.sleep(2)
	move_anti_clockwise(base)
	time.sleep(2)
	stop(base)
		
	# Move the base 
	print "Grip control"
	grip= robotic_arm_path+"gripmotor"
	move_clockwise(grip)
	time.sleep(1)
	move_anti_clockwise(grip)
	time.sleep(1)
	stop(grip)

	# Move the Wrist
	print "Wrist Motor control"
	wrist= robotic_arm_path+"motor2"
	move_clockwise(wrist)
	time.sleep(1)
	move_anti_clockwise(wrist)
	time.sleep(1)
	stop(wrist)

	# Move the Elbow
	print "Elbow Motor control"
	elbow= robotic_arm_path+"motor3"
	move_clockwise(elbow)
	time.sleep(1)
	move_anti_clockwise(elbow)
	time.sleep(1)
	stop(elbow)

	# Move the Shoulder
	print "Shoulder Motor control"
	shoulder= robotic_arm_path+"motor4"
	move_clockwise(shoulder)
	time.sleep(1)
	move_anti_clockwise(shoulder)
	time.sleep(1)
	stop(shoulder)


