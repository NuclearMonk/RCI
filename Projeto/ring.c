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

void leave_ring(node_t *node)
{
    char buffer[128];
    if (NULL != node->antecessor && NULL != node->sucessor && node->sucessor->key != node->self->key && node->antecessor->key != node->self->key)
    {
        sprintf(buffer, "PRED %d %15s %5s", node->antecessor->key, node->antecessor->ip, node->antecessor->port);
        send_tcp_message(buffer, node->self, node->sucessor);
    }
}

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
        destroy_node_data(node->antecessor); /* we make sure we don't leak memory by destroying the existing antecessor */
        if (antecessor_node->key == node->self->key) /* if our antecessor is ourselves then we must also change our sucessor, as it will also be ourselves */
        {
            set_sucessor_node(node, create_node_data(antecessor_node->key, antecessor_node->ip, antecessor_node->port));
        }
    }

    node->antecessor = antecessor_node;
    char buffer[128];
    sprintf(buffer, "SELF %d %15s %5s", node->self->key, node->self->ip, node->self->port);
    send_tcp_message(buffer, node->self, node->antecessor);
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
    // printf("\033[0;31m");
    // printf("%s", buffer);
    // printf("\033[0m");
    // fflush(stdout);

    shutdown(newfd, SHUT_RDWR);
    close(newfd);
    return message;
}

int send_tcp_message(const char *message, const node_data_t *self, node_data_t *destination)
{

    if (!destination)
        return -1;
    if (self->key == destination->key)
        return 0;

    /* Debug Prints */
    printf("\033[0;32m");
    printf("MENSAGEM ENVIADA\nDestino:%d\nContent:%s\n\n", destination->key, message);
    printf("\033[0m");
    fflush(stdout);
    /* end of debug prints */

    int fd = open_tcp_connection(destination);
    if (fd < 0)
        return -1;
    int length = strlen(message) + 1;
    int errorcode = write(fd, message, length);
    shutdown(fd, SHUT_RDWR);
    close(fd);
    return errorcode;
}

void handle_message(const char *message, node_t *node)
{
    printf("\033[0;31m");
    printf("MENSAGEM RECEBIDA:%s\n", message);
    printf("\033[0m");
    fflush(stdout);
    char header_buffer[6] = "";
    char buffer_ip[INET_ADDRSTRLEN] = "";
    char buffer_port[6] = "";
    char buffer[128];
    int argument_buffer;

    if (sscanf(message, "%5s", header_buffer) == 1)
    {
        if (strcmp(header_buffer, "SELF") == 0)
        {
            if (sscanf(message, "%*s %d %15s %5s", &argument_buffer, buffer_ip, buffer_port) == 3)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return;
                if (!is_string_valid_port(buffer_port))
                    return;

                // if the sucessor node is already the current one we don't need to change anything
                if (node->sucessor)
                {
                    if (argument_buffer == node->sucessor->key)
                    {
                        return;
                    }
                }

                // operation to insert the new node in the ring
                sprintf(buffer, "PRED %d %15s %5s", argument_buffer, buffer_ip, buffer_port);
                send_tcp_message(buffer, node->self, node->sucessor);
                set_sucessor_node(node, create_node_data(argument_buffer, buffer_ip, buffer_port));
            }
        }
        if (strcmp(header_buffer, "PRED") == 0)
        {
            if (sscanf(message, "%*s %d %15s %5s", &argument_buffer, buffer_ip, buffer_port) == 3)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return;
                if (!is_string_valid_port(buffer_port))
                    return;
                set_antecessor_node(node, create_node_data(argument_buffer, buffer_ip, buffer_port));
            }
        }
    }
}