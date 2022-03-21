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
int         open_listen_socket(server_t* server);
int         open_send_socket(server_t* server);
