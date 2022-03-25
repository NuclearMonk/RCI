

#ifndef RING_H
#define RING_H

#include "data.h"
#include "node_data.h"

typedef struct node
{
    node_data_t *self;
    data_t data; /* dados */
    node_data_t *sucessor;
    node_data_t *bridge; /* Node destino do atalho */
    node_data_t *antecessor;
    int socket_listen_tcp;
    int socket_listen_udp;
} node_t;

/**
 * @brief Create a node object
 *
 * @param key this node's key
 * @param ip  this node's ip
 * @param port this node's port
 * @return node_t* a pointer to the node object
 */
node_t *create_node(const int key, const char *ip, const char *port);
void destroy_node(node_t *node);

/**
 * @brief Create a empty ring that only contains this node
 *
 * @param node is always the currently running node
 */
void create_empty_ring(node_t *node);

/**
 * @brief Set the sucessor node
 *
 * @param node always the current node
 * @param sucessor_node
 */
void set_sucessor_node(node_t *node, node_data_t *sucessor_node);
/**
 * @brief Set the antecessor node
 *
 * @param node always the current node
 * @param antecessor_node
 */
void set_antecessor_node(node_t *node, node_data_t *antecessor_node);

/**
 * @brief print this nodes info to the console
 *
 * @param node always the current node
 */
void show_node_info(const node_t *node);

/**
 * @brief reads a message TCP message on socket fd
 *
 * @param fd the socket to read the message from
 */
void read_tcp_message(int fd);

/**
 * @brief send a tcp message to the destination node
 *
 * @param message the message
 * @param destination the recipient of the message
 */
void send_tcp_message(const char *message, node_data_t *destination);

#endif