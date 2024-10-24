# dns-attack-tools

Tools for generating DNS attacks.

## DNS random prefix flood

Floods a destination IP with DNS queries for random prefixed subdomains.

## DNS ANY amplification

Floods a destination IP with DNS ANY responses, simlulating a DNS ANY amplification.

## Setup

```
sudo apt update
sudo apt install git gcc make

git clone https://github.com/cjdoucette/dns-attack-tools.git
cd dns-attack-tools

# Running DNS random prefix flood
cd dns-random  # from dns-attack-tools folder
make
sudo ./dnsflood cf.com 8.31.160.5  # sample domain and destination IP address

# Running DNS ANY amplification
cd dns-any  # from dns-attack-tools folder
sudo systemctl stop systemd-resolved
make
sudo ./dns_any_reply 8.31.160.5  # sample destination IP address
```
