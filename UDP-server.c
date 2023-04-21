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

int initialization();
void execution( int internet_socket );
void cleanup( int internet_socket );
int randomNumber();
uint16_t sendNumber(int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_send);
void receiveNumber(char buffer[1000],uint16_t highestNumber,int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_received);

int main( int argc, char * argv[] )
{
    srand(time(NULL));

    //initialize the socket and the OS
    OSInit();

    int internet_socket = initialization();

    //execute the code for the server

    execution( internet_socket );

    //clean everything

    cleanup( internet_socket );

    OSCleanup();

    return 0;
}

int initialization()
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

void execution( int internet_socket )
{
    //declare parameters for the execution
    int number_of_bytes_send = 0;
    int number_of_bytes_received = 0;
    char buffer[1000];
    struct sockaddr_storage client_internet_address;
    socklen_t client_internet_address_length = sizeof client_internet_address;

    //need to receive GO before sending the numbers
    while(strcmp(buffer,"GO")!=0){
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
    uint16_t highestNumber=sendNumber(internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_send);

    //receive the highest number from the client and check if it was the highest number the server send
    receiveNumber(buffer,highestNumber,internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_received);

    //send 42 random numbers
    highestNumber=sendNumber(internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_send);

    //receive the highest number from the client and check if it was the highest number the server send
    receiveNumber(buffer,highestNumber,internet_socket,client_internet_address,client_internet_address_length,number_of_bytes_received);

    //if the client send the highest number 2 times correctly the server sends OK back
    number_of_bytes_send = sendto( internet_socket,"OK",2, 0, (struct sockaddr *) &client_internet_address, client_internet_address_length );
    if( number_of_bytes_send == -1 )
    {
        perror( "sendto" );
    }
}

void cleanup( int internet_socket )
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

uint16_t sendNumber(int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_send){
    uint16_t number = 0;
    uint16_t net_num = htons(number);
    uint16_t highestNumber = 0;

    //loop 42 times
    for(int i=0;i<42;i++){
        number = randomNumber();//generate a random number
        printf("random number = %d\n",number);
        net_num = htons(number);
        if(number > highestNumber){
            highestNumber = number;//keep track of the highest number
        }
        number_of_bytes_send = sendto( internet_socket, &net_num, sizeof(net_num), 0, (struct sockaddr *) &client_internet_address, client_internet_address_length );//send the number
        if( number_of_bytes_send == -1 )
        {
            perror( "sendto" );
        }
    }
    return highestNumber;
}

void receiveNumber(char buffer[1000],uint16_t highestNumber,int internet_socket,struct sockaddr_storage client_internet_address,socklen_t client_internet_address_length,int number_of_bytes_received){
    //need to receive the highest number
    while(ntohs(buffer) != highestNumber){
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
}
