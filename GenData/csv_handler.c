#include "csv_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

void insert_csv_data(PGconn *conn, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "[ERROR]: Failed to open file\n");
        return;
    }

    // Read the header line and ignore it
    char buffer[1024];
    if (!fgets(buffer, sizeof(buffer), file)) {
        fprintf(stderr, "[ERROR]: Failed to read the header line from the CSV file\n");
        fclose(file);
        return;
    }

    // Process each subsequent line of the CSV
    int line_count = 0; // To track the number of lines processed
    while (fgets(buffer, sizeof(buffer), file)) {
        line_count++;
        char *datetime = strtok(buffer, ",");
        char *location = strtok(NULL, ",");
        char *temp = strtok(NULL, ",");
        char *uv = strtok(NULL, ",");
        char *grass_pollen = strtok(NULL, ",");
        char *wind = strtok(NULL, ",");
        char *feels_like = strtok(NULL, ",");
        char *pressure = strtok(NULL, ",");
        char *rain = strtok(NULL, "\n"); // Last field

        if (!datetime || !location || !temp || !uv || !grass_pollen || !wind || !feels_like || !pressure || !rain) {
            fprintf(stderr, "[ERROR]: Failed to parse a line from CSV file\n");
            continue;  // Skip bad line
        }

        char query[2048];
        snprintf(query, sizeof(query),
            "INSERT INTO weather_data (datetime, location, temp, uv, grass_pollen, wind, feels_like, pressure, rain) "
            "VALUES ('%s', '%s', %s, %s, %s, %s, %s, %s, %s) "
            "ON CONFLICT (datetime, location) DO UPDATE SET "
            "temp = EXCLUDED.temp, uv = EXCLUDED.uv, grass_pollen = EXCLUDED.grass_pollen, "
            "wind = EXCLUDED.wind, feels_like = EXCLUDED.feels_like, pressure = EXCLUDED.pressure, rain = EXCLUDED.rain",
            datetime, location, temp, uv, grass_pollen, wind, feels_like, pressure, rain);

        execute_query(conn, query);
    }

    fprintf(stdout, "[DONE]: Processed %d lines from the CSV file.\n", line_count);
    fclose(file);
}