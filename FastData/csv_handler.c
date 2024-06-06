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






//             case 2:

// int loadWeatherData(const char* filename, FILE* fp, BTreeNode** root) {
//     FILE* file = fopen(filename, "r");
//     if (!file) {
//         perror("Error opening CSV file");
//         return -1;
//     }
//
//     char line[1024];  // Adjust size as necessary
//     fgets(line, sizeof(line), file);  // Skip header line
//
//     while (fgets(line, sizeof(line), file)) {
//         WeatherData data;
//         if (sscanf(line, "%[^,],%[^,],%f,%f,%d,%f,%f,%f,%f",
//                    data.datetime, data.location, &data.temp, &data.UV,
//                    &data.grass_pollen, &data.wind, &data.feels_like,
//                    &data.pressure, &data.rain) == 9) {
//             // Debugging statement to verify data
//             printf("Loaded data: %s, %s, %.2f, %.2f, %d, %.2f, %.2f, %.2f, %.2f\n",
//                    data.datetime, data.location, data.temp, data.UV, data.grass_pollen,
//                    data.wind, data.feels_like, data.pressure, data.rain);
//
//             // Check if B-tree is initialized
//             if (*root == NULL) {
//                 *root = createNode(1, fp);
//             }
//             insert(root, data, fp);
//         } else {
//             printf("Error parsing line: %s\n", line);
//         }
//     }
//
//     fclose(file);
//     printf("Loaded weather data from %s\n", filename);
//     return 0;
// }
//
// This version uses sscanf to parse the CSV line directly, which can simplify the code.
// It also includes error handling for parsing failures.












//            case 3:

// int loadWeatherData(const char* filename, FILE* fp, BTreeNode** root) {
//     FILE* file = fopen(filename, "r");
//     if (!file) {
//         perror("Error opening CSV file");
//         return -1;
//     }
//
//     char line[1024];  // Adjust size as necessary
//     fgets(line, sizeof(line), file);  // Skip header line
//
//     while (fgets(line, sizeof(line), file)) {
//         WeatherData data;
//         char* token = strtok(line, ",");
//         if (token) {
//             strncpy(data.datetime, token, sizeof(data.datetime) - 1);
//             data.datetime[sizeof(data.datetime) - 1] = '\0';
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             strncpy(data.location, token, sizeof(data.location) - 1);
//             data.location[sizeof(data.location) - 1] = '\0';
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             data.temp = atof(token);
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             data.UV = atof(token);
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             data.grass_pollen = atoi(token);
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             data.wind = atof(token);
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             data.feels_like = atof(token);
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             data.pressure = atof(token);
//             token = strtok(NULL, ",");
//         }
//         if (token) {
//             data.rain = atof(token);
//         }
//
//         // Debugging statement to verify data
//         printf("Loaded data: %s, %s, %.2f, %.2f, %d, %.2f, %.2f, %.2f, %.2f\n",
//                data.datetime, data.location, data.temp, data.UV, data.grass_pollen,
//                data.wind, data.feels_like, data.pressure, data.rain);
//
//         // Check if B-tree is initialized
//         if (*root == NULL) {
//             *root = createNode(1, fp);
//         }
//         insert(root, data, fp);
//     }
//
//     fclose(file);
//     printf("Loaded weather data from %s\n", filename);
//     return 0;
// }
//
// This version uses strtok to tokenize the CSV line, which can be more efficient than sscanf.
// It also includes error handling for missing tokens.
