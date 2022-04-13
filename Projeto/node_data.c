#include "node_data.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

node_data_t *create_node_data(int key, const char ip[INET_ADDRSTRLEN], const char port[6],int fd)
{
    node_data_t *node = malloc(sizeof(node_data_t));
    node->key = key;
    memcpy((node->ip), ip, INET_ADDRSTRLEN);
    memcpy((node->port), port, 6);
    node->fd = fd;
    return node;
}

void destroy_node_data(node_data_t *node)
{
    if (!node)
        return;
    if(node->fd !=-1)
    {
        shutdown(node->fd,SHUT_RDWR);
        close(node->fd);
    }
    free(node);
}