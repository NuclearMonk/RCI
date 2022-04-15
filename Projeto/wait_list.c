#include "wait_list.h"
#include <stdlib.h>

wait_list_t *add_element(wait_list_t *list, int message_id, int is_find, node_data_t *node_data)
{
    wait_list_t *new_element = malloc(sizeof(wait_list_t));
    new_element->message_id = message_id;
    new_element->associated_node = node_data;
    new_element->is_find = is_find;
    new_element->next = list;
    return new_element;
}

wait_list_t *find_and_pop_element(wait_list_t *list, int message_id, int *is_find, node_data_t **node_data)
{
    wait_list_t *next, *previous = NULL;
    for (wait_list_t *element = list; element != NULL; previous = element, element = next)
    {
        next = element->next;
        if (element->message_id == message_id)
        {

            *node_data = element->associated_node;
            *is_find = element->is_find;
            free(element);
            if (!previous)
            {
                return next;
            }
            else
            {
                previous->next = next;
                return list;
            }
        }
    }
    return list;
}

void destroy_list(wait_list_t *list)
{
    if (!list)
        return;
    wait_list_t *next;
    for (wait_list_t *element = list; element != NULL; element = next)
    {
        next = element->next;
        destroy_node_data(element->associated_node);
        free(element);
    }
}