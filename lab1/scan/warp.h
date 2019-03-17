#ifndef __WARP_H__
#define __WARP_H__

#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>

extern void perror_exit(const char* s);
extern int Socket(int family, int type, int protocol);
extern void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern void Listen(int sockfd, int backlog);
extern void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern void Close(int fd);
extern ssize_t Read(int fd, void *buf, size_t count);
extern ssize_t Write(int fd, const void *buf, size_t count);

#endif