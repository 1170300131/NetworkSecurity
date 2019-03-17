#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include "warp.h"

void perror_exit(const char* s)
{
	perror(s);
	exit(1);
}

int Socket(int family, int type, int protocol)
{
	int fd;
	if((fd = socket(family, type, protocol))<0){
		perror_exit("socket error");
	}
	return fd;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{

	if((bind(sockfd, addr, addrlen))<0){
		perror_exit("bind failed");
	}
}

void Listen(int sockfd, int backlog)
{
	if(listen(sockfd, backlog)<0){
		perror_exit("listen failed");
	}
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	if(connect(sockfd, addr,addrlen)<0){
		perror_exit("connect failed");
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int connectfd;
again:
	if((connectfd = accept(sockfd, addr, addrlen))<0){
		if((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			perror_exit("accept failed");
	}
	return connectfd;
}

void Close(int fd)
{
	if(close(fd)<0){
		perror_exit("close failed");
	}
}

ssize_t Read(int fd, void *buf, size_t count)
{
	ssize_t n;
again:
	if((n=read(fd, buf, count))<0){
		if((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			perror_exit("read error");
	}
	return n;
}

ssize_t Write(int fd, const void *buf, size_t count)
{
	ssize_t n;
again:
	if((n=write(fd, buf, count))<0){
		if((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			perror_exit("write error");
	}
	return n;
}

// ssize_t Readn(int fd, void *vptr, size_t n)
// {
// 	size_t nleft;
// 	ssize_t nread;
// 	char* ptr;

// 	ptr = vptr;
// 	nleft = n;
// 	while(nleft>0){
// 		if()
// 	}
// }

// ssize_t Readline(int fd, void *vptr, size_t maxlen)
// {
// 	char* ptr;

// 	ptr = vptr;
// 	for(ssize_t n=1; n<maxlen;n++){
// 		if(Read(fd, buf, sizeof(buf))){

// 		}
// 	}
// }


