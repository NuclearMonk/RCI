#include "node.h"
#include <stdlib.h>

node_t* create_node(int key,int ip, int port)
{
    node_t* node = malloc(sizeof(node_t));
    node->key = key;
    node->ip = ip;
    node->port = port;
    return node;
}

void destroy_node(node_t* node)
{
    if(!node)return;
    free(node);
}