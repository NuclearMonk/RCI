#ifndef WAITLIST_H
#define WAITLIST_H
#include "node_data.h"
/**
 * @brief the waitlist stores data relating to the messages that we are awayting responses for, it does so by using a single linked list and searching it by message id
 * 
 */
typedef struct wait_list
{
    int message_id;
    node_data_t* associated_node;
    int is_find;
    struct wait_list* next;
}wait_list_t;

/**
 * @brief adds a message to the list of messages we are awayting responses to
 * 
 * @param list 
 * @param message_id 
 * @param is_find 
 * @param node_data 
 * @return wait_list_t* the new head of the list
 */
wait_list_t* add_element(wait_list_t* list, int message_id,int is_find, node_data_t* node_data);


/**
 * @brief finds and pops the element of the list that has the correct message_id it returns the new head of the list and sets is_find and nodedata to the correct data
 * 
 * @param list 
 * @param message_id 
 * @param is_find
 * @param node_data 
 * @return wait_list_t* 
 */
wait_list_t* find_and_pop_element(wait_list_t* list,int message_id,int* is_find,node_data_t** node_data);

/**
 * @brief destroys the wait list and all messages inside it
 * 
 * @param list 
 */
void destroy_list(wait_list_t* list);

#endif