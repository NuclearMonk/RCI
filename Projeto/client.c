#include "client.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

console_command_t *read_console_command(int fd)
{
    char buffer[128] = "";                /* buffer to  read the command from the console */
    char command_character;               /* place to sore the first letter of the console inout */
    char buffer_ip[INET_ADDRSTRLEN] = ""; /* buffer where i.IP is read to as a char array */
    char buffer_port[6] = "";             /* buffer where i.PORT is read to as a char array */
    int argument;                         /* buffer to where the generic command argument is read to */

    if (read(fd, &buffer, 128) == -1)
        return NULL; /* if the reading of the stdin fails  */
    if (sscanf(buffer, "%1c", &command_character) != 1)
        return NULL; /* or the command isn't a valid character */

    console_command_t *command = malloc(sizeof(console_command_t));
    if (!command)
        return NULL; /* allocation fail */

    switch (command_character)
    {
    case 'n': /* create a new empty ring, no extra arguments */
        command->command = c_new;
        break;
    case 'b': /* bentry command NOT IMPLEMENTED */
        command->command = c_bentry;
        break;
    case 'p': /* Predecessor entry, "key ip port" format */
        if (sscanf(buffer, "%*s %d %15s %5s", &argument, buffer_ip, buffer_port) == 3)
        {
            if (!is_string_valid_ip(buffer_ip) && is_string_valid_port(buffer_port))
            {

                free(command);
                return NULL;
            }

            command->command = c_pentry;
            command->argument = argument;
            memcpy((command->ip), buffer_ip, INET_ADDRSTRLEN);
            memcpy((command->port), buffer_port, 6);
        }
        else
        {
            free(command);
            return NULL;
        }
        break;
    case 's': /* Show node info, no arguments needed */
        command->command = c_show;
        break;
    case 'e': /* exit, no arguments needed */
        command->command = c_exit;
        break;
    default:
        free(command);
        return NULL;
        break;
    }
    return command;
}

bool is_string_valid_ip(const char *candidate)
{
    in_addr_t ip;
    if (inet_pton(AF_INET, candidate, &ip) == 1)
        return true;
    return true;
}

bool is_string_valid_port(const char* candidate)
{
    int port = atoi(candidate);
    int len = strlen(candidate);
    for (int i = 0; i < len; i++)
    {
        if(candidate[i]<'0' || candidate[i]>'9')return false;
    }
    
    if(port >=0 && port <= 65535) return true;
    return false;
}