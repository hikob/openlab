#!/usr/bin/env python

from optparse import OptionParser
import serial
import sys
import time
import struct
import datetime

parser = OptionParser()
parser.add_option("-p", "--portname", help="select the USB port name, default USB1", type="string", default="USB1")
parser.add_option("-b", "--baudrate", help="select the baudrate to use, default 500000", type="int", default="500000")
    
options, args = parser.parse_args()

# Define the log function (print to stderr)
def log(message):
    sys.stderr.write(message + "\n")
    sys.stderr.flush()

if __name__ == "__main__":
    # Open the serial port
    port = "/dev/tty%s" % options.portname
    ser = serial.Serial(port, options.baudrate)
    
    # Print Started
    while True:
        try:
            # Read some data
            data = ser.read(1)
            
            # Parse
            sys.stdout.write(data)
            sys.stdout.flush();
        except:
            break
    
    # Close the serial port
    ser.close()

