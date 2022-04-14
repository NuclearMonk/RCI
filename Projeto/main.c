#include "client.h"
#include <stdlib.h>
#include "sys/select.h"
#include "ring.h"
#include <arpa/inet.h>
#include <signal.h>
#include "client.h"
#include "message.h"

void run_ring(int key, char *ip, char *port)
{
    struct sigaction act = {.sa_handler = SIG_IGN};
    if (sigaction(SIGPIPE, &act, NULL) == -1)
        exit(-1);
    if (!is_string_valid_ip(ip))
        exit(-1);
    if (!is_string_valid_port(port))
        exit(1);
    console_command_t *command;
    struct sockaddr sender_info;
    socklen_t sender_info_len = sizeof(sender_info);
    node_t *node = create_node(key, ip, port);
    if (!node)
    {
        fprintf(stdout, "The specified port could not be used\nPlease run again with different parameters\n");
        fflush(stdout);
        exit(-1);
    }

    FD_ZERO(&(node->set));
    FD_SET(0, &(node->set));
    FD_SET(node->socket_tcp, &(node->set));
    FD_SET(node->socket_udp, &(node->set));
    node->max_fd = MAX(node->socket_tcp, node->socket_udp);
    while (1)
    {
        FD_ZERO(&(node->set));
        FD_SET(0, &(node->set));
        FD_SET(node->socket_tcp, &(node->set));
        FD_SET(node->socket_udp, &(node->set));
        if (node->antecessor && node->antecessor->fd > 0)
        {
            FD_SET(node->antecessor->fd, &(node->set));
            node->max_fd = MAX(node->max_fd, node->antecessor->fd);
        }
        if (node->sucessor && node->sucessor->fd > 0)
        {
            FD_SET(node->sucessor->fd, &(node->set));
            node->max_fd = MAX(node->max_fd, node->sucessor->fd);
        }
        int count = select(node->max_fd + 1, (&node->set), NULL, NULL, NULL);
        while (count > 0)
        {
            if (FD_ISSET(0, (&node->set)))
            {
                command = read_console_command(0);
                if (command)
                {
                    switch (command->command)
                    {
                    case c_new:
                        create_empty_ring(node);
                        break;
                    case c_pentry:
                        leave_ring(node);
                        set_antecessor_node(node, create_node_data(command->argument, command->ip, command->port, -1));
                        break;
                    case c_bentry:
                        enter_ring(node, create_node_data(command->argument, command->ip, command->port, -1));
                        break;
                    case c_chord:
                        set_chord(node, create_node_data(command->argument, command->ip, command->port, -1));
                        break;
                    case c_dchord:
                        remove_chord(node);
                        break;
                    case c_find:
                        find_key(command->argument, node);
                        break;
                    case c_show:
                        show_node_info(node);
                        break;
                    case c_leave:
                        leave_ring(node);
                        break;
                    case c_exit:
                        destroy_node(node);
                        free(command);
                        return;
                        break;
                    default:
                        break;
                    }
                    free(command);
                }
            }
            if (FD_ISSET(node->socket_tcp, (&node->set)))
            {
                int new_fd = -1;
                message_t *message = string_to_message(read_tcp_message(node->socket_tcp, 1, &new_fd, &sender_info, &sender_info_len), &sender_info);
                if (message)
                {
                    handle_message(message, node, new_fd);
                    free(message);
                }
                break;
            }
            if (FD_ISSET(node->socket_udp, (&node->set)))
            {
                message_t *message = string_to_message(read_udp_message(node->socket_udp, &sender_info, &sender_info_len), &sender_info);
                if (message)
                {
                    handle_message(message, node, -1);
                    free(message);
                }
                break;
            }
            if ((node->sucessor != NULL) && (node->sucessor->fd != -1))
            {
                if (FD_ISSET(node->sucessor->fd, (&node->set)))
                {
                    int new_fd = -1;
                    message_t *message = string_to_message(read_tcp_message(node->sucessor->fd, 0, &new_fd, &sender_info, &sender_info_len), &sender_info);
                    if (message)
                    {
                        handle_message(message, node, new_fd);
                        free(message);
                    }
                    break;
                }
            }
            if ((node->antecessor != NULL) && (node->antecessor->fd != -1))
            {
                if (FD_ISSET(node->antecessor->fd, (&node->set)))
                {
                    int new_fd = -1;
                    message_t *message = string_to_message(read_tcp_message(node->antecessor->fd, 0, &new_fd, &sender_info, &sender_info_len), &sender_info);
                    if (message)
                    {
                        handle_message(message, node, new_fd);
                        free(message);
                    }
                    break;
                }
            }
            count--;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
        return -1;
    int key = atoi(argv[1]);
    run_ring(key, argv[2], argv[3]);
    return 0;
}