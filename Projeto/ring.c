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

node_t *create_node(int key, const char *ip, const char *port)
{
    node_t *node = calloc(1, sizeof(node_t));
    if (!node)
        return NULL;
    node->self = create_node_data(key, ip, port);
    node->socket_listen_tcp = socket(AF_INET, SOCK_STREAM, 0);
    int errorcode = bind_tcp_socket(node->socket_listen_tcp, node->self);
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
    close(node->socket_listen_tcp);
    destroy_node_data(node->self);
    destroy_node_data(node->sucessor);
    destroy_node_data(node->antecessor);
    destroy_node_data(node->bridge);
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
        send_tcp_message((create_message(PRED, -1, -1, node->antecessor->key, node->antecessor->ip, node->antecessor->port)), node->self, node->sucessor);
    }
    destroy_node_data(node->sucessor);
    destroy_node_data(node->antecessor);
    node->sucessor = NULL;
    node->antecessor = NULL;
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

    node->antecessor = antecessor_node;
    send_tcp_message(create_message(SELF, -1, -1, node->self->key, node->self->ip, node->self->port), node->self, node->antecessor);
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

char *read_tcp_message(int fd)
{
    struct sockaddr addr;
    socklen_t addrlen = sizeof(addr);
    char buffer[128] = "";
    int newfd, nread;
    if ((newfd = accept(fd, &addr, &addrlen)) == -1)
        return NULL;
    if ((nread = read(newfd, &buffer, 128)) == -1)
        return NULL;
    buffer[nread] = '\0';
    char *message = malloc((strlen(buffer) + 1) * sizeof(char));
    if (!message)
        return NULL;
    strcpy(message, buffer);
    printf("\033[0;31m");
    printf("MENSAGEM RECEBIDA\n%s\n", buffer);
    printf("\033[0m");
    fflush(stdout);

    shutdown(newfd, SHUT_RDWR);
    close(newfd);
    return message;
}

int send_tcp_message(message_t *message, const node_data_t *self, node_data_t *destination)
{
    if (!message)
        return -1;
    if (!destination)
    {
        free(message);
        return -1;
    }
    if (self->key == destination->key)
    {
        free(message);
        return 0;
    }

    /* end of debug prints */

    int fd = open_tcp_connection(destination);
    if (fd < 0)
    {
        free(message);
        fflush(stdout);
        return -1;
    }
    char *message_string = message_to_string(message);
    free(message);
    /* Debug Prints */
    printf("\033[0;32m");
    printf("MENSAGEM ENVIADA\nDestino:%d\nContent:%s\n\n", destination->key, message_string);
    printf("\033[0m");
    fflush(stdout);
    int length = strlen(message_string) + 1;
    int errorcode = write(fd, message_string, length);
    shutdown(fd, SHUT_RDWR);
    close(fd);
    free(message_string);
    return errorcode;
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
        send_tcp_message(create_message(PRED, -1, -1, message->i_key, message->i_ip, message->i_port), node->self, node->sucessor);
        set_sucessor_node(node, create_node_data(message->i_key, message->i_ip, message->i_port));
        break;
    case PRED:
        set_antecessor_node(node, create_node_data(message->i_key, message->i_ip, message->i_port));
        break;
    case FND:
        if (node->self->key > node->sucessor->key)
        {
            send_tcp_message(create_message(RSP, message->i_key, message->message_id, node->self->key, node->self->ip, node->self->port), node->self, node->sucessor);
        }
        else if (message->key >= node->self->key && message->key < node->sucessor->key)
        {
            send_tcp_message(create_message(RSP, message->i_key, message->message_id, node->self->key, node->self->ip, node->self->port), node->self, node->sucessor);
        }
        else
        {
            send_tcp_message(copy_message(message), node->self, node->sucessor);
        }
        break;
    case RSP:
        if(message->key != node->self->key)send_tcp_message(copy_message(message), node->self, node->sucessor);
        break;
    default:
        break;
    }
}