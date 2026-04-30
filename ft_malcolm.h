#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
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

extern int g_sockfd;

struct __attribute__((packed)) ethernet_header {
	uint8_t  dst_mac[6];
	uint8_t  src_mac[6];
	uint16_t ethertype;
};

struct __attribute__((packed)) arp_packet {
	uint16_t hw_type; // type d hardware
	uint16_t proto_type; // protocol ipv4
	uint8_t  hw_size; // taille du hw
	uint8_t  proto_size; // taile de ip
	uint16_t opcode; // si qsk ou reply
	uint8_t  src_mac[6];
	uint8_t  src_ip[4];
	uint8_t  dst_mac[6];
	uint8_t  dst_ip[4];
};

typedef struct s_addrs {
	uint8_t source_ip[4];
	uint8_t source_mac[6];
	uint8_t target_ip[4];
	uint8_t target_mac[6];
} t_addrs;

// utils.c
void         *ft_memset(void *s, int c, size_t n);
void         *ft_memcpy(void *dst, const void *src, size_t n);
int           ft_memcmp(const void *s1, const void *s2, size_t n);
int           ft_strcmp(const char *s1, const char *s2);
size_t        ft_strlen(const char *s);
unsigned long ft_strtoul(const char *ptr, char **endptr, int base);

// args.c
int  parse_args(int ac, char **av, int *verbose, int *gratuitous,
		int *hex, int *arg_offset);

// parse.c
int  MAC_getter(char *to_convert, uint8_t *converted);
int  ip_getter(char *to_convert, uint8_t *converted);
int  load_addresses(char **args, t_addrs *a);

// network.c
int  setup_network(int *interface_idx);

// arp.c
int  receive_arp(int sockfd, uint8_t *buffer, t_addrs *addrs);
void build_reply(uint8_t *reply, t_addrs *addrs);
int  send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac, int interface_idx, int hex);
int  send_gratuitous(t_addrs *addrs, int interface_idx, int verbose, int hex);
int  run_spoof(t_addrs *addrs, int interface_idx, int verbose, int hex);

// log.c
void log_arp_request(uint8_t *buffer);
void print_verbose(uint8_t *buffer, t_addrs *addrs);
void print_verbose_gratuitous(t_addrs *addrs);
void hexdump(uint8_t *data, int len);
