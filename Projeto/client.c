#include "client.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

console_command_t *read_console_command(int fd)
{
    char buffer[128] = "";
    char command[10] = "";
    char buffer_ip[128] = "";
    int argument, port;
    read(fd, &buffer, 128);
    sscanf(buffer, "%128c", buffer2);
    console_command_t *command = NULL;
    switch (buffer2[0])
    {
    case 'n':
        command = malloc(sizeof(console_command_t));
        if (!command)
            break;
        command->command = c_new;
        break;
    case 'b':
        command = malloc(sizeof(console_command_t));
        if (!command)
            break;
        command->command = c_bentry;
        break;
    case 'p':
        if (sscanf(buffer, "%*s %d %15s %d", &argument, buffer_ip, &port) != 3)
            break;
        command = malloc(sizeof(console_command_t));
        if (!command)
            break;
        command->command = c_pentry;
        command->argument = argument;
        inet_pton(AF_INET, buffer_ip, &(command->ip));
        command->port = port;

        break;
    case 'e':
        command = malloc(sizeof(console_command_t));
        command->command = c_exit;
    default:
        break;
    }
    return command;
}