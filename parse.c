#include "ft_malcolm.h"


int MAC_getter(char *to_convert, uint8_t *converted)
{
	char *tok[6];
	
	int i = 0;

	tok[i] = strtok(to_convert, ":");
	if (!tok[i])
		return 0;
	for (i = 1; i < 6; i++)
	{
		tok[i] = strtok(NULL, ":");
		if (!tok[i])
			return 0;
	}

	char *cursor;
	for (i = 0; i < 6; i++)
	{
		unsigned long val = strtoul(tok[i], &cursor, 16);
		if (*cursor != '\0')
			return 0;
		if (val > 0xFF)
    		return 0;
		converted[i] = val;
	}

	return 1;
}

int ip_getter(char *to_convert, uint8_t *converted)
{
    // essai direct IP d'abord
    if (inet_pton(AF_INET, to_convert, converted))
	{
        return 1;
	}

	// essai notation décimale pure (ex: 167772161 = 10.0.0.1)
	char *endptr;
	unsigned long decimal = strtoul(to_convert, &endptr, 10);
	if (*endptr == '\0' && decimal <= 0xFFFFFFFF)
	{
		decimal = htonl(decimal);
		memcpy(converted, &decimal, 4);
		return 1;
	}

    // sinon tentative résolution hostname ====> avec les noms de domaines
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_RAW;

    if (getaddrinfo(to_convert, NULL, &hints, &res) != 0)
    {
        printf("ft_malcolm: unknown host or invalid IP address: (%s)\n", to_convert);
        return 0;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    memcpy(converted, &addr->sin_addr, 4);
    freeaddrinfo(res);

    return 1;
}
