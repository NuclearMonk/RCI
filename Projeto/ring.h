#ifndef RING_H
#define RING_H

#include "node_data.h"
#include "message.h"
#include "wait_list.h"

#define MAX_KEY 32
typedef struct node
{
    node_data_t *self;
    node_data_t *sucessor;
    node_data_t *chord; /* Node destino do atalho */
    node_data_t *antecessor;
    wait_list_t *wait_list;
    int message_id;
    int socket_tcp;
    int socket_udp;
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
 * @brief Leaves the current ring
 *
 * @param node always self
 */
void leave_ring(node_t *node);

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

void set_chord(node_t* node, node_data_t* chord_node);
void remove_chord(node_t* node);

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
 * @return char* an heap allocated string that contains the message, NULL in case of failure
 */
char *read_tcp_message(int fd, struct sockaddr* addr, socklen_t* addrlen);

/**
 * @brief reads a message UDP message on socket fd
 *
 * @param fd the socket to read the message from
 * @return char* an heap allocated string that contains the message, NULL in case of failure
 */
char *read_udp_message(int fd, struct sockaddr* addr, socklen_t* addrlen);

/**
 * @brief handles a message
 * 
 * @param message the message
 * @param self  always self 
 */
void handle_message(message_t *message, node_t *self);

void find_key(int key, node_t* node);

int calculate_distance(int start, int end);

#endif