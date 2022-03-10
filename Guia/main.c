#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
int main(void)
{
    struct addrinfo hints, *res;
    int fd, errcode;
    ssize_t n;
    fd = socket(AF_INET, SOCK_DGRAM, 0); // UDP socket
    if (fd == -1)                        /*error*/
        exit(1);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "58001", &hints, &res);
    if (errcode != 0) /*error*/
        exit(1);
    n = sendto(fd, "Hello!\n", 7, 0, res->ai_addr, res->ai_addrlen);
    if(n==-1)exit(1);
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    char buffer[INET_ADDRSTRLEN];
    n = recvfrom(fd, buffer, 128, 0, &addr, &addrlen);
    if (n == -1)exit(1);
    buffer[n] = '\0';
    printf("echo: %s\n", buffer);
    if (n == -1)
        exit(1);
    freeaddrinfo(res);
    exit(0);
}