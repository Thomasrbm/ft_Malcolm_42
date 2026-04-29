#include "ft_malcolm.h"

int MAC_getter(char *to_convert, uint8_t *converted)
{
	char *token;
	int   i = 0;

	token = strtok(to_convert, ":");
	while (token && i < 6)
	{
		char         *cursor;
		unsigned long val = strtoul(token, &cursor, 16);
		if (*cursor != '\0' || val > 0xFF)
			return 0;
		converted[i++] = val;
		token = strtok(NULL, ":");
	}
	return (i == 6);
}

static int parse_decimal_ip(char *str, uint8_t *converted)
{
	char         *endptr;
	unsigned long decimal = strtoul(str, &endptr, 10);

	if (*endptr != '\0' || decimal > 0xFFFFFFFF)
		return 0;
	decimal = htonl(decimal);
	memcpy(converted, &decimal, 4);
	return 1;
}

static int resolve_hostname(char *hostname, uint8_t *converted)
{
	struct addrinfo  hints;
	struct addrinfo *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	if (getaddrinfo(hostname, NULL, &hints, &res) != 0)
	{
		printf("ft_malcolm: unknown host or invalid IP address: (%s)\n", hostname);
		return 0;
	}
	struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
	memcpy(converted, &addr->sin_addr, 4);
	freeaddrinfo(res);
	return 1;
}

int ip_getter(char *to_convert, uint8_t *converted)
{
	// essai direct IP d'abord
	if (inet_pton(AF_INET, to_convert, converted))
		return 1;
	// essai notation décimale pure (ex: 167772161 = 10.0.0.1)
	if (parse_decimal_ip(to_convert, converted))
		return 1;
	// sinon tentative résolution hostname ====> avec les noms de domaines
	return resolve_hostname(to_convert, converted);
}

// args order: source_ip [0], source_mac [1], target_ip [2], target_mac [3]
int load_addresses(char **args, t_addrs *a)
{
	if (!ip_getter(args[0],  a->source_ip))  return 0;
	if (!MAC_getter(args[1], a->source_mac)) return 0;
	if (!ip_getter(args[2],  a->target_ip))  return 0;
	if (!MAC_getter(args[3], a->target_mac)) return 0;
	return 1;
}
