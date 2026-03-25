#include <stdint.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>

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
	if (!inet_pton(AF_INET, to_convert, converted))
	{
		printf("invalide IP adress provided");
		return 0;
	}
	return 1;
}


int main(int ac, char **av)
{
	if (ac != 5)
	{
		printf("wrong number of arguments, usage : ...\n");
		return 0;
	}

	char *source_ip  = av[1]; // l'IP que tu vas usurper (IP_A)
	char *source_mac = av[2]; // la fausse MAC que tu vas annoncer (MAC_toi)
	char *target_ip  = av[3]; // la victime à empoisonner
	char *target_mac = av[4]; // la vraie MAC de la victime 



	// IPs : 4 octets
	uint8_t source_ip_converted[4];   // uint car un chiffre par octet. si avait fait tableau d int soit on aurait les 4 chiffre sur le meme octet (pas bon)  sooit 1 par 4 octet donc de la perte useless
	uint8_t target_ip_converted[4];  // uint permet d avoit 4 * 1 octet 
	
	// Pour les MACs : 6 octets
	uint8_t source_mac_converted[6];
	uint8_t target_mac_converted[6];

	
	if (!ip_getter(source_ip , source_ip_converted))
		return 1;
	if (!ip_getter(target_ip, target_ip_converted))
		return 1;
	if (!MAC_getter(source_mac , source_mac_converted))
		return 1;
	if (!MAC_getter(target_mac, target_mac_converted))
		return 1;
	

	return 0;
}