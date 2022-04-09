#ifndef MESSAGES_H
#define MESSAGES_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define MESSAGE_BUFFER_SIZE 128

typedef enum
{
    PRED,
    SELF,
    FND,
    RSP,
    EFND,
    EPRED,
    ACK
} message_header;

typedef struct message
{
    message_header header;
    int key;
    int message_id;
    int i_key;
    char i_ip[INET_ADDRSTRLEN];
    char i_port[6];
} message_t;

char *message_to_string(const message_t *message);

message_t *string_to_message(char *string, struct sockaddr* sender_info, socklen_t* sender_info_len);

message_t *create_message(const message_header header,const int key,const int message_id,const int i_key,const char i_ip[INET_ADDRSTRLEN], const char i_port[6]);

message_t *copy_message(message_t* message);
#endif