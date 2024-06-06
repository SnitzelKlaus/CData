// csv_handler.h
#ifndef CSV_HANDLER_H
#define CSV_HANDLER_H

#include "btree.h"
#include <stdio.h>

int loadWeatherData(const char* filename, FILE* fp, BTreeNode** root);

#endif
