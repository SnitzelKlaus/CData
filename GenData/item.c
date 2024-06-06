#include "item.h"
#include <stdio.h>

void print_item(const Item *item)
{
    printf("Item %d: %s\n", item->id, item->name);
}