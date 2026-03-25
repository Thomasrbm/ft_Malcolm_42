#include <stdint.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>


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
	if (!ip_getter(source_mac, target_ip_converted))
		return 1;

	   // af = INET ip v4,  char de la string brut, dest = convertion string to format ip usable


	return 0;
}