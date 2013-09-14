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
payload = struct.pack('I', 4)

print payload + 'abcd' 
s.send(payload + 'abcd')

# Close the connection
input("Press any key to close the connection...")
s.close()
