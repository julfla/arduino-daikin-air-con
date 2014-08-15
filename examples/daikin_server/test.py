#!/usr/bin/python

""" This test script should restart you aircon. """

import json
import time
import requests

HOST = 'http://localhost:8000/'
STATE = {"temperature": 26, "power": False, "off_timer": False,
         "fan_speed": "AUTO", "vertical_swing": False, "mode": "COOL",
         "on_timer": False, "horizontal_swing": False}

print "Stopping the aircon"
requests.post(HOST, data=json.dumps(STATE))

print "Waiting 10sec"
time.sleep(10)

print "Starting the aircon"
STATE['power'] = True
requests.post(HOST, data=json.dumps(STATE))
