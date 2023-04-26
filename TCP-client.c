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
	void OSInit( void ) {}
	void OSCleanup( void ) {}
#endif

#include <stdint.h>
#include <time.h>

int initialization();
void execution( int internet_socket );
void cleanup( int internet_socket );
int randomNumber();
char randomOperation();
void checkAnswer(char buffer[100],int number1,int number2,char operation);

int main( int argc, char * argv[] )
{
    srand(time(NULL));

    OSInit();

    int internet_socket = initialization();

    execution( internet_socket );

    cleanup( internet_socket );

    OSCleanup();

    return 0;
}

int initialization()
{
    //Step 1.1
    struct addrinfo internet_address_setup;
    struct addrinfo * internet_address_result;
    memset( &internet_address_setup, 0, sizeof internet_address_setup );
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_STREAM;
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
            int connect_return = connect( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
            if( connect_return == -1 )
            {
                perror( "connect" );
                close( internet_socket );
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
    uint16_t number1 = 0;
    uint16_t number2 = 0;
    char operation = '0';
    char sendOperation[7];
    int number_of_bytes_send = 0;
    int number_of_bytes_received = 0;
    char buffer[1000];
    uint16_t amountOfOperations = 0;

    amountOfOperations = (rand() % 20) +1;

    for(int i=0;i<amountOfOperations;i++){
        number1 = randomNumber();
        number2 = randomNumber();
        operation = randomNumber();

        sprintf(sendOperation,"%d %c %d",number1,operation,number2);


        number_of_bytes_send = send( internet_socket, (const char*)sendOperation, strlen(sendOperation), 0 );
        if( number_of_bytes_send == -1 )
        {
            perror( "send" );
        }



        number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
        if( number_of_bytes_received == -1 )
        {
            perror( "recv" );
        }
        else
        {
            buffer[number_of_bytes_received] = '\0';
            printf( "Received : %s\n", buffer );
        }

        checkAnswer(buffer,number1,number2,operation);
    }


    number_of_bytes_send = send( internet_socket,"STOP\n", strlen("STOP\n"), 0 );
    if( number_of_bytes_send == -1 )
    {
        perror( "send" );
    }

    number_of_bytes_received = recv( internet_socket, buffer, ( sizeof buffer ) - 1, 0 );
    if( number_of_bytes_received == -1 )
    {
        perror( "recv" );
    }
    else
    {
        buffer[number_of_bytes_received] = '\0';
        printf( "Received : %s\n", buffer );
    }
    if(strcmp(buffer,"OK\n")==0){
        number_of_bytes_send = send( internet_socket,"KTNXBYE\n", strlen("KTNXBYE\n"), 0 );
        if( number_of_bytes_send == -1 )
        {
            perror( "send" );
        }
    }


    //Step 2.1


    //Step 2.2

}

void cleanup( int internet_socket )
{
    //Step 3.2
    int shutdown_return = shutdown( internet_socket, SD_SEND );
    if( shutdown_return == -1 )
    {
        perror( "shutdown" );
    }

    //Step 3.1
    close( internet_socket );
}

int randomNumber(){
    //generate a random number between 0 and 99
    uint16_t randomNumber = 0;
    randomNumber = rand() % 100;
    return randomNumber;
}

char randomOperation(){
    uint16_t randomNumberOperation = rand() % 4;
    char randomOperation = '0';
    switch (randomNumberOperation) {
        case 0:
            randomOperation = '+';
            break;
        case 1:
            randomOperation = '-';
            break;
        case 2:
            randomOperation = '*';
            break;
        case 3:
            randomOperation = '/';
            break;
    }
    return randomOperation;
}

void checkAnswer(char buffer[100],int number1,int number2,char operation){
    uint16_t result=0;
    uint16_t expectedVal = 0;

    switch (operation) {
        case '+':
            expectedVal = number1 + number2;
            break;
        case '-':
            expectedVal = number1 - number2;
            break;
        case '*':
            expectedVal = number1 * number2;
            break;
        case '/':
            if(number2 == 0){
                printf("Division By 0\n");;
            }
            else {
                expectedVal = number1 / number2;
            }
            break;
        default:
            printf("Invalid operator\n");
    }
    result = atoi(buffer);
    if(result == expectedVal){
        printf("correct");
    } else {
        printf("incorrect");
    }
}
