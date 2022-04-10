#include "ring.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "client.h"

int open_tcp_connection(const node_data_t *target);

int bind_tcp_socket(int fd, const node_data_t *node);
int bind_udp_socket(int fd, const node_data_t *node);

int send_tcp_message(message_t *message, node_t *node, node_data_t *destination);
int send_udp_message(message_t *message, node_t *node, node_data_t *destination);

node_t *create_node(int key, const char *ip, const char *port)
{
    node_t *node = calloc(1, sizeof(node_t));
    if (!node)
        return NULL;
    node->self = create_node_data(key, ip, port);
    node->socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
    int errorcode = bind_tcp_socket(node->socket_tcp, node->self);
    if (errorcode == -1)
    {
        destroy_node(node);
        return NULL;
    }
    node->socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    errorcode = bind_udp_socket(node->socket_udp, node->self);
    if (errorcode == -1)
    {
        destroy_node(node);
        return NULL;
    }

    return node;
}

void destroy_node(node_t *node)
{

    if (!node)
        return;
    leave_ring(node);
    close(node->socket_tcp);
    close(node->socket_udp);
    destroy_node_data(node->self);
    destroy_node_data(node->sucessor);
    destroy_node_data(node->antecessor);
    destroy_node_data(node->chord);
    destroy_list(node->wait_list);
    free(node);
}

void create_empty_ring(node_t *node)
{
    destroy_node_data(node->sucessor);
    destroy_node_data(node->antecessor);
    node->sucessor = create_node_data(node->self->key, node->self->ip, node->self->port);
    node->antecessor = create_node_data(node->self->key, node->self->ip, node->self->port);
}

void leave_ring(node_t *node)
{
    if (NULL != node->antecessor && NULL != node->sucessor && node->sucessor->key != node->self->key && node->antecessor->key != node->self->key)
    {
        send_tcp_message((create_message(PRED, -1, -1, node->antecessor->key, node->antecessor->ip, node->antecessor->port)), node, node->sucessor);
    }
    destroy_node_data(node->sucessor);
    destroy_node_data(node->antecessor);
    destroy_node_data(node->chord);
    node->sucessor = NULL;
    node->antecessor = NULL;
    node->chord = NULL;
}

void show_node_info(const node_t *node)
{
    if (!node)
    {
        printf("NO RING ON THIS NODE\n");
        fflush(stdout);
        return;
    }
    if (!(node->self))
    {
        printf("NO RING ON THIS NODE\n");
        fflush(stdout);
        return;
    }
    printf("This node:\n");
    printf("KEY: %d\n", node->self->key);
    printf("IP: %s\n", node->self->ip);
    printf("PORT: %s\n", node->self->port);
    if (node->antecessor)
    {
        printf("Antecessor node:\n");
        printf("KEY: %d\n", node->antecessor->key);
        printf("IP: %s\n", node->antecessor->ip);
        printf("PORT: %s\n", node->antecessor->port);
    }
    if (node->sucessor)
    {
        printf("Sucessor node:\n");
        printf("KEY: %d\n", node->sucessor->key);
        printf("IP: %s\n", node->sucessor->ip);
        printf("PORT: %s\n", node->sucessor->port);
    }
    if (node->chord)
    {
        printf("Chord node:\n");
        printf("KEY: %d\n", node->chord->key);
        printf("IP: %s\n", node->chord->ip);
        printf("PORT: %s\n", node->chord->port);
    }
    fflush(stdout);
}

void set_sucessor_node(node_t *node, node_data_t *sucessor_node)
{
    if (!node || !sucessor_node) /* null checking */
        return;
    if (node->sucessor)
    {
        if (node->antecessor)
        {
            if (node->antecessor->key == node->sucessor->key && node->self->key == node->sucessor->key) /* if we  are our antecessor and predecessor then we need to set our antecessor too */
            {
                set_antecessor_node(node, create_node_data(sucessor_node->key, sucessor_node->ip, sucessor_node->port));
            }
        }

        destroy_node_data(node->sucessor);
    }
    node->sucessor = sucessor_node;
}

void set_antecessor_node(node_t *node, node_data_t *antecessor_node)
{
    if (!node || !antecessor_node) /* null checks */
        return;
    if (node->antecessor) /* if we have an antecessor */
    {
        if (antecessor_node->key == node->self->key) /* if our antecessor is ourselves then we must also change our sucessor, as it will also be ourselves */
        {
            set_sucessor_node(node, create_node_data(antecessor_node->key, antecessor_node->ip, antecessor_node->port));
        }
        destroy_node_data(node->antecessor); /* we make sure we don't leak memory by destroying the existing antecessor */
    }

    if (send_tcp_message(create_message(SELF, -1, -1, node->self->key, node->self->ip, node->self->port), node, antecessor_node) != -1)
    {
        node->antecessor = antecessor_node;
    }
}

void set_chord(node_t *node, node_data_t *chord_node)
{
    if (!node || !chord_node) /* null checks */
        return;
    node->chord = chord_node;
}

void remove_chord(node_t *node)
{
    destroy_node_data(node->chord);
    node->chord = NULL;
}

int open_tcp_connection(const node_data_t *target)
{
    if (!target)
        return -1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    int errorcode;
    errorcode = getaddrinfo(target->ip, target->port, &hints, &res);
    if (errorcode != 0)
        return errorcode;
    errorcode = connect(fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    if (errorcode == -1)
        return -1;
    return fd;
}

int bind_tcp_socket(int fd, const node_data_t *node)
{
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM, .ai_flags = AI_PASSIVE};
    int errorcode = getaddrinfo(node->ip, node->port, &hints, &res);
    if (errorcode != 0)
    {
        freeaddrinfo(res);
        return -1;
    }
    errorcode = bind(fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    if (errorcode == -1)
        return -1;
    return listen(fd, 5);
}

int bind_udp_socket(int fd, const node_data_t *node)
{
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_DGRAM, .ai_flags = AI_PASSIVE};
    int errorcode = getaddrinfo(node->ip, node->port, &hints, &res);
    if (errorcode != 0)
    {
        freeaddrinfo(res);
        return -1;
    }
    errorcode = bind(fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    if (errorcode == -1)
        return -1;
    return errorcode;
}

char *read_tcp_message(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    // struct sockaddr addr;
    // socklen_t addrlen = sizeof(addr);
    char buffer[128] = "";
    int newfd, nread;
    if ((newfd = accept(fd, addr, addrlen)) == -1)
        return NULL;
    if ((nread = read(newfd, &buffer, 128)) == -1)
        return NULL;
    buffer[nread] = '\0';
    char *message = malloc((strlen(buffer) + 1) * sizeof(char));
    if (!message)
        return NULL;
    strcpy(message, buffer);
    printf("\033[0;31m");
    printf("MENSAGEM RECEBIDA TCP\n%s\n", buffer);
    printf("\033[0m");
    fflush(stdout);

    shutdown(newfd, SHUT_RDWR);
    close(newfd);
    return message;
}

char *read_udp_message(int fd, struct sockaddr *addr, socklen_t *addrlen)
{
    char buffer[128] = "";
    int nread;
    if ((nread = recvfrom(fd, buffer, 128, 0, addr, addrlen)) == -1)
        return NULL;
    buffer[nread] = '\0';
    char *message = malloc((strlen(buffer) + 1) * sizeof(char));
    if (!message)
        return NULL;
    strcpy(message, buffer);
    message_t *msg = string_to_message(message, addr, addrlen);
    if (msg->header != ACK)
    {
        message_t *msg_2 = create_message(ACK, 0, 0, 0, NULL, NULL);
        char *message_string = message_to_string(msg_2);
        free(msg_2);
        if (!message_string)
        {
            free(msg);
            free(message);
            return NULL;
        }
        sendto(fd, message_string, strlen(message_string) + 1, 0, addr, *addrlen);
        free(message_string);
    }
    free(msg);
    message = malloc((strlen(buffer) + 1) * sizeof(char));
    if (!message)
        return NULL;
    strcpy(message, buffer);
    printf("\033[0;31m");
    printf("MENSAGEM RECEBIDA UDP\n%s\n", buffer);
    printf("\033[0m");
    fflush(stdout);
    return message;
}

/**
 * @brief sends a message to the destination node using the TCP protocol
 *
 * @param message the message
 * @param destination the recipient of the message
 * @return int, 1 on case of success, -1 otherwise
 */
int send_tcp_message(message_t *message, node_t *node, node_data_t *destination)
{
    if (!message)
        return -1;
    if (!destination)
    {
        free(message);
        return -1;
    }
    if (node->self->key == destination->key)
    {
        if (message->header == FND || message->header == RSP)
        {
            handle_message(message, node);
        }
        free(message);
        return 0;
    }
    int fd = open_tcp_connection(destination);
    if (fd < 0)
    {
        free(message);
        return -1;
    }
    char *message_string = message_to_string(message);
    free(message);
    /* Debug Prints */
    printf("\033[0;32m");
    printf("MENSAGEM ENVIADA TCP\nDestino:%d\nContent:%s\n\n", destination->key, message_string);
    printf("\033[0m");
    fflush(stdout);
    int length = strlen(message_string) + 1;
    int errorcode = write(fd, message_string, length);
    shutdown(fd, SHUT_RDWR);
    close(fd);
    free(message_string);
    return errorcode;
}

/**
 * @brief sends a message to the destination node using the UDP protocol
 *
 * @param message the message
 * @param destination the recipient of the message
 * @return int, 1 on case of success, -1 otherwise
 */
int send_udp_message(message_t *message, node_t *node, node_data_t *destination)
{
    if (!destination)
    {
        free(message);
        return -1;
    }
    if (node->self->key == destination->key)
    {
        if (message->header == FND || message->header == RSP)
        {
            handle_message(message, node);
        }
        free(message);
        return 0;
    }
    struct addrinfo *res;
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_DGRAM};
    int errorcode;
    errorcode = getaddrinfo(destination->ip, destination->port, &hints, &res);
    if (errorcode != 0)
    {
        freeaddrinfo(res);
        free(message);
        return -1;
    }
    char *message_string = message_to_string(message);
    free(message);
    /* Debug Prints */
    printf("\033[0;32m");
    printf("MENSAGEM ENVIADA UDP\nDestino:%d\nContent:%s\n\n", destination->key, message_string);
    printf("\033[0m");
    fflush(stdout);
    int length = strlen(message_string) + 1;
    errorcode = sendto(node->socket_udp, message_string, length, 0, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    free(message_string);
    return errorcode;
}

int send_message(message_t *message, node_t *node, int destination_key)
{
    if (!node || !node->sucessor)
    {
        free(message);
        return -1;
    }
    if (!node->chord)
    {
        return send_tcp_message(message, node, node->sucessor);
    }
    if (calculate_distance(node->chord->key, destination_key) < calculate_distance(node->sucessor->key, destination_key))
    {
        return send_udp_message(message, node, node->chord);
    }
    return send_tcp_message(message, node, node->sucessor);
}

void handle_message(message_t *message, node_t *node)
{
    switch (message->header)
    {
    case SELF:
        if (node->sucessor)
        {
            if (message->i_key == node->sucessor->key)
            {
                return;
            }
        }
        send_tcp_message(create_message(PRED, -1, -1, message->i_key, message->i_ip, message->i_port), node, node->sucessor);
        set_sucessor_node(node, create_node_data(message->i_key, message->i_ip, message->i_port));
        break;
    case PRED:
        if (node->sucessor)
        {
            set_antecessor_node(node, create_node_data(message->i_key, message->i_ip, message->i_port));
        }
        break;
    case FND:
        if (node->sucessor)
        {
            if (calculate_distance(node->self->key, message->key) <= calculate_distance(node->sucessor->key, message->key))
            {
                send_message(create_message(RSP, message->i_key, message->message_id, node->self->key, node->self->ip, node->self->port), node, message->i_key);
                /*  send_tcp_message(create_message(RSP, message->i_key, message->message_id, node->self->key, node->self->ip, node->self->port), node, node->sucessor); */
            }
            else
            {
                send_message(copy_message(message), node, node->sucessor->key);
                /* send_tcp_message(copy_message(message), node, node->sucessor); */
            }
        }
        break;
    case RSP:
        if (node->sucessor)
        {
            if (!(calculate_distance(node->self->key, message->key) < calculate_distance(node->sucessor->key, message->key)) && node->self->key != node->sucessor->key)
                send_message(copy_message(message), node, message->i_key);
            else
            {
                node_data_t *node_data;
                int is_find;
                node->wait_list = find_and_pop_element(node->wait_list, message->message_id, &is_find, &node_data);
                if (is_find)
                {
                    printf("FIND RESULTS FOR SEARCH ID:%d\n", message->message_id);
                    printf("KEY:%d\n", message->i_key);
                    printf("IP:%s\n", message->i_ip);
                    printf("port:%s\n", message->i_port);
                }
                else
                {
                    send_udp_message(create_message(EPRED, -1, -1, message->i_key, message->i_ip, message->i_port), node, node_data);
                    destroy_node_data(node_data);
                }
            }
        }
        break;
    case EFND:
        if (node->sucessor)
        {
            node->wait_list = add_element(node->wait_list, node->message_id, 0, create_node_data(message->key, message->i_ip, message->i_port));
            send_message(create_message(FND, message->key, node->message_id, node->self->key, node->self->ip, node->self->port), node, node->sucessor->key);
            node->message_id++;
        }
        break;
    case EPRED:
        set_antecessor_node(node, create_node_data(message->i_key, message->i_ip, message->i_port));
        break;
    case ACK:
        break;
    default:
        break;
    }
}

void find_key(int key, node_t *node)
{
    node->wait_list = add_element(node->wait_list, node->message_id, 1, NULL);
    send_message(create_message(FND, key, node->message_id, node->self->key, node->self->ip, node->self->port), node, node->sucessor->key);
    node->message_id++;
}

void enter_ring(node_t *node, node_data_t *existing_member)
{
    send_udp_message(create_message(EFND, node->self->key, -1, -1, node->self->ip, node->self->port), node, existing_member);
    destroy_node_data(existing_member);
}

int calculate_distance(int start, int end)
{
    return abs((end - start) % MAX_KEY);
}