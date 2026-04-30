#include "ft_malcolm.h"

void log_arp_request(uint8_t *buffer)
{
	struct arp_packet *arp;
	char               ip_str[INET_ADDRSTRLEN];

	arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE);
	printf("An ARP request has been broadcast.\n");
	printf("mac address of request: %02x:%02x:%02x:%02x:%02x:%02x\n",
		arp->src_mac[0], arp->src_mac[1], arp->src_mac[2],
		arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
	inet_ntop(AF_INET, arp->src_ip, ip_str, INET_ADDRSTRLEN);
	printf("IP address of request: %s\n", ip_str);
}

void print_verbose(uint8_t *buffer, t_addrs *addrs)
{
	struct arp_packet *arp;
	char               ip_str[INET_ADDRSTRLEN];

	arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE);
	inet_ntop(AF_INET, arp->src_ip, ip_str, INET_ADDRSTRLEN); // from binair ip octet to text 107.x.x.x : ipv4, version binaire, buffer ou ecrire, taille du buffer (16 pour les chiffret et les . et \0 au max cette taille)
	printf("[REQUEST] from IP:  %s\n", ip_str);
	printf("[REQUEST] from MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		arp->src_mac[0], arp->src_mac[1], arp->src_mac[2],
		arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
	inet_ntop(AF_INET, addrs->source_ip, ip_str, INET_ADDRSTRLEN);
	printf("[REPLY]   spoofing IP:  %s\n", ip_str);
	printf("[REPLY]   spoofing MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->source_mac[0], addrs->source_mac[1], addrs->source_mac[2],
		addrs->source_mac[3], addrs->source_mac[4], addrs->source_mac[5]);
	inet_ntop(AF_INET, addrs->target_ip, ip_str, INET_ADDRSTRLEN);
	printf("[REPLY]   target IP:    %s\n", ip_str);
	printf("[REPLY]   target MAC:   %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->target_mac[0], addrs->target_mac[1], addrs->target_mac[2],
		addrs->target_mac[3], addrs->target_mac[4], addrs->target_mac[5]);
}

void print_verbose_gratuitous(t_addrs *addrs)
{
	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, addrs->source_ip, ip_str, INET_ADDRSTRLEN);
	printf("[SEND] spoofing IP:  %s\n", ip_str);
	printf("[SEND] spoofing MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		addrs->source_mac[0], addrs->source_mac[1], addrs->source_mac[2],
		addrs->source_mac[3], addrs->source_mac[4], addrs->source_mac[5]);
	inet_ntop(AF_INET, addrs->target_ip, ip_str, INET_ADDRSTRLEN);
	printf("[SEND] target IP:    %s\n", ip_str);
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
		printf("%04x  ", i); // offset
		j = 0;
		while (j < 16)
		{
			if (i + j < len)
				printf("%02x ", data[i + j]); // les octets 2 par 2 en hexa
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
			if (data[i + j] >= 32 && data[i + j] < 127)// rpz ascii
    			printf("%c", data[i + j]);
			else
    			printf(".");
			j++;
		}
		printf("|\n");
		i += 16;
	}
}
