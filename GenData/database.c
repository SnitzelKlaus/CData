#include "database.h"
#include "item.h"
#include "generic_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void close_conn(PGconn *conn) {
    PQfinish(conn);
}

// cd "C:\Code\CData\GenData\Database" && gcc -mconsole -I"C:/Frameworks/Postgresql/16/include" -L"C:/Frameworks/PostgreSQL/16/lib" ../main.c database.c ../Items/item.c -o database -lpq
// gcc -I"C:/Frameworks/PostgreSQL/16/include" -L"C:/Frameworks/PostgreSQL/16/lib" main.c -o main -lpq -mconsole

// WORKS!
// gcc -I"C:/Frameworks/PostgreSQL/16/include" -L"C:/Frameworks/PostgreSQL/16/lib" main.c database.c item.c -o main -lpq -mconsole

PGconn *init_db() {
    const char *conninfo = "dbname=GenData user=postgres password=Hoffstein hostaddr=127.0.0.1 port=5432";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        close_conn(conn);
    } else {
        fprintf(stderr, "Connected to database successfully.\n");
    }
    return conn;
}

// void create_table(PGconn *conn) {
//     PGresult *res = PQexec(conn, "CREATE TABLE IF NOT EXISTS items(id SERIAL PRIMARY KEY, name VARCHAR(255))");
//     if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//         fprintf(stderr, "CREATE TABLE failed: %s\n", PQerrorMessage(conn));
//         PQclear(res);
//         close_conn(conn);
//     } else {
//         fprintf(stderr, "Table created or verified successfully.\n");
//         PQclear(res);
//     }
// }

void create_table(PGconn *conn, TableDescriptor *table) {
    char query[1024] = "CREATE TABLE IF NOT EXISTS ";
    strcat(query, table->table_name);
    strcat(query, " (");
    for (int i = 0; i < table->num_fields; i++) {
        strcat(query, table->fields[i].field_name);
        strcat(query, " ");
        strcat(query, table->fields[i].data_type);
        if (i < table->num_fields - 1) {
            strcat(query, ", ");
        }
    }
    strcat(query, ")");
    
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "CREATE TABLE failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        close_conn(conn);
    } else {
        fprintf(stderr, "Table created or verified successfully.\n");
        PQclear(res);
    }
}

void insert_item(PGconn *conn, const Item *item) {
    char query[1024];
    snprintf(query, 1024, "INSERT INTO items (name, quantity) VALUES ('%s', %d)", item->name);
    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "INSERT INTO failed: %s", PQerrorMessage(conn));
        PQclear(res);
        close_conn(conn);
    }
    PQclear(res);
}

void query_items(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT id, name, quantity FROM items");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        close_conn(conn);
    }

    for (int i = 0; i < PQntuples(res); i++) {
        Item item;
        item.id = atoi(PQgetvalue(res, i, 0));
        strncpy(item.name, PQgetvalue(res, i, 1), sizeof(item.name) - 1);  // Safely copy the string
        item.name[sizeof(item.name) - 1] = '\0';
        print_item(&item);
    }
    PQclear(res);
}
