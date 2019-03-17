#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
// #include <fcntl.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <sys/uio.h>


#include "warp.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 2222
#define BUF_SIZE 1024
#define FILENAME_MAX_SIZE 256

int main(void)
{
	struct sockaddr_in server_addr;
	int sockfd;
	char str[INET_ADDRSTRLEN];
	char buf[FILENAME_MAX_SIZE];
	char file_name[FILENAME_MAX_SIZE];
	int n;

	// char SERVER_ADDR[32];
	// printf("Please input a server address to connect\n");
	// scanf("%s", SERVER_ADDR);

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	server_addr.sin_port = htons(SERVER_PORT);
	printf("Ready to connect %s:%d\n", 
			inet_ntop(AF_INET, &server_addr.sin_addr, str, sizeof(str)),
			ntohs(server_addr.sin_port));
	Connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	printf("Connected!!\n");

	printf("Please input the download filename:\n");
	scanf("%s", file_name);

	bzero(buf, sizeof(buf));
	strncpy(buf, file_name, strlen(file_name)>sizeof(buf)?sizeof(buf):strlen(file_name));
	send(sockfd, buf, sizeof(buf), 0);

	FILE* fp = fopen(file_name, "w");
    int recv_len = 0, write_len = 0;
    while((recv_len = recv(sockfd, buf, sizeof(buf), 0))>0){
        write_len = fwrite(buf, sizeof(char), recv_len, fp);
        if(write_len < recv_len){
            printf("File: %s write failed!\n", file_name);
            break;
        }
    }
    fclose(fp);
    printf("All write file done!\n");
   	Close(sockfd);
   	
	return 0;
}
