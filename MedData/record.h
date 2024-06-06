#ifndef RECORD_H
#define RECORD_H

#include <stdbool.h>

typedef struct Record {
    char datetime[20];
    char location[50];
    double temp;
    double UV;
    int grass_pollen;
    double wind;
    double feels_like;
    double pressure;
    double rain;
} Record;

Record* parse_record(const char *line);
bool is_within_interval(const char *datetime, const char *start_time, const char *end_time);

#endif // RECORD_H
