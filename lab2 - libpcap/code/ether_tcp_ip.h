#include <pcap.h>
#include <arpa/inet.h>

/* Ethernet addresses are 6 bytes*/
/* 以太网地址为6个字节*/
#define	ETHER_ADDER_LEN	6

/*Ethernet header*/
/*以太网头部*/
struct sniff_ethernet
{
	u_char ether_dhost[ETHER_ADDER_LEN];	/* Destination host address 目标地址*/
	u_char ether_shost[ETHER_ADDER_LEN];	/* Source host address 源地址*/
	u_short ether_type;						/* IP? ARP? RARP? etc.*/
};

/*IP header*/
/*IP头部*/
struct sniff_ip
{
	u_char ip_vhl;					/* version << 4 | header length >> 2*/	/* 4位版本号 4位IP包头长度*/
	u_char ip_tos;					/* type of service*/		/* 服务类型*/
	u_short ip_len;					/* total length*/			/* IP包总长度(包括头部和数据)*/
	u_short ip_id;					/* indentification*/		/* 标识*/
	u_short ip_off;					/* fragment offset field*/	/* 分片使用 3位标志 13位片偏移*/
	#define IP_RF 0x8000			/* reserved fragment flag*/	/* 保留位*/
	#define IP_DF 0x4000			/* dont fragment flag*/		/* 不使用分段 010*/
	#define IP_MF 0x2000			/* more fragment flag*/		/* 1:不是最后一片,0:最后一片或者未分片*/
	#define IP_OFFMASK 0x1fff		/* mask for fragment bits*/	/* ip_off与之&得到偏移量*/
	u_char ip_ttl;					/* time to live */			/* 生存时间*/
	u_char ip_p;					/* protocol */				/* 协议*/
	u_short ip_sum;					/* checksum */				/* 首部校验和*/
	struct in_addr ip_src, ip_dst;	/* source and dest address */	/* 源IP, 目的IP*/
};
#define	IP_HL(ip)	(((ip)->ip_vhl)&0x0f)	/* 取ip_vhl的低4位, 即IP头长度 Header Length*/
#define IP_V(ip)	(((ip)->ip_vhl)>>4)		/* 取ip_vhl的高4位, 即版本号 Version*/

/*TCP header*/
/*TCP头部*/
typedef u_int tcp_seq;

struct sniff_tcp
{
	u_short th_sport;				/* source port */			/* 源端口*/
	u_short th_dport;				/* destination port */		/* 目的端口*/
	tcp_seq th_seq;					/* sequence number*/		/* 序列号*/
	tcp_seq th_ack;					/* acknowledgement number */	/* 确认号*/
	u_char th_offx2;				/* data offset, rsvd */		/* 首部长度和保留位*/
	#define	TH_OFF(th)	(((th)->th_offx2&0xf0)>>4)
	u_char th_flags;				/* 标志字段*/
	#define	TH_FIN	0x01
	#define	TH_SYN	0x02
	#define	TH_RST	0x03
	#define	TH_PUSH	0x04
	#define	TH_ACK	0x05
	#define	TH_URG	0x06
	#define	TH_ECE	0x07
	#define	TH_CWR	0x08
	#define	TH_FLAGS	(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
	u_short th_win;					/* window */				/* 接收窗口*/
	u_short th_sum;					/* checksum*/				/* 首部校验和*/
	u_short th_urp;					/* urgent pointer */		/* 紧急数据指针*/
};