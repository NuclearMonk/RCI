#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>

server_t *create_server()
{
    server_t *server = calloc(1, sizeof(server_t));
    if(!server) return NULL;
    server->self = create_node(0,"0.0.0.0","8080");
    server->socket_send = socket(AF_INET,SOCK_STREAM,0);
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

void show_server_info(server_t *server)
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
    fflush(stdout);
}

int open_tcp_connection(int fd, node_t* target)
{
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET,.ai_socktype = SOCK_STREAM};
    return 0;
}