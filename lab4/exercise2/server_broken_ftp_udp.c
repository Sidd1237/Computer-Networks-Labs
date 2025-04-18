#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#define MAX_PENDING 10

void die(char* s){
    perror(s);
    exit(EXIT_FAILURE);
}

int main(void)
{

    int sockfd = 0;  //listening  socket
    int connfd = 0; //connection socket

    struct sockaddr_in serv_addr = {};
    struct sockaddr_in client_addr = {};
    socklen_t client_length = sizeof(client_addr); //important
    char sendBuff[1025];
    int numrv;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd< 0){
        die("socket");
    }
    printf("Socket retrieve success\n");

    memset(sendBuff, '0', sizeof(sendBuff));

    //binding listening socket to address and port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5001);
    int bind_status = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(bind_status<0){
        die("bind");
    }

    // if (listen(sockfd, MAX_PENDING) == -1)
    // {
    //     printf("Failed to listen\n");
    //     return -1;
    // }
    while (1)
    {
        unsigned char offset_buffer[10] = {'\0'};
        unsigned char command_buffer[2] = {'\0'};
        int offset;
        int command;

        //fetching connection socket
        //connfd = accept(sockfd, (struct sockaddr *)NULL, NULL);

        printf("Waiting for client to send the command (Full File (0) Partial File (1)\n");
        
        while (recvfrom(sockfd, command_buffer, 2,0,(struct sockaddr *)&client_addr, &client_length)
        == 0);
        //extracting command from command buffer
        sscanf(command_buffer, "%d", &command);

        printf("Client entered command: %d\n",command);
        if (command == 0)
            offset = 0;
        else
        {
            printf("Waiting for client to send the offset\n");
            while (recvfrom(sockfd, offset_buffer, 10,0,(struct sockaddr *)&client_addr, &client_length)
 == 0);

            //extracting offset from offset buffer
            sscanf(offset_buffer, "%d", &offset);

        }

        /* Open the file that we wish to transfer */
        FILE *fp = fopen("source_file.txt", "rb");
        if (fp == NULL)
        {
            printf("File open error");
            return 1;
        }
        /* Read data from file and send it */
        //setting file pointer to extracted offset value
        fseek(fp, offset, SEEK_SET);
        while (1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[256] = {0};
            int nread = fread(buff, 1, 256, fp);
            printf("Bytes read %d \n", nread);
            /* If read was success, send data. */
            if (nread > 0)
            {
                printf("Sending \n");
                int bytesSent;
                bytesSent = sendto(sockfd, buff, nread, 0, (struct sockaddr *)&client_addr, client_length);
                if(bytesSent != nread){
                    die("sendto");
                }
                // write(connfd, buff, nread);
            }
            /*
             * There is something tricky going on with read ..
             * Either there was error, or we reached end of file.
             */
            if (nread < 256)
            {
                if (feof(fp)){
                    printf("End of file\n");
                    sendto(sockfd, "EOF",3,0,(struct sockaddr *)&client_addr, client_length );
                }
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
        }
        // close(connfd);
        sleep(1);
    }
    return 0;
}
