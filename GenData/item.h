#ifndef ITEM_H

#define ITEM_H

typedef struct Item {
    int id;
    char name[255];
} Item;

void print_item(const Item *item);

#endif