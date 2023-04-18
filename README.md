# UDP_TCP_project
1- Make a UDP random number generator that will return the highest number given by the UDP-server.

2- Make a tcp random mathematical operation generator that returns the correct solution corresponding to the received equation from the TCP-client.

## Checklist

- [x] UDP socket is made correctly and connects.
- [x] UDP client sends 'GO' and is received by the UDP server.
- [x] UDP server sends a max of 42 random numbers in network-byte-order and UDP client receives this data.
- [x] UDP client sends highest number back. UDP server expects this return.
- [x] UDP client resends highest number after timeout of 1 second. UDP server expects answer in 3 seconds.
- [x] After a couple resends, UDP server will send next 42 random numbers. UDP-client stops sending highest number and receives.
- [x] UDP client sends highest number back. UDP server expects this return.
- [x] UDP client resends highest number after timeout of 1 second. UDP server expects answer in 3 seconds.
- [x] After a couple resends, UDP server will send 'OK' and connection is cut off.
- [x] The TCP communication will start when UDP connection is shut off without human interference.
- [x] TCP socket is correctly made and connects.
- [x] TCP client sends a random mathematical operation, the TCP server receives this and splits it up.
- [x] TCP server returns the correct solution of the operation to the TCP client who accepts and double checks.
- [x] TCP client sends a second random mathematical operation, the TCP server receives this and splits it up.
- [x] TCP server returns the correct solution of the second operation to the TCP client who accepts and double checks.
- [x] TCP client continues to send random mathematical operations, the TCP server receives these and splits them up and sends "STOP", this is received correctly.
- [x] TCP server sends "OK", TCP client returns with "KTNXBYE".
- [x] TCP client shuts off connection and TCP server waits to shut off connection.
- [x] Full program works without crashing and uses its resources efficiently.
- [x] Professional GitHub with code, multiple commits and explanations.
