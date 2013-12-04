#!/usr/bin/env python

#
# Copyright 2013 HiKoB, all rights reserved
#

import sys
import time
import serial
import struct
import threading
import traceback

class Handler:
    def __init__(self):
        pass
    
    def process_indication(self, opcode, data):
        print "Got indication %x" % opcode, data

class SerialIf(threading.Thread):
    
    SYNC = 0x80
    HEADER_LENGTH = 2
    
    def __init__(self, port, baudrate=500000):
        """ Create and open a IOTLAB Serial interface
        
            Keyword arguments:
            port -- the serial port path to open 
            baudrate -- the baudrate to use (default 500000)
        """
        
        threading.Thread.__init__(self)
        # Store parameters
        self.port = port
        self.baudrate = baudrate
        
        # Running variables
        self.running = False
        self.debug = ""
        
        # Handlers
        self.commands = {}
        
        # Semaphore for command/response
        self.lock = threading.Lock()
        
        # Current command category/opcode
        self.cmd = 0
        self.resp_data = []
        
        # Open serial port
        self.serial = serial.Serial(self.port, self.baudrate, timeout=1)
        
    def register_handler(self, cmd, handler):
        """Associates a given handler with a command
    
        Keyword arguments:
        cmd -- the command number to register
        handler -- the Handler-derived object to process indications, or None
                    to remove association
        """
        if handler is not None:
            self.commands[cmd] = handler
        elif category in self.categories:
            del self.commands[cmd]
    
    def close(self):
        """ Close the serial port and stop the thread
        """
        
        self.serial.close()
        self.running = False
    
    def send_command(self, command, data=[]):
        """Send a command with the given parameters and return the response
    
        Keyword arguments:
        command -- the command number of the command
        data -- the payload of the command (default: no data)
        
        Return:
        the received response, or None on timeout
        """
        
        # Prepare command
        cmd = [self.SYNC, len(data) + 1, command] + data
        cmd = "".join([chr(i) for i in cmd])
        
        # Wait for semaphore availability
        self.lock.acquire(True)
        
        # Store parameters
        self.cmd = command
        
        # Send command
        self.serial.write(cmd)
        
        # Start a timer, 1 second
        tim = threading.Timer(1.0, self._command_timeout)
        tim.start()
        
        # Wait for response
        self.lock.acquire(True)
        
        # Cancel timer
        tim.cancel();
        
        # Get response and clear parameters
        resp_data = self.resp_data
        self.cmd = 0
        
        # Release lock
        self.lock.release()
        
        # Return response
        return resp_data

    def run(self):
        type = 0
        length = 0
        data = []
        
        index = 0
        self.running = True
        while self.running:
            try:
                # Read next char
                d = self.serial.read()
                if len(d) == 1:
                    b = ord(d)
                else:
                    continue
            except Exception as e:
                print "[SmartIF] Closing: ", e
                return
            
            if index == 0:
                # Check for start byte
                if b != self.SYNC:
                    self._process_debug(chr(b))
                    continue
                index += 1
            
            elif index == 1:
                # This is the length
                length = b
                index += 1
                data = []
            
            elif index - self.HEADER_LENGTH < length:
                # Data byte
                data.append(b)
                index += 1
                
            if index - self.HEADER_LENGTH == length:
                # Valid data
                self._process_input(data)
                index = 0

    def _process_debug(self, debug):
        if debug == "\n":
            self.debug = ""
        else:
            self.debug += debug
        
    def _process_input(self, data):
        cmd = data[0]
        data = data[1:]
        
        if self.cmd == cmd:
            # Store response and release lock
            self.resp_data = data
            self.lock.release()
        
        else:
            if cmd in self.commands:
                self.commands[cmd].process_indication(cmd, data)
            else:
                print "No handler for cmd %x" % cmd
                
    def _command_timeout(self):
        print "[SmartIF] Command timeout"
        self.resp_data = None
        self.lock.release()



class SniffInput:
    def __init__(self):
        pass
    
    def process_indication(self, cmd, data):
        if len(data) < 8:
            print "ERROR length too small"
            return
        
        ts = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24)
        chan = data[4]
        rssi = -128 + ((data[5] + 128) % 256)
        lqi = data[6]
        length = data[7]
        data = data[8:]
        print "Sniff: %u\tch %u\trssi %i\tlqi %u\tlen %u\t" % (ts, chan, rssi, lqi, length), ":".join(["%02x" % i for i in data])
        if len(data) != length:
            print "ERROR bad length %u" % len(data), data 
            
            
class RadioPollInput:
    def __init__(self):
        pass
    
    def process_indication(self, cmd, data):
        if len(data) < 4:
            print "ERROR length too small"
            return
        
        ts = data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24)
        data = data[4:]
        print "Poll: %u\t" % ts, " ".join(["%s" % (((i + 128) % 256) - 128) for i in data])

if __name__ == "__main__":
    try:
        ser = SerialIf("/dev/ttyUSB0", 500000)
        ser.start()
        
        ser.register_handler(0xA1, SniffInput())
        ser.register_handler(0xA2, RadioPollInput())
        
        print "Stop Radio",
        ret = ser.send_command(0x60)
        if ret[0] == 0x0A:
            print "OK"
        else:
            print "Failed"
        
        if len(sys.argv) > 1:
            if sys.argv[1] in ("sniff", "sniffer", "Sniff", "Sniffer"):
                print "Start Sniffer",
                ret = ser.send_command(0x61, [0, 0x80, 0, 4, 0, 2])
                if ret[0] == 0x0A:
                    print "OK"
                else:
                    print "Failed"
            elif sys.argv[1] in ("poll", "Poll"):
                print "Start Polling",
                ret = ser.send_command(0x62, [26, 2, 0])
                if ret[0] == 0x0A:
                    print "OK"
                else:
                    print "Failed"
            elif sys.argv[1] in ("inject", "Inject", "injection", "Injection"):
                print "Start Injection",
                ret = ser.send_command(0x63, [0,0xF8,0,0,    20,0,    10,0,    0,0,0,0,   10])
                if ret[0] == 0x0A:
                    print "OK"
                else:
                    print "Failed"
            elif sys.argv[1] in ("jam", "Jam", "jamming", "Jamming"):
                print "Start Jamming",
                ret = ser.send_command(0x64, [0,0x80,0,4,    20,0,    0,0,0,0])
                if ret[0] == 0x0A:
                    print "OK"
                else:
                    print "Failed"
           
        
        while True:
            time.sleep(10)
        
    except:
        traceback.print_exc(file=sys.stderr)
    finally:
        print "Closing"
        ser.close()
    
