#include "database.h"
#include "item.h"
#include "generic_structs.h"
#include<stdio.h>

int main()
{
    PGconn *conn = init_db();

    FieldDescriptor item_fields[] = {
        {"id", "SERIAL PRIMARY KEY"},
        {"datetime", "TIMESTAMP"},
        {"location", "VARCHAR(255)"},
        {"temp", "VARCHAR(255)"},
    };

    TableDescriptor item_table = {
        "tesie",
        item_fields,
        2,
    };
    
    create_table(conn, &item_table);

    // Item item = {
    //     .id = 1,
    //     .name = "Item 1",
    // };

    // insert_item(conn, &item);

    query_items(conn);

    close_conn(conn);
    return 0;
}