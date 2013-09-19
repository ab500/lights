#!/usr/bin/python

import socket
import struct

print 'Welcome to the LightServer Socket library test client!'

port = 5001
host = ''

# Connect to the local light server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))

# Send a basic command
payload = struct.pack('IB', 5, 0)

print payload + 'abcd' 
s.send(payload + 'abcd')

payload = s.recv(8, socket.MSG_WAITALL)

print 'received ack ' + payload[4:]
# Close the connection
raw_input("Press any key to close the connection...")
s.close()

