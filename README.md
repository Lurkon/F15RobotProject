# F15RobotProject
# Group 9: Mitchell Devenport, Nathan Jones, Richard Orr, and Sameer Singh

Description: This project includes a UDP client which sends requests over UDP to a proxy server, which then processes the request and sends a request across a TCP connection to a robot. The robot sends data back to the proxy over TCP which is then sent back to the original client over UDP.

Known Problems: As the image server was down, there was no way to test our 
image retrieving functionality. As such, it is not activated in our code.
	Additionally, due to network latency, we were not able to actually 
trace polygons.

Design: 

The UDP side of the Proxy uses an API to communicate with the TCP side of the proxy. An API call is provided for each type of data the robot will send back. When the API function is called, a connection is created with the appropriate port on the robot's IP address and a HTTP get request is sent to the robot to request the appropriate data. When the data is received, the header is processed and the payload is returned by the API to the user.

The UDP client establishes a UDP socket connection with the proxy and then 
communicates, using the class protocol, each command in sequence. The 
client looks for a response after every command (with a 10 second timeout) 
before moving to the next command. If it does recieve a response, it saves 
the data according to what kind of data it was, including a timestamp. 
The data from each instrument is stored in a separate file. All the data 
from a single session is included in the file, complete with timestamp. 
This data is all erased when the client is run again.
