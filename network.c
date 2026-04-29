#include "ft_malcolm.h"

int setup_socket(void)
{
	// AF_PACKET = layer 2 Ethernet/ARP, SOCK_RAW = trame brute, ETH_P_ARP = filtre uniquement les trames ARP
	int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	return sockfd;
}

static struct ifaddrs *find_active_interface(struct ifaddrs *ifaddr)
{
	struct ifaddrs *ptr = ifaddr;

	while (ptr)
	{
		// interface active et non loopback (lo) = eth0 ou équivalent
		if (ptr->ifa_flags & IFF_RUNNING && !(ptr->ifa_flags & IFF_LOOPBACK))
			return ptr;
		ptr = ptr->ifa_next;
	}
	return NULL;
}

static int bind_to_interface(int sockfd, struct ifaddrs *iface, int *ifindex)
{
	// sockaddr_ll décrit une adresse layer 2 (Ethernet/ARP)
	struct sockaddr_ll sll;
	memset(&sll, 0, sizeof(sll));
	sll.sll_family   = AF_PACKET;
	sll.sll_protocol = htons(ETH_P_ARP);
	sll.sll_ifindex  = if_nametoindex(iface->ifa_name); // index numérique de l'interface (ex: eth0 → 2)

	// bind = lie le socket à cette interface pour écouter dessus
	if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0)
	{
		perror("bind");
		return 0;
	}
	*ifindex = sll.sll_ifindex;
	return 1;
}

int get_interface(int sockfd)
{
	struct ifaddrs *ifaddr;

	// demande au kernel toutes les interfaces réseau actives
	if (getifaddrs(&ifaddr) < 0)
	{
		perror("getifaddrs");
		return -1;
	}
	struct ifaddrs *iface = find_active_interface(ifaddr);
	if (!iface)
	{
		printf("no interface found\n");
		freeifaddrs(ifaddr);
		return -1;
	}
	int ifindex;
	if (!bind_to_interface(sockfd, iface, &ifindex))
	{
		freeifaddrs(ifaddr);
		return -1;
	}
	freeifaddrs(ifaddr);
	return ifindex;
}

int setup_network(int *ifindex)
{
	g_sockfd = setup_socket();
	if (g_sockfd < 0)
		return 0;
	*ifindex = get_interface(g_sockfd);
	return (*ifindex >= 0);
}
