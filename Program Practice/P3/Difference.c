#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NTP_SERVER "pool.ntp.org"
#define NTP_PORT 123
#define NTP_PACKET_SIZE 48
#define NTP_TIMESTAMP_DELTA 2208988800ull

typedef struct {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t reference_id;
    uint32_t ref_ts_sec;
    uint32_t ref_ts_frac;
    uint32_t orig_ts_sec;
    uint32_t orig_ts_frac;
    uint32_t recv_ts_sec;
    uint32_t recv_ts_frac;
    uint32_t trans_ts_sec;
    uint32_t trans_ts_frac;
} ntp_packet;

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void getLocalTime() {
    time_t current_time;
    struct tm *local_time;

    // Get current local time
    current_time = time(NULL);
    local_time = localtime(&current_time);

    // Print local time
    printf("Local time: %s", asctime(local_time));
}

void getNTPTime(const char *ntp_server) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    ntp_packet packet;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        error("ERROR opening socket");

    struct hostent *server = gethostbyname(ntp_server);
    if (server == NULL)
        error("ERROR getting host by name");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NTP_PORT);
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    memset(&packet, 0, sizeof(packet));
    packet.li_vn_mode = (0x03 << 6) | (0x03 << 3) | 0x03;

    if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR sending packet");

    n = recv(sockfd, &packet, sizeof(packet), 0);
    if (n < 0)
        error("ERROR receiving response");

    close(sockfd);

    time_t current_ntp_time = (ntohl(packet.trans_ts_sec) - NTP_TIMESTAMP_DELTA);
    printf("NTP time: %s", ctime(&current_ntp_time));
}

int main() {
    printf("Getting local time:\n");
    getLocalTime();
    
    printf("\nGetting NTP time:\n");
    getNTPTime(NTP_SERVER);
    
    // Calculate difference between local time and NTP time
    time_t current_time;
    time(&current_time);
    time_t local_time = mktime(localtime(&current_time));
    time_t ntp_time = mktime(gmtime(&current_time));
    double diff_seconds = difftime(local_time, ntp_time);
    
    printf("\nDifference between local time and NTP time: %.2f seconds\n", diff_seconds);

    return 0;
}

