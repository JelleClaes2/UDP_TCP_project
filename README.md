# UDP_TCP_project
1- Make a UDP random number generator that will return the highest number given by the UDP-server.

2- Make a TCP random mathematical operation generator that returns the correct solution corresponding to the received equation from the TCP-client.

## Checklist

- [x] [1] UDP socket is made correctly and connects.
- [x] [2] UDP client sends 'GO' and is received by the UDP server.
- [x] [3] UDP server sends a max of 42 random numbers in network-byte-order and UDP client receives this data.
- [x] [4] UDP client sends highest number back. UDP server expects this return.
- [x] [5] UDP client resends highest number after timeout of 1 second. UDP server expects answer in 3 seconds.
- [x] [6] After a couple resends, UDP server will send next 42 random numbers. UDP-client stops sending highest number and receives.
- [x] [7] UDP client sends highest number back. UDP server expects this return.
- [x] [8] UDP client resends highest number after timeout of 1 second. UDP server expects answer in 3 seconds.
- [x] [9] After a couple resends, UDP server will send 'OK' and connection is cut off.
- [ ] [10] The TCP communication will start when UDP connection is shut off without human interference.
- [x] [11] TCP socket is correctly made and connects.
- [x] [12] TCP client sends a random mathematical operation, the TCP server receives this and splits it up.
- [x] [13] TCP server returns the correct solution of the operation to the TCP client who accepts and double checks.
- [x] [14] TCP client sends a second random mathematical operation, the TCP server receives this and splits it up.
- [x] [15] TCP server returns the correct solution of the second operation to the TCP client who accepts and double checks.
- [x] [16] TCP client continues to send random mathematical operations, the TCP server receives these and splits them up until it receives "STOP", this is received correctly.
- [x] [17] TCP server sends "OK", TCP client returns with "KTNXBYE".
- [x] [18] TCP client shuts off connection and TCP server waits to shut off connection.
- [ ] [19] Full program works without crashing and uses its resources efficiently.
- [ ] [20] Professional GitHub with code, multiple commits and explanations.
