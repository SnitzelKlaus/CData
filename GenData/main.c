#include "database.h"
#include "models.h"
#include "csv_handler.h"

int main() {
    const char *conninfo = "dbname=GenData user=postgres password=Hoffstein hostaddr=127.0.0.1 port=5432";
    PGconn *conn = init_db(conninfo);

    if (conn == NULL) return 1;

    create_table(conn, WEATHER_TABLE_CREATION);

    // Inserts data from the CSV file
    insert_csv_data(conn, "../weather_data.csv");

    close_conn(conn);
    return 0;
}


// gcc -I"C:/Frameworks/PostgreSQL/16/include" -L"C:/Frameworks/PostgreSQL/16/lib" main.c database.c csv_handler.c -o main -lpq