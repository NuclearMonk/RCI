#include "node.h"
#include <stdlib.h>
#include <string.h>

node_t *create_node(int key, char ip[INET_ADDRSTRLEN], char port[6])
{
    node_t *node = malloc(sizeof(node_t));
    node->key = key;
    strncpy((node->ip), ip, INET_ADDRSTRLEN);
    strncpy((node->port), port, 6);
    return node;
}

void destroy_node(node_t *node)
{
    if (!node)
        return;
    free(node);
}