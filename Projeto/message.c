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
    case FND:
        sprintf(buffer, "FND %d %d %d %15s %5s\n", message->key, message->message_id, message->i_key, message->i_ip, message->i_port);
        return buffer;
        break;
    case RSP:
        sprintf(buffer, "RSP %d %d %d %15s %5s\n", message->key, message->message_id, message->i_key, message->i_ip, message->i_port);
        return buffer;
        break;
    case EFND:
        sprintf(buffer, "EFND %d\n", message->key);
        return buffer;
        break;
    case EPRED:
        sprintf(buffer, "EPRED %d %15s %5s\n", message->i_key, message->i_ip, message->i_port);
        return buffer;
        break;
    case ACK:
        sprintf(buffer, "ACK\n");
        return buffer;
        break;
    default:
        return NULL;
        break;
    }
}

message_t *string_to_message(char *string, struct sockaddr *sender_info, socklen_t *sender_info_len)
{
    if (!string)
    {
        return NULL;
    }
    char buffer_header[7] = "";

    int buffer_argument, buffer_key, buffer_message_id;

    if (sscanf(string, "%6s", buffer_header) == 1)
    {
        char buffer_ip[INET_ADDRSTRLEN] = "";
        char buffer_port[6] = "";
        if (strcmp(buffer_header, "SELF") == 0)
        {
            if (sscanf(string, "%*s %d %15s %5s", &buffer_argument, buffer_ip, buffer_port) == 3)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return NULL;
                if (!is_string_valid_port(buffer_port))
                    return NULL;
                free(string);
                return create_message(SELF, -1, -1, buffer_argument, buffer_ip, buffer_port);
            }
            return NULL;
        }
        else if (strcmp(buffer_header, "PRED") == 0)
        {
            if (sscanf(string, "%*s %d %15s %5s", &buffer_argument, buffer_ip, buffer_port) == 3)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return NULL;
                if (!is_string_valid_port(buffer_port))
                    return NULL;
                free(string);
                return create_message(PRED, -1, -1, buffer_argument, buffer_ip, buffer_port);
            }
            return NULL;
        }
        else if (strcmp(buffer_header, "FND") == 0)
        {
            if (sscanf(string, "%*s %d %d %d %15s %5s", &buffer_key, &buffer_message_id, &buffer_argument, buffer_ip, buffer_port) == 5)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return NULL;
                if (!is_string_valid_port(buffer_port))
                    return NULL;
                free(string);
                return create_message(FND, buffer_key, buffer_message_id, buffer_argument, buffer_ip, buffer_port);
            }
            return NULL;
        }
        else if (strcmp(buffer_header, "RSP") == 0)
        {
            if (sscanf(string, "%*s %d %d %d %15s %5s", &buffer_key, &buffer_message_id, &buffer_argument, buffer_ip, buffer_port) == 5)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return NULL;
                if (!is_string_valid_port(buffer_port))
                    return NULL;
                free(string);
                return create_message(RSP, buffer_key, buffer_message_id, buffer_argument, buffer_ip, buffer_port);
            }
            return NULL;
        }
        else if (strcmp(buffer_header, "EFND") == 0)
        {
            if (sscanf(string, "%*s %d", &buffer_argument) == 1)
            {
                free(string);
                if (getnameinfo(sender_info, *sender_info_len, buffer_ip, INET_ADDRSTRLEN, buffer_port, 6, 0) != 0)
                    return NULL;
                return create_message(EFND, buffer_argument, -1, -1, buffer_ip, buffer_port);
            }
            return NULL;
        }
        else if (strcmp(buffer_header, "EPRED") == 0)
        {
            if (sscanf(string, "%*s %d %15s %5s", &buffer_argument, buffer_ip, buffer_port) == 3)
            {
                if (!is_string_valid_ip(buffer_ip))
                    return NULL;
                if (!is_string_valid_port(buffer_port))
                    return NULL;
                free(string);
                return create_message(EPRED, -1, -1, buffer_argument, buffer_ip, buffer_port);
            }
            return NULL;
        }
        else if (strcmp(buffer_header, "ACK") == 0)
        {
            free(string);
            return (create_message(ACK, 0, 0, 0, NULL, NULL));
        }
    }
    free(string);
    return NULL;
}

message_t *create_message(const message_header header, const int key, const int message_id, const int i_key, const char i_ip[INET_ADDRSTRLEN], const char i_port[6])

{
    message_t *message = calloc(1, sizeof(message_t));
    if (!message)
        return NULL;
    if (header == ACK)
    {
        message->header = ACK;
        return message;
    }
    if (header == EFND)
    {
        message->header = EFND;
        message->key = key;
        strcpy(message->i_ip, i_ip);
        strcpy(message->i_port, i_port);
        return message;
    }
    message->header = header;
    message->key = key;
    message->message_id = message_id;
    message->i_key = i_key;
    strcpy(message->i_ip, i_ip);
    strcpy(message->i_port, i_port);
    return message;
}

message_t *copy_message(message_t *message)
{
    message_t *ret = malloc(sizeof(message_t));
    if (!ret)
        return NULL;
    return memcpy(ret, message, sizeof(message_t));
}