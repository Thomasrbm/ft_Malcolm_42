#include "ft_malcolm.h"

int g_sockfd = -1;

void handle_sigint(int sig)
{
	(void)sig;
	printf("\nExiting program...\n");
	if (g_sockfd != -1)
		close(g_sockfd);
	exit(0);
}

static void setup_signal_handler(void)
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint; // la fonction a appliquer
	sigemptyset(&sa.sa_mask); // remet a 0 pour pas catch des signaux random
	sa.sa_flags = 0; // same
	if (sigaction(SIGINT, &sa, NULL) < 0) // le signal a catch pour la structure DONC la ft a appliquer
		perror("sigaction");
}

static int run_ipv4(t_addrs *addrs, int gratuitous, int verbose, int hex)
{
	int     ifindex;
	uint8_t buffer[ARP_FRAME_SIZE];

	if (!setup_network(&ifindex))
		return (0);
	if (gratuitous)
		return (send_gratuitous(addrs, ifindex, verbose, hex)); // cest quand on envoit ip et mac en brodcase + sans demander qui a cette ip ? je cherche la mac => quand on  s annonce sur le reseau.
	return (run_spoof(addrs, ifindex, buffer, verbose, hex));
}

static int dispatch(t_addrs *addrs, char **args, int gratuitous, int verbose,
					int hex)
{
	if (!load_addresses(args, addrs))
		return (0);
	return (run_ipv4(addrs, gratuitous, verbose, hex));
}

int main(int ac, char **av)
{
	int     verbose;
	int     gratuitous;
	int     hex;
	int     arg_offset;
	char  **args;
	t_addrs addrs;
	int     ret;

	if (getuid() != 0)
	{
		printf("ft_malcolm: must be run as root\n");
		return (1);
	}
	if (!parse_args(ac, av, &verbose, &gratuitous, &hex, &arg_offset))
		return (1);
	setup_signal_handler();
	args = av + 1 + arg_offset; // commence apres le flag
	memset(&addrs, 0, sizeof(addrs)); // avant de parser les ips et mac (pas de garbage value)
	ret = !dispatch(&addrs, args, gratuitous, verbose, hex);
	if (g_sockfd != -1)
		close(g_sockfd); // close si pas ctrl c mais prog fini
	if (!ret)
		printf("Exiting program...\n");
	return (ret);
}
