# F15RobotProject
# Group 9: Mitchell Devenport, Nathan Jones, Richard Orr, and Sameer Singh

Description: This project includes a UDP client which sends requests over UDP to a proxy server, which then processes the request and sends a request across a TCP connection to a robot. The robot sends data back to the proxy over TCP which is then sent back to the original client over UDP.

Known Problems: None

Design: 

The UDP side of the Proxy uses an API to communicate with the TCP side of the proxy. An API call is provided for each type of data the robot will send back. When the API function is called, a connection is created with the appropriate port on the robot's IP address and a HTTP get request is sent to the robot to request the appropriate data. When the data is received, the header is processed and the payload is returned by the API to the user.