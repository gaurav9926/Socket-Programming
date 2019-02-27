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
    DIR *pDir;char strings[11234],files[11234]={};
    struct dirent *pDirent;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1,chk=-1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    while(1){ 
        memset(buffer , 0 , sizeof(buffer));
        memset(files , 0 ,sizeof(files));
        // printf("BEF\n");   
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
        {
            // printf("F\n");
            perror("accept");
            exit(EXIT_FAILURE);
            // continue;
        }
        // printf("AFT\n");   
        int pid = fork();
        if(pid == 0){
            valread = read( new_socket , buffer, 1024);  // read infromation received into the buffer
            // printf("FROM USER : %s\n",buffer );
            char* temp = strtok(buffer," ");
            char first[11234];
            char file1[11234];
            strcpy(first,temp);
            temp = strtok(NULL, " ");
            if(strcmp(first,"listall\n")==0){
                // printf("IN\n");
                char cwd[11234];	
                if (getcwd(cwd, sizeof(cwd)) == NULL) {
                    perror("getcwd() error");
                    return -1;
                }
                pDir = opendir(cwd);
                if (pDir == NULL) {
                    printf ("Cannot open directory '%s'\n", cwd);
                    return -1;
                }
                while ((pDirent = readdir(pDir)) != NULL) {
                        if((strcmp(pDirent->d_name,".")==0 || strcmp(pDirent->d_name,"..")==0 ))
                        {}
                        else{
                            strcat(strings , pDirent->d_name);
                            strcat(strings, "\n");
                        }
                }
                // printf("strings => %s",strings);     
                printf("Sent List Of Files\n");	 
                closedir (pDir);   		  
                send(new_socket , strings , strlen(strings) , 0 );  // use sendto() and recvfrom() for DGRAM
                // printf("MSG SENT TO USER : %s",strings);
            }
            else if(strcmp(first,"send")==0){
    
                // printf("temp: %s",temp);
                temp = strtok(temp,"\n");
                // printf("temp: %s",temp);
                FILE* file = fopen(temp, "r"); 
                int cnt = 0;
                char* line=NULL;
                char newbuffer[112345]={0};
                int nread;
                size_t len = 0;

                if(file == NULL){
                    printf("File not present\n");
                    strcpy(newbuffer,"Filename not present\n");
		    send(new_socket , newbuffer, strlen(newbuffer), 0);  // use sendto() and recvfrom() for DGRAM
		    return 0;        
                }  
                else{    
                    while((nread = getline(&line, &len, file)) != -1){
                        // printf("%d\n",nread);
                        strcat(files,line);
                        // strcat(files,"\n");
                    }
                    send(new_socket , files , strlen(files) , 0 );  // use sendto() and recvfrom() for DGRAM            
                    // printf("FILES: %s\n",files);
                    printf("File Sent.\n");
                }
                fclose(file);
                //closedir(pDir);
            }
                
        }
        
    }
    
    close(new_socket);
    return 0;
}




