#include "record.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

Record* parse_record(const char *line) {
    Record *record = (Record*)malloc(sizeof(Record));
    sscanf(line, "%19[^,],%49[^,],%lf,%lf,%d,%lf,%lf,%lf,%lf",
           record->datetime, record->location, &record->temp, &record->UV, &record->grass_pollen,
           &record->wind, &record->feels_like, &record->pressure, &record->rain);
    return record;
}

bool is_within_interval(const char *datetime, const char *start_time, const char *end_time) {
    struct tm tm_start, tm_end, tm_dt;

    // Replace strptime with sscanf and strftime
    sscanf(start_time, "%4d-%2d-%2d %2d:%2d:%2d",
           &tm_start.tm_year, &tm_start.tm_mon, &tm_start.tm_mday,
           &tm_start.tm_hour, &tm_start.tm_min, &tm_start.tm_sec);
    tm_start.tm_year -= 1900;
    tm_start.tm_mon -= 1;

    sscanf(end_time, "%4d-%2d-%2d %2d:%2d:%2d",
           &tm_end.tm_year, &tm_end.tm_mon, &tm_end.tm_mday,
           &tm_end.tm_hour, &tm_end.tm_min, &tm_end.tm_sec);
    tm_end.tm_year -= 1900;
    tm_end.tm_mon -= 1;

    sscanf(datetime, "%4d-%2d-%2d %2d:%2d:%2d",
           &tm_dt.tm_year, &tm_dt.tm_mon, &tm_dt.tm_mday,
           &tm_dt.tm_hour, &tm_dt.tm_min, &tm_dt.tm_sec);
    tm_dt.tm_year -= 1900;
    tm_dt.tm_mon -= 1;

    time_t start = mktime(&tm_start);
    time_t end = mktime(&tm_end);
    time_t dt = mktime(&tm_dt);

    return dt >= start && dt <= end;
}
