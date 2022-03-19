#include "data.h"
#include <assert.h>
#include <stdio.h>

int data_tests()
{
    data_t data = CREATE_DATA_STRUCT;
    STORE_KEY(data, 1);
    assert(IS_KEY_VALID(-1) == false);
    assert(IS_KEY_VALID(100) == false);
    assert(IS_KEY_VALID(0) == true);
    assert(IS_KEY_VALID(99) == true);
    assert(CHECK_KEY(data, 1) == true);
    assert(CHECK_KEY(data, 2) == false);
    REMOVE_KEY(data, 1);
    assert(CHECK_KEY(data, 1) == false);
    DESTROY_DATA_STRUCT(data);
    printf("DATA TESTS SUCCESS\n");
}

int main(int argc, char *argv[])
{
    data_tests();
    return 0;
}