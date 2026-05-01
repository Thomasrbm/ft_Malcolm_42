#include "ft_malcolm.h"

static int parse_flag(char *flag, t_flags *flags)
{
	if (ft_strcmp(flag, "-v") == 0)
		flags->has_verbose = 1;
	else if (ft_strcmp(flag, "-g") == 0)
		flags->has_gratuitous = 1;
	else if (ft_strcmp(flag, "-hex") == 0)
		flags->has_hex = 1;
	else
		return (0);
	return (1);
}

int parse_args(int ac, char **av, t_flags *flags, int *arg_offset)
{
	int i;

	flags->has_verbose = 0;
	flags->has_gratuitous = 0;
	flags->has_hex = 0;
	*arg_offset = 0;
	i = 1;
	while (i < ac && av[i][0] == '-')
	{
		if (!parse_flag(av[i], flags))
		{
			printf("Usage: ./ft_malcolm [-v] [-g] [-hex] <source_ip> <source_mac> <target_ip> <target_mac>\n");
			return (0);
		}
		i++;
		(*arg_offset)++; // si pas flag actualiser on sort de la boucle car parse flag ret 0 => off set au dernier flag.
	}
	if (ac - i != 4)
	{
		printf("Usage: ./ft_malcolm [-v] [-g] [-hex] <source_ip> <source_mac> <target_ip> <target_mac>\n");



		return (0);
	}
	return (1);
}
