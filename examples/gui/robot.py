#!/usr/bin/env python

# example robot.py

from gi.repository import Gtk
import os
import fnmatch
import sys
import time

class Robot(Gtk.Window):

    def hello(self, widget, data=None):
        print "Hello World"

    def init_base_clockwise(self,widget,data=None):
        print "Intitate Clockwise Base"
        self.base_motor.write("1")
        self.base_motor.flush()

    def init_base_anticlockwise(self,widget,data=None):
        print "Initiate AntiClockwise Base"
        self.base_motor.write("2")
        self.base_motor.flush()

    def init_base_stop(self,widget,data=None):
        print "Stop Base"
        self.base_motor.write("0")
        self.base_motor.flush()

    def init_arm_up(self,widget,data=None):
        print "Initiate Arm Up"
        self.arm_motor.write("1")
        self.arm_motor.flush() 

    def init_arm_down(self,widget,data=None):
        print "Initiate Arm Down"
        self.arm_motor.write("2")
        self.arm_motor.flush()

    def init_arm_stop(self,widget,data=None):
        print "Arm Stop"
        self.arm_motor.write("0")
        self.arm_motor.flush()

    def init_elbow_up(self,widget,data=None):
        print "Initiate Elbow Up"
        self.elbow_motor.write("1")
        self.elbow_motor.flush()

    def init_elbow_down(self,widget,data=None):
        print "Initiate Elbow Down"
        self.elbow_motor.write("2")
        self.elbow_motor.flush()

    def init_elbow_stop(self,widget,data=None):
        print "Elbow Stop"
        self.elbow_motor.write("0")
        self.elbow_motor.flush()

    def init_wrist_up(self,widget,data=None):
        print "Initiate Wrist Up"
        self.wrist_motor.write("1")
        self.wrist_motor.flush()

    def init_wrist_down(self,widget,data=None):
        print "Initiate Wrist Down"
        self.wrist_motor.write("2")
        self.wrist_motor.flush()

    def init_wrist_stop(self,widget,data=None):
        print "Wrist Stop"
        self.wrist_motor.write("0")
        self.wrist_motor.flush()

    def init_jaw_open(self,widget,data=None):
        print "Initiate Jaw Open"
        self.jaw_motor.write("2")
        self.jaw_motor.flush()

    def init_jaw_close(self,widget,data=None):
        print "Initiate Jaw Close"
        self.jaw_motor.write("1")
        self.jaw_motor.flush()

    def init_jaw_stop(self,widget,data=None):
        print "Jaw Stop"
        self.jaw_motor.write("0")
        self.jaw_motor.flush()

    def switch_led(self,widget,data=None):
        if(self.led.get_label() == "Off"):
            self.led.set_label("On")
            print "LED: On"
            self.led_device.write("1")
            self.led_device.flush()
            return(None)

        if(self.led.get_label() == "On"):
            self.led.set_label("Off")
            print "LED: Off"
            self.led_device.write("0")
            self.led_device.flush()
            return(None)

    def delete_event(self, widget, event, data=None):
        return False

    # Another callback
    def destroy(self, widget, data=None):
        Gtk.main_quit()

    def __init__(self):

        BASE_DIR = "/sys/bus/usb/drivers/robotic_arm/"

        for file in os.listdir(BASE_DIR):
                if fnmatch.fnmatch(file, '*:*'):
                        self.file = file

        self.base_motor = open("%s%s/basemotor" % (BASE_DIR,file),"w")
        self.arm_motor = open("%s%s/motor4" % (BASE_DIR,file),"w")
        self.elbow_motor = open("%s%s/motor3" % (BASE_DIR,file),"w")
        self.wrist_motor = open("%s%s/motor2" % (BASE_DIR,file),"w")
        self.jaw_motor = open ("%s%s/gripmotor" % (BASE_DIR,file),"w")
        self.led_device = open ("%s%s/led" % (BASE_DIR,file),"w")

        Gtk.Window.__init__(self,title="Robotic Arm Manual Control Program")
        self.connect("delete_event", self.delete_event)
        self.connect("destroy", self.destroy)
        self.set_border_width(10)

        self.base_label = Gtk.Label("Base: ")
        self.base_label.set_alignment(0,0)
        self.base_label.show()

        self.base_clockwise = Gtk.Button("Clockwise")
        self.base_clockwise.connect("clicked",self.init_base_clockwise,None)
        self.base_clockwise.show()

        self.base_anticlockwise = Gtk.Button("AntiClockwise")
        self.base_anticlockwise.connect("clicked",self.init_base_anticlockwise,None)
        self.base_anticlockwise.show()

        self.base_stop = Gtk.Button("Stop")
        self.base_stop.connect("clicked",self.init_base_stop,None)
        self.base_stop.show()

        self.arm_label = Gtk.Label("Arm: ")
        self.arm_label.set_alignment(0,0)
        self.arm_label.show()

        self.arm_up = Gtk.Button("Up")
        self.arm_up.connect("clicked",self.init_arm_up,None)
        self.arm_up.show()

        self.arm_down = Gtk.Button("Down")
        self.arm_down.connect("clicked",self.init_arm_down,None)
        self.arm_down.show()

        self.arm_stop = Gtk.Button("Stop")
        self.arm_stop.connect("clicked",self.init_arm_stop,None)
        self.arm_stop.show()

        self.elbow_label = Gtk.Label("Elbow: ")
        self.elbow_label.set_alignment(0,0)
        self.elbow_label.show()

        self.elbow_up = Gtk.Button("Up")
        self.elbow_up.connect("clicked",self.init_elbow_up,None)
        self.elbow_up.show()

        self.elbow_down = Gtk.Button("Down")
        self.elbow_down.connect("clicked",self.init_elbow_down,None)
        self.elbow_down.show()

        self.elbow_stop = Gtk.Button("Stop")
        self.elbow_stop.connect("clicked",self.init_elbow_stop,None)
        self.elbow_stop.show()

        self.wrist_label = Gtk.Label("Wrist: ")
        self.wrist_label.set_alignment(0,0)
        self.wrist_label.show()

        self.wrist_up = Gtk.Button("Up")
        self.wrist_up.connect("clicked",self.init_wrist_up,None)
        self.wrist_up.show()

        self.wrist_down = Gtk.Button("Down")
        self.wrist_down.connect("clicked",self.init_wrist_down,None)
        self.wrist_down.show()

        self.wrist_stop = Gtk.Button("Stop")
        self.wrist_stop.connect("clicked",self.init_wrist_stop,None)
        self.wrist_stop.show()

        self.jaw_label = Gtk.Label("Jaw: ")
        self.jaw_label.set_alignment(0,0)
        self.jaw_label.show()

        self.jaw_open = Gtk.Button("Open")
        self.jaw_open.connect("clicked",self.init_jaw_open,None)
        self.jaw_open.show()

        self.jaw_close = Gtk.Button("Close")
        self.jaw_close.connect("clicked",self.init_jaw_close,None)
        self.jaw_close.show()

        self.jaw_stop = Gtk.Button("Stop")
        self.jaw_stop.connect("clicked",self.init_jaw_stop,None)
        self.jaw_stop.show()

        self.led_label = Gtk.Label("LED: ")
        self.led_label.set_alignment(0,0)
        self.led_label.show()

        self.led = Gtk.Button("Off")
        self.led.connect("clicked",self.switch_led,None)
        self.led.show()

        self.controls = Gtk.Grid()
        self.controls.attach(self.base_label,0,0,1,1)
        self.controls.attach(self.base_clockwise,1,0,1,1)
        self.controls.attach(self.base_anticlockwise,2,0,1,1)
        self.controls.attach(self.base_stop,3,0,1,1)
        self.controls.attach(self.arm_label,0,1,1,1)
        self.controls.attach(self.arm_up,1,1,1,1)
        self.controls.attach(self.arm_down,2,1,1,1)
        self.controls.attach(self.arm_stop,3,1,1,1)
        self.controls.attach(self.elbow_label,0,2,1,1)
        self.controls.attach(self.elbow_up,1,2,1,1)
        self.controls.attach(self.elbow_down,2,2,1,1)
        self.controls.attach(self.elbow_stop,3,2,1,1)
        self.controls.attach(self.wrist_label,0,3,1,1)
        self.controls.attach(self.wrist_up,1,3,1,1)
        self.controls.attach(self.wrist_down,2,3,1,1)
        self.controls.attach(self.wrist_stop,3,3,1,1)
        self.controls.attach(self.jaw_label,0,4,1,1)
        self.controls.attach(self.jaw_open,1,4,1,1)
        self.controls.attach(self.jaw_close,2,4,1,1)
        self.controls.attach(self.jaw_stop,3,4,1,1)
        self.controls.attach(self.led_label,0,5,1,1)
        self.controls.attach(self.led,1,5,3,1)
        self.controls.show()

        self.add(self.controls)
        self.show()

    def main(self):
        Gtk.main()

if __name__ == "__main__":
    robot = Robot()
    robot.main()
