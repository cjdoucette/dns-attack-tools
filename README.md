# dns-attack-tools

Tools for generating DNS attacks.

## DNS random prefix flood

Floods a destination IP with DNS queries for random prefixed subdomains.

## DNS ANY amplification

Floods a destination IP with DNS ANY responses, simlulating a DNS ANY amplification.

## Setup

```
apt install gcc make

# Running DNS random prefix flood
cd dns-random
make
./dnsflood cf.com 8.31.160.5  # sample domain and destination IP address

# Running DNS ANY amplification
cd dns-any
make
/dns_any_reply 8.31.160.5  # sample destination IP address
```
