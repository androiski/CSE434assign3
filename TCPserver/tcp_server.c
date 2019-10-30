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
    uint16_t filebufferlen;
    //length of actual file
    uint32_t filelen;
    //filename
    char filename[256];
    //chunk of data
    unsigned char data[1024];

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
    FILE * fromclient;
    FILE * toclient;

    printf("[%d] worker thread started.\n", connfd);

    while (1) {

        memset(&datafile, 0, sizeof(datafilestruct));
        // memset(&datafile.firstnm, 0, sizeof(datafile.firstnm));
        // memset(&datafile.lastnm, 0, sizeof(datafile.lastnm));
        // memset(&datafile.opcode, 0, sizeof(datafile.opcode));
        // memset(&datafile.filenamelen, 0, sizeof(datafile.filenamelen));
        // memset(&datafile.filebufferlen, 0,sizeof(datafile.filebufferlen));
        // memset(&datafile.filelen, 0, sizeof(datafile.filelen));
        // memset(&datafile.filename, 0, sizeof(datafile.filename));
        // memset(&datafile.data, 0, sizeof(datafile.data));

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

        if(datafile.opcode == 0x80 && datafile.firstnm == 'A' && datafile.lastnm == 'M'){
            //printf("UPLOAD: \n");
            
            //gets name of file
            fromclient = fopen(datafile.filename, "wb");
            int i = 0;  int j = 0;
            //printf("FILE LEN: %u\n", datafile.filelen);
            for(i = 0;  i < datafile.filelen; i+= 1024){
                //printf("UOTER FOORLOOP\n");
                
                
                //write to file
                for(j = 0; j <= datafile.filebufferlen && i+j < datafile.filelen; j++){
                    //printf("%c", datafile.data[j]);
                    fwrite(&datafile.data[j], 1, 1, fromclient);
                    fflush(fromclient);
                }

                memset(&datafile.data, 0, sizeof(datafilestruct));
                //printf("i:%d  j:%d\n", i , j);

                if(i + j < datafile.filelen){
                    ret = recv(connfd, &datafile, sizeof(datafilestruct), 0);
                }
                else{
                    datafilestruct ack;
                    ack.opcode = 0x81;
                    ack.firstnm = 'A';
                    ack.lastnm = 'M'; 
                    
                    printf("sends da ack\n");
                    ret = send(connfd, &ack, sizeof(datafilestruct), 0);
                }
            }

            fclose(fromclient);
            printf("UPLOAD DONE\n");

        }
        else if(datafile.opcode == 0x82 && datafile.firstnm == 'A' && datafile.lastnm == 'M'){
            printf("DOWNLOAD: \n");
            
            
            
            datafilestruct downloadfile;
            downloadfile.opcode = 0x83;
            downloadfile.firstnm = 'A';
            downloadfile.lastnm = 'M'; 
            
            //printf("sends da ack\n");
            ret = send(connfd, &downloadfile, sizeof(datafilestruct), 0);
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

