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

typedef struct{

    char firstnm; //first name 
    char lastnm; //last name 
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
    socklen_t len;

    char posthash[5] = {'p', 'o', 's', 't', '#'};
    char retcmd[11] = {'r', 'e', 't', 'r', 'i', 'e', 'v', 'e', '#', '\n', '\0'};
    int posterrck = 0;
    int recerrck = 0;
    msgstruct msg;
    time_t _time;


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
        resetmsg(&msg);
        posterrck = 0;
        recerrck = 0;

        //compares first 5 char from send_buffer to 'post#'
        if(strncmp(send_buffer, posthash, 5) == 0){
            //printf("posthash cmp\n");
            posterrck = 0;
            if(sizeof(send_buffer) <= 5){
                posterrck = 1;
               //printf("posthash cmp2\n");
            }
        }
        else{
            //printf("posthash cmp else\n");
            posterrck = 1;
        }
        
        if(send_buffer[5] == '\n'){
            //printf("n erro\n");
            posterrck = 1;
        }

        //compares the first 11 char from send_buffer to see if 'retrieve#'
        if(strncmp(send_buffer, retcmd, 11) == 0){
            //printf("retcmd cmp\n");
            recerrck = 0;
            if(sizeof(send_buffer) <= 11){
                //printf("retcmd cmp2\n");
                recerrck = 1;
            }
        }
        else{
            //printf("retcmd cmp else\n");
            recerrck = 1;
        }

        //if its a post#...
        if(posterrck == 0 && recerrck != 0){
            //printf("posterrck 0 recerrck 1\n");
            
            //gets size of message after #
            int j = 0;
            //printf("%s", send_buffer);
            while(send_buffer[j+5] != '\n'){
                //printf("while%d", j);
                j = j + 1;
            }
            msg.length = (uint8_t)j + 1;
            //printf("msglen: %u", msg.length);

            //get time
            time(&_time);
            msg.timepost = *localtime(&_time);

            //codes
            msg.firstnm = 'A';
            msg.lastnm = 'M';
            msg.opcode = (uint8_t)1;
            
            //save send_buffer message to msg.memo
            for(int i = 0; i < msg.length; i++){
                //printf("index saved: %d", i);
                msg.memo[i] = send_buffer[5+i];
            }
            //printf("memo: %s", msg.memo);

            //send msg to server
            sendto(sockfd, &msg, sizeof(msgstruct), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

            //receieve what server replies
            len = sizeof(servaddr);
            msgstruct ack;
            //recv(sockfd, &ack, sizeof(msgstruct), 0);
            recvfrom(sockfd, &ack, sizeof(msgstruct), 0, (struct sockaddr *) &servaddr, &len);
            //printf("%u %u %u\n", ack.firstnm, ack.lastnm, ack.opcode);

            //if whatever is received is an ACK, opcode2, print success
            if(ack.opcode == 2 && ack.firstnm == 'A' && ack.lastnm == 'M'){
                printf("post_ack#successful\n");
            }
        
        }
        //if its a receive#
        else if(posterrck != 0 && recerrck == 0){
            //printf("posterrck 1 recerrck 0\n");
            time(&_time);
            msg.timepost = *localtime(&_time);
            msg.firstnm = 'A';
            msg.lastnm = 'M';
            msg.opcode = (uint8_t)3;

            //printf("Sent RET\n");
            sendto(sockfd, &msg, sizeof(msgstruct), 0 , (struct sockaddr *) & servaddr, sizeof(servaddr));

            len = sizeof(servaddr);
            msgstruct ack;
            //recv(sockfd, &ack, sizeof(msgstruct), 0);
            recvfrom(sockfd, &ack, sizeof(msgstruct), 0, (struct sockaddr *) &servaddr, &len);

            if(ack.opcode == 4 && ack.firstnm == 'A' && ack.lastnm == 'M'){
                printf("retrieve_ack#%s", ack.memo);
            }
        }
        else{
            printf("Error: Unrecognized command format\n");
        }
        for(int i = 0; i < sizeof(send_buffer); i++){
            send_buffer[i] = '\0';
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
        msg->firstnm = 'A';
        msg->lastnm = 'M';
        msg->opcode = (uint8_t)0;
        time(&actualtime);
        msg->timepost = *localtime(&actualtime);
        msg->length = (uint8_t)0;
        for(int i = 0; i < 201; i++){
            msg->memo[i] = '\0';
        }
};
