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

static void setup_signal_handler(void)
{
	struct sigaction sa;
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
}

static int parse_args(int ac, char **av, int *verbose, int *arg_offset)
{
	if (ac == 6 && strcmp(av[1], "-v") == 0)
	{
		*verbose    = 1;
		*arg_offset = 1;
		return 1;
	}
	if (ac == 5)
		return 1;
	printf("Usage: ./ft_malcolm [-v] <source_ip> <source_mac> <target_ip> <target_mac>\n");
	return 0;
}

static void print_verbose(uint8_t *buffer, char **args)
{
	struct arp_packet *arp = (struct arp_packet *)(buffer + ETH_HEADER_SIZE);
	printf("[REQUEST] from IP:  %s\n", inet_ntoa(*(struct in_addr *)arp->src_ip));
	printf("[REQUEST] from MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		arp->src_mac[0], arp->src_mac[1], arp->src_mac[2],
		arp->src_mac[3], arp->src_mac[4], arp->src_mac[5]);
	printf("[REPLY]   spoofing IP:  %s\n", args[0]);
	printf("[REPLY]   spoofing MAC: %s\n", args[1]);
	printf("[REPLY]   target IP:    %s\n", args[2]);
	printf("[REPLY]   target MAC:   %s\n", args[3]);
}

int main(int ac, char **av)
{
	int verbose    = 0;
	int arg_offset = 0;

	if (getuid() != 0)
	{
		printf("ft_malcolm: must be run as root\n");
		return 1;
	}
	if (!parse_args(ac, av, &verbose, &arg_offset))
		return 1;
	setup_signal_handler();

	char   **args = av + 1 + arg_offset; // source_ip, source_mac, target_ip, target_mac
	t_addrs  addrs;
	if (!load_addresses(args, &addrs))
		return 1;

	int ifindex;
	if (!setup_network(&ifindex))
		return 1;

	uint8_t buffer[ARP_FRAME_SIZE];
	if (!run_spoof(&addrs, ifindex, buffer))
		return 1;

	if (verbose)
		print_verbose(buffer, args);
	return 0;
}
