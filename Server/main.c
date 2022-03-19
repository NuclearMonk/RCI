#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
extern int errno;


void receive_udp_message(int fd)
{
    struct sockaddr addr;
    char buffer[129];
    int addrlen = sizeof(addr);
    int nread = recvfrom(fd, buffer, 128, 0, &addr, &addrlen);
    if (nread == -1) /*error*/
        exit(1);
    buffer[nread] = '\0';
    printf("echo: %s\n", buffer);
    int errorcode = sendto(fd, buffer, nread, 0, &addr, addrlen);
    if (errorcode == -1) /*error*/
        exit(1);
}

void read_console()
{
    FILE* file = fdopen(0,"r");
    char buffer[129];
    fscanf(file,"%s", &buffer);
    printf("%s\n",buffer);
    fclose(file);
    // char buffer[129];
    // int nread = read(0, buffer, 128);
    // if (nread == -1) /*error*/
    //     exit(1);
    // buffer[nread] = '\0';
    // if(strcmp(buffer,"exit\n")==0)exit(0);
}

int main(void)
{
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_DGRAM, .ai_flags = AI_PASSIVE};
    int fd, errorcode;
    socklen_t addrlen;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    errorcode = getaddrinfo(NULL, "58001", &hints, &res);
    if (errorcode != 0)
    {
        fprintf(stdout, "Error: getaddrinfo: %s\n", gai_strerror(errorcode));
        exit(EXIT_FAILURE);
    }
    errorcode = bind(fd, res->ai_addr, res->ai_addrlen);
    if (errorcode == -1)
    {
        perror("Error: bind");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(res);
    fd_set set, temp_set;
    FD_ZERO(&set);
    FD_SET(0,&set);
    FD_SET(fd,&set);
    while (1)
    {   
        temp_set = set;
        int count = select(fd+1,&temp_set,NULL,NULL,NULL);
        while(count>0)
        {
            if(FD_ISSET(fd,&temp_set))receive_udp_message(fd);
            if(FD_ISSET(0,&temp_set))read_console();
            count--;
        }
    }
    exit(EXIT_SUCCESS);
}