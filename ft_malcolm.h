#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>   // strtok, memcpy, memset, memcmp
#include <stdlib.h>   // strtoul, exit
#include <unistd.h>   // close, getuid
#include <signal.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

#define ARP_FRAME_SIZE  42
#define ETH_HEADER_SIZE 14

extern int g_sockfd;

struct ethernet_header {
	uint8_t  dst_mac[6];    // 6 x 1 octet
	uint8_t  src_mac[6];
	uint16_t ethertype;     // 2 octets
};

struct arp_packet {
	uint16_t hw_type;
	uint16_t proto_type;
	uint8_t  hw_size;
	uint8_t  proto_size;
	uint16_t opcode;
	uint8_t  src_mac[6];
	uint8_t  src_ip[4];
	uint8_t  dst_mac[6];
	uint8_t  dst_ip[4];
};

// adresses converties en octets, prêtes à être copiées dans les trames
typedef struct s_addrs {
	uint8_t source_ip[4];
	uint8_t source_mac[6];
	uint8_t target_ip[4];
	uint8_t target_mac[6];
} t_addrs;

// parse.c
int  MAC_getter(char *to_convert, uint8_t *converted);
int  ip_getter(char *to_convert, uint8_t *converted);
int  load_addresses(char **args, t_addrs *a);

// network.c
int  setup_socket(void);
int  get_interface(int sockfd);
int  setup_network(int *ifindex);

// arp.c
int  receive_arp(int sockfd, uint8_t *buffer, uint8_t *source_ip);
void build_reply(uint8_t *reply, t_addrs *addrs);
int  send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac, int ifindex);
int  run_spoof(t_addrs *addrs, int ifindex, uint8_t *buffer);
