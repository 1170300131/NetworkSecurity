



小型的接口函数库，提供了低层网络数据包的构造、处理和发送功能。。

建立一个简单统一的网络编程接口以屏蔽不同操作系统底层网络编程的差别。

libnet 库提供的接口函数包含 15 种数据包生成器和两种数据包发送器（IP 层和数据链路层）

接口函数包括：

* 内存管理(分配和释放)函数
* 地址解析函数
* 各种协议类型的数据包构造函数
* 数据包发送函数(IP层和链路层)
* 一些辅助函数，如产生随机数、错误报告、端口列表管理等



利用libnet开发应用程序

1. 数据包内存初始化
2. 构造数据包
3. 发送数据
4. 释放资源

UDP为例

```c
//初始化
libnet_init();
//数据包处理(上层到底层，udp-ip-mac)
libnet_buid_udp();
libnet_build_ipv4();
libnet_build_ethernet();
//发送数据
libnet_write();
//释放资源
libnet_destroy();
```

```c
/*数据包内存初始化及环境建立
 *injection_type(构造的类型):LIBNET_LINK(链路层), LIBNET_RAW4(网络接口层),...*/
libnet_t *libnet_init(int injection_type, char *device, char *err_buf);
/*释放资源*/
void libnet_destroy(libnet_t *l);
/*将网络字节序转换成点分十进制数串
 *in: 网络字节序的ip地址
 *use_name: LIBNET_RESOLVE(对应主机名), LIBNET_DONT_RESOLVE(对应点分十进制IPv4地址)
 *返回点分十进制ip地址*/
char* libnet_addr2name4(u_int32_t in, u_int8_t use_name);
u_int32_t libnet_name2addr4(libnet_t *l,  char *host_name,  u_int8_t use_name);
/*获取接口设备ip地址*/
u_int32_t libnet_get_ipaddr4(libnet_t *l);
/*获取接口设备硬件地址
 *返回指向mac地址的指针*/
struct libnet_ether_addr* libnet_get_hwaddr(libnet_t *l);
/*构造udp包
 *sp:源端口, dp:目的端口, len:udp包总长度, sum:校验和,设为0则libnet自动填充; payload:发送的文本内容
 *payload_s:内容长度, l:libnet_t*指针, 
 *ptag:协议标记,第一次组新的发送包时写0,同一个应用程序,下次组包时写函数返回值*/
libnet_ptag_t libnet_build_udp(u_int16_t sp, u_int16_t dp,
                               u_int16_t len, u_int16_t sum,
                               u_int8_t *payload, u_int32_t payload_s,
                               libnet_t *l, libnet_ptag_t ptag);
/*构造tcp包
 *sp:源端口, dp:目的端口, seq:序号, ack:ack标记, control:控制标记, win:窗口大小,
 *sum:校验和,设为0则libnet自动填充; urg:紧急指针, len:tcp包总长度, payload:发送的文本内容
 *payload_s:内容长度, l:libnet_t*指针, 
 *ptag:协议标记,第一次组新的发送包时写0,同一个应用程序,下次组包时写函数返回值*/
libnet_ptag_t libnet_build_tcp(u_int16_t sp, u_int16_t dp,
                               u_int32_t seq, u_int32_t ack,
                               u_int8_t control, u_int16_t win
                               u_int16_t sum, u_int16_t urg,
                               u_int16_t len, u_int8_t *payload,
                               u_int32_t payload_s, libnet_t *l,libnet_ptag_t ptag );
/*构造tcp选项包
 *options: tcp选项字符串, options_s:选项长度, l:libnet句柄, ptag:协议标记*/
libnet_ptag_t libnet_build_tcp_options(u_int8_t *options, u_int32_t options_s,
                                       libnet_t *l, libnet_ptag_t ptag );
/*构造IPv4数据包
 *ip_len:ip包总长, tos:服务类型, id:ip标识, flag:片偏移, ttl:生存时间, prot:上层协议, sum:校验和,
 *src:源ip, dst:目的ip, payload:内容, payload_s:内容长度, l:libnet句柄, ptag:协议标记*/
libnet_ptag_t libnet_build_ipv4(u_int16_t ip_len, u_int8_t tos,u_int16_t id, 
                                u_int16_t flag,u_int8_t ttl, u_int8_t prot,
                                u_int16 sum, u_int32_t src,u_int32_t dst, 
                                u_int8_t *payload,u_int32_t payload_s,libnet_t *l,
                                libnet_ptag_t ptag );
/*构造IPv4选项数据包*/
libnet_ptag_t libnet_build_ipv4_options(u_int8_t*options, u_int32_t options,
                                        libnet_t*l, libnet_ptag_t ptag);
/*构造arp数据包
 *hrd:硬件地址格式,ARPHRD_ETHER(以太网), pro:协议地址格式,ETHERTYPE_IP(IP协议)
 *hln:硬件地址长度, pln:协议地址长度,
 *op:ARP协议操作类型(1.ARP请求,2.ARP回应,3.RARP请求,4.RARP回应)
 *sha:发送者硬件地址, spa:发送者协议地址, tha:目标硬件地址, tpa:目标协议地址,
 *payload, payload_s, l, ptag*/
libnet_ptag_t libnet_build_arp(u_int16_t hrd, u_int16_t pro, u_int8_t hln, u_int8_t pln,
                               u_int16_t op, u_int8_t *sha,u_int8_t *spa, u_int8_t *tha,
                               u_int8_t *tpa, u_int8_t *payload, 
                               u_int32_t payload_s, libnet_t *l, libnet_ptag_t ptag );
/*构造以太网数据包
 *dst:目的mac, src:源mac, type:上层协议类型, payload, payload_s, l, ptag*/
libnet_ptag_t libnet_build_ethernet(u_int8_t*dst, u_int8_t *src,
                                    u_int16_ttype, u_int8_t*payload,
                                    u_int32_tpayload_s, libnet_t*l,libnet_ptag_t ptag );
/*发送数据包*/
int libnet_write(libnet_t * l);
```



##### 参考

[Linux网络编程 - libnet使用指南](https://blog.csdn.net/tennysonsky/article/details/44944849)

