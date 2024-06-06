// csv_handler.c
#include "csv_handler.h"
#include <stdlib.h>
#include <string.h>

int loadWeatherData(const char* filename, FILE* fp, BTreeNode** root) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening CSV file");
        return -1;
    }

    char line[1024];  // Adjust size as necessary
    fgets(line, sizeof(line), file);  // Skip header line

    while (fgets(line, sizeof(line), file)) {
        WeatherData data;
        char* token;
        int field = 0;

        token = strtok(line, ",");
        while (token != NULL) {
            switch (field) {
                case 0: strncpy(data.datetime, token, sizeof(data.datetime) - 1); data.datetime[sizeof(data.datetime) - 1] = '\0'; break;
                case 1: strncpy(data.location, token, sizeof(data.location) - 1); data.location[sizeof(data.location) - 1] = '\0'; break;
                case 2: data.temp = atof(token); break;
                case 3: data.UV = atof(token); break;
                case 4: data.grass_pollen = atoi(token); break;
                case 5: data.wind = atof(token); break;
                case 6: data.feels_like = atof(token); break;
                case 7: data.pressure = atof(token); break;
                case 8: data.rain = atof(token); break;
                default: break;  // Handle unexpected extra data
            }
            token = strtok(NULL, ",");
            field++;
        }

        // Debugging statement to verify data
        printf("Loaded data: %s, %s, %.2f, %.2f, %d, %.2f, %.2f, %.2f, %.2f\n",
               data.datetime, data.location, data.temp, data.UV, data.grass_pollen,
               data.wind, data.feels_like, data.pressure, data.rain);

        // Check if B-tree is initialized
        if (*root == NULL) {
            *root = createNode(1, fp);
        }
        insert(root, data, fp);
    }

    fclose(file);
    printf("Loaded weather data from %s\n", filename);
    return 0;
}
