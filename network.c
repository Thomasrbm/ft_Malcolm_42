#include "ft_malcolm.h"

static int setup_socket(void)
{
	int sockfd;

	sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (sockfd < 0)
	{
		printf("socket: %s\n", strerror(errno));
		return (-1);
	}
	return (sockfd);
}

static struct ifaddrs *find_active_interface(struct ifaddrs *ifaddr_head)
{
	struct ifaddrs *ptr;

	ptr = ifaddr_head;
	while (ptr)
	{
		if (ptr->ifa_addr != NULL
			&& (ptr->ifa_flags & IFF_RUNNING) // premier running
			&& !(ptr->ifa_flags & IFF_LOOPBACK)) // et n est pas lo  (wifi / eth) selon sur quoi co peu importe = meme requete arp pour tous)
			return (ptr);
		ptr = ptr->ifa_next;
	}
	return (NULL);
}

static int bind_to_interface(int sockfd, struct ifaddrs *selected_interface, int *interface_idx)
{
	unsigned int idx;

	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, selected_interface->ifa_name, ft_strlen(selected_interface->ifa_name) + 1) < 0) // SO_BINDTODEVICE va bind (ecouter recevoir que la pour le socket),  SOL_SOCKET config pour tous niveau pas de focus precis sur ll ou ip ou tcp etc.
	{
		printf("setsockopt: %s\n", strerror(errno));
		return (0);
	}
	idx = if_nametoindex(selected_interface->ifa_name); // choppe l index par le nom d interface
	if (idx == 0)
	{
		printf("if_nametoindex: %s\n", strerror(errno));
		return (0);
	}
	*interface_idx = (int)idx;
	return (1);
}

// ifaddrs struc avec toute les interface dans  : -> char	*ifa_name;
int get_interface(int sockfd)
{
	struct ifaddrs *ifaddr_head; // pour boucler
	struct ifaddrs *selected_interface; // la bonne interface => sera pas free car va pointer va ifaddr_head (seul allocated)
	int             interface_idx;

	if (getifaddrs(&ifaddr_head) < 0) // recup toutes les interface reseau de la machine avec adresss
	{
		printf("getifaddrs: %s\n", strerror(errno));
		return (-1);
	}
	selected_interface = find_active_interface(ifaddr_head);
	if (!selected_interface)
	{
		printf("no interface found\n");
		freeifaddrs(ifaddr_head);
		return (-1);
	}
	if (!bind_to_interface(sockfd, selected_interface, &interface_idx))
	{
		freeifaddrs(ifaddr_head);
		return (-1);
	}
	printf("Found available interface: %s\n", selected_interface->ifa_name);
	freeifaddrs(ifaddr_head);
	return (interface_idx);
}

int setup_network(int *interface_idx)
{
	g_sockfd = setup_socket();
	if (g_sockfd < 0)
		return (0);
	*interface_idx = get_interface(g_sockfd);
	return (*interface_idx >= 0);
}
