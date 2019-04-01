#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "warp.h"

#define SERVER_PORT 2222
#define BUF_SIZE 1024
#define FILENAME_MAX_SIZE 256

int main(void)
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    int listenfd, connectfd;
    char buf[BUF_SIZE];
    char str[INET_ADDRSTRLEN];
    int n;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);	//IPv4 面向流

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    Bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("Server address is %s:%d\n", 
            inet_ntop(AF_INET, &server_addr.sin_addr,str, sizeof(str)), 
            ntohs(server_addr.sin_port));

    Listen(listenfd, 20);

    while(1){
        printf("Listening...\n");
    	client_len = sizeof(client_addr);
    	connectfd = Accept(listenfd, (struct sockaddr*)&client_addr, &client_len);
    	printf("connect with %s at port %d\n", 
    			inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),
    			ntohs(client_addr.sin_port));

        char file_name[FILENAME_MAX_SIZE];
        bzero(file_name, sizeof(file_name));
        if(recv(connectfd, buf, sizeof(buf), 0) < 0){
            printf("Fail to recv file_name.\n");
            exit(1);
        }
        strncpy(file_name, buf, strlen(buf)>FILENAME_MAX_SIZE?FILENAME_MAX_SIZE:strlen(buf));
       
        FILE* fp = fopen(file_name, "r");
        if(fp == NULL){
            printf("File: %s not found!\n", file_name);
        }
        else{
            bzero(buf, sizeof(buf));
            int read_len = 0, send_len = 0;
            while((read_len = fread(buf, sizeof(char), sizeof(buf), fp))>0){
                send_len = send(connectfd, buf, read_len, 0);
                if(send_len < read_len){
                    printf("Send file: %s failed. Send %d of %d bytes!\n",file_name, send_len, read_len );
                    exit(1);
                }
            }
	    fclose(fp);
            printf("File: %s send successful!\n", file_name);
        }
        shutdown(connectfd, SHUT_RDWR);
        // Close(connectfd);
    }
}
