#ifndef WAITLIST_H
#define WAITLIST_H
#include "node_data.h"

typedef struct wait_list
{
    int message_id;
    node_data_t* associated_node;
    int is_find;
    struct wait_list* next;
}wait_list_t;


wait_list_t* add_element(wait_list_t* list, int message_id,int is_find, node_data_t* node_data);

wait_list_t* find_and_pop_element(wait_list_t* list,int message_id,int* is_list,node_data_t** node_data);

void destroy_list(wait_list_t* list);

#endif