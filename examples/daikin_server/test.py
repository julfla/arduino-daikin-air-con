#!/usr/bin/python

""" This test script should restart you aircon. """

import serial
import time

from daikin_protocol import encode, bin2hex

SERIAL_PORT = "/dev/ttyACM0"
HEADER = "885be400800000000000000000000000000000c8"
STATE = {"temperature": 26, "power": False, "off_timer": False,
         "fan_speed": "AUTO", "vertical_swing": False, "mode": "COOL",
         "on_timer": False, "horizontal_swing": False}

print "Connecting to the Arduino"
ser = serial.Serial(SERIAL_PORT, 9600)

print "Stopping the aircon"
ser.write('SEND {} {}\n'.format(HEADER, bin2hex(encode(STATE))))

print "Waiting 10sec"
time.sleep(10)

print "Starting the aircon"
STATE['power'] = True
ser.write('SEND {} {}\n'.format(HEADER, bin2hex(encode(STATE))))
