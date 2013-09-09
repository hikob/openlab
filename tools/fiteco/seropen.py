#!/usr/bin/env python

import serial
import sys

if len(sys.argv) > 1:
	port = sys.argv[1]
else:
	port = "/dev/ttyUSB1"

if len(sys.argv) > 2:
	baud = int(sys.argv[2])
else:
	baud = 500000

s = serial.Serial(port, baud)

print "[Opened Serial Port %s @%s]" % (port, baud)

try:
	while True:
		data = s.read()
		sys.stdout.write(data)
		sys.stdout.flush()

except:
	pass

s.close()
print "[Closed Serial Port %s]" % port

