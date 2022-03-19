#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
extern int errno;

/**
 * @brief Writes the message to the socket or pipe, will keep trying in case a partial write occurs.
 *        Will return -1 in case it fails to write anything at all or the write call fails
 *        Returns 0 in success
 * @param fd socket or pipe to write the message
 * @param message_buffer buffer that holds the message
 * @param message_length the length of the message in bytes
 * @return int
 */
int send_message(int fd, void *message_buffer, size_t message_length)
{
    ssize_t bytes_written = 0;
    char *buffer = (char *)message_buffer;
    for (size_t total_bytes_written = 0; total_bytes_written < message_length; total_bytes_written += bytes_written)
    {
        size_t bytes_left = message_length - total_bytes_written;
        bytes_written = write(fd, &(buffer[total_bytes_written]), bytes_left);
        if (bytes_written <= 0)
            return -1;
    }
    return 0;
}

ssize_t recv_message(int fd, void *message_buffer, size_t message_length)
{
    size_t bytes_read;
    char *buffer = (char *)message_buffer;
    for (size_t total_bytes_read = 0; total_bytes_read < message_length; total_bytes_read += bytes_read)
    {
        size_t bytes_left = message_length - total_bytes_read;
        bytes_read = read(fd, &(buffer[total_bytes_read]), bytes_left);
        if (bytes_read == -1)
            return -1;
        if (bytes_read == 0)
            return total_bytes_read;
    }
    return message_length;
}

int main(void)
{
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_DGRAM};
    int fd, errorcode;
    struct sockaddr addr;
    socklen_t addrlen;
    char buffer[128 + 1];

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    errorcode = getaddrinfo("127.0.0.1", "58001", &hints, &res);
    if (errorcode != 0)
    {
        fprintf(stdout, "Error: getaddrinfo: %s\n", gai_strerror(errorcode));
        exit(EXIT_FAILURE);
    }
    errorcode = sendto(fd,"Hallo!\n",7,0,res->ai_addr,res->ai_addrlen);
    if (errorcode == -1)
    {
        perror("Error: connect");
        exit(EXIT_FAILURE);
    }
    addrlen = sizeof(addr);
    int n = recvfrom(fd, buffer, 128, 0, &addr, &addrlen);
    if (n == -1) /*error*/
        exit(1);
    buffer[n] = '\0';
    printf("echo: %s\n", buffer);
    close(fd);
    freeaddrinfo(res);
    exit(EXIT_SUCCESS);
}