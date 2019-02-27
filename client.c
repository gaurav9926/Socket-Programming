// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <sys/dir.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    char tmp[100];

    struct sockaddr_in address;
    int sock = 0, valread,fcount;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1123456] = {0};

    while(1)
    {   
        fcount = 0;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                    // which is meant to be, and rest is defined below

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Converts an IP address in numbers-and-dots notation into either a
        // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
        {
            printf("\nConnection Failed \n");
            return -1;
        }

        memset(buffer,0,sizeof(buffer));

        printf("Write Command:\n");
        // scanf("%s",tmp);
        fgets(tmp,100,stdin);
        // printf("tmp: %s",tmp);
        send(sock , tmp , strlen(tmp) , 0 );  // send the message.
        // printf("SENT TO SERVER : %s \n",tmp);
        for(int i=0;i<strlen(tmp);i++){
            if(tmp[i] == *" ")
                fcount ++;
        }
        // printf("%d",fcount);

        char* fname = strtok(tmp," ");
        char cmd[112345];
        strcpy(cmd,fname);
        fname = strtok(NULL," ");

 
        // printf("FILENAME: %s\n",fname);
        // printf("CMD: %s\n",cmd);
        int chk=-1;DIR *pDir;struct dirent *pDirent;
        
        if(strcmp(cmd,"send")==0){
            //char *filename;
            if(fcount > 1){
                printf("Enter Only one File name\n");    
                continue;
            }
                
            valread = read( sock , buffer, 1126);  // receive message back from server, into the buffer
                
            if(strcmp(buffer,"Filename not present\n")==0){
                // printf("TF\n");
                printf("%s",buffer);    
            }
            else{
                FILE* file = fopen(fname,"w");
                printf("BUFER: %s\n",buffer);
                fputs( buffer , file);
                close(file);
            }
        }
        else if(strcmp(cmd,"listall\n")==0){
            valread = read( sock , buffer, 11234);  // receive message back from server, into the buffer
            // printf("INLISTALL\n");
            printf("%s\n",buffer);
        }
        else if(strcmp(cmd,"send\n")==0){
            printf("Enter File Name.\n");
        }
        else{
            printf("Invalid Command\n");
        }
        printf("\n");
    }
    return 0;
}
