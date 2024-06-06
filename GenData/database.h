#ifndef DATABASE_H

#define DATABASE_H

#include "generic_structs.h"
#include "item.h"
#include <libpq-fe.h>

PGconn *init_db();
void close_conn(PGconn *conn);
void create_table(PGconn *conn, TableDescriptor *table);
void insert_item(PGconn *conn, const Item *item);
void query_items(PGconn *conn);

#endif