#include <stdio.h>
#include <stdlib.h>
#include <libnet.h>
#include <pcap.h>

int main(int argc, char** argv)
{
	u_long payload_s = 0;
	u_long LIBNET_PROTO_H;
	char payload[255];
	char err_buf[LIBNET_ERRBUF_SIZE] = "";
	char* device = "en0";
	u_short proto = IPPROTO_TCP;
	u_char src_mac[6] = {0x98, 0x01, 0xa7, 0xd6, 0x77, 0x43};
	u_char dst_mac[6] = {0x58, 0x69, 0x6c, 0xa5, 0xe2, 0xd3};
	char* src_ip_str = "172.20.26.208";
	char* dst_ip_str = "61.167.60.70";
	int src_port = 51938;
	int dst_port = 80;
	u_long dst_ip, src_ip;

	if(argc == 2){
		if(!strcmp(argv[1], "tcp")){
			LIBNET_PROTO_H = LIBNET_TCP_H;
			proto = IPPROTO_TCP;
		}
		else if(!strcmp(argv[1], "udp")){
			LIBNET_PROTO_H = LIBNET_UDP_H;
			proto = IPPROTO_UDP;
		}
		else{
			printf("wrong protol\n");
			exit(1);
		}
		printf("   Proto:%s\n", argv[1]);
	}

	payload_s = sprintf(payload, "%s", "Can you hear me?");
	// payload_s *= 4;
	/*初始化*/
	libnet_t* lib_net = NULL;
	if((lib_net = libnet_init(LIBNET_LINK_ADV, device, err_buf)) == NULL){
		perror("libnet_init error");
		exit(1);
	}
	/*将字符串类型转为顺序网络字节流*/
	src_ip = libnet_name2addr4(lib_net, src_ip_str, LIBNET_RESOLVE);
	dst_ip = libnet_name2addr4(lib_net, dst_ip_str, LIBNET_RESOLVE);

	libnet_ptag_t lib_t = 0;
	uint32_t seq = 0;
	uint32_t ack = 0;
	uint16_t win = 65535;
	switch(proto){
		case IPPROTO_UDP:
			/*构造udp包*/
			lib_t = libnet_build_udp(src_port, dst_port, LIBNET_UDP_H+payload_s, 
				0, (u_char*)payload, payload_s, lib_net, 0);
			break;
		case IPPROTO_TCP:
			// lib_t = libnet_build_tcp_options((u_char*)payload, payload_s, lib_net, 0);
			/*构造tcp包*/
			lib_t = libnet_build_tcp(src_port, dst_port, seq, ack, TH_PUSH|TH_ACK, 
				win, 0, 0, LIBNET_TCP_H+payload_s, (u_char*)payload, payload_s, lib_net, 0);
			break;
		default:break;
	}
	
	if(lib_t == -1){
		perror("libnet_buld error");
		exit(1);
	}
	

	/*构造ip包*/
	lib_t = libnet_build_ipv4(
		LIBNET_IPV4_H+LIBNET_PROTO_H+payload_s,
		0,
		(u_short)libnet_get_prand(LIBNET_PRu16),	/*id随机产生0~65535*/
		0,
		(u_int8_t)libnet_get_prand(LIBNET_PR8),
		proto,
		0,
		src_ip, dst_ip,
		NULL, 
		0,
		lib_net, 0
	);
	if(lib_t == -1){
		perror("libnet_buld_ipv4 error");
		exit(1);
	}

	/*构造以太网头部*/
	lib_t = libnet_build_ethernet(
		(u_int8_t*)dst_mac,
		(u_int8_t*)src_mac, 
		ETHERTYPE_IP, 
		NULL, 
		0, 
		lib_net, 
		0
	);
	if(lib_t == -1){
		perror("libnet_build_ethernet error");
		exit(1);
	}

	/*发送数据*/
	int res = 0;
	if((res = libnet_write(lib_net)) == -1){
		perror("libnet_write error");
		exit(1);
	}

	libnet_destroy(lib_net);
	printf("  Device:%s\n", device);
	printf("  Src ip:%s\n", src_ip_str);
	printf("  Dst ip:%s\n", dst_ip_str);
	printf("Src port:%d\n", src_port);
	printf("Dst port:%d\n", dst_port);
	printf("    Send:%s\n", payload);
	printf(" Pay_len:%ld\n", payload_s);
	printf("----------OK!----------\n");
	return 0;
}