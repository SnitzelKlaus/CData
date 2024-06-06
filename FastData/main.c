#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "csv_handler.h"

void printWeatherData(const WeatherData* wd) {
    printf("Datetime: %s, Location: %s, Temp: %.2f, UV: %.2f, Grass Pollen: %d, Wind: %.2f, Feels Like: %.2f, Pressure: %.2f, Rain: %.2f\n",
           wd->datetime, wd->location, wd->temp, wd->UV, wd->grass_pollen, wd->wind, wd->feels_like, wd->pressure, wd->rain);
}

void traverseAndPrint(BTreeNode* root, FILE* fp) {
    if (root != NULL) { // Base case: If the node is NULL, do nothing
        printf("Traversing node at offset %ld with %d keys\n", root->self_offset, root->n_keys);
        for (int i = 0; i < root->n_keys; i++) {
            // Load and traverse the child node before printing the current node's key
            BTreeNode* child = loadNode(fp, root->children_offsets[i]);
            if (child != NULL) {
                printf("Traversing child at index %d of node at offset %ld\n", i, root->self_offset);
                traverseAndPrint(child, fp); // Recursive call for the child
                free(child); // Free the memory after use
            }
            printWeatherData(&root->keys[i]); // Print the current key after traversing the left child
        }
        // Traverse the last child after printing all keys
        BTreeNode* lastChild = loadNode(fp, root->children_offsets[root->n_keys]);
        if (lastChild != NULL) {
            printf("Traversing last child of node at offset %ld\n", root->self_offset);
            traverseAndPrint(lastChild, fp); // Recursive call for the last child
            free(lastChild); // Free the memory after use
        }
    }
}


void searchAndPrint(BTreeNode* root, char* datetime, FILE* fp) {
    WeatherData* result = search(root, datetime, fp);
    if (result != NULL) {
        printWeatherData(result);
    } else {
        printf("No weather data found for %s.\n", datetime);
    }
}

int main() {
    BTreeNode* root = NULL;
    const char* db_filename = "weather_data.db";
    FILE* db_file = fopen(db_filename, "r+b");  // Open the file for update

    if (!db_file) {  // Create new file if it does not exist
        db_file = fopen(db_filename, "w+b");
    }

    if (!db_file) {
        printf("Failed to open database file.\n");
        return 1;
    }

    char input[256];
    int running = 1;
    while (running) {
        printf("\nMenu:\n");
        printf("1. Load weather data from CSV\n");
        printf("2. Search weather data by datetime\n");
        printf("3. Display all weather data\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);

        switch (atoi(input)) {
            case 1:
                printf("Enter CSV filename: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;  // Remove newline character
                if (loadWeatherData(input, db_file, &root) != 0) {
                    printf("Failed to load data from %s.\n", input);
                } else {
                    printf("Data loaded successfully from %s.\n", input);
                }
                break;
            case 2:
                printf("Enter datetime (YYYY-MM-DD HH:MM:SS): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                searchAndPrint(root, input, db_file);
                break;
            case 3:
                traverseAndPrint(root, db_file);
                break;
            case 4:
                running = 0;
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    fclose(db_file);  // Close the database file properly
    printf("Database file closed.\n");
    return 0;
}
