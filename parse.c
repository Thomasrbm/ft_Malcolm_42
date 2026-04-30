#include "ft_malcolm.h"

int MAC_getter(char *to_convert, uint8_t *converted)
{
	const char   *ptr;
	int           i;

	ptr = to_convert;
	i = 0;
	while (i < 6)
	{
		char         *end;
		unsigned long val;

		val = ft_strtoul(ptr, &end, 16);
		if (end == ptr || val > 0xFF) // fait pas 6 byte  OU hexa sup a 255
			return (0);
		converted[i++] = (uint8_t)val;
		if (i == 6)
			return (*end == '\0'); // si bonne taille = 1
		if (*end != ':')
			return (0);
		ptr = end + 1; // avance apres le :
	}
	return (0);
}

int ip_getter(char *to_convert, uint8_t *converted)
{
	struct addrinfo  hints; // filtre ipv4
	struct addrinfo *res; // stock le resultat

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	if (getaddrinfo(to_convert, NULL, &hints, &res) != 0)
		return (0);
	ft_memcpy(converted, &((struct sockaddr_in *)res->ai_addr)->sin_addr, 4);  // de base ai_addr est sockaddr  generique: ip v4 cast avec sockaddr_in
	freeaddrinfo(res);
	return (1);
}

int load_addresses(char **args, t_addrs *addrs)
{
	if (!ip_getter(args[0], addrs->source_ip))
	{
		printf("ft_malcolm: unknown host or invalid IP address: (%s)\n", args[0]);
		return (0);
	}
	if (!MAC_getter(args[1], addrs->source_mac))
	{
		printf("ft_malcolm: invalid mac address: (%s)\n", args[1]);
		return (0);
	}
	if (!ip_getter(args[2], addrs->target_ip))
	{
		printf("ft_malcolm: unknown host or invalid IP address: (%s)\n", args[2]);
		return (0);
	}
	if (!MAC_getter(args[3], addrs->target_mac))
	{
		printf("ft_malcolm: invalid mac address: (%s)\n", args[3]);
		return (0);
	}
	return (1);
}
