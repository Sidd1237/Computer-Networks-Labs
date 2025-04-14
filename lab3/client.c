#include <stdio.h>
#include <sys/socket.h> //for socket(), connect(), send(), recv() functions
#include <arpa/inet.h>  // different address structures are declared here
#include <stdlib.h>     // atoi() which convert string to integer
#include <string.h>
#include <unistd.h> // close() function
#define BUFSIZE 32

int main(int argc, char const *argv[])
{
    // creating tcp socket: int socket(int protocolFamily, int type, int protocol)
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("Error in opening a socket");
        exit(0);
    }
    printf("Client Socket Created\n");

    // creating server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Specify server's IP address here
    printf("Address assigned\n");

    /*ESTABLISH CONNECTION*/
    printf("trying to establish connection....\n");
    int c = connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    printf("%d\n", c);
    if (c < 0)
    {
        printf("Error while establishing connection");
        exit(0);
    }
    printf("Connection Established\n");
    /*SEND DATA*/
    printf("ENTER MESSAGE FOR SERVER with max 32 characters\n");
    char msg[BUFSIZE];
    fgets(msg,BUFSIZE,stdin);
    msg[strcspn(msg,"\n")] = '\0';
    int bytesSent = send(sock, msg, strlen(msg), 0);
    if (bytesSent != strlen(msg))
    {
        printf("Error while sending the message");
        exit(0);
    }
    printf("Data Sent\n");
    /*RECEIVE BYTES*/
    char recvBuffer[BUFSIZE];
    int bytesRecvd = recv(sock, recvBuffer, BUFSIZE - 1, 0);
    if (bytesRecvd < 0)
    {
        printf("Error while receiving data from server");
        exit(0);
    }
    recvBuffer[bytesRecvd] = '\0';
    printf("%s\n", recvBuffer);
    close(sock);

    return 0;
}
