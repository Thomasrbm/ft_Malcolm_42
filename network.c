#include "ft_malcolm.h"



int setup_socket(void)
{
	// Af = Adresse familly (INET ipv4 = TCP/UDP etc.) ici AF_PACKET  =    layer 2 Ethernet/ARP ,              SOCK RAW = le kernell donne la trame brut.  (STREAM, tcp gere tout,  DGRAM  UDP le kernelle gere et envoi paquetes isoles ),       irc c etait 0 (tcp devine), ici c est ETH_P_ARP      filtre trames ARP, kernell check et evoit que les trames ARP
	int sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	return sockfd;
}

int get_interface(int sockfd)
{
	struct ifaddrs *ifaddr; // besoin de  struct sockaddr_ll  pour 
	if (getifaddrs(&ifaddr) < 0)// demande au kernell tout les interface reseaux actives, remplit une liste chaine avec tout 
	{
		perror("getifaddrs");
		return -1;
	}

	struct ifaddrs *ptr = ifaddr;

	while (ptr)
	{
		if (ptr->ifa_flags & IFF_RUNNING && !(ptr->ifa_flags & IFF_LOOPBACK))  // verifie si interface active +  est ce que cest un lo (loopback)   si non  = eth0 (notre interface)
			break;
		ptr = ptr->ifa_next;
	}
	if (!ptr)
	{
		printf("no interface found\n");
		freeifaddrs(ifaddr);
		return -1;
	}

	struct sockaddr_ll sll; // decrit adresse layer 2 : arp/ethernet
	sll.sll_family   = AF_PACKET; // uint16_t qui dit au kernetl cest une addresse 2 ethernet
	sll.sll_protocol = htons(ETH_P_ARP); //  from little to big endiant  pour stocker ETH p arp protocl
	sll.sll_ifindex  = if_nametoindex(ptr->ifa_name); // prend l index numerique de l interface eth0 (2 par exemple)
	if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0) //  bind = syscall  qui dit lie le socket a cette interface on va ecouter tout dessus plus tard
	{
		perror("bind");
		freeifaddrs(ifaddr);
		return -1;
	}

	int ifindex = sll.sll_ifindex;
	freeifaddrs(ifaddr); // free
	return ifindex;
}
