#ifndef DATABASE_H
#define DATABASEH

#include <libpq-fe.h>

PGconn *init_db(const char *conninfo);
void close_conn(PGconn *conn);
void create_table(PGconn *conn, const char* query);
void execute_query(PGconn *conn, const char* query);

#endif