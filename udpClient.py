import socket
import sys

# Here argument 1 is the hostname(www.website.com)
# And argument 2 is the port number
# The rest are yet to be implemented

if len(sys.argv) < 3:
	print "you must input argument hostname, followed by the port#!"
	sys.exit()

print "sys.argv[1] " + sys.argv[1]
print "socket.gethostbyname(sys.argv[1]) " + socket.gethostbyname(sys.argv[1])
UDP_IP = socket.gethostbyname(sys.argv[1])
UDP_PORT = int(sys.argv[2])

MESSAGE = "Hello, World~"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE

sock = socket.socket(socket.AF_INET, # Internet 
	socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

data, addr = sock.recvfrom(256) # buffer size is 256 bytes
print "received message:", data