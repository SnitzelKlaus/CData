#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "record.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct FileSystem {
    char *filename;
    FILE *file;
} FileSystem;

FileSystem* fs_open(const char *filename);
void fs_close(FileSystem *fs);
bool fs_add_record(FileSystem *fs, const Record *record);
Record* fs_get_record(FileSystem *fs, int index);
Record* fs_query_interval(FileSystem *fs, const char *start_time, const char *end_time, int *count);
Record* fs_search_record(FileSystem *fs, const char *datetime);
bool fs_delete_record(FileSystem *fs, const char *datetime);

#endif // FILESYSTEM_H
