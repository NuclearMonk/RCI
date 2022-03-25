#ifndef CLIENT_H
#define CLIENT_H
#include <stdio.h>
#include <arpa/inet.h>
#include <stdbool.h>

/* Enum of valid commands */
typedef enum
{
   c_new,
   c_bentry,
   c_pentry,
   c_chord,
   c_echord,
   c_show,
   c_find,
   c_leave,
   c_exit
} console_commands;

typedef struct console_command
{
   console_commands command;
   int argument;
   char ip[INET_ADDRSTRLEN];
   char port[6];
} console_command_t;

/**
 * @brief Reads a user command from the file descriptor fd, and returns a command struct with the necessary data to accomplish the command
 *
 * @param fd
 * @return console_command_t* a command with only the needed data filled, NULL in case of failure
 */
console_command_t *read_console_command(int fd);

/**
 * @brief Takes in a string and evaluates wether or not it is a valid IPv4 address
 *
 * @return bool wether or not the arg is convertible to a valid ip address
 */
bool is_string_valid_ip(const char *);

#endif