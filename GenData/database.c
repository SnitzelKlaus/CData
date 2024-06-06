#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

PGconn *init_db(const char *conninfo) {
    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "[ERROR]: Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    fprintf(stdout, "[DONE]: Connected to database successfully.\n");
    return conn;
}

void close_conn(PGconn *conn) {
    PQfinish(conn);
}

void create_table(PGconn *conn, const char* query) {
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "[ERROR]: CREATE TABLE failed: %s\n", PQerrorMessage(conn));
        fflush(stderr);
        PQclear(res);
        close_conn(conn);
        exit(1);
    }

    fprintf(stdout, "[DONE]: Table created or verified successfully.\n");
    PQclear(res);
}

void execute_query(PGconn *conn, const char* query) {
    // Check connection status
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "[ERROR]: Connection to database lost: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "[ERROR]: Query execution failed: %s\n", PQerrorMessage(conn));
        fflush(stderr);
        PQclear(res);
        return;
    }
    
    PQclear(res);

    // Check connection status
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "[ERROR]: Connection to database lost: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }
}

void execute_timed_query(PGconn *conn, const char* query) {
    // Check connection status
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "[ERROR]: Connection to database lost: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    LARGE_INTEGER frequency;
    LARGE_INTEGER start, end;
    double interval;
    
    // Get the frequency of the high-resolution performance counter
    if (!QueryPerformanceFrequency(&frequency)) {
        fprintf(stderr, "[ERROR]: Failed to get the performance counter frequency\n");
        return;
    }

    // Start timer
    QueryPerformanceCounter(&start);

    PGresult *res = PQexec(conn, query);

    // Stop timer
    QueryPerformanceCounter(&end);

    // Calculate the interval in seconds
    interval = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "[ERROR]: Query execution failed: %s\n", PQerrorMessage(conn));
        fflush(stderr);
        PQclear(res);
        return;
    }

    // Print query data:
    int rows = PQntuples(res);
    int cols = PQnfields(res);

    fprintf(stdout, "Query returned %d rows and %d columns\n", rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(stdout, "%s\t", PQgetvalue(res, i, j));
        }
        fprintf(stdout, "\n");
    }

    fprintf(stdout, "\n[DONE]: Query executed successfully. Time taken: %lf seconds\n", interval);
    PQclear(res);

    // Check connection status
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "[ERROR]: Connection to database lost: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }
}