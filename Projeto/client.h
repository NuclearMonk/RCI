#include <stdio.h>
#include <arpa/inet.h>

typedef enum {c_new,c_bentry,c_pentry,c_chord,c_echord,c_show,c_find,c_leave,c_exit}console_commands;

typedef struct console_command
{
   console_commands command;
   int argument;
   char ip[INET_ADDRSTRLEN];
   char port[6];
}console_command_t;


console_command_t* read_console_command(int fd);