#include "client.h"
#include <stdlib.h>
#include "sys/select.h"
#include "server.h"
#include <arpa/inet.h>

void run_ring()
{
    fd_set set, temp_set;
    FD_ZERO(&set);
    FD_SET(0,&set);
    console_command_t* command;
    server_t* server = NULL;
    while (1)
    {   
        temp_set = set;
        int count = select(1,&temp_set,NULL,NULL,NULL);
        while(count>0)
        {
            if(FD_ISSET(0,&temp_set))
            {
                command = read_console_command(0);
                if(!command)continue;
                switch (command->command)
                {
                case c_new:
                    server = create_server();
                    break;
                case c_pentry:
                    printf("PENTRY %d %u %d", command->argument,ntohl(command->ip), command->port);
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
            count--;
        }
    }
}



int main(int argc, char *argv[])
{
    run_ring();
    return 0;
}