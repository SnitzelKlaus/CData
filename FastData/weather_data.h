// weather_data.h
#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

#pragma pack(push, 1)
typedef struct {
    char datetime[20];
    char location[100];
    double temp;
    double UV;
    int grass_pollen;
    double wind;
    double feels_like;
    double pressure;
    double rain;
} WeatherData;
#pragma pack(pop)

#endif

