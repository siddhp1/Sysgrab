#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum {
    USERNAME,
    HOSTNAME,
    OS,
    ARCHITECTURE,
    KERNEL,
    COMPUTER,
    SHELL,
    UPTIME,
    CPU,
    MEMORY
} DataPoint;

char *get_info (DataPoint dp);

#endif