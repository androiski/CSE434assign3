//Andrew Murza 
//UDP_client.c
//CSE 434
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "msgstruct.h"

int main() {

    int ret;
    int sockfd;
    struct sockaddr_in servaddr;
    char send_buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
    printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }

    // The "servaddr" is the server's address and port number, 
    // i.e, the destination address if the client needs to send something. 
    // Note that this "servaddr" must match with the address in the 
    // UDP server code.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(32000);

    // TODO: You may declare a local address here.
    // You may also need to bind the socket to a local address and a port
    // number so that you can receive the echoed message from the socket.
    // You may also skip the binding process. In this case, every time you
    // call sendto(), the source port may be different.

    // Optionally, you can call connect() to bind the socket to a 
    // destination address and port number. Since UDP is connection less,
    // the connect() only set up parameters of the socket, no actual
    // datagram is sent. After that, you can call send() and recv() instead
    // of sendto() and recvfrom().

    while (1) {

        // The fgets() function read a line from the keyboard (i.e, stdin)
        // to the "send_buffer".
        fgets(send_buffer,               
            sizeof(send_buffer), 
            stdin);

        // TODO: Check the user input format.

        // The sendto() function send the designated number of bytes in the
        // "send_buffer" to the destination address.
        ret = sendto(sockfd,                   // the socket file descriptor
               send_buffer,                    // the sending buffer
               sizeof(send_buffer), // the number of bytes you want to send
               0,
               (struct sockaddr *) &servaddr, // destination address
               sizeof(servaddr));             // size of the address

        if (ret <= 0) {
            printf("sendto() error: %s.\n", strerror(errno));
            return -1;
        }

        // TODO: You are supposed to call the recvfrom() function here.
        // The client will receive the acknowledgement from the server.

    }

    return 0;
}

