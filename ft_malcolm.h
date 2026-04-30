#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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

#define ARP_FRAME_SIZE     42
#define ETH_HEADER_SIZE    14
#define NA_FRAME_SIZE      86
#define NS_BUFFER_SIZE     256
#define NA_FLAG_SOLICITED  0x40000000
#define NA_FLAG_OVERRIDE   0x20000000

extern int g_sockfd;

struct __attribute__((packed)) ethernet_header {
	uint8_t  dst_mac[6];
	uint8_t  src_mac[6];
	uint16_t ethertype;
};

struct __attribute__((packed)) arp_packet {
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

struct __attribute__((packed)) ipv6_header {
	uint32_t version_tc_flow;
	uint16_t payload_len;
	uint8_t  next_header;
	uint8_t  hop_limit;
	uint8_t  src[16];
	uint8_t  dst[16];
};

struct __attribute__((packed)) icmpv6_ns {
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
	uint32_t reserved;
	uint8_t  target[16];
};

struct __attribute__((packed)) icmpv6_na {
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
	uint32_t flags;
	uint8_t  target[16];
	uint8_t  opt_type;
	uint8_t  opt_len;
	uint8_t  opt_mac[6];
};

typedef struct s_addrs {
	uint8_t source_ip[4];
	uint8_t source_mac[6];
	uint8_t target_ip[4];
	uint8_t target_mac[6];
	uint8_t source_ipv6[16];
	uint8_t target_ipv6[16];
} t_addrs;

// args.c
int  parse_args(int ac, char **av, int *verbose, int *gratuitous,
		int *ipv6_mode, int *arg_offset);

// parse.c
int  MAC_getter(char *to_convert, uint8_t *converted);
int  ip_getter(char *to_convert, uint8_t *converted);
int  ip6_getter(char *to_convert, uint8_t *converted);
int  load_addresses(char **args, t_addrs *a);
int  load_addresses_v6(char **args, t_addrs *a);

// network.c
int  setup_socket(void);
int  get_interface(int sockfd, int proto);
int  setup_network(int *ifindex);

// arp.c
int  receive_arp(int sockfd, uint8_t *buffer, t_addrs *addrs);
void build_reply(uint8_t *reply, t_addrs *addrs);
int  send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac, int ifindex);
int  send_gratuitous(t_addrs *addrs, int ifindex, int verbose);
int  run_spoof(t_addrs *addrs, int ifindex, uint8_t *buffer, int verbose);

// log.c
void log_arp_request(uint8_t *buffer);
void print_verbose(uint8_t *buffer, t_addrs *addrs);
void print_verbose_gratuitous(t_addrs *addrs);
void log_ns_received(uint8_t *buffer);
void print_ndp_verbose(uint8_t *buffer, t_addrs *addrs);

// ndp_build.c
void build_na(uint8_t *reply, t_addrs *addrs);

// ndp.c
int  setup_ndp_socket(void);
int  receive_ns(int sockfd, uint8_t *buffer, t_addrs *addrs);
int  send_na(int sockfd, uint8_t *reply, uint8_t *target_mac, int ifindex);
int  run_ndp_spoof(t_addrs *addrs, int verbose);
