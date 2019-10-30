//Andrew Murza 
//TCP_server.c
//CSE 434

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdint.h>

typedef struct{
    
    //firstname
    char firstnm;
    //lastname
    char lastnm;
    //opcode
    //x80 file upload
    //x81 file upload ack
    //x82 file download
    //x83 file download ack
    uint8_t opcode;
    //filename length
    uint8_t filenamelen;
    //specific length of the current buffer
    uint8_t filebufferlen;
    //length of actual file
    uint32_t filelen;
    //filename
    char filename[256];
    //chunk of data
    unsigned char data[256];

} datafilestruct;

// This line must be included if you want to use multithreading.
// Besides, use "gcc ./tcp_receive.c -lpthread -o tcp_receive" to compile
// your code. "-lpthread" means link against the pthread library.
#include <pthread.h>

// This the "main" function of each worker thread. All worker thread runs
// the same function. This function must take only one argument of type 
// "void *" and return a value of type "void *". 
void *worker_thread(void *arg) {

    int ret;
    int connfd = (int) (long)arg;
    char recv_buffer[1024];
    datafilestruct datafile;

    printf("[%d] worker thread started.\n", connfd);

    while (1) {
        ret = recv(connfd, 
                    &datafile, 
                    sizeof(datafilestruct), 
                    0);

        if (ret < 0) {
            // Input / output error.
        printf("[%d] recv() error: %s.\n", connfd, strerror(errno));
            return NULL;
        } else if (ret == 0) {
            // The connection is terminated by the other end.
        printf("[%d] connection lost\n", connfd);
            break;
        }

        // TODO: Process your message, receive chunks of the byte stream, 
        // write the chunks to a file. You also need an inner loop to 
        // receive and write each chunk.

        if(datafile.opcode == 1 && datafile.firstnm == 'A' && datafile.lastnm == 'M'){
            printf("UPLOAD: \n");
            
            
            
            datafilestruct ack;
            ack.opcode = (uint8_t)2;
            ack.firstnm = 'A';
            ack.lastnm = 'M'; 
            
            printf("sends da ack\n");
            ret = send(connfd, &ack, sizeof(datafilestruct), 0);
            if (ret < 0) {
                printf("connect() error: %s.\n", strerror(errno));
                return -1;
            }
        }
        else if(datafile.opcode == 3 && datafile.firstnm == 'A' && datafile.lastnm == 'M'){
            printf("DOWNLOAD: \n");
            
            
            
            datafilestruct downloadfile;
            downloadfile.opcode = (uint8_t)4;
            downloadfile.firstnm = 'A';
            downloadfile.lastnm = 'M'; 
            
            //printf("sends da ack\n");
            ret = send(connfd, &downloadfile, sizeof(datafilestruct), 0);
            if (ret < 0) {
                printf("connect() error: %s.\n", strerror(errno));
                return -1;
            }
        }


    }

    printf("[%d] worker thread terminated.\n", connfd);
}


// The main thread, which only accepts new connections. Connection socket
// is handled by the worker thread.
int main(int argc, char *argv[]) {

    int ret;
    socklen_t len;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
      printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(31000);

    ret = bind(listenfd, (struct sockaddr*) 
               &serv_addr, sizeof(serv_addr));
    if (ret < 0) {
      printf("bind() error: %s.\n", strerror(errno));
        return -1;
    }


    if (listen(listenfd, 10) < 0) {
      printf("listen() error: %s.\n", strerror(errno));
        return -1;
    }

    while (1) {
      printf("waiting for connection...\n");
        connfd = accept(listenfd, 
                 (struct sockaddr*) &client_addr, 
                 &len);

        if(connfd < 0) {
        printf("accept() error: %s.\n", strerror(errno));
            return -1;
        }
      printf("conn accept - %s.\n", inet_ntoa(client_addr.sin_addr));

      pthread_t tid;
      pthread_create(&tid, NULL, worker_thread, (void *)(long)connfd);

    }
    return 0;
}
