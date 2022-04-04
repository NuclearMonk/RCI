#include "client.h"
#include <stdlib.h>
#include "sys/select.h"
#include "ring.h"
#include <arpa/inet.h>
#include <signal.h>
#include "client.h"
#include "message.h"

#define MAX(A, B) ((A > B) ? A : B)

void run_ring(int key, char *ip, char *port)
{
    struct sigaction act = {.sa_handler = SIG_IGN};
    if (sigaction(SIGPIPE, &act, NULL) == -1)
        exit(-1);
    if (!is_string_valid_ip(ip))
        exit(-1);
    if (!is_string_valid_port(port))
        exit(1);

    fd_set set, temp_set;
    console_command_t *command;

    node_t *node = create_node(key, ip, port);
    if (!node)
    {
        fprintf(stdout, "The specified port could not be used\nPlease run again with different parameters\n");
        fflush(stdout);
        exit(-1);
    }

    FD_ZERO(&set);
    FD_SET(0, &set);
    FD_SET(node->socket_tcp, &set);
    FD_SET(node->socket_udp, &set);

    while (1)
    {
        temp_set = set;
        int count = select(MAX(node->socket_tcp, node->socket_udp) + 1, &temp_set, NULL, NULL, NULL);
        while (count > 0)
        {
            if (FD_ISSET(0, &temp_set))
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
                        set_antecessor_node(node, create_node_data(command->argument, command->ip, command->port));
                        break;
                    case c_chord:
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
            if (FD_ISSET(node->socket_tcp, &temp_set))
            {
                message_t *message = string_to_message(read_tcp_message(node->socket_tcp));
                if (message)
                {
                    handle_message(message, node);
                    free(message);
                }
            }
            if (FD_ISSET(node->socket_udp, &temp_set))
            {
                message_t *message = string_to_message(read_udp_message(node->socket_udp));
                if (message)
                {
                    handle_message(message, node);
                    free(message);
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