#!/usr/bin/env python

from optparse import OptionParser
import serial
import sys
import time
import struct
import datetime

parser = OptionParser()
parser.add_option("-p", "--portnum", help="select the ttyUSB port number, default 0", type="int", default=0)
parser.add_option("-b", "--baudrate", help="select the baudrate to use, default 500000", type="int", default="500000")
    
options, args = parser.parse_args()

# Define the log function (print to stderr)
def log(message):
    sys.stderr.write(message + "\n")
    sys.stderr.flush()

# Define the Sniffer class
class Sniffer:
    # Startup bytes
    STARTUP = [0xAB, 0xCD]
    STARTCHAN = [0xAB, 0xCC]
    
    # States
    STATE_START_0 = 0
    STATE_START_1 = 1
    STATE_LENGTH = 2
    STATE_DATA = 3
    
    # Wireshark constants
    WS_magic_number = 0xa1b2c3d4 # magic number
    WS_version_major = 2 # major version number
    WS_version_minor = 4 # minor version number
    WS_thiszone = 0 # GMT to local correction
    WS_sigfigs = 0 #  accuracy of timestamps
    WS_snaplen = 65535 #  max length of captured packets, in octets
    WS_network = 195 #  data link type (DLT) - IEEE 802.15.4

    def __init__(self):
        # Initialize variables
        self.len = 0
        self.data = []
        self.state = Sniffer.STATE_START_0
        
        # Write pcap header
        self.write_global_header()
        
        self.mode_packet = True

    def write_global_header(self):
        # Write header
        header = struct.pack("!LHHlLLL", self.WS_magic_number,
                    self.WS_version_major, self.WS_version_minor,
                    self.WS_thiszone, self.WS_sigfigs, self.WS_snaplen,
                    self.WS_network)
        sys.stdout.write(header)
        sys.stdout.flush()

    def parse(self, data):
        # Parse each byte individually
        for i in data:
            self.parse_one(i)
        # Flush stderr
        sys.stderr.flush()

    def parse_one(self, i):
        # Switch on the actual state
        if self.state == Sniffer.STATE_START_0:
            # Expecting the first STARTUP byte:
            if i == Sniffer.STARTUP[0]:
                # Good, Store state and return
                self.state = Sniffer.STATE_START_1
                return
            else:
                # Bad byte, discard
                return
        elif self.state == Sniffer.STATE_START_1:
            # Expecting second STARTUP byte
            if i == Sniffer.STARTUP[1]:
                # Good, store state and return
                self.state = Sniffer.STATE_LENGTH
                self.mode_packet = True
                return
            elif i == Sniffer.STARTCHAN[1]:
                self.state = Sniffer.STATE_LENGTH
                self.mode_packet = False
                return
        elif self.state == Sniffer.STATE_LENGTH:
            
            if not self.mode_packet:
                log("[%s] Channel %u..." % (time.time(), i))
                # Reset state
                self.state = Sniffer.STATE_START_0
                return
            
            # Store the Length
            self.len = i
            
            # Reset data
            self.data = []
            
            # Update state
            self.state = Sniffer.STATE_DATA
            return
        else:
            # Store byte
            self.data.append(i)
            
            # Check if length is reached
            if len(self.data) == self.len:
                # Print packet
                self.print_packet()
                
                log("[%s] \t Pkt(%u): %s" % (time.time(), len(self.data), ":".join(["%02X" % x for x in self.data])))
                
                # Reset state
                self.state = Sniffer.STATE_START_0
                
    def print_packet(self):
        # Set timestamp
        now = time.time()

        # Get packet attributes
        sec = int(now)
        usec = int((now - sec) * 1e6)
        incl_len = self.len
        orig_len = self.len + 2 # Including the FCS fields
        
        # Write packet header
        header = struct.pack("!LLLL", sec, usec, incl_len, orig_len)
        sys.stdout.write(header)
        
        # Write packet
        sys.stdout.write("".join([chr(i) for i in self.data[:incl_len]]))
        
        # Flush
        sys.stdout.flush()

if __name__ == "__main__":
    # Open the serial port
    port = "/dev/ttyUSB%i" % options.portnum
    ser = serial.Serial(port, options.baudrate)
    
    # Print Started
    log("Sniffer Bridge Started on port %s at %i baud" % (port, options.baudrate))
    log("You may use \"%s | wireshark -k -i -\"" % " ".join(sys.argv) + 
        " to have an interactive wireshark sniffing")
    sniffer = Sniffer()
    while True:
        try:
            # Read some data
            data = [ord(i) for i in ser.read(1)]
            
            # Parse
            sniffer.parse(data)
        except:
            break
    
    # Close the serial port
    ser.close()

