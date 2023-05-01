# UDP_TCP_project
This is a C program that consists of two parts - a UDP server that generates random numbers and a TCP server that performs mathematical operations. This program demonstrates the two different types of communication in a network and how to perform tasks over a network.

## UDP Server
The UDP server generates a maximum of 42 random numbers and sends them in network-byte-order to the client upon receiving the message "GO". The client then finds the highest number and sends it back to the server. If the server does not receive a response within 3 seconds, it sends the same set of random numbers again, and the client retries to send the highest number. This process continues until the server sends the message "OK" and disconnects. Below you will find the image used as reference for this project and for an extra insight in the actual communication between the server and the client.
![UDP-opdracht](https://user-images.githubusercontent.com/114076101/235452786-43e12cd2-6842-44b7-841d-d53661cb108e.png)
### Checklist
- [x] UDP socket is made correctly and connects.
- [x] UDP client sends 'GO' and is received by the UDP server.
- [x] UDP server sends a max of 42 random numbers in network-byte-order and UDP client receives this data.
- [x] UDP client sends highest number back. UDP server expects this return.
- [x] UDP client resends highest number after timeout of 1 second. UDP server expects answer in 3 seconds.
- [x] After a couple resends, UDP server will send next 42 random numbers. UDP-client stops sending highest number and receives.
- [x] UDP client sends highest number back. UDP server expects this return.
- [x] UDP client resends highest number after timeout of 1 second. UDP server expects answer in 3 seconds.
- [x] After a couple resends, UDP server will send 'OK' and connection is cut off.
## TCP Server
The TCP server generates the correct solutions for the mathematical operations it receives from the client. The client sends the server a string with a randomly generated mathematical operation, and the server returns the solution. This process continues until the client sends the message "STOP", which the server receives and responds with the message "OK". The client then sends the message "KTNXBYE" to shut down the connection. Below you will find the image used as reference for this project and for an extra insight in the actual communication between the server and the client.
![TCP-opdracht](https://user-images.githubusercontent.com/114076101/235452769-65a17000-8ab5-4d3a-950e-888731459a71.png)
### Checklist
- [x] TCP communication will start when UDP connection is shut off without human interference.
- [x] TCP socket is correctly made and connects.
- [x] TCP client sends a random mathematical operation, the TCP server receives this and splits it up.
- [x] TCP server returns the correct solution of the operation to the TCP client who accepts and double checks.
- [x] TCP client sends a second random mathematical operation, the TCP server receives this and splits it up.
- [x] TCP server returns the correct solution of the second operation to the TCP client who accepts and double checks.
- [x] TCP client continues to send random mathematical operations, the TCP server receives these and splits them up until it receives "STOP", this is received correctly.
- [x] TCP server sends "OK", TCP client returns with "KTNXBYE".
- [x] TCP client shuts off connection and TCP server waits to shut off connection.
## Conclusion
This program demonstrates the ability to communicate over a network with both UDP and TCP communication forms. This code can be used as a foundation for future programs and as a learning experience for new coders learning about network communications. 
