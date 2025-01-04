#ifndef SYSGRAB_DATA_H
#define SYSGRAB_DATA_H

#include <stdbool.h>
#include <stddef.h>

typedef struct DataPoint {
  char *key;
  char *value;
} DataPoint;

typedef struct Data {
  size_t length;
  DataPoint **datapoints;
} Data;

DataPoint *get_datapoint(const char *data_point);

Data *create_data(void);
bool add_datapoint(Data *data, DataPoint *datapoint);
void free_datapoint(DataPoint *datapoint);
void free_data(Data *data);

#endif
