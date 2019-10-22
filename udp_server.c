//Andrew Murza 
//UDP_server 
//CSE 434
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {

    int ret;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char recv_buffer[1024];
    int recv_len;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("socket() error: %s.\n", strerror(errno));
        return -1;
}

    // The servaddr is the address and port number that the server will 
    // keep receiving from.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(32000);

    bind(sockfd, 
         (struct sockaddr *) &servaddr, 
         sizeof(servaddr));

    while (1) {
        len = sizeof(cliaddr);
        recv_len = recvfrom(sockfd, // socket file descriptor
                 recv_buffer,       // receive buffer
                 sizeof(recv_buffer),  // max number of bytes to be received
                 0,
                 (struct sockaddr *) &cliaddr,  // client address
                 &len);             // length of client address structure

        if (recv_len <= 0) {
            printf("recvfrom() error: %s.\n", strerror(errno));
            return -1;
        }

      // TODO: check whether the recv_buffer contains a proper header
        // at the beginning. If it does not have a proper header, just
        // ignore the datagram. Note that a datagram can be up to 64 KB,
        // and hence, the input parameter of recvfrom() on the max number
        // of bytes to be received should be the maximum message size in
        // the protocol we designed, i.e., size of the header (4 bytes) + 
        // the size of the maximum payload (200 bytes). For the same reason,
        // the receiver buffer should be at least this size big.

        // One more thing about datagram is that one datagram is a one
        // unit of transport in UDP, and hence, you either receive the whole
        // datagram or nothing. If you provide a size parameter less than
        // the actual size of the received datagram to recvfrom(), the OS
        // kernel will just truncates the datagram, fills your buffer with
        // the beginning part of the datagram and silently throws away the
        // remaining part. As a result, you can not call recvfrom() twice
        // to expect to receive the header first and then the message
        // payload next in UDP. That can only work with TCP.

        // If the received message has the correct format, send back an ack
        // of the proper type.

        // You are supposed to call the sendto() function here to send back
        // the echoed message, using "cliaddr" as the destination address. 
        
    }

    return 0;
}
