#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

int open_tcp_connection(int fd,const node_t *target);
void bind_tcp_socket(int fd,const node_t *node);

server_t *create_server(int key, char* ip, char* port)
{
    server_t *server = calloc(1, sizeof(server_t));
    if (!server)
        return NULL;
    server->self = create_node(key, ip, port);
    server->socket_send = socket(AF_INET, SOCK_STREAM, 0);
    server->socket_listen = socket(AF_INET, SOCK_STREAM, 0);
    bind_tcp_socket(server->socket_listen, server->self);
    return server;
}

void destroy_server(server_t *server)
{

    if (!server)
        return;
    destroy_node(server->self);
    destroy_node(server->successor);
    destroy_node(server->antecessor);
    destroy_node(server->bridge);
    free(server);
}

void show_server_info(const server_t *server)
{
    if (!server)
    {
        printf("NO RING ON THIS NODE\n");
        fflush(stdout);
        return;
    }
    if (!(server->self))
    {
        printf("NO RING ON THIS NODE\n");
        fflush(stdout);
        return;
    }
    printf("This node:\n");
    printf("KEY: %d\n", server->self->key);
    printf("IP: %s\n", server->self->ip);
    printf("PORT: %s\n", server->self->port);
    if (server->antecessor)
    {
        printf("Antecessor node:\n");
        printf("KEY: %d\n", server->antecessor->key);
        printf("IP: %s\n", server->antecessor->ip);
        printf("PORT: %s\n", server->antecessor->port);
    }
    fflush(stdout);
}

void set_antecessor_node(server_t *server,node_t *node)
{
    if (!server)
        return;
    server->antecessor = node;
    open_tcp_connection(server->socket_send, server->antecessor);
    fflush(stdout);
}

int open_tcp_connection(int fd, const node_t *target)
{
    if (!target)
        return -1;
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
    return 0;
}

void bind_tcp_socket(int fd,const node_t *node)
{
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_flags = AI_PASSIVE};
    getaddrinfo(node->ip, node->port, &hints, &res);
    bind(fd, res->ai_addr, res->ai_addrlen);
    listen(fd, 5);
    freeaddrinfo(res);
}

void read_message(int fd)
{
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    int newfd;
    if ((newfd = accept(fd, &addr, &addrlen)) == -1)
        return;
    char buffer[128] = "";
    if (read(newfd, &buffer, 128) == -1)
        return;
    printf("%s", buffer);
    fflush(stdout);
    shutdown(newfd, SHUT_RDWR);
    close(newfd);
}
