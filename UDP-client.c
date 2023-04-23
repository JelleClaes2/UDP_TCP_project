
#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h> //for all socket programming
#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
#include <stdio.h> //for fprintf, perror
#include <unistd.h> //for close
#include <stdlib.h> //for exit
#include <string.h> //for memset
#include <stdint.h>
#include <time.h>
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
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif

int initialization( struct sockaddr ** internet_address, socklen_t * internet_address_length );
void execution( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length );
void cleanup( int internet_socket, struct sockaddr * internet_address );

int main( int argc, char * argv[] )
{
    OSInit();

    struct sockaddr * internet_address = NULL;
    socklen_t internet_address_length = 0;
    int internet_socket = initialization( &internet_address, &internet_address_length );

    execution( internet_socket, internet_address, internet_address_length );

    cleanup( internet_socket, internet_address );

    OSCleanup();

    return 0;
}

int initialization( struct sockaddr ** internet_address, socklen_t * internet_address_length )
{
    //Step 1.1
    struct addrinfo internet_address_setup;
    struct addrinfo * internet_address_result;
    memset( &internet_address_setup, 0, sizeof internet_address_setup );
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_DGRAM;
    int getaddrinfo_return = getaddrinfo( "::1", "24042", &internet_address_setup, &internet_address_result );
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
            *internet_address_length = internet_address_result_iterator->ai_addrlen;
            *internet_address = (struct sockaddr *) malloc( internet_address_result_iterator->ai_addrlen );
            memcpy( *internet_address, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
            break;
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

void execution( int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length ) {
// Initialize counters for the number of times to execute the loop
    int num_loops = 2;
    int num_responses = 0;

// Loop until you have received two responses
    while (num_responses < num_loops) {
        uint16_t highest_number = 0;
        uint16_t current_number = 0;
        uint16_t previous_number = 0;

        //Step 2.1
        int number_of_bytes_send = 0;
        number_of_bytes_send = sendto(internet_socket, "GO", 16, 0, internet_address, internet_address_length);
        if (number_of_bytes_send == -1) {
            perror("sendto");
        }

        //Step 2.2
        int number_of_bytes_received = 0;
        char buffer[1000];
        number_of_bytes_received = recvfrom(internet_socket, buffer, (sizeof buffer) - 1, 0, internet_address,
                                            &internet_address_length);
        if (number_of_bytes_received == -1) {
            perror("recvfrom");
        } else {
            buffer[number_of_bytes_received] = '\0';
            printf("Received : %s\n", buffer);

            // Parse the numbers
            char *token = strtok(buffer, " ");
            while (token != NULL) {
                current_number = atoi(token);
                if (current_number > highest_number) {
                    highest_number = current_number;
                }
                token = strtok(NULL, " ");
            }
            printf("Highest number: %d\n", highest_number);

            // Send back the highest number every 1 second until you receive another 40 numbers
            time_t start_time = time(NULL);
            while (difftime(time(NULL), start_time) < 1 && num_responses == 0) {
                char response[3];
                sprintf(response, "%02d", highest_number);
                number_of_bytes_send = sendto(internet_socket, response, 2, 0, internet_address,
                                              internet_address_length);
                if (number_of_bytes_send == -1) {
                    perror("sendto");
                }
            }

            // Update the response counter if you have received the 40 numbers
            num_responses++;
        }
    }
}


void cleanup( int internet_socket, struct sockaddr * internet_address )
{
    //Step 3.2
    free( internet_address );

    //Step 3.1
    close( internet_socket );
}
