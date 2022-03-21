#include "server.h"
#include <stdlib.h>
#include <stdio.h>
server_t *create_server()
{
    printf("SERVER CREATED\n");
    return calloc(1, sizeof(server_t));
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
int open_listen_socket(server_t *server)
{
    return 0;
}
int open_send_socket(server_t *server)
{
    return 0;
}