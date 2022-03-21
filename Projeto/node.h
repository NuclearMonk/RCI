typedef struct node
{
    int key;    /* chave do snode */
    int ip;     /* ip do node */
    int port;   /* porta */
}node_t;


node_t* create_node(int key,int ip, int port);
void destroy_node(node_t* node);