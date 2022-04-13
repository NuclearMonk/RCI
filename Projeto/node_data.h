#ifndef NODE_DATA_H
#define NODE_DATA_H

#include <arpa/inet.h>

typedef struct node_data
{
    int key;                  /* chave do node */
    char ip[INET_ADDRSTRLEN]; /* ip do node */
    char port[6];             /* porta */
    int  fd;
} node_data_t;

node_data_t *create_node_data(int key,const char ip[INET_ADDRSTRLEN],const char port[6],int fd);
void destroy_node_data(node_data_t *node);

#endif