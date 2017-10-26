import socket *

UDP_IP_ADDRESS = "127.0.0.1"
UDP_PORT_NO = 6789
Message = "Hello, Server"

#sends the UDP message
clientSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
clientSock.sendto(Message, (UDP_IP_ADDRESS, UDP_PORT_NO))

# listens for UDP messages
serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# binds our declared IP address and port number to serverSock
serverSock.bind((UDP_IP_ADDRESS, UDP_PORT_NO))

while True:
    data, addr = serverSock.recvfrom(1024)
    print "Message: ", data
