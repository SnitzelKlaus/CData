#include "filesystem.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

FileSystem* fs_open(const char *filename) {
    FileSystem *fs = (FileSystem*)malloc(sizeof(FileSystem));
    fs->filename = strdup(filename);
    fs->file = fopen(filename, "a+");
    return fs;
}

void fs_close(FileSystem *fs) {
    if (fs->file) {
        fclose(fs->file);
    }
    free(fs->filename);
    free(fs);
}

bool fs_add_record(FileSystem *fs, const Record *record) {
    if (!fs->file) return false;

    fprintf(fs->file, "%s,%s,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%.2f\n",
            record->datetime, record->location, record->temp, record->UV, record->grass_pollen,
            record->wind, record->feels_like, record->pressure, record->rain);
    return true;
}

Record* fs_get_record(FileSystem *fs, int index) {
    if (!fs->file) return NULL;

    fseek(fs->file, 0, SEEK_SET);
    char buffer[256];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), fs->file)) {
        if (count == index) {
            return parse_record(buffer);
        }
        count++;
    }
    return NULL;
}

Record* fs_query_interval(FileSystem *fs, const char *start_time, const char *end_time, int *count) {
    if (!fs->file) return NULL;

    fseek(fs->file, 0, SEEK_SET);
    char buffer[256];
    int record_count = 0;
    Record *results = NULL;

    while (fgets(buffer, sizeof(buffer), fs->file)) {
        Record *record = parse_record(buffer);
        if (is_within_interval(record->datetime, start_time, end_time)) {
            results = (Record*)realloc(results, sizeof(Record) * (record_count + 1));
            results[record_count] = *record;
            record_count++;
        }
        free(record);
    }
    *count = record_count;
    return results;
}

Record* fs_search_record(FileSystem *fs, const char *datetime) {
    if (!fs->file) return NULL;

    fseek(fs->file, 0, SEEK_SET);
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fs->file)) {
        Record *record = parse_record(buffer);
        if (strcmp(record->datetime, datetime) == 0) {
            return record;
        }
        free(record);
    }
    return NULL;
}

bool fs_delete_record(FileSystem *fs, const char *datetime) {
    if (!fs->file) return false;

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) return false;

    fseek(fs->file, 0, SEEK_SET);
    char buffer[256];
    bool deleted = false;

    while (fgets(buffer, sizeof(buffer), fs->file)) {
        Record *record = parse_record(buffer);
        if (strcmp(record->datetime, datetime) != 0) {
            fprintf(temp_file, "%s,%s,%.2f,%.2f,%d,%.2f,%.2f,%.2f,%.2f\n",
                    record->datetime, record->location, record->temp, record->UV, record->grass_pollen,
                    record->wind, record->feels_like, record->pressure, record->rain);
        } else {
            deleted = true;
        }
        free(record);
    }

    fclose(fs->file);
    fclose(temp_file);

    remove(fs->filename);
    rename("temp.txt", fs->filename);

    fs->file = fopen(fs->filename, "a+");
    return deleted;
}
