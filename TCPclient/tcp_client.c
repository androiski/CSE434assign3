//Andrew Murza 
//TCP_client.c
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
#include <unistd.h>

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


int main(int argc, char *argv[]) {
    int ret;
    int sockfd = 0;
    char send_buffer[1024];
    struct sockaddr_in serv_addr;

    char uploadcmd[7] = {'u', 'p','l', 'o', 'a', 'd', '$'};
    char downloadcmd[9] = {'d', 'o', 'w', 'n', 'l', 'o', 'a', 'd', '$'};
    int uploadcmderr = 0;
    int downloadcmderr = 0;
    int fileexists = 0;
    datafilestruct datafile;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      printf("socket() error: %s.\n", strerror(errno));
        return -1;
    }

    // Note that this is the server address that the client will connect to.
    // We do not care about the source IP address and port number. 

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(31000);

    ret = connect(sockfd, 
                  (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr));
    if (ret < 0) {
      printf("connect() error: %s.\n", strerror(errno));
        return -1;
    }

    while (1) {
        fgets(send_buffer, 
              sizeof(send_buffer), 
              stdin);

        // These two lines allow the client to "gracefully exit" if the
        // user type "exit".
        
        if (strncmp(send_buffer, "exit", strlen("exit")) == 0)
        break;

      // TODO: You need to parse the string you read from the keyboard,
      // check the format, extract the file name, open the file, 
      // read each chunk into the buffer and send the chunk.
      // You need to write an inner loop to read and send each chunk. 
      //       
      // Note that you need to send the header before sending the actual
        // file data

        while (1) {//fix 1
        // This the inner loop.

        uploadcmderr = 0;
        downloadcmderr = 0;
        fileexists = 0;

        //compares first 7 char from send_buffer to 'upload$'
        if(strncmp(send_buffer, uploadcmd, 7) == 0){
            printf("uploadcmd given\n");
            uploadcmderr = 0;
            if(sizeof(send_buffer) <= 7){//check if there is actually filename given
                uploadcmderr = 1;
                printf("filename given\n");
            }
        }
        else{
            printf("uploadcmd not given\n");
            uploadcmderr = 0;
        }

        //compares the first 9 char from send_buffer to see if 'download$'
        if(strncmp(send_buffer, downloadcmd, 9) == 0){
            printf("downloadcmd given\n");
            downloadcmderr = 0;
            if(sizeof(send_buffer) <= 9){//check if there is actually filename given
                printf("filename given\n");
                downloadcmderr = 1;
            }
        }
        else{
            printf("downloadcmd not given\n");
            downloadcmderr = 0;         
        }

       //if its a upload$...
        if(uploadcmderr == 1 && downloadcmderr != 1){
            printf("uploadcmderr 0 downloadcmderr 1\n");

            //gets filename length of message after $
            int j = 0;
            while(send_buffer[j+5] != '\n'){
                //printf("while%d", j);
                j = j + 1;
            }
            datafile.filenamelen = (uint8_t)j;
            printf("filenamelen: %u", datafile.filenamelen);

            //codes
            datafile.firstnm = 'A';
            datafile.lastnm = 'M';
            datafile.opcode = (uint8_t)1;
            
            //save send_buffer message to datafile.filename
            for(int i = 0; i < datafile.filenamelen; i++){
                //printf("index saved: %d", i);
                datafile.filename[i] = send_buffer[7+i];
            }
            printf("filename: %s\n", datafile.filename);

            // file exists
            if(access(datafile.filename, F_OK) != -1) {
                fileexists = 1;
            } 
            // file doesn't exist
            else {
                fileexists = 0;
                printf("Error: filename does not exist in current directory\n");
            }

            if(fileexists){
                //open file in read
                FILE * uploadfile = fopen(datafile.filename, 'rb');

                //check file size
                fseek(uploadfile, 0L, SEEK_END);
                // calculating the size of the file 
                datafile.filelen = ftell(uploadfile); 
                printf("filelen: %s\n", datafile.filelen);

                //iterates by 1 byte, 256
                for(int i = 0; i < datafile.filelen; i += 255){
                    //clear old or null data for new existing data
                    memset(&datafile.data, 0 ,sizeof(datafile.data));

                    int j;
                    //gets 256 bytes from file to read
                    for(j = 0; j < 256 && j < datafile.filelen; j++){
                        fread(&datafile.data[j], 1, 1, uploadfile);
                    }
                    //saves bufferlen (useful for when the last stream is <256)
                    datafile.filebufferlen = j;

                    //send the data
                    ret = send(sockfd, &datafile, sizeof(datafile), 0);
                    if (ret < 0) {
                        printf("connect() error: %s.\n", strerror(errno));
                        return -1;
                    }
                }
            }             
        }
        //if its a download$...
        else if(uploadcmderr != 1 && downloadcmderr == 1){
            //printf("uploadcmderr 1 downloadcmderr 0\n");
            
        }
        else{
            printf("Error: Unrecognized command format\n");
        }

    }

    close(sockfd);

    return 0;
}
