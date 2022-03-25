#ifndef DATA_H
#define DATA_H

#include <stdbool.h>
#include <stdlib.h>

typedef bool *data_t;

#define MIN_KEY 0
#define MAX_KEY 99

#define CREATE_DATA_STRUCT ((data_t)calloc(MAX_KEY + 1, sizeof(bool)));
#define DESTROY_DATA_STRUCT(DATA) free(DATA);

#define IS_KEY_VALID(KEY) (MIN_KEY <= KEY && KEY <= MAX_KEY)
#define STORE_KEY(DATA, KEY) DATA[KEY] = true;
#define REMOVE_KEY(DATA, KEY) DATA[KEY] = false;
#define CHECK_KEY(DATA, KEY) DATA[KEY]

#endif