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

typedef struct{
    
    //firstname
    char firstnm;
    //lastname
    char lastnm;
    //opcode
    //0x80 file upload
    //0x81 file upload ack
    //0x82 file download
    //0x83 file download ack
    //0x69 file dne on server
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
    FILE * fromserver;

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
        memset(&datafile, 0, sizeof(datafilestruct));
        memset(send_buffer, 0, sizeof(send_buffer));
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


        uploadcmderr = 0;
        downloadcmderr = 0;
        fileexists = 0;

        //compares first 7 char from send_buffer to 'upload$'
        if(strncmp(send_buffer, uploadcmd, 7) == 0){
            //printf("uploadcmd given\n");
            uploadcmderr = 1;
            if(send_buffer[8] == '\n'){//check if there is actually filename given
                uploadcmderr = 0;
                //printf("filename not given\n");
            }
        }
        else{
            //printf("uploadcmd not given\n");
            uploadcmderr = 0;
        }

        //compares the first 9 char from send_buffer to see if 'download$'
        if(strncmp(send_buffer, downloadcmd, 9) == 0){
            //printf("downloadcmd given\n");
            downloadcmderr = 1;
            if(send_buffer[10] == '\n'){//check if there is actually filename given
                //printf("filename not given\n");
                downloadcmderr = 0;
            }
        }
        else{
            //printf("downloadcmd not given\n");
            downloadcmderr = 0;         
        }

    //if its a upload$...
        if(uploadcmderr == 1 && downloadcmderr != 1){
            //printf("uploadcmderr 1 downloadcmderr 0\n");

            //gets filename length of message after $
            int j = 0;
            while(send_buffer[j+7] != '\n'){
            //printf("%c",send_buffer[j+7]);
                j = j + 1;
            }
            datafile.filenamelen = (uint8_t)j;
            //printf("filenamelen: %u\n", datafile.filenamelen);

            //codes
            datafile.firstnm = 'A';
            datafile.lastnm = 'M';
            datafile.opcode = 0x80;
            
            //save send_buffer message to datafile.filename
            for(int i = 0; i < datafile.filenamelen; i++){
                //printf("index saved: %d", i);
                datafile.filename[i] = send_buffer[7+i];
            }
            //printf("filename: %s\n", datafile.filename);

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
                printf("FILE EXISTS\t\t\tUPLOADING TO SERVER...\n");
                //open file in read
                FILE * uploadfile = fopen(datafile.filename, "rb");

                //check file size
                fseek(uploadfile, 0L, SEEK_END);
                // calculating the size of the file 
                datafile.filelen = ftell(uploadfile); 
                //printf("filelen: %u\n", datafile.filelen);

                fseek(uploadfile, 0L, SEEK_SET);
                
                int i, j;
                //iterates by 1024 bytes
                for(int i = 0; i < datafile.filelen; i += 1024){
                    //clear old or null data for new existing data
                    //printf("outer forloop\n");
                    memset(&datafile.data, 0 ,sizeof(datafile.data));

                    
                    //gets 1024 bytes from file to read
                    for(j = 0; j <= 1023 && j+i < datafile.filelen; j++){
                        //printf("inner forloop\n");
                        fread(&datafile.data[j], 1, 1, uploadfile);
                        //printf("buffer size: %u\n", j);
                    }
                    //saves bufferlen (useful for when the last stream is <1024)
                    datafile.filebufferlen = j - 1;

                    //send the data
                    //printf("total buffer size: %u\n", datafile.filebufferlen);
                    //printf("what i'm sending:\n%s", datafile.data);
                    ret = send(sockfd, &datafile, sizeof(datafilestruct), 0);
                    usleep(5000);
                }
                fclose(uploadfile);
            }
            
            datafilestruct ack;
            recv(sockfd, &ack, sizeof(datafilestruct), 0);
            //if whatever is received is an ACK, opcode2, print success
            if(ack.opcode == 0x81 && ack.firstnm == 'A' && ack.lastnm == 'M'){
                printf("upload_ack#uploaded_file_successfully!\n");
            }            
        }
        //if its a download$...
        else if(uploadcmderr != 1 && downloadcmderr == 1){
            //printf("uploadcmderr 0 downloadcmderr 1\n");
            //gets filename length of message after $

            datafilestruct downloadreq;
            int j = 0;
            while(send_buffer[j+9] != '\n'){
            //printf("%c",send_buffer[j+9]);
                j = j + 1;
            }
            downloadreq.filenamelen = (uint8_t)j;
            //printf("filenamelen: %u\n", datafile.filenamelen);

            //codes
            downloadreq.firstnm = 'A';
            downloadreq.lastnm = 'M';
            downloadreq.opcode = 0x82;
            
            //save send_buffer message to datafile.filename
            for(int i = 0; i < downloadreq.filenamelen; i++){
                //printf("index saved: %d", i);
                downloadreq.filename[i] = send_buffer[9+i];
            }
            //printf("filename: %s\n", downloadreq.filename);

            ret = send(sockfd, &downloadreq, sizeof(datafilestruct), 0);
            ret = recv(sockfd, &datafile, sizeof(datafilestruct), 0);

            if(datafile.opcode == 0x83 && datafile.firstnm == 'A' && datafile.lastnm == 'M'){
                printf("FILE EXISTS\t\t\tDOWNLOADING...\n");
        
                //gets name of file
                fromserver = fopen(datafile.filename, "wb");
                int i = 0;  int j = 0;
                //printf("FILE LEN: %u\n", datafile.filelen);
                for(i = 0;  i < datafile.filelen; i+= 1024){
                    //printf("UOTER FOORLOOP\n");
                    
                    
                    //write to file
                    for(j = 0; j <= datafile.filebufferlen && i+j < datafile.filelen; j++){
                        //printf("%c", datafile.data[j]);
                        fwrite(&datafile.data[j], 1, 1, fromserver);
                        fflush(fromserver);
                    }

                    memset(&datafile.data, 0, sizeof(datafilestruct));
                    //printf("i:%d  j:%d\n", i , j);

                    if(i + j < datafile.filelen){
                        ret = recv(sockfd, &datafile, sizeof(datafilestruct), 0);
                    }
                    // else{
                    //     datafilestruct ack;
                    //     ack.opcode = 0x81;
                    //     ack.firstnm = 'A';
                    //     ack.lastnm = 'M'; 
                        
                    //     printf("sends da ack\n");
                    //     ret = send(sockfd, &ack, sizeof(datafilestruct), 0);
                    // }
                }
                fclose(fromserver);
                printf("download_ack$file_downloaded_successfully!\n");
            }
            else if(datafile.opcode == 0x69 && datafile.firstnm == 'A' && datafile.lastnm == 'M'){
                printf("Error: The file does not exist in the server.\n");
            }


        }
        else{
            printf("Error: Unrecognized command format\n");
        }
    }

    close(sockfd);

    return 0;
}
