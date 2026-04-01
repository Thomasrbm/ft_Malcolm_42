#pragma once

#include <stdint.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <ifaddrs.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <linux/if.h>
#include <string.h>   // strtok
#include <stdlib.h>   // strtoul
#include <unistd.h>   // close, getuid
#include <signal.h>
#include <stdlib.h>  
#include <netdb.h>

extern int g_sockfd;


struct ethernet_header {
    uint8_t  dst_mac[6];    // 6 x 1 octet
    uint8_t  src_mac[6];
    uint16_t ethertype;   // 2 octet
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

int		MAC_getter(char *to_convert, uint8_t *converted);
int		ip_getter(char *to_convert, uint8_t *converted);
int		setup_socket(void);
int		get_interface(int sockfd);
int		receive_arp(int sockfd, uint8_t *buffer, uint8_t *source_ip_converted);
void	build_reply(uint8_t *reply, uint8_t *target_mac_converted, uint8_t *source_mac_converted, uint8_t *source_ip_converted, uint8_t *target_ip_converted);
int		send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac_converted, int ifindex);