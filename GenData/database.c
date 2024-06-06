#include "database.h"
#include <stdio.h>
#include <stdlib.h>

PGconn *init_db(const char *conninfo) {
    PGconn *conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    fprintf(stdout, "Connected to database successfully.\n");
    return conn;
}

void close_conn(PGconn *conn) {
    PQfinish(conn);
}

void create_table(PGconn *conn, const char* query) {
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "CREATE TABLE failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        close_conn(conn);
        exit(1);
    }
    fprintf(stdout, "Table created or verified successfully.\n");
    PQclear(res);
}

void execute_query(PGconn *conn, const char* query) {
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        close_conn(conn);
        exit(1);
    }
    PQclear(res);
}