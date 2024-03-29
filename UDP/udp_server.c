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

#include <time.h>
#include <stdint.h>

typedef struct{

    char firstnm; //first name 
    char lastnm; //last name 
    uint8_t opcode; //opcode
    struct tm timepost; //timepost
    uint8_t length; //number of char to send
    char memo[201]; //msg to send

}msgstruct;

int main() {

    int ret;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char recv_buffer[1024];
    socklen_t len;

    //added myself
    msgstruct msg;
    msgstruct recent;
    time_t _time;
    struct tm svrtime;
    FILE *log;
    ////
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // The servaddr is the address and port number that the server will 
    // keep receiving from.
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(32000);

    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    
    bind(sockfd, 
         (struct sockaddr *) &servaddr, 
         sizeof(servaddr));

    while (1) {
        memset(&recv_buffer, 0, sizeof(recv_buffer));
        recvfrom(sockfd, &msg, sizeof(msgstruct), 0, (struct sockaddr *) &cliaddr, &len);

        if(msg.opcode == 1 && msg.firstnm == 'A' && msg.lastnm == 'M'){
            //printf("POST#: %s", msg.memo);
            time(&_time);
            svrtime = *localtime(&_time);

            log = fopen("serverlog.txt", "a");
            //printf("log opened\n");
            fprintf(log, "<%d:%d:%d> [%s:%hu] post#%s", msg.timepost.tm_hour,msg.timepost.tm_min,msg.timepost.tm_sec,inet_ntoa(cliaddr.sin_addr),
                        cliaddr.sin_port,msg.memo);
           // printf("wrote log msg\n");
            fprintf(log, "<%d:%d:%d> [%s:%hu] post_ack#successful\n", svrtime.tm_hour,svrtime.tm_min,svrtime.tm_sec,inet_ntoa(cliaddr.sin_addr),
                        cliaddr.sin_port);
            //printf("wrote log svr\n");
            fflush(log);
            fclose(log);

            int j = 0;
            while(msg.memo[j] != '\n'){
                //printf("msglen%u\nsave to recent index: %u\n", msg.length, j);
                recent.memo[j] = msg.memo[j];
                //printf("recent: %s\n", recent.memo);
                j = j + 1;
            }
            recent.memo[j] = '\n';

            recent.length = msg.length;
            //printf("recent len: %u\nrecent memo: %s", recent.length, recent.memo);
            msgstruct ack;
            ack.opcode = 2;
            ack.firstnm = 'A';
            ack.lastnm = 'M'; 
            
            //printf("sends da ack\n");
            sendto(sockfd, &ack, sizeof(msgstruct), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
            //printf("sent da ack\n");
        }
        else if(msg.opcode == 3 && msg.firstnm == 'A' && msg.lastnm == 'M'){
            //printf("RETRIEVE: %s", msg.memo);
            time(&_time);
            svrtime = *localtime(&_time);
            recent.opcode = 4;
            recent.firstnm = 'A';
            recent.lastnm = 'M'; 

            log = fopen("serverlog.txt", "a");
            fprintf(log, "<%d:%d:%d> [%s:%hu] retrieve#\n", msg.timepost.tm_hour,msg.timepost.tm_min,msg.timepost.tm_sec,inet_ntoa(cliaddr.sin_addr),
                        cliaddr.sin_port);
            fprintf(log, "<%d:%d:%d> [%s:%hu] retrieve#%s", svrtime.tm_hour,svrtime.tm_min,svrtime.tm_sec,inet_ntoa(cliaddr.sin_addr),
                        cliaddr.sin_port, recent.memo);
            fflush(log);
            fclose(log);
            //printf("recent len: %u\nrecent memo: %s", recent.length, recent.memo);
            sendto(sockfd, &recent, sizeof(msgstruct), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
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
