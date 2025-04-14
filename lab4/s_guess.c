#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUFLEN 512 // Max length of buffer
#define PORT 8888  // The port on which to listen for incoming data

void die(char *s)
{
    perror(s);
    exit(1);
}
int main(void)
{
    struct sockaddr_in si_me = {};
    struct sockaddr_in si_other = {};
    int s, i, slen = sizeof(si_other), recv_len;
    char buf[BUFLEN];

    // create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to port
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
    {
        die("bind");
    }

    // keep listening for data
    while (1)
    {
        memset(buf, '\0', BUFLEN); // Clear the buffer
        printf("Waiting for guess...");
        fflush(stdout);

        int r_no = (rand() % 6) + 1;
        printf("generated random number: %d\n", r_no);

        // try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other,
                                 &slen)) == -1)
        {
            die("recvfrom()");
        }
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr),
               ntohs(si_other.sin_port));

        int guess = atoi(buf);
        memset(buf, '\0', BUFLEN);

        if(r_no == guess) strcpy(buf, "You Win!\n");
        else strcpy(buf, "You lose!\n");

        if (sendto(s, buf, strlen(buf), 0, (struct sockaddr *)&si_other, slen) == -1)
        {
            die("sendto()");
        }
        memset(buf, '\0', BUFLEN);
    }
    close(s);
    return 0;
}
