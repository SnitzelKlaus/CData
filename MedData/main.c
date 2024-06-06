#include "filesystem.h"
#include "timer.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_menu() {
    printf("\nMenu:\n");
    printf("1. Load data from CSV\n");
    printf("2. Add a new record\n");
    printf("3. Query records by interval\n");
    printf("4. Search for a specific record\n");
    printf("5. Delete a specific record\n");
    printf("6. Exit\n");
    printf("Enter your choice: ");
}

void get_record_input(Record *record) {
    printf("Enter datetime (YYYY-MM-DD HH:MM:SS): ");
    scanf(" %19[^\n]", record->datetime);
    printf("Enter location: ");
    scanf(" %49[^\n]", record->location);
    printf("Enter temperature: ");
    scanf("%lf", &record->temp);
    printf("Enter UV: ");
    scanf("%lf", &record->UV);
    printf("Enter grass pollen: ");
    scanf("%d", &record->grass_pollen);
    printf("Enter wind: ");
    scanf("%lf", &record->wind);
    printf("Enter feels like: ");
    scanf("%lf", &record->feels_like);
    printf("Enter pressure: ");
    scanf("%lf", &record->pressure);
    printf("Enter rain: ");
    scanf("%lf", &record->rain);
}

void get_interval_input(char *start_time, char *end_time) {
    printf("Enter start datetime (YYYY-MM-DD HH:MM:SS): ");
    scanf(" %19[^\n]", start_time);
    printf("Enter end datetime (YYYY-MM-DD HH:MM:SS): ");
    scanf(" %19[^\n]", end_time);
}

int main() {
    FileSystem *fs = fs_open("dat.dat");
    int choice;
    char datetime[20], start_time[20], end_time[20];

    while (1) {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char csv_filename[100];
                printf("Enter CSV filename: ");
                scanf(" %99[^\n]", csv_filename);

                Timer timer;
                timer_start(&timer);
                load_csv_to_fs(csv_filename, fs);
                timer_end(&timer);

                printf("Data loaded in %f seconds\n", timer_elapsed(&timer));
                break;
            }
            case 2: {
                Record record;
                get_record_input(&record);

                if (fs_add_record(fs, &record)) {
                    printf("Record added successfully.\n");
                } else {
                    printf("Failed to add record.\n");
                }
                break;
            }
            case 3: {
                get_interval_input(start_time, end_time);

                int count;

                Timer timer;
                timer_start(&timer);
                Record *results = fs_query_interval(fs, start_time, end_time, &count);
                timer_end(&timer);

                printf("Found %d records in the interval.\n", count);

                for (int i = 0; i < count; i++) {
                    printf("Record %d: %s, %s, %.2f, %.2f, %d, %.2f, %.2f, %.2f, %.2f\n",
                           i, results[i].datetime, results[i].location, results[i].temp, results[i].UV,
                           results[i].grass_pollen, results[i].wind, results[i].feels_like, results[i].pressure,
                           results[i].rain);
                }

                printf("Query executed in %f seconds\n", timer_elapsed(&timer));

                free(results);
                break;
            }
            case 4: {
                printf("Enter datetime (YYYY-MM-DD HH:MM:SS) to search: ");
                scanf(" %19[^\n]", datetime);

                Timer timer;
                timer_start(&timer);

                Record *record = fs_search_record(fs, datetime);

                timer_end(&timer);
                printf("Search executed in %f seconds\n", timer_elapsed(&timer));

                if (record) {
                    printf("Record found: %s, %s, %.2f, %.2f, %d, %.2f, %.2f, %.2f, %.2f\n",
                           record->datetime, record->location, record->temp, record->UV,
                           record->grass_pollen, record->wind, record->feels_like, record->pressure,
                           record->rain);
                    free(record);
                } else {
                    printf("Record not found.\n");
                }
                break;
            }
            case 5: {
                printf("Enter datetime (YYYY-MM-DD HH:MM:SS) to delete: ");
                scanf(" %19[^\n]", datetime);

                Timer timer;
                timer_start(&timer);

                bool deleted = fs_delete_record(fs, datetime);

                timer_end(&timer);
                printf("Delete executed in %f seconds\n", timer_elapsed(&timer));

                if (deleted) {
                    printf("Record deleted successfully.\n");
                } else {
                    printf("Record not found or failed to delete.\n");
                }
                break;
            }
            case 6:
                fs_close(fs);
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

// gcc -o xFileManager main.c filesystem.c record.c timer.c utils.c