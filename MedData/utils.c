#include "utils.h"
#include "record.h"
#include "filesystem.h"
#include <stdlib.h>
#include <string.h>

void load_csv_to_fs(const char *filename, FileSystem *fs) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        Record *record = parse_record(buffer);
        fs_add_record(fs, record);
        free(record);
    }
    fclose(file);
}
