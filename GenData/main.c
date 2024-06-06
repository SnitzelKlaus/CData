#include "database.h"
#include "models.h"
#include "csv_handler.h"
#include <stdio.h>
#include <windows.h>

#define SLEEP_TIME 20

int main() {
    printf("Starting program...\n");
    const char *conninfo = "dbname=GenData user=postgres password=Hoffstein hostaddr=127.0.0.1 port=5432";
    PGconn *conn = init_db(conninfo);

    printf("\n[DB]\n");
    if (conn == NULL){
        fprintf(stderr, "[ERROR]: Failed to connect to the database\n");
        Sleep(SLEEP_TIME);
        return 1;
    }

    printf("Connected to database.\n");
    Sleep(SLEEP_TIME);

    create_table(conn, WEATHER_TABLE_CREATION);

    // Inserts data from the CSV file
    printf("\n[CSV]\n");
    insert_csv_data(conn, "../weather_data.csv");
    printf("Data inserted.\n");
    Sleep(SLEEP_TIME);

    // Query the database
    printf("\n[Query]\n");
    printf("Querying all data...\n");
    execute_timed_query(conn, "SELECT * FROM weather_data LIMIT 10");
    Sleep(SLEEP_TIME);

    // Search for data in the database
    printf("\n[Query]\n");
    printf("Querying specific location...\n");
    execute_timed_query(conn, "SELECT * FROM weather_data WHERE location = 'Location 1' LIMIT 10");
    Sleep(SLEEP_TIME);

    close_conn(conn);

    system("pause");
    return 0;
}


// gcc -I"C:/Frameworks/PostgreSQL/16/include" -L"C:/Frameworks/PostgreSQL/16/lib" main.c database.c csv_handler.c -o GenData -lpq