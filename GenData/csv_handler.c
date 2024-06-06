#include "csv_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

void insert_csv_data(PGconn *conn, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file\n");
        return;
    }

    // Read the header line and ignore it
    char buffer[1024];
    if (!fgets(buffer, sizeof(buffer), file)) {
        fprintf(stderr, "Failed to read the header line from the CSV file\n");
        fclose(file);
        return;
    }

    // Process each subsequent line of the CSV
    while (fgets(buffer, sizeof(buffer), file)) {
        char *datetime = strtok(buffer, ",");
        char *location = strtok(NULL, ",");
        char *temp = strtok(NULL, ",");
        char *uv = strtok(NULL, ",");
        char *grass_pollen = strtok(NULL, ",");
        char *wind = strtok(NULL, ",");
        char *feels_like = strtok(NULL, ",");
        char *pressure = strtok(NULL, ",");
        char *rain = strtok(NULL, "\n"); // Remove newline at the end

        if (!datetime || !location || !temp || !uv || !grass_pollen || !wind || !feels_like || !pressure || !rain) {
            fprintf(stderr, "Failed to parse a line from CSV file\n");
            continue;  // Skip bad lines
        }

        char query[2048];  // Ensure the query buffer is large enough for the query
        snprintf(query, sizeof(query),
                 "INSERT INTO weather_data (datetime, location, temp, uv, grass_pollen, wind, feels_like, pressure, rain) "
                 "VALUES ('%s', '%s', %s, %s, %s, %s, %s, %s, %s)",
                 datetime, location, temp, uv, grass_pollen, wind, feels_like, pressure, rain);

        execute_query(conn, query);
    }

    fclose(file);
}