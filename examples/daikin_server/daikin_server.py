#!/usr/bin/python

"""
>>> python daikin_server.py 0.0.0.0 8001
Serving on 0.0.0.0:8001

or simply

>>> python daikin_server.py
Serving on localhost:8000

You can use this to test GET and POST methods.

"""

import SimpleHTTPServer
import SocketServer
import logging

import sys
import json
import serial
from daikin_protocol import encode, bin2hex

IR_HEADER = "885be400800000000000000000000000000000c8"
SERIAL_PORT = "/dev/ttyACM0"
DEFAULT_PORT = 8000

if len(sys.argv) > 2:
    PORT = int(sys.argv[2])
    I = sys.argv[1]
elif len(sys.argv) > 1:
    PORT = int(sys.argv[1])
    I = ""
else:
    PORT = DEFAULT_PORT
    I = ""


logging.info("Connecting to the Arduino")
ser = serial.Serial(SERIAL_PORT, 9600)


class ServerHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):

    def do_GET(self):
        logging.warning("======= GET STARTED =======")
        # We do not accept GET requests
        self.send_error(404)

    def do_POST(self):
        logging.warning("======= POST STARTED =======")
        logging.warning(self.headers)
        content_length = int(self.headers.getheader('content-length'))
        aircon_state = json.loads(self.rfile.read(content_length))
        logging.info(aircon_state)
        ser.write('SEND {} {}\n'.format(IR_HEADER, bin2hex(encode(aircon_state))))
        self.send_response(200, "MESSAGE SENT")

Handler = ServerHandler

httpd = SocketServer.TCPServer(("", PORT), Handler)

print "@rochacbruno Python http server version 0.1 (for testing purposes only)"
print "Serving at: http://%(interface)s:%(port)s" % dict(interface=I or "localhost", port=PORT)
httpd.serve_forever()
