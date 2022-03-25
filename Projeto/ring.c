#include "ring.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int open_tcp_connection(const node_data_t *target);

void bind_tcp_socket(int fd, const node_data_t *node);

node_t *create_node(int key, const char *ip, const char *port)
{
    node_t *node = calloc(1, sizeof(node_t));
    if (!node)
        return NULL;
    node->self = create_node_data(key, ip, port);
    node->socket_listen_tcp = socket(AF_INET, SOCK_STREAM, 0);
    bind_tcp_socket(node->socket_listen_tcp, node->self);
    return node;
}

void destroy_node(node_t *node)
{

    if (!node)
        return;
    destroy_node_data(node->self);
    destroy_node_data(node->sucessor);
    destroy_node_data(node->antecessor);
    destroy_node_data(node->bridge);
    free(node);
}

void create_empty_ring(node_t *node)
{
    set_sucessor_node(node, create_node_data(node->self->key, node->self->ip, node->self->port));
    set_antecessor_node(node, create_node_data(node->self->key, node->self->ip, node->self->port));
}

void show_node_info(const node_t *node)
{
    if (!node)
    {
        printf("NO RING ON THIS NODE\n");
        fflush(stdout);
        return;
    }
    if (!(node->self))
    {
        printf("NO RING ON THIS NODE\n");
        fflush(stdout);
        return;
    }
    printf("This node:\n");
    printf("KEY: %d\n", node->self->key);
    printf("IP: %s\n", node->self->ip);
    printf("PORT: %s\n", node->self->port);
    if (node->antecessor)
    {
        printf("Antecessor node:\n");
        printf("KEY: %d\n", node->antecessor->key);
        printf("IP: %s\n", node->antecessor->ip);
        printf("PORT: %s\n", node->antecessor->port);
    }
    if (node->sucessor)
    {
        printf("Sucessor node:\n");
        printf("KEY: %d\n", node->sucessor->key);
        printf("IP: %s\n", node->sucessor->ip);
        printf("PORT: %s\n", node->sucessor->port);
    }
    fflush(stdout);
}

void set_sucessor_node(node_t *node, node_data_t *sucessor_node)
{
    if (!node)
        return;
    node->sucessor = sucessor_node;
    send_tcp_message("HELLO WORLD!\n", node->sucessor);
}

void set_antecessor_node(node_t *node, node_data_t *antecessor_node)
{
    if (!node)
        return;
    node->antecessor = antecessor_node;
}

int open_tcp_connection(const node_data_t *target)
{
    if (!target)
        return -1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    int errorcode;
    errorcode = getaddrinfo(target->ip, target->port, &hints, &res);
    if (errorcode != 0)
        return errorcode;
    errorcode = connect(fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    if (errorcode == -1)
        return -1;
    return fd;
}

void bind_tcp_socket(int fd, const node_data_t *node)
{
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_flags = AI_PASSIVE};
    getaddrinfo(node->ip, node->port, &hints, &res);
    bind(fd, res->ai_addr, res->ai_addrlen);
    listen(fd, 5);
    freeaddrinfo(res);
}

void read_tcp_message(int fd)
{
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    char buffer[128] = "";
    int newfd;
    if ((newfd = accept(fd, &addr, &addrlen)) == -1)
        return;
    if (read(newfd, &buffer, 128) == -1)
        return;
    printf("\033[0;31m"); 
    printf("%s", buffer);
    printf("\033[0m"); 
    fflush(stdout);
    shutdown(newfd, SHUT_RDWR);
    close(newfd);
}

void send_tcp_message(const char *message, node_data_t *destination)
{
    int fd = open_tcp_connection(destination);
    int length = strlen(message) + 1;
    int errorcode = write(fd, message, length);
    if (errorcode == -1)
        printf("erro");
    shutdown(fd, SHUT_RDWR);
    close(fd);
}
