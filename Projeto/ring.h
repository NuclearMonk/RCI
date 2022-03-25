#include "data.h"
#include "node_data.h"


typedef struct node
{
    node_data_t* self; 
    data_t data;/* dados */
    node_data_t* successor;  
    node_data_t* bridge; /* Node destino do atalho */
    node_data_t* antecessor;
    int socket_listen_tcp;
    int socket_listen_udp;
    int socket_send;
}node_t;

node_t*     create_server(int key, char* ip, char* port);
void        destroy_server(node_t* node);

void        create_empty_ring(node_t* node);

void        set_sucessor_node(node_t* node, node_data_t* antecessor_node);
void        set_antecessor_node(node_t *node, node_data_t *antecessor_node);


void        show_node_info(const node_t* node);

void        read_message(int fd);

void        send_message(char* message, node_data_t* destination);
