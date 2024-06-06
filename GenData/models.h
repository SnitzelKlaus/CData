#ifndef MODELS_H
#define MODELS_H

#define WEATHER_TABLE_CREATION "CREATE TABLE IF NOT EXISTS weather_data (" \
                               "datetime TIMESTAMP, " \
                               "location VARCHAR(255), " \
                               "temp REAL, " \
                               "uv REAL, " \
                               "grass_pollen INT, " \
                               "wind REAL, " \
                               "feels_like REAL, " \
                               "pressure REAL, " \
                               "rain REAL, " \
                               "PRIMARY KEY(datetime, location))"

#endif