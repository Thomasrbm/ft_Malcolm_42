#include "ft_malcolm.h"

int g_sockfd = -1;

void handle_sigint(int sig)
{
	(void)sig;
	printf("\nExiting program...\n");
	if (g_sockfd != -1)
	{
		close(g_sockfd);
		g_sockfd = -1;
	}
}

static void setup_signal_handler(void)
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint; // la fonction a appliquer
	sigemptyset(&sa.sa_mask); // remet a 0 pour pas catch des signaux random
	sa.sa_flags = 0; // same
	if (sigaction(SIGINT, &sa, NULL) < 0) // le signal a catch pour la structure DONC la ft a appliquer
		printf("sigaction: %s\n", strerror(errno));
}

static int run_ipv4(char **args, t_addrs *addrs, t_flags *flags)
{
	int     interface_idx; // index de l interface

	if (!load_addresses(args, addrs))
		return (0);
	if (!setup_network(&interface_idx))
		return (0);
	if (flags->has_gratuitous)
		return (send_gratuitous(addrs, interface_idx, flags)); // cest quand on envoit ip et mac en brodcase + sans demander qui a cette ip ? je cherche la mac => quand on  s annonce sur le reseau.
	return (run_spoof(addrs, interface_idx, flags));
}

int main(int ac, char **av)
{
	t_flags flags;
	int     arg_offset;
	char  **args;
	t_addrs addrs;
	int     ret;

	if (getuid() != 0)
	{
		printf("ft_malcolm: must be run as root\n");
		return (1);
	}
	if (!parse_args(ac, av, &flags, &arg_offset))
		return (1);
	setup_signal_handler();
	args = av + 1 + arg_offset; // commence apres le flag
	ft_memset(&addrs, 0, sizeof(addrs)); // avant de parser les ips et mac (pas de garbage value)
	ret = run_ipv4(args, &addrs, &flags);;
	if (g_sockfd != -1)
		close(g_sockfd); // close si pas ctrl c mais prog fini
	if (!ret)
		printf("Exiting program...\n");
	return (ret);
}
