#include "ft_malcolm.h"

void log_arp_request(uint8_t *buffer)
{
	struct arp_packet *arp;

	arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE);
	printf("An ARP request has been broadcast.\n");
	printf("mac address of request: %02x:%02x:%02x:%02x:%02x:%02x\n",
		arp->src_mac[0], arp->src_mac[1], arp->src_mac[2],
		arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
	printf("IP address of request: %s\n", inet_ntoa(*(struct in_addr *)arp->src_ip));
}

void print_verbose(uint8_t *buffer, t_addrs *addrs)
{
	struct arp_packet *arp;

	arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE);
	printf("[REQUEST] from IP:  %s\n", inet_ntoa(*(struct in_addr *)arp->src_ip));
	printf("[REQUEST] from MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		arp->src_mac[0], arp->src_mac[1], arp->src_mac[2],
		arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
	printf("[REPLY]   spoofing IP:  %s\n", inet_ntoa(*(struct in_addr *)addrs->source_ip));
	printf("[REPLY]   spoofing MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->source_mac[0], addrs->source_mac[1], addrs->source_mac[2],
		addrs->source_mac[3], addrs->source_mac[4], addrs->source_mac[5]);
	printf("[REPLY]   target IP:    %s\n", inet_ntoa(*(struct in_addr *)addrs->target_ip));
	printf("[REPLY]   target MAC:   %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->target_mac[0], addrs->target_mac[1], addrs->target_mac[2],
		addrs->target_mac[3], addrs->target_mac[4], addrs->target_mac[5]);
}

void print_verbose_gratuitous(t_addrs *addrs)
{
	printf("[SEND] spoofing IP:  %s\n", inet_ntoa(*(struct in_addr *)addrs->source_ip));
	printf("[SEND] spoofing MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->source_mac[0], addrs->source_mac[1], addrs->source_mac[2],
		addrs->source_mac[3], addrs->source_mac[4], addrs->source_mac[5]);
	printf("[SEND] target IP:    %s\n", inet_ntoa(*(struct in_addr *)addrs->target_ip));
	printf("[SEND] target MAC:   %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->target_mac[0], addrs->target_mac[1], addrs->target_mac[2],
		addrs->target_mac[3], addrs->target_mac[4], addrs->target_mac[5]);
}

void log_ns_received(uint8_t *buffer)
{
	struct ipv6_header *ip6;
	char                ipstr[INET6_ADDRSTRLEN];

	ip6 = (struct ipv6_header *)(buffer + ETH_HEADER_SIZE);
	inet_ntop(AF_INET6, ip6->src, ipstr, INET6_ADDRSTRLEN);
	printf("A Neighbor Solicitation has been broadcast.\n");
	printf("IPv6 address of request: %s\n", ipstr);
}

void print_ndp_verbose(uint8_t *buffer, t_addrs *addrs)
{
	struct ethernet_header *eth;
	char                    ipstr[INET6_ADDRSTRLEN];

	eth = (struct ethernet_header *)buffer;
	printf("[NS]   from MAC:      %02x:%02x:%02x:%02x:%02x:%02x\n",
		eth->src_mac[0], eth->src_mac[1], eth->src_mac[2],
		eth->src_mac[3], eth->src_mac[4], eth->src_mac[5]);
	inet_ntop(AF_INET6, addrs->source_ipv6, ipstr, INET6_ADDRSTRLEN);
	printf("[NA]   spoofing IPv6: %s\n", ipstr);
	printf("[NA]   spoofing MAC:  %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->source_mac[0], addrs->source_mac[1], addrs->source_mac[2],
		addrs->source_mac[3], addrs->source_mac[4], addrs->source_mac[5]);
}
