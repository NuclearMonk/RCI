#include <arpa/inet.h>


typedef struct node
{
    int key;                        /* chave do node */
    char ip[INET_ADDRSTRLEN];       /* ip do node */
    char port [6];                  /* porta */
}node_t;


node_t *create_node(int key,char ip[INET_ADDRSTRLEN], char port[6]);
void destroy_node(node_t* node);