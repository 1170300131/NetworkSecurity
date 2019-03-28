Libpcap 运行在类UNIX系统下的网络数据包捕获函数库，捕获网卡上的数据，也可以发送数据包。

##### 配置设备

```c
char *pcap_lookupdev(char *errbuf)
int pcap_lookupnet(char *device, bpf_u_int32 *netp,bpf_u_int32 *maskp, char *errbuf);
//device	获取的设备名称
//snaplen	pcap捕获的最大字节数
//promisc	表示布尔值的整数，true时进入混杂模式
//to_ms		读取数据超时时间 0表示不超时
//ebuf		存储错误信息的字符串
//return	返回会话处理程序
pcap_t *pcap_open_live(char *device, int snaplen, int promisc, int to_ms, char *ebuf)
```

混杂模式：一台机器能够接收所有经过它的数据流，不管目标地址是不是它。是相对于普通模式而言的，这被网络管理员使用来诊断网络问题，但也被无认证的想偷听网络通信的人利用。

> You need to determine the type of link-layer headers the device provides, and use that type when processing the packet contents. The pcap_datalink() routine returns a value indicating the type of link-layer headers; see [the list of link-layer header type values](http://www.tcpdump.org/linktypes.html). The values it returns are the `DLT_` values in that list.

由于不是所有设备都在读取的包中提供相同类型的数据链路层头部。

甄别数据链路层的头部类型 return link-layer header type for the live capture or save-file specified byp.

```c
int pcap_datalink(pcap_t *p);
```



```c
int pcap_compile(pcap_t *p, struct bpf_program *fp, char *str, int optimize, bpf_u_int32 netmask);
int pcap_setfilter(pcap_t *p, struct bpf_program *fp);
```



```c
//cnt	告诉pcap_loop在返回之前应该嗅探多少个数据包, 负值表示循环直到发生错误
//callback	回调函数名称
int pcap_loop(pcap_t * p, int cnt, pcap_handler callback, u_char * user)
```

```c
//user		对应pcap_loop中最后一个参数, 用户传入的参数
//header	pcap头，包含有关数据包嗅探的信息
//packet	指向包含整个数据包的数据部分的第一个字节
void callback(u_char * user, const struct pcap_pkthdr * header, const u_char * packet);
```

```c
 struct pcap_pkthdr { 
   struct timeval ts;		//时间戳
   bpf_u_int32 caplen;	//目前部分的长度
   bpf_u_int32 len;			//数据包长度
 };
```

不是一个字符串，而是一个结构的集合。

例如，一个TCP / IP数据包将有一个以太网头部，一个IP头部，一个TCP头部，最后一个，分组的有效载荷

u_char指针指向这些结构的序列化版本，使用时需要做类型转换。



```c
/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN  6

    /* Ethernet header */
    struct sniff_ethernet {
        u_char ether_dhost[ETHER_ADDR_LEN]; /* Destination host address */
        u_char ether_shost[ETHER_ADDR_LEN]; /* Source host address */
        u_short ether_type; /* IP? ARP? RARP? etc */
    };

    /* IP header */
    struct sniff_ip {
        u_char ip_vhl;      /* version << 4 | header length >> 2 */
        u_char ip_tos;      /* type of service */
        u_short ip_len;     /* total length */
        u_short ip_id;      /* identification */
        u_short ip_off;     /* fragment offset field */
    #define IP_RF 0x8000        /* reserved fragment flag */
    #define IP_DF 0x4000        /* dont fragment flag */
    #define IP_MF 0x2000        /* more fragments flag */
    #define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */
        u_char ip_ttl;      /* time to live */
        u_char ip_p;        /* protocol */
        u_short ip_sum;     /* checksum */
        struct in_addr ip_src,ip_dst; /* source and dest address */
    };
    #define IP_HL(ip)       (((ip)->ip_vhl) & 0x0f)
    #define IP_V(ip)        (((ip)->ip_vhl) >> 4)

    /* TCP header */
    typedef u_int tcp_seq;

    struct sniff_tcp {
        u_short th_sport;   /* source port */
        u_short th_dport;   /* destination port */
        tcp_seq th_seq;     /* sequence number */
        tcp_seq th_ack;     /* acknowledgement number */
        u_char th_offx2;    /* data offset, rsvd */
    #define TH_OFF(th)  (((th)->th_offx2 & 0xf0) >> 4)
        u_char th_flags;
    #define TH_FIN 0x01
    #define TH_SYN 0x02
    #define TH_RST 0x04
    #define TH_PUSH 0x08
    #define TH_ACK 0x10
    #define TH_URG 0x20
    #define TH_ECE 0x40
    #define TH_CWR 0x80
    #define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;     /* window */
        u_short th_sum;     /* checksum */
        u_short th_urp;     /* urgent pointer */
};
```



IP报头，不像以太网报头，并不具有固定的长度; 它的长度是由IP报头的头长度字段指定的，每4字节计数。由于它是每4字节计数，它必须乘以4，以字节数来表示大小。该头的最小长度为20字节。 

TCP头也是可变长度; 其长度由TCP头部的“数据偏移”域指定为每4字节计数，其最小长度也为20个字节。 

| **Variable**   | **Location (in bytes)**                                      |
| -------------- | ------------------------------------------------------------ |
| sniff_ethernet | X                                                            |
| sniff_ip       | X + SIZE_ETHERNET                                            |
| sniff_tcp      | X + SIZE_ETHERNET + {IP header length}                       |
| payload        | X + SIZE_ETHERNET + {IP header length} + {TCP header length} |

##### IP Header

* 版本号 (4bit)

  路由器通过版本号确定如何解释数据报的剩余部分。

* 首部长度 (4bit)

  一个IPv4数据报可包含一些可变数量的选项(这些选项包括在IPv4数据报首部中), 故需要4bit来确定IP数据报中数据部分实际从哪开始。

  大多数IP不包含选项，一般长度为20个字节。

* 服务类型 (TOS) (8bit) - 区分服务时使用

  按位被定义 PP DTRC0

  PPP: 定义包的优先级, 取值越大数据越重要

  * 000 普通的(Routine)
  * 001 优先的(Priority)
  * 010 立即发送(Immediate)
  * 011 闪电式的(Flash)
  * 100 Flash Override
  * 101 CRI/TIC/ECP
  * 110 网间控制 Internetwork Control
  * 111 网络控制 Network Control

  D: 延时

  T: 吞吐量

  R: 可靠性

  M: 传输成本

  0: 最后一位保留, 恒定为0

* 数据报总长度 (16bit)

* 标识(ID) (16bit) - 标识一个IP分组，每产生一个IP分组计数器加1 

* 标记(FLags) (3bit)

  保留 - DF - MF

  * DF - Don't Fragment - 1:禁止分片; 0: 允许分片
  * MF - More Fragment - 1:非最后一片; 0: 最后一片, 或者未分片

* 偏移字段 (13bit)

  指定该片应放在初始IP数据报的哪个位置

* 生存时间(TTL) (8bit)

  每当数据报由一台路由器处理时, TTL减1, 当TTL=0时丢弃该IP分组。

* 协议 (8bit)

  指示IP数据报的数据部分应交给哪个特定的传输层协议

  6 - TCP; 17 - UDP etc.

* 首部校验和 (16bit)

  用于帮助路由器检测收到的IP数据报中的比特错误。

  校验和：首部每2个字节当作一个数，反码运算求和，和的反码(因特网校验和)存在首部校验和字段中。

  每台路由器上必须重新计算校验和并再次存放到原处，因为TTL字段以及可能的选项字段会改变。

* 源IP和目的IP(32bit)

* 可选项

* 数据(有效载荷)

##### TCP Header

* 源端口号(16bit)

* 目的端口号 (16bit)

* 序号 (32bit) - 可靠传输 - 报文段首字节的字节流编号

  序号建立在传输的字节流之上，而不是建立在传送的报文段的序列之上。

* 确认号 (32bit) - 可靠传输

  TCP是全双工的。

  A - B通信，A填充的确认号是期望从B收到的下一个字节的序号，一方面可以通知B已经收到了上一个序号的数据，一方面可以发送数据。

* 首部长度 (4bit)

  4个字节为单位的TCP首部长度，有选项字段，长度可变，一般20字节

* 保留未用 (6bit) ？

* 标志字段 (6bit)

  URG - ACK - PSH - RST - SYN - FIN

  ACK：指示确认字段中的值是有效的，即该报文段包括一个对已成功接收报文段的确认，

  RST、SYN、FIN：连接建立和拆除

  URG：指示报文段里存在被发送端上层实体置为"紧急"的数据

* 接受窗口 (16bit)

  用于流量控制，指示接收方愿意接受的字节数量

* 因特网校验和 (16bit)

* 紧急数据指针 (16bit) (实际不使用)

  当紧急数据存在并给出指向紧急数据尾的指针时，TCP通知接收端的上层实体。

* 选项

* 数据

##### 参考

[Libpcap库编程指南-数据包捕获](https://blog.csdn.net/thinkerleo1997/article/details/77931789)

[Programming with pcap](http://www.tcpdump.org/pcap.html)

[sniffex.c](http://www.tcpdump.org/sniffex.c)



##### 待办

[打印时间](https://blog.csdn.net/WU9797/article/details/76922323)