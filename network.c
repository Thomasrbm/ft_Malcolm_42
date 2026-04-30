#include "ft_malcolm.h"

int setup_socket(void)
{
	int sockfd;

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (sockfd < 0)
	{
		perror("socket");
		return (-1);
	}
	return (sockfd);
}

static struct ifaddrs *find_active_interface(struct ifaddrs *ifaddr)
{
	struct ifaddrs *ptr;

	ptr = ifaddr;

	while (ptr)
	{
		if (ptr->ifa_addr != NULL
			&& (ptr->ifa_flags & IFF_RUNNING)
			&& !(ptr->ifa_flags & IFF_LOOPBACK))
			return (ptr);
		ptr = ptr->ifa_next;
	}
	return (NULL);
}

static int bind_to_interface(int sockfd, struct ifaddrs *iface, int *ifindex, int proto)
{
	struct sockaddr_ll sll;
	unsigned int       idx;

	idx = if_nametoindex(iface->ifa_name);
	if (idx == 0)
	{
		perror("if_nametoindex");
		return (0);
	}
	memset(&sll, 0, sizeof(sll));
	sll.sll_family   = AF_PACKET;
	sll.sll_protocol = htons(proto);
	sll.sll_ifindex  = (int)idx;
	if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
	{
		perror("bind");
		return (0);
	}
	*ifindex = sll.sll_ifindex;
	return (1);
}

int get_interface(int sockfd, int proto)
{
	struct ifaddrs *ifaddr;
	struct ifaddrs *iface;
	int             ifindex;

	if (getifaddrs(&ifaddr) < 0)
	{
		perror("getifaddrs");
		return (-1);
	}
	iface = find_active_interface(ifaddr);
	if (!iface)
	{
		printf("no interface found\n");
		freeifaddrs(ifaddr);
		return (-1);
	}
	if (!bind_to_interface(sockfd, iface, &ifindex, proto))
	{
		freeifaddrs(ifaddr);
		return (-1);
	}
	printf("Found available interface: %s\n", iface->ifa_name);
	freeifaddrs(ifaddr);
	return (ifindex);
}

int setup_network(int *ifindex)
{
	g_sockfd = setup_socket();
	if (g_sockfd < 0)
		return (0);
	*ifindex = get_interface(g_sockfd, ETH_P_ARP);
	return (*ifindex >= 0);
}
