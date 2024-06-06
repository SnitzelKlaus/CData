#ifndef CSV_HANDLER_H
#define CSV_HANDLER_H

#include <libpq-fe.h>

void insert_csv_data(PGconn *conn, const char *filename);

#endif
