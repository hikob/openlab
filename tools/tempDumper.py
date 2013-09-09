#!/usr/bin/env python
 # coding=utf8
from optparse import OptionParser
import serial
import sys
import time
import struct
import datetime
import re

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

    data_str = []

    # Print Started
    while True:
        try:
            data = ser.read(1)
            
            if data == '\n':
                s = "".join(data_str);
                matchObj = re.match( r'.*\(T=([0-9\.]+)°C\)', s, re.M|re.I)
                if matchObj:
                    log("%s %s %s" % (time.time(), str(datetime.datetime.now()), matchObj.group(1)))
                else:
                    sys.stdout.write("%s\n" % s)
                    sys.stdout.flush()
                     
                data_str = []
            else:
                data_str.append(data)
                
        except:
            break
    
    # Close the serial port
    ser.close()

