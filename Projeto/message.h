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

/**
 * @brief struct that olds all that was contained in a message so we aren't passing around unsafe strings inside out code
 *
 */
typedef struct message
{
    message_header header;
    int key;
    int message_id;
    int i_key;
    char i_ip[INET_ADDRSTRLEN];
    char i_port[6];
} message_t;

/**
 * @brief convers a message to it's string equvalent
 *
 * @param message
 * @return char*
 */
char *message_to_string(const message_t *message);

/**
 * @brief converts a string to a message, if the message is of type EPRED it encodes the sender information on unused fields in the message struct
 *
 * @param string
 * @param sender_info
 * @return message_t*
 */
message_t *string_to_message(char *string, struct sockaddr *sender_info);

/**
 * @brief allocates a message and fills it's fields. unused fields should be set to -1 as 0 is a valid value for the majority of them
 *
 * @param header
 * @param key
 * @param message_id
 * @param i_key
 * @param i_ip
 * @param i_port
 * @return message_t*
 */
message_t *create_message(const message_header header, const int key, const int message_id, const int i_key, const char i_ip[INET_ADDRSTRLEN], const char i_port[6]);

/**
 * @brief duplicates a message to a second allocate location, this is use to keep a copy of a message after sending it to a function that has the side effect of freeing the message
 *
 * @param message
 * @return message_t*
 */
message_t *copy_message(message_t *message);
#endif