#include "client.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

console_command_t *read_console_command(int fd)
{
    char buffer[128] = "";
    char command_buffer[10] = "";
    char buffer_ip[INET_ADDRSTRLEN] = "";
    char buffer_port[6] = "";
    int argument;
    if(read(fd, &buffer, 128)==-1)return NULL;
    sscanf(buffer, "%10c", command_buffer);
    console_command_t *command = malloc(sizeof(console_command_t));
    if (!command)
        return NULL;
    switch (command_buffer[0])
    {
    case 'n':
        command->command = c_new;
        break;
    case 'b':
        command->command = c_bentry;
        break;
    case 'p':
        if (sscanf(buffer, "%*s %d %15s %5s", &argument, buffer_ip, buffer_port) == 3)
        {
            if(is_string_valid_ip(buffer_ip)){
                free(command);
                return NULL;
            }
            command->command = c_pentry;
            command->argument = argument;
            strncpy((command->ip), buffer_ip, INET_ADDRSTRLEN);
            strncpy((command->port), buffer_port, 6);
        }
        else
        {
            free(command);
            return NULL;
        }
        break;
    case 's':
        command->command = c_show;
        break;
    case 'e':
        command->command = c_exit;
        break;
    default:
        free(command);
        return NULL;
        break;
    }
    return command;
}

int is_string_valid_ip(const char* candidate)
{
            in_addr_t ip;
            if(inet_pton(AF_INET,candidate,&ip)!=1)
            {
                return 0;
            }
            return 1;
}