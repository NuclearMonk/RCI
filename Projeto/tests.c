#include "wait_list.h"
#include <assert.h>
#include <stdio.h>

void data_tests()
{
    wait_list_t* lista = NULL;
    assert(lista==NULL);
    node_data_t* node_data = NULL;
    lista = add_element(lista,0,node_data);
    assert(lista->message_id == 0);
    assert(lista->associated_node == NULL);
    lista = find_and_pop_element(lista,0,&node_data);
    assert(node_data == NULL);
    destroy_list(lista);
}

int main(int argc, char *argv[])
{
    data_tests();
    return 0;
}