#include "ft_malcolm.h"

static uint32_t partial_sum(uint8_t *data, int len)
{
	uint32_t sum;
	int      i;

	sum = 0;
	i   = 0;
	while (i < len - 1)
	{
		sum += ((uint32_t)data[i] << 8) | (uint32_t)data[i + 1];
		i += 2;
	}
	if (len & 1)
		sum += (uint32_t)data[len - 1] << 8;
	return (sum);
}

static uint16_t icmpv6_checksum(t_addrs *addrs, uint8_t *icmpv6, uint16_t len)
{
	uint32_t sum;
	uint8_t  plen[4];

	plen[0] = 0;
	plen[1] = 0;
	plen[2] = (uint8_t)(len >> 8);
	plen[3] = (uint8_t)(len & 0xFF);
	sum  = partial_sum(addrs->source_ipv6, 16);
	sum += partial_sum(addrs->target_ipv6, 16);
	sum += partial_sum(plen, 4);
	sum += 58;
	sum += partial_sum(icmpv6, (int)len);
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	return ((uint16_t)(~sum & 0xFFFF));
}

static void build_na_eth_ip6(uint8_t *reply, t_addrs *addrs)
{
	struct ethernet_header *eth;
	struct ipv6_header     *ip6;

	eth = (struct ethernet_header *)reply;
	memcpy(eth->dst_mac, addrs->target_mac, 6);
	memcpy(eth->src_mac, addrs->source_mac, 6);
	eth->ethertype = htons(0x86DD);
	ip6 = (struct ipv6_header *)(reply + ETH_HEADER_SIZE);
	ip6->version_tc_flow = htonl(0x60000000);
	ip6->payload_len     = htons(32);
	ip6->next_header     = 58;
	ip6->hop_limit       = 255;
	memcpy(ip6->src, addrs->source_ipv6, 16);
	memcpy(ip6->dst, addrs->target_ipv6, 16);
}

static void build_na_icmpv6(uint8_t *reply, t_addrs *addrs)
{
	struct icmpv6_na *na;
	uint16_t          cksum;

	na = (struct icmpv6_na *)(reply + ETH_HEADER_SIZE + sizeof(struct ipv6_header));
	na->type     = 136;
	na->code     = 0;
	na->checksum = 0;
	na->flags    = htonl(NA_FLAG_SOLICITED | NA_FLAG_OVERRIDE);
	memcpy(na->target, addrs->source_ipv6, 16);
	na->opt_type = 2;
	na->opt_len  = 1;
	memcpy(na->opt_mac, addrs->source_mac, 6);
	cksum = icmpv6_checksum(addrs, (uint8_t *)na, 32);
	na->checksum = htons(cksum);
}

void build_na(uint8_t *reply, t_addrs *addrs)
{
	memset(reply, 0, NA_FRAME_SIZE);
	build_na_eth_ip6(reply, addrs);
	build_na_icmpv6(reply, addrs);
}
