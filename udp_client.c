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

#include <time.h>
#include <stdint.h>
#include <limits.h>

typedef struct{

    uint8_t firstnm; //first name 
    uint8_t lastnm; //last name 
    uint8_t opcode; //opcode
    struct tm timepost; //timepost
    uint8_t length; //number of char to send
    char memo[201]; //msg to send

}msgstruct;

void resetmsg(msgstruct *m);


int main() {

    int ret;
    int sockfd;
    struct sockaddr_in servaddr;
    char send_buffer[1024];
    char restrict_buffer[1024];

    char posthash[5] = {'p', 'o', 's', 't', '#'};
    char retcmd[11] = {'r', 'e', 't', 'r', 'i', 'e', 'v', 'e', '#', '\n', '\0'};
    int posterrct = 0;
    int recerrct = 0;
    msgstruct msg;
    time_t _time;
    int msgct = 0;


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

/* 
stole this snippet from assign 1 udp_client.c 
probably need to change some stuff
*/

    ret = connect(sockfd,
                    (struct sockaddr *) &servaddr,
                    sizeof(servaddr));

    if (ret < 0 ){
        printf("connect() error: %s.\n", strerror(errno));
        return -1;
    }

/////////////////////////////////////////////////////////////////////////////////////////
    
    while (1) {

        // The fgets() function read a line from the keyboard (i.e, stdin)
        // to the "send_buffer".
        fgets(send_buffer,               
            sizeof(send_buffer), 
            stdin);

        // TODO: Check the user input format.
        resetmsg(&msg);
        posterrct = 0;
        recerrct = 0;

        //compares first 5 char from send_buffer to 'post#'
        for (int i = 0; i < 5 && !posterrct; i++){
            if(send_buffer[i] != posthash[i]){
                posterrct = send_buffer[i] - posthash[i];
            }
        }
        if(send_buffer[5] == '\n'){
            posterrct = INT_MAX;
        }

        //compares the first 11 char from send_buffer to see if 'retrieve#'
        for(int i = 0; i < 11 && !recerrct; i++){
            if(send_buffer[i] != retcmd[i]){
                recerrct = send_buffer[i] - retcmd[i];
            }
        }

        if(posterrct == 0 && recerrct != 0){
            int j = 5;
            while(send_buffer[j] != '\n'){
                j+=1;
            }
            msg.length = (uint8_t)j + 1;
            time(&_time);
            msg.timepost = *localtime(&_time);
            msg.firstnm = (uint8_t)'A';
            msg.lastnm = (uint8_t)'M';
            msg.opcode = (uint8_t)1;
            for(int i = 0; msg.length; i++){
                msg.memo[i] = send_buffer[5+i];
            }

            sendto(sockfd, &msg, sizeof(msgstruct), 0, (struct sockaddr *) & servaddr, sizeof(servaddr));
            recv(sockfd, &msg, sizeof(msgstruct), 0);

            if(msg.opcode == 2 && msg.firstnm == (uint8_t)'A' && msg.lastnm == (uint8_t)'M'){
                printf("post_ack#successful\n");
            }
            else if(posterrct != 0 && recerrct == 0){
                if(msgct != 0){
                    time(&_time);
                    msg.timepost = *localtime(&_time);
                    msg.opcode = (uint8_t)3;
                    msg.firstnm = (uint8_t)'A';
                    msg.lastnm = (uint8_t)'M';

                    sendto(sockfd, &msg, sizeof(msgstruct), 0 , (struct sockaddr *) & servaddr, sizeof(servaddr));
                    recv(sockfd, &msg, sizeof(msgstruct), 0);

                    if(msg.opcode == 4 && msg.firstnm == (uint8_t)'A' && msg.lastnm == (uint8_t)'M'){
                        printf("retrieve_ack#%s", msg.memo);
                    }
                }
            }
            else{
                printf("Error: Unrecognized command format\n");
            }
            for(int i = 0; i < sizeof(send_buffer); i++){
                send_buffer[i] = '\0';
            }
        }



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

void resetmsg(msgstruct * msg){
        time_t actualtime;
        msg->firstnm = (uint8_t)'A';
        msg->lastnm = (uint8_t)'M';
        msg->opcode = (uint8_t)0;
        time(&actualtime);
        msg->timepost = *localtime(&actualtime);
        msg->length = (uint8_t)0;
        for(int i = 0; i < 201; i++){
            msg->memo[i] = '\0';
        }
};
