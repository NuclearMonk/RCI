#include "message.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
char *message_to_string(const message_t *message)
{
    if (!message)
        return NULL;
    char *buffer = malloc(MESSAGE_BUFFER_SIZE);
    if (!buffer)
        return NULL;
    switch (message->header)
    {
    case SELF:
        sprintf(buffer, "SELF %d %15s %5s\n", message->i_key, message->i_ip, message->i_port);
        return buffer;
        break;
    case PRED:
        sprintf(buffer, "PRED %d %15s %5s\n", message->i_key, message->i_ip, message->i_port);
        return buffer;
        break;
    default:
        return NULL;
        break;
    }
}

message_t *string_to_message(char *string)
{
    char header_buffer[6] = "";

    int argument_buffer;

    if (sscanf(string, "%5s", header_buffer) == 1)
    {
        char buffer_ip[INET_ADDRSTRLEN] = "";
        char buffer_port[6] = "";
        if (strcmp(header_buffer, "SELF") == 0)
        {
            if (sscanf(string, "%*s %d %15s %5s", &argument_buffer, buffer_ip, buffer_port) == 3)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return NULL;
                if (!is_string_valid_port(buffer_port))
                    return NULL;
                free(string);
                return create_message(SELF, argument_buffer, buffer_ip, buffer_port);
            }
            return NULL;
        }
        else if (strcmp(header_buffer, "PRED") == 0)
        {
            if (sscanf(string, "%*s %d %15s %5s", &argument_buffer, buffer_ip, buffer_port) == 3)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return NULL;
                if (!is_string_valid_port(buffer_port))
                    return NULL;
                free(string);
                return create_message(PRED, argument_buffer, buffer_ip, buffer_port);
            }
            return NULL;
        }
    }
    free(string);
    return NULL;
}

message_t *create_message(const message_header header, const int i_key, const char i_ip[INET_ADDRSTRLEN], const char i_port[6])
{
    message_t *message = calloc(1, sizeof(message_t));
    if (!message)
        return NULL;
    message->header = header;
    message->i_key = i_key;
    strcpy(message->i_ip, i_ip);
    strcpy(message->i_port, i_port);
    return message;
}
