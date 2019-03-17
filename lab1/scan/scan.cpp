#include <iostream>
#include <pthread.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>

#include "warp.h"

using namespace std;

struct thread_para
{
	char thread_ip[32];
	int thread_port_start;
	int thread_port_end;
};

void* scan_port(void* args)
{
	struct thread_para* para;
	para = (struct thread_para*) args;
	const char* SCAN_ADDR = para->thread_ip;
	int SCAN_PORT_START = para->thread_port_start;
	int SCAN_PORT_END = para->thread_port_end;

	char str[INET_ADDRSTRLEN];
	struct sockaddr_in server_addr;
	int sockfd;

	for(int port=SCAN_PORT_START;port<=SCAN_PORT_END;port++){
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		inet_pton(AF_INET, SCAN_ADDR, &server_addr.sin_addr);
		server_addr.sin_port = htons(port);
		
		int ret = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if(ret == 0){
			printf("ip:%s port:%d open!\n",
					inet_ntop(AF_INET, &server_addr.sin_addr, str, sizeof(str)), port);
		}
		Close(sockfd);
	}
	return 0;
}

int main(void)
{
	int thread_num;
	cout << "please input thread num:" << endl;
	cin >> thread_num;

	char ip_start[32] = "127.0.0.1";
	// char ip_end[32] = "127.0.0.1";
	int port_start = 0;
	int port_end = 60000;
	int each_num = (port_end - port_start+1)/thread_num;
	cout << "each thread should scan " << each_num << " ports" << endl;

	char str[INET_ADDRSTRLEN];
	struct thread_para* para;
	pthread_t* thread_id;
	para = (struct thread_para*)malloc(sizeof(struct thread_para) * thread_num);
	thread_id = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);
	for(int i=0;i<thread_num;i++)
	{
		strcpy(para[i].thread_ip, ip_start);
		para[i].thread_port_start = port_start + each_num * i;
		if(i==thread_num-1){
			para[i].thread_port_end = port_end;
		}
		else{
			para[i].thread_port_end = para[i].thread_port_start + each_num - 1;
		}

		int ret = pthread_create(&thread_id[i], NULL, scan_port, (void*)&(para[i]));
		if(ret != 0){
			cout << "pthread_create error: error_code = "<< ret <<endl;
		}
	}
	pthread_exit(NULL);
	return 0;
}