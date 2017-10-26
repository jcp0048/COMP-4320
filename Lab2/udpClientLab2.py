import socket
import sys
import struct
import binascii
import time

# Here argument 1 is the Server name(ie. tux057.eng.auburn.edu)
# And argument 2 is the port number (ie. 10031)
# Argument 3 is the request ID (a number from 0-127)
# Argument 4 to argument n is a hostname (ie. www.google.com)

if len(sys.argv) < 5:
	print "you must input argument hostname, followed by the port#, followed by a requestID number (0-127), then one more more strings of websites like www.google.com !"
	sys.exit()
	
print "sys.argv[1] " + sys.argv[1]
print "socket.gethostbyname(sys.argv[1]) " + socket.gethostbyname(sys.argv[1])

BUFFER_SIZE = 256
UDP_IP = socket.gethostbyname(sys.argv[1])
UDP_PORT = int(sys.argv[2])
requestID = int(sys.argv[3])
hostNameArg = (sys.argv[4:]) # joins the string arguments with spaces???

startTime = time.time() # The start time

sock = socket.socket(socket.AF_INET, # Internet 
	socket.SOCK_DGRAM) # UDP

# Message Format:
# |Magic# 0x4A6F7921| TML | GID | Checksum | RequestID | Length1 | Hostname 1 | ... | Ln | Hn |
# | 	4 bytes		|  2  |  1  |     1    |      1    |    1    |  variable  | ... | 1  |var |

"""
	TODO: Should eventually check if the magic number (send and recieve) works on other teams' clients and servers.
"""

magicNum = 0x4A6F7921
GID = 21 # This is our group number 21 + 10010
checksum = 00000000 # this is temporary 
TML = 0 # This is a 2byte value.

# We don't know the TML, so we just do the parts we know first.
packet = struct.pack("!BBB", GID, checksum, requestID)
TML = 3 # At this point

print "packet: " 
print struct.unpack_from("!BBB", packet)

"""
	TODO: Calculate and program the checksum. It is calculated by adding the binary of each byte. In the event of a carry in the addition, add the carry to the Least Significant Bit in the result and continue to add the next number. At the end, take the number we get and do 1's compliment on it and that number will be our checksum.
	(note: the checksum we get should give us 1111 1111 when added to the result of all the other bytes combined.)
"""

# TML can have a total value of 65535??? Is this the buffer we should use?

for x in hostNameArg: 
	packet = packet + struct.pack("!B",len(x))
	TML = TML + 1 # Add 1 byte for the length value
	packet = packet + struct.pack("!" + str(len(x)) + "s", x)
	TML = TML + len(x) # Add the length of our string var
	
# Now we finally at the magic number and TML(now that we have it) in the front.
TML = TML + 4 + 2 # But not without adding 4 bytes for the magicNum and 2 for the TML First

#TEst
TML = 65534 # Need to delete this...
packet = struct.pack("!IH", magicNum, TML) + packet
	
print "packet = "
# We can test our packed string by hardcoding the value, here we assume we have one input (www.google.com)
# print struct.unpack_from("!IIIIII14s", packet) 
# We can test our packed string by hardcoding the value, here we assume we have two input (www.google.com then www.bing.com)

print "# |Magic# 0x4A6F7921| TML | GID | Checksum | RequestID | Length1 | Hostname 1 | ... | Ln | Hn | "
print "# | 	4 bytes		|  2  |  1  |     1    |      1    |    1    |  variable  | ... | 1  |var |"

print struct.unpack_from("!IHBBBB14sB12s", packet) 

for x in hostNameArg:
	print x

sock.sendto(packet, (UDP_IP, UDP_PORT)) # Sends the packet

data = sock.recvfrom(1024) # Putting 1024 here for now... 

print "type(data[0])" + str(type(data[0]))

print "type(data) " + str(type(data))
realData = data[0]
#reMagicNum = realData[0:4]
#reTML = realData[4:6]
#reGID = realData[6]
#reChecksum = realData[7]
#reReqestID = realData[8]

reHeader = struct.unpack_from("!IHBBB", realData, 0)
#reHeader = struct.unpack_from("!IHBBB", data, 0)
print "reHeader: " + str(reHeader)

"""
	TODO: Create a seperate path for if the last byte is BEC instead pf Request ID.
	Need to find a way to differciate that byte between a Request ID and 
	Byte Error Code.
	
	The error path should see the error and display an error message, then terminate.
	
	The correct path should carry on as normal, terminating after its run.
"""

reTML = reHeader[1]
print "reTML: " + str(reTML)

#print "Magic Number: " + str(reMagicNum)
#print "TML: " + str(int(reTML))
#print "GID: " + str(ord(reGID))
#print "Checksum: " + str(ord(reChecksum))
#print "RequestID: " + str(ord(reReqestID))

print "len of data is :" + str(len(realData))

iter = 9 # Starting pos for IP addrs
print str(realData[9: 17]) 
IPCount = (reTML - 9)/4 # Get the count of IPs returned.
FourS_Str = ""
# Try and form the "4s"'s required to unpack all our IPs together
for num in range(0,IPCount): # This might be a very stupid way to do this, but can't think of much else ATM, will change if can later. :/
	FourS_Str = FourS_Str + "4s"

IPs = struct.unpack_from("!" + FourS_Str, realData, 9)
print "My IPs: " + str(struct.unpack_from("!" + FourS_Str, realData, 9))

"""
	TODO: Figure out if IPs coming back are correct. Likely not. So how to read or send or recieve them correctly...???
"""

for ip in IPs:
	print ip

print "Elasped time: " + str(time.time() - startTime)