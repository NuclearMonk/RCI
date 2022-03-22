#include "data.h"
#include "node.h"


typedef struct server
{
    node_t* self; 
    data_t data;/* dados */
    node_t* successor;  
    node_t* bridge; /* Node destino do atalho */
    node_t* antecessor;
    int socket_listen;
    int socket_send;
}server_t;

server_t*   create_server();
void        destroy_server(server_t* server);

void        set_antecessor_node(server_t* server, node_t* node);

void        show_server_info(server_t* server);

void        read_message(int fd);
