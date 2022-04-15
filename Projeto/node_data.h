#ifndef NODE_DATA_H
#define NODE_DATA_H

#include <arpa/inet.h>
/**
 * @brief Struct that holds all the data we need to know about a node
 * 
 */
typedef struct node_data
{
    int key;                  /* the node's key */
    char ip[INET_ADDRSTRLEN]; /* the node's ip */
    char port[6];             /* the nodes's port */
    int  fd;                  /* the file descriptor for the current tcp socket used to communicate with the node, its -1 if we haven't opened one  */
} node_data_t;

node_data_t *create_node_data(int key,const char ip[INET_ADDRSTRLEN],const char port[6],int fd);

void destroy_node_data(node_data_t *node);

#endif