* 网络中唯一标识进程

  [本地]进程PID

  [网络]网络层"ip地址"+传输层"协议+端口"

* socket

  UNIX BSD的套接字

  一切皆文件，open - read/write - close

  socket pair唯一标识一个连接

* 函数

  ```c
  /*创建一个socket描述符 类似打开一个文件*/
  //domain	协议域，协议族
  //type		socket类型	SOCK_STREAM 面向流的传输协议 SOCK_DGRAM - 面向数据报的传输协议 - UDP协议
  //protocol	指定协议 0为默认
  //return	监听socket描述字
  int socket(int domain, int type, int protocol);
  
  /*给socket赋特定地址 一般为server*/
  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  
  /*server调用listen监听socket*/
  //backlog	相应socket可排队的最大连接数,待连接数
  int listen(int sockfd, int backlog);
  
  /*client调用connect发出连接请求*/
  //sockaddr	server的socket地址
  int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  
  /*server监听到请求调用accept接收请求*/
  //sockaddr	用于返回客户端协议地址，是一个传出参数
  //addrlen	协议地址长度
  //return	已连接的socket描述字
  int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  
  /*关闭socket描述字*/
  int close(int fd);
  ```

  通常server端 socket - bind - listen

  client端 socket - connect (端口号调用connect时随机生成)

  > accept()
  >
  > addrlen参数是一个传入传出参数（value-result argument），传入的是调用者提供的缓冲区cliaddr的长度以避免缓冲区溢出问题，传出的是客户端地址结构体的实际长度（有可能没有占满调用者提供的缓冲区）。如果给cliaddr参数传NULL，表示不关心客户端的地址。
  >
  > 由于cliaddr_len是传入传出参数，每次调用accept()之前应该重新赋初值。

  ```c
  #include <unistd.h>
  //read()/write()返回小于0表示错误
  ssize_t read(int fd, void *buf, size_t count);
  ssize_t write(int fd, const void *buf, size_t count);
  
  #include <sys/types.h>
  #include <sys/socket.h>
  
  ssize_t send(int sockfd, const void *buf, size_t len, int flags);
  ssize_t recv(int sockfd, void *buf, size_t len, int flags);
  
  ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                 const struct sockaddr *dest_addr, socklen_t addrlen);
  ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                   struct sockaddr *src_addr, socklen_t *addrlen);
  
  ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
  ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
  ```

* 网络字节序与主机字节序

  主机字节序：大端模式、小端模式

  > a) Little-Endian就是低位字节排放在内存的低地址端，高位字节排放在内存的高地址端。
  >
  > b) Big-Endian就是高位字节排放在内存的低地址端，低位字节排放在内存的高地址端。

  网络字节序：TCP/IP首部中所有二进制整数传输

  > 网络字节序：4个字节的32 bit值以下面的次序传输：首先是0～7bit，其次8～15bit，然后16～23bit，最后是24~31bit。这种传输次序称作大端字节序。

  **注：将地址绑定到socket，先将主机字节序转换为网络字节序**

  > 为使网络程序具有可移植性，使同样的C代码在大端和小端计算机上编译后都能正常运行，可以调用以下库函数做网络字节序和主机字节序的转换。

  ```c
  #include <arpa/inet.h>
  /*h表示host 主机字节序，n表示network 网络字节序*/
  uint32_t htonl(uint32_t hostlong);
  uint16_t htons(uint16_t hostshort);
  uint32_t ntohl(uint32_t netlong);
  uint16_t ntohs(uint16_t netshort);
  ```

  hton 建立连接前

  ntoh 连接建立之后

* socket地址的数据类型

  ![socket_struct](/Users/admin/Library/Application Support/typora-user-images/image-20190309110949861.png)

  IPv4 - AF_INET - sockaddr_in - 16位端口号+32位IP地址

  IPv6 - AF_INET6 - sockaddr_in6 - 16位端口号+128位IP地址+控制字段

  UNIX Domain Socket - AF_UNIX - sockaddr_un

  > socket API可以接受各种类型的sockaddr结构体指针做参数，例如bind、accept、connect等函数，这些函数的参数应该设计成void *类型以便接受各种类型的指针，但是sock API的实现早于ANSI C标准化，那时还没有void *类型，因此这些函数的参数都用struct sockaddr *类型表示，在传递参数之前要强制类型转换一下。

  ```c
  struct sockaddr_in servaddr;
  
  /* initialize servaddr */
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;	//IPv4
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  
  bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  ```

  > 网络地址为INADDR_ANY，这个宏表示本地的任意IP地址，因为服务器可能有多个网卡，每个网卡也可能绑定多个IP地址，这样设置可以在所有的IP地址上监听，直到与某个客户端建立了连接时才确定下来到底用哪个IP地址。

  ```c
  #include <arpa/inet.h>
  /*字符串转in_addr*/
  int inet_aton(const char *strptr, struct in_addr *addrptr);
  in_addr_t inet_addr(const char *strptr);
  int inet_pton(int family, const char *strptr, void *addrptr);
  
  /*in_addr转字符串*/
  char *inet_ntoa(struct in_addr inaddr);
  const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
  ```

  

* TCP三次握手建立连接

  ![3hands](/Users/admin/Library/Application Support/typora-user-images/image-20190309102322145.png)

  > 客户端的connect在三次握手的第二个次返回，而服务器端的accept在三次握手的第三次返回。

* TCP四次握手释放连接

  ![4handfree](/Users/admin/Library/Application Support/typora-user-images/image-20190309102423757.png)

  

* 错误分析

  > 调用accept、read和write被信号中断时应该重试。connect虽然也会阻塞，但是被信号中断时不能立刻重试。对于accept，如果errno是ECONNABORTED，也应该重试。

* [Linux Socket编程(不限Linux)](https://www.cnblogs.com/skynet/archive/2010/12/12/1903949.html)

* [Linux C一站式学习 - socket编程](https://www.kancloud.cn/wizardforcel/linux-c-book/134964)

* >通信双方建立TCP连接后，主动关闭连接的一方就会进入TIME_WAIT状态。
  >
  >客户端主动关闭连接时，会发送最后一个ack后，然后会进入TIME_WAIT状态，再停留2个MSL时间(后有MSL的解释)，进入CLOSED状态。





* 文件位于执行程序所在的目录下

* 发送文件sendfile()

  linux 与 mac不兼容

  ```c
  struct stat {
      dev_t         st_dev;       //文件的设备编号
      ino_t         st_ino;       //节点
      mode_t        st_mode;      //文件的类型和存取的权限
      nlink_t       st_nlink;     //连到该文件的硬连接数目，刚建立的文件值为1
      uid_t         st_uid;       //用户ID
      gid_t         st_gid;       //组ID
      dev_t         st_rdev;      //(设备类型)若此文件为设备文件，则为其设备编号
      off_t         st_size;      //文件字节数(文件大小)
      unsigned long st_blksize;   //块大小(文件系统的I/O缓冲区大小)
      unsigned long st_blocks;    //块数
      time_t        st_atime;     //最后一次访问时间
      time_t        st_mtime;     //最后一次修改时间
      time_t        st_ctime;     //最后一次改变时间(指属性)
  };
  ```

  

* Write(STDOUT_FILENO, buf, n);

* socket error: Too many open files

  查看进程pid

  ps

  查看进程的socket数

  lsof -p [pid] | wc -l

  临时修改进程数

  ulimit -n 

* close的位置

* 调用类内的对象

* [Qt多线程](https://blog.csdn.net/czyt1988/article/details/64441443)

* socket qt connect

* 互锁

* Q_Object 执行qmake

* [malloc: * error for object 0x7f9fcc448a58: pointer being freed was not allocated * set a breakpoint in malloc_error_break to debug](https://stackoverflow.com/questions/12049058/malloc-error-for-object-0x165060-pointer-being-freed-was-not-allocated)

* 疑问 运行多次槽函数

* [Qt使用多线程的一些心得——1.继承QThread的多线程使用方法](https://blog.csdn.net/czyt1988/article/details/64441443)

* [Qt使用多线程的一些心得——2.继承QObject的多线程使用方法]()https://blog.csdn.net/czyt1988/article/details/71194457
