import socket
import sys
import struct
import binascii

# Here argument 1 is the hostname(www.website.com)
# And argument 2 is the port number
# The rest are yet to be implemented

if len(sys.argv) < 3:
	print "you must input argument hostname, followed by the port#!"
	sys.exit()

TCP_IP = socket.gethostbyname(sys.argv[1])
TCP_PORT = int(sys.argv[2])
BUFFER_SIZE = 256
MESSAGE = "Hello, World!"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)
# data = s.recvfrom(BUFFER_SIZE)
data = s.recv(BUFFER_SIZE)

requestID = 1;
while True:

	operation = input("Please select an operation: NumberOfConsonants(1), Disemvoweling(2), Uppercasing(3), or Quit(3)")
	if operation == 1:
		strArg = raw_input('please type the string uh you want: \n')
		#TML = sys.getsizeof(requestID) 
		#TML = TML + sys.getsizeof(strArg) + 1
		TML = len(strArg) + 3 #variable length + TML + requestID + Op
		print "sys.getsizeof(requestID): " + str(sys.getsizeof(chr(1))) + "\n"
		print "sys.getsizeof(strArg): " + str(sys.getsizeof(list(strArg))) + "\n"
		packet = struct.pack("!BBB" + str(len(strArg)) + "s", TML, requestID,operation, strArg)
		#print "sys.getsizeof(packet): " + str(sys.getsizeof(packet)) + "\n"
		values = (TML, int(requestID), int(operation), strArg)
		packer = struct.Struct('I I I ' + str(len(strArg)) + 's' )
		packed_data = packer.pack(*values)
		print sys.stderr, 'sending "%s"' % binascii.hexlify(packed_data), values
		#print str(sys.getsizeof(packed_data))
		#s.send(packed_data)
		s.send(packet)
		
	elif operation == 2:
		print 'you chose 2 ah'
	elif operation == 3:
		print 'you chose 3 ah'
		strArg = raw_input('please type the string uh you want: \n')
		TML = len(strArg) + 3 #variable length + TML + requestID + Op
		print "sys.getsizeof(requestID): " + str(sys.getsizeof(chr(1))) + "\n"
		print "sys.getsizeof(strArg): " + str(sys.getsizeof(list(strArg))) + "\n"
		packet = struct.pack("!BBB" + str(len(strArg)) + "s", TML, requestID,operation, strArg)
		#print "sys.getsizeof(packet): " + str(sys.getsizeof(packet)) + "\n"
		values = (TML, int(requestID), int(operation), strArg)
		packer = struct.Struct('I I I ' + str(len(strArg)) + 's' )
		packed_data = packer.pack(*values)
		print sys.stderr, 'sending "%s"' % binascii.hexlify(packed_data), values
		#print str(sys.getsizeof(packed_data))
		#s.send(packed_data)
		s.send(packet)
		data = s.recv(BUFFER_SIZE)
		
		print "data: " + str(data)
		dataArray = list(data)
		#ReTML, RerequestID, RestrArg = struct.unpack("!II" + str(len(data)- 2) + "c", data)
		#ReTML, RerequestID, RestrArg = struct.unpack("!II" + str(ReTML - 2) + "c", data)
		#fields = struct.unpack("!II" + str(len(data) -2) + "c", data)
		
		#(ReTML,), data = struct.unpack("I", data[:1]), data[1:]
		#(RerequestID,), data = struct.unpack("I", data[:1]), data[1:]
		#RestrArg = struct.unpack(len(data) + "c", data[0:])
		
		print str(chr(dataArray[0]))
		print str(chr(dataArray[1]))
		print str(dataArray[2])
		
		#print " string(ReTML)" + string(ReTML)
		#ReTML = field[0]
		#RerequestID = field[1]
		#RestrArg = field[2]
		
		#print str(dataArray[0]) + str(dataArray[1]) + str(dataArray[2])
		#print str(ReTML) + str(RerequestID) + str(RestrArg)
	elif operation == 4:
		print 'you quit'
		break
	else: 
		print "Please choose only from number 1,2,or 3"
	
	



s.close()

print "received data:", data