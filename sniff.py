#!/usr/bin/env python3
from scapy.all import sniff, ARP

print("==================================")
print("Depuis le PC hote lance :")
print("  ip neigh show")
print("Puis : ping -c 1 <ip eno2>")
print("Sniffing ARP requests...")

results = []

def handle(pkt):
    if pkt.haslayer(ARP) and pkt[ARP].op == 1:
        results.append((pkt[ARP].psrc, pkt[ARP].hwsrc, pkt[ARP].pdst))

sniff(filter="arp", prn=handle, stop_filter=lambda p: len(results) > 0, store=0, timeout=30)

if not results:
    print("Aucune ARP request captee en 30 sec.")
else:
    src_ip, src_mac, dst_ip = results[0]
    print("\n==================================")
    print("COMMANDE ft_malcolm :")
    print(f"  sudo ./ft_malcolm {dst_ip} aa:bb:cc:dd:ee:ff {src_ip} {src_mac}")
    print("==================================")