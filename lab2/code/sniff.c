#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "ether_tcp_ip.h"

/* default snap length (maximum bytes per packet to capture)*/
/* 默认的snap长度, 捕获的每个数据包的最大字节数*/
#define	DEFAULT_SNAPLEN	2000

/* ethernet headers are always exactly 14 bytes [1]*/
/* 以太网的头部总是刚好14个字节*/
#define	SIZE_ETHERNET	14

void print_hex_ascii_line(const u_char* payload, int len, int offset)
{
	int i;
	int gap;
	const u_char* ch;

	/* offset*/		/* 偏移量*/
	printf("%05d   ", offset);

	/* hex*/		/* 16进制*/
	ch = payload;
	for(i = 0; i < len; i++){
		printf("%02x ", *ch);
		ch++;
		/* print extra space after 8th byte for visual aid
		 * 打印第8个字节之后的空格 */
		if(i == 7)
			printf(" ");
	}
	/* print space to handle line less than 8bytes
	 * 如果少于8个字节, 打印空格*/
	if(len < 8)
		printf(" ");

	/* fill hex gap with spaces if not full line
	 * 补齐不满16个字节的空格, 显示ascii时美观 */
	if(len < 16){
		gap = 16 - len;
		for(i = 0; i < gap; i++)
			printf("   ");
	}
	printf("   ");

	/* ascii (if printable)
	 * ascii码打印*/
	ch = payload;
	for(i = 0; i < len; i++){
		if(isprint(*ch))
			printf("%c", *ch);
		else
			printf(".");	/* 不可打印用.代替*/
		ch++;
	}

	printf("\n");

	return;
}

/* print packet payload data (avoid printing binary data)
 * 打印数据包的内容 (避免打印出二进制数) */
void print_payload(const u_char* payload, int len, FILE* fp)
{
	int len_rem = len;
	int line_width = 16;		/* number of bytes per line */	/* 每行的字节数*/
	int line_len;
	int offset = 0;				/* zero-based offset counter */	/* 初始化为0的偏移量*/
	const u_char* ch = payload;
	
	if(len <= 0)
		return;

	/* data fits on one line 
	 * 数据刚好只有一行 */
	if(len <= line_width){
		print_hex_ascii_line(ch, len, offset);
		return;
	}

	/* data spans multiple lines
	 * 多行数据*/
	for(;;){
		/* compute current line length */				/* 计算当前行长度*/
		line_len = line_width % len_rem;
		printf("width:%d len_rem:%d len:%d\n", line_width, len_rem, line_len);
		/* print line*/									/* 打印一行*/
		print_hex_ascii_line(ch, line_len, offset);
		/* compute total remaining */					/* 计算剩余总数*/
		len_rem = len_rem - line_len;
		/* shift pointer to remaining bytes to print*/	/* 将指针指向剩余要打印的字节开头*/
		ch = ch + line_len;
		/* add offset */								/* 改变输出偏移量*/
		offset = offset + line_width;
		/* check if we have line width chars or less*/	/* 检查是否只剩下一行*/
		if(len_rem <= line_width){
			/* print last line and get out */			/* 打印最后一行*/
			print_hex_ascii_line(ch, len_rem, offset);
			break;
		}
	}
}

void grap(u_char* user, const struct pcap_pkthdr* h, const u_char* packet)
{
	static int count = 0;					/* packect counter*/	/* 包计数变量*/

	/* declare pointers to packet headers*/
	/* 声明指向数据包头部的指针*/
	const struct sniff_ethernet* ethernet;	/* The ethernet header [1]*/	/* 以太网头部*/
	const struct sniff_ip* ip;				/* The IP header*/	/* IP头部*/
	const struct sniff_tcp* tcp;			/* The TCP header*/	/* TCP头部*/
	const u_char* payload;					/* Packet payload*/	/* 数据包内容*/

	int size_ip;
	int size_tcp;
	int size_payload;

	count++;
	printf("\nPacket number %d:\n", count);

	/* define ethernet header*/
	/* 定义以太网头部*/
	ethernet = (struct sniff_ethernet*)packet;

	/* define/compute ip header offset*/
	/* 计算IP头部偏移*/
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;	/*每4字节计数*/
	if(size_ip<20){
		printf(" * Invalid IP header length:%u bytes\n", size_ip);
		return;
	}

	/* print source and destination IP address*/
	/* 打印源IP, 目的IP*/
	printf("    From: %s\n", inet_ntoa(ip->ip_src));
	printf("      To: %s\n", inet_ntoa(ip->ip_dst));

	/* determine protocol*/
	/* 定义/选择协议类型*/
	switch(ip->ip_p){
		case IPPROTO_TCP:
			printf("Protocol: TCP\n");
			break;
		case IPPROTO_UDP:
			printf("Protocol: UDP\n");
			return;
		case IPPROTO_ICMP:
			printf("Protocol: ICMP\n");
			return;
		case IPPROTO_IP:
			printf("Protocol: IP\n");
			return;
		default:
			printf("Protocol: Unknown\n");
			return;
	}

	/* OK, this packet is TCP*/
	/* 此时可以确保数据包为TCP包*/

	/* define/compute tcp header offset*/
	/* 计算tcp头部偏移*/
	tcp = (struct sniff_tcp*)(packet+SIZE_ETHERNET+size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if(size_tcp<20){
		printf(" * Invalid TCP header length:%u bytes\n", size_tcp);
		return;
	}
	printf("Src Port: %d\n", ntohs(tcp->th_sport));
	printf("Dst Port: %d\n", ntohs(tcp->th_dport));

	/* define/compute tcp payload (segment) offset*/
	/* 计算tcp内容偏移*/
	payload = (u_char*)(packet+SIZE_ETHERNET+size_ip+size_tcp);

	/* compute tcp payload (segment) size*/
	/* 计算tcp内容的长度*/
	size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

	/* Print payload data; it might be binary, so  don'the just treat it as a string*/
	/* 打印tcp内容; 可能是二进制形式, 不能将其简单的视为字符串*/
	if(size_payload > 0){
		printf("Payload (%d bytes):\n", size_payload);
		// print_payload(payload, size_payload, (FILE*)user);
	}

	/* 使用传入的参数 user为文件名*/
	if(user != NULL){
		FILE* fp = fopen((char*)user, "a");
		fprintf(fp, "\nPacket number %d:\n", count);
		fprintf(fp, "    From: %s\n", inet_ntoa(ip->ip_src));
		fprintf(fp, "      To: %s\n", inet_ntoa(ip->ip_dst));
		fprintf(fp, "Src Port: %d\n", ntohs(tcp->th_sport));
		fprintf(fp, "Dst Port: %d\n", ntohs(tcp->th_dport));
		fclose(fp);
	}
}

int main(int argc, char **argv)
{
	char* device = NULL;			/* capture device name*/	/* 捕获设备名称*/
	char ebuf[PCAP_ERRBUF_SIZE];	/* error buffer*/			/* 存储错误信息的数组*/
	pcap_t* pd;						/* packet capture handle */	/* 捕包句柄*/

	char* filter_exp = "tcp";			/* filter expression*/		/*过滤器表达式*/ 
	struct bpf_program fcode;		/* compiled filter program (expression)*/	/* 编译后的过滤器程序*/
	bpf_u_int32 localnet, netmask;	/* ip and subnet mask*/		/* ip和子网掩码*/
	int num_packets = -1;			/* number of packets to capture
									 * 捕包数量 默认-1 为循环捕包*/

	/*check for capture device name on command-line*/
	/* 检查命令行输入的文件名和过滤表达式. 源代码中输入参数为捕获设备, 此处作需求更改*/	
	char* filename = "sniffer.log";
	int opt;
	while((opt = getopt(argc, argv, "f:e:n:"))!=-1){
		switch(opt){
			case 'f':
				filename = optarg;
				printf("   Filename: %s\n", filename);
				break;
			case 'e':
				filter_exp = optarg;
				printf(" Filter_exp: %s\n",filter_exp);
				break;
			case 'n':
				num_packets = atoi(optarg);
				printf("Packets_num: %d\n",num_packets);
				break;
			default:
				printf("./sniff -f <filename> -e <filter_exp> -n <packets_num>\n");
				exit(1);
		}
	}
	printf("\n");

	/* find a capture device*/
	/* 寻找捕获设备*/
	device = pcap_lookupdev(ebuf);
	if(device == NULL){
		printf("Couldn't find default device:%s\n", ebuf);
		exit(EXIT_FAILURE);
	}

	/* get network number and mask associated with capture device
	 * 获取指定网络设备的网络号和掩码*/
	if(pcap_lookupnet(device, &localnet, &netmask, ebuf) == -1){
		printf("Couldn't get netmask for device %s: %s\n", device, ebuf);
		netmask = 0;
		localnet = 0;
	}

	/* print capture info 
	 * 打印捕包信息*/
	printf("Device: %s\n", device);
	printf("%u.%u.%u.%u", localnet&0xff, localnet>>8&0xff, localnet>>16&0xff, localnet>>24&0xff);
	printf(":%d.%d.%d.%d\n", netmask&0xff, netmask>>8&0xff, netmask>>16&0xff, netmask>>24&0xff);

	/* open capture device 
	 * 打开捕获设备*/
	pd = pcap_open_live(device, DEFAULT_SNAPLEN, 1, 1000, ebuf);
	if(pd == NULL){
		printf("Couldn't open device %s: %s\n", device, ebuf);
		exit(EXIT_FAILURE);
	}

	/* make sure we're capturing on an Ethernet device [2]
	 * 确保捕获的设备在以太网上*/
	if(pcap_datalink(pd) == DLT_EN10MB)
		printf("10Mb以太网\n");
	else{
		printf("%s is not an Ethernet\n", device);
		exit(EXIT_FAILURE);
	}

	/* compile the filter expression 
	 * 将表达式编译到过滤程序中 */
	if(pcap_compile(pd, &fcode, filter_exp, 1, netmask) == -1){
		printf("Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(pd));
		exit(EXIT_FAILURE);
	}

	/* apply the compiled filter 
	 * 应用编译过的过滤程序*/
	if(pcap_setfilter(pd, &fcode) == -1){
		printf("Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(pd));
		exit(EXIT_FAILURE);
	}

	/* now we can set our callback function 
	 * 设定callback回调函数*/
	pcap_loop(pd, num_packets, grap, (u_char*)filename);

	/* cleanup */
	pcap_freecode(&fcode);
	pcap_close(pd);

	printf("\nCapture complete.\n");
	return 0;
}