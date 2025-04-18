/* Client program Broken FTP */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

void die(char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}
int main(void)
{
    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[256];
    unsigned char buff_offset[10]; // buffer to send the File offset value
    unsigned char buff_command[2]; // buffer to send the Complete File (0) or Partial File Command(1).
    int offset;                    // required to get the user input for offset in case of partial file command
    int command;                   // required to get the user input for command
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;
    /* Create a socket first */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    printf("Socket created!\n");

    /* Initialize sockaddr_in data structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5001); // port
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t serv_len = sizeof(serv_addr);

    // /* Attempt a connection */
    // if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    // {
    //     printf("\n Error : Connect Failed \n");
    //     return 1;
    // }
    // printf("Successfully connected!\n");

    /* Create file where data will be stored */
    FILE *fp;
    fp = fopen("destination_file.txt", "ab");
    if (NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

    fseek(fp, 0, SEEK_END); // putting file pointer to the end
    offset = ftell(fp);     // for (2): calculating size of file so the file can resume downloading from here.
    fclose(fp);
    fp = fopen("destination_file.txt", "ab");
    if (NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

    printf("Enter (0) to get complete file, (1) to specify offset, (2) calculate the offset value from local file\n");
    scanf("%d", &command);
    sprintf(buff_command, "%d", command); // write() expects buffer of bytes for command
    sendto(sockfd, buff_command, strlen(buff_command), 0, (struct sockaddr *)&serv_addr, serv_len);
    // write(sockfd, buff_command, 2);

    if (command == 1 || command == 2) // We need to specify the offset
    {
        if (command == 1) // get the offset from the user
        {
            printf("Enter the value of File offset\n");
            scanf("%d", &offset);
        }
        // otherwise offset = size of local partial file, that we have already calculated
        sprintf(buff_offset, "%d", offset);
        /* sending the value of file offset */
        sendto(sockfd, buff_offset, strlen(buff_offset), 0, (struct sockaddr *)&serv_addr, serv_len);
    }

    // Else { command = 0 then no need to send the value of offset }

    /* Receive data in chunks of 256 bytes */
    printf("waiting to receive bytes...\n");

    while ((bytesReceived = recvfrom(sockfd, recvBuff, 256, 0, (struct sockaddr *)&serv_addr, &serv_len)) >= 0)
    {
        if (strcmp(recvBuff, "EOF") == 0)
        {
            printf("End of file received. Stopping.\n");
            break;
        }
        printf("Bytes received %d\n", bytesReceived);
        // recvBuff[n] = 0;
        //printf("recvBuff: \"%s\"\n\n", recvBuff);
        size_t written = fwrite(recvBuff, 1, bytesReceived, fp);
        if (written != bytesReceived)
        {
            die("fwrite failed");
        }
        memset(recvBuff, 0, sizeof(recvBuff));
        // printf("%s \n", recvBuff);
    }
    if (bytesReceived < 0)
    {
        die("recvfrom");
    }
    return 0;
}