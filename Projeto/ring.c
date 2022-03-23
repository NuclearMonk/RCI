#include "client.h"
#include <stdlib.h>
#include "sys/select.h"
#include "server.h"
#include <arpa/inet.h>
#include <signal.h>
#include "client.h"

void run_ring(int key, char*ip ,char* port)
{
    struct sigaction act = {.sa_handler = SIG_IGN};
    if (sigaction(SIGPIPE, &act, NULL) == -1)exit(1);
    if (!is_string_valid_ip(ip))exit(1);
    fd_set set, temp_set;

    console_command_t *command;
    server_t *server = create_server(key,ip,port);
    FD_ZERO(&set);
    FD_SET(0, &set);
    FD_SET(server->socket_listen, &set);

    while (1)
    {
        temp_set = set;
        int count = select(server->socket_listen+1, &temp_set, NULL, NULL, NULL);
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
                        break;
                    case c_pentry:
                        set_antecessor_node(server, create_node(command->argument, command->ip, command->port));
                        break;
                    case c_show:
                        show_server_info(server);
                        break;
                    case c_exit:
                        destroy_server(server);
                        free(command);
                        return;
                        break;
                    default:
                        break;
                    }
                    free(command);
                }
            }
            if (FD_ISSET(server->socket_listen, &temp_set))
            {
                read_message(server->socket_listen);
            }
            count--;
        }
    }
}

int main(int argc , char* argv[])
{
    if(argc != 4) return -1;
    int key = atoi(argv[1]);
    run_ring(key,argv[2],argv[3]);
    return 0;
}