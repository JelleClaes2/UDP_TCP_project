#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h> //for all socket programming
#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
#include <stdio.h> //for fprintf, perror
#include <unistd.h> //for close
#include <stdlib.h> //for exit
#include <string.h> //for memset
void OSInit( void )
{
    WSADATA wsaData;
    int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
    if( WSAError != 0 )
    {
        fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
        exit( -1 );
    }
}
void OSCleanup( void )
{
    WSACleanup();
}
#define perror(string) fprintf( stderr, string ": WSA errno = %d\n", WSAGetLastError() )
#else
#include <sys/socket.h> //for sockaddr, socket, socket
#include <sys/types.h> //for size_t
#include <netdb.h> //for getaddrinfo
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
#include <errno.h> //for errno
#include <stdio.h> //for fprintf, perror
#include <unistd.h> //for close
#include <stdlib.h> //for exit
#include <string.h> //for memset
int OSInit( void ) {}
int OSCleanup( void ) {}
#endif

#include <time.h>
#include <stdint.h>

int initializationUDP();
void executionUDP( int internet_socket );
void cleanupUDP( int internet_socket );
int randomNumberUDP();
void sendNumberUDP(int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_send);
void receiveNumberUDP(char buffer[1000],int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_received);

int initializationTCP();
int connectionTCP( int internet_socket );
void executionTCP( int internet_socket );
void cleanupTCP( int internet_socket, int client_internet_socket );


int main( int argc, char * argv[] ) {

    srand(time(NULL));

    //START UDP
    printf("Start UDP server\n");
    printf("\n");
    //initialize the socket and the OS
    OSInit();

    int internet_socketUDP = initializationUDP();

    /* struct timeval timeout;
     timeout.tv_sec = 3;
     timeout.tv_usec = 0;
     if (setsockopt(internet_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,sizeof timeout) < 0){
         perror("setsockopt failed\n");
     }*/

    //execute the code for the server

    executionUDP(internet_socketUDP);

    //clean everything

    cleanupUDP(internet_socketUDP);

    OSCleanup();

    //END UDP

    //START TCP
    printf("\n");
    printf("Start TCP server\n");
    printf("\n");
    OSInit();

    int internet_socketTCP = initializationTCP();

    int client_internet_socketTCP = connectionTCP( internet_socketTCP );

    executionTCP( client_internet_socketTCP );

    cleanupTCP( internet_socketTCP, client_internet_socketTCP );

    OSCleanup();
    //END TCP

    printf("\n");
    printf("all ended well\n");

    return 0;
}

int initializationUDP()
{
    //set up the internet address
    struct addrinfo internet_address_setup;
    struct addrinfo * internet_address_result;
    memset( &internet_address_setup, 0, sizeof internet_address_setup );
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_DGRAM;
    internet_address_setup.ai_flags = AI_PASSIVE;
    int getaddrinfo_return = getaddrinfo( NULL, "24042", &internet_address_setup, &internet_address_result );
    if( getaddrinfo_return != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
        exit( 1 );
    }

    int internet_socket = -1;
    struct addrinfo * internet_address_result_iterator = internet_address_result;
    while( internet_address_result_iterator != NULL )
    {
        //open the socket
        internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
        if( internet_socket == -1 )
        {
            perror( "socket" );
        }
        else
        {
            //bind
            int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
            if( bind_return == -1 )
            {
                close( internet_socket );
                perror( "bind" );
            }
            else
            {
                break;
            }
        }
        internet_address_result_iterator = internet_address_result_iterator->ai_next;
    }

    freeaddrinfo( internet_address_result );

    if( internet_socket == -1 )
    {
        fprintf( stderr, "socket: no valid socket address found\n" );
        exit( 2 );
    }

    return internet_socket;
}

void executionUDP( int internet_socket )
{
    //declare parameters for the execution
    int number_of_bytes_send = 0;
    int number_of_bytes_received = 0;
    char buffer[1000];
    struct sockaddr_storage client_internet_address;
    socklen_t client_internet_address_length = sizeof client_internet_address;

    //need to receive GO before sending the numbers
    while (strcmp(buffer,"GO")!=0){
        number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
        if( number_of_bytes_received == -1 )
        {
            perror( "recvfrom" );
        }
        else
        {
            buffer[number_of_bytes_received] = '\0';
            printf( "Received : %s\n", buffer );
        }
    }

    //send 42 random numbers
    sendNumberUDP(internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_send);

    //receive the highest number from the client and check if it was the highest number the server send
    receiveNumberUDP(buffer,internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_received);

    //send 42 random numbers
    sendNumberUDP(internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_send);

    //receive the highest number from the client and check if it was the highest number the server send
    receiveNumberUDP(buffer,internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_received);

    //if the client send the highest number 2 times correctly the server sends OK back
    number_of_bytes_send = sendto( internet_socket,"OK",2, 0, (struct sockaddr *) &client_internet_address, client_internet_address_length );
    if( number_of_bytes_send == -1 )
    {
        perror( "sendto" );
    }
}

void cleanupUDP( int internet_socket )
{
    //close the socket
    close( internet_socket );
}

int randomNumber(){
    //generate a random number between 0 and 99
    uint16_t randomNumber = 0;
    randomNumber = rand() % 100;
    return randomNumber;
}

void sendNumberUDP(int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_send){
    uint16_t number = 0;
    uint16_t net_num = htons(number);

    //loop 42 times
    for(int i=0;i<42;i++){
        number = randomNumber();//generate a random number
        printf("random number = %d\n",number);
        net_num = htons(number);
        number_of_bytes_send = sendto( internet_socket, (const char*)&net_num, sizeof(net_num), 0, (struct sockaddr *) &client_internet_address, client_internet_address_length );//send the number
        if( number_of_bytes_send == -1 )
        {
            perror( "sendto" );
        }
    }
}

void receiveNumberUDP(char buffer[1000],int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_received){
    number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
    if( number_of_bytes_received == -1 )
    {
        perror( "recvfrom" );
    }
    else
    {
        buffer[number_of_bytes_received] = '\0';
        printf( "Received : %hd\n", ntohs(*((short*)buffer)) );
    }
}

int initializationTCP()
{
    //Step 1.1
    struct addrinfo internet_address_setup;
    struct addrinfo * internet_address_result;
    memset( &internet_address_setup, 0, sizeof internet_address_setup );
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_STREAM;
    internet_address_setup.ai_flags = AI_PASSIVE;
    int getaddrinfo_return = getaddrinfo( NULL, "24042", &internet_address_setup, &internet_address_result );
    if( getaddrinfo_return != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
        exit( 1 );
    }

    int internet_socket = -1;
    struct addrinfo * internet_address_result_iterator = internet_address_result;
    while( internet_address_result_iterator != NULL )
    {
        //Step 1.2
        internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
        if( internet_socket == -1 )
        {
            perror( "socket" );
        }
        else
        {
            //Step 1.3
            int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
            if( bind_return == -1 )
            {
                perror( "bind" );
                close( internet_socket );
            }
            else
            {
                //Step 1.4
                int listen_return = listen( internet_socket, 1 );
                if( listen_return == -1 )
                {
                    close( internet_socket );
                    perror( "listen" );
                }
                else
                {
                    break;
                }
            }
        }
        internet_address_result_iterator = internet_address_result_iterator->ai_next;
    }

    freeaddrinfo( internet_address_result );

    if( internet_socket == -1 )
    {
        fprintf( stderr, "socket: no valid socket address found\n" );
        exit( 2 );
    }

    return internet_socket;
}

int connectionTCP( int internet_socket )
{
    //Step 2.1
    struct sockaddr_storage client_internet_address;
    socklen_t client_internet_address_length = sizeof client_internet_address;
    int client_socket = accept( internet_socket, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
    if( client_socket == -1 )
    {
        perror( "accept" );
        close( internet_socket );
        exit( 3 );
    }
    return client_socket;
}

void executionTCP(int internet_socket) {
    char buffer[1000];
    int number_of_bytes_received;
    while (1) {
        // Step 3.1
        memset(buffer, 0, sizeof(buffer));
        number_of_bytes_received = 0;
        //char buffer[1000];
        number_of_bytes_received = recv(internet_socket, buffer, (sizeof buffer) - 1, 0);
        if (number_of_bytes_received == -1) {
            perror("recv");
            break;
        } else if (number_of_bytes_received == 0) {
            printf("Client disconnected\n");
            break;
        }

        buffer[number_of_bytes_received] = '\0';
        if (strcmp(buffer, "STOP\n") == 0) {
            printf("Received Stop\n");
            send(internet_socket, "OK\n", strlen("OK\n"), 0);

            number_of_bytes_received = recv(internet_socket, buffer, (sizeof buffer) -1 , 0);
            if(number_of_bytes_received == -1){
                perror("recv");
                break;
            }
            buffer[number_of_bytes_received] = '\0';
            if(strcmp(buffer, "KTNXBYE\n") == 0){
                printf("received KTNXBYE\n");
                break;
            }
        }
        printf("Received message: %s\n", buffer);
        //send(internet_socket, buffer, strlen(buffer), 0);
        // }

        /*while (1){
            memset(buffer, 0 , sizeof(buffer));
            number_of_bytes_received = recv(internet_socket, buffer, (sizeof buffer) -1 , 0);
            if(number_of_bytes_received == -1){
                perror("recv");
                break;
            }
            else if(number_of_bytes_received == 0){
                printf("Client disconnected\n");
                break;
            }
            buffer[number_of_bytes_received] = '\0';
            if(strcmp(buffer, "KTNXBYE\n") == 0){
                printf("received KTNXBYE\n");
                break;
            }*/

        // Step 3.2: parse the operation
        int num1, num2;
        char op;
        if (sscanf(buffer, "%d %c %d", &num1, &op, &num2) != 3) {
            printf("Invalid operation format\n");
            continue;
        }

        // Step 3.3: perform the calculation
        float result;
        switch (op) {
            case '+':
                result = num1 + num2;
                break;
            case '-':
                result = num1 - num2;
                break;
            case '*':
                result = num1 * num2;
                break;
            case '/':
                if(num2 == 0){
                    printf("Division By 0\n");
                    int number_of_bytes_sent = send(internet_socket, "Division by 0\n", strlen("Division by 0\n"), 0);
                    if (number_of_bytes_sent == -1) {
                        perror("send");
                        break;
                    }
                }
                else {
                    result = num1 / (float) num2;
                }
                break;
            default:
                printf("Invalid operator\n");
                continue;
        }

        // Step 3.4: send the result back to the client
        char result_str[100];
        snprintf(result_str, 5 , "%f" , result);
        printf("result string= %s\n",result_str);
        printf("result= %f\n",result);
        int number_of_bytes_sent = send(internet_socket, result_str, strlen(result_str), 0);
        if (number_of_bytes_sent == -1) {
            perror("send");
            break;
        } else {
            printf("Sent: %s\n", result_str);
        }
    }
}

void cleanupTCP( int internet_socket, int client_internet_socket )
{
    //Step 4.2
    int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
    if( shutdown_return == -1 )
    {
        perror( "shutdown" );
    }

    //Step 4.1
    close( client_internet_socket );
    close( internet_socket );
}
