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

		// strtoul = atoi base avec buffer stock l endroit de la fin
		val = strtoul(ptr, &end, 16); // va jusqu au premier "non hexa" = : ou \0
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
	struct addrinfo  hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	if (getaddrinfo(to_convert, NULL, &hints, &res) != 0)
		return (0);
	memcpy(converted, &((struct sockaddr_in *)res->ai_addr)->sin_addr, 4);
	freeaddrinfo(res);
	return (1);
}

int load_addresses(char **args, t_addrs *a)
{
	if (!ip_getter(args[0], a->source_ip))
	{
		printf("ft_malcolm: unknown host or invalid IP address: (%s)\n", args[0]);
		return (0);
	}
	if (!MAC_getter(args[1], a->source_mac))
	{
		printf("ft_malcolm: invalid mac address: (%s)\n", args[1]);
		return (0);
	}
	if (!ip_getter(args[2], a->target_ip))
	{
		printf("ft_malcolm: unknown host or invalid IP address: (%s)\n", args[2]);
		return (0);
	}
	if (!MAC_getter(args[3], a->target_mac))
	{
		printf("ft_malcolm: invalid mac address: (%s)\n", args[3]);
		return (0);
	}
	return (1);
}
