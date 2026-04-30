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

void hexdump(uint8_t *data, int len)
{
	int i;
	int j;

	i = 0;
	while (i < len)
	{
		printf("%04x  ", i);
		j = 0;
		while (j < 16)
		{
			if (i + j < len)
				printf("%02x ", data[i + j]);
			else
				printf("   ");
			if (j == 7)
				printf(" ");
			j++;
		}
		printf(" |");
		j = 0;
		while (j < 16 && i + j < len)
		{
			printf("%c", (data[i + j] >= 32 && data[i + j] < 127) ? data[i + j] : '.');
			j++;
		}
		printf("|\n");
		i += 16;
	}
}
