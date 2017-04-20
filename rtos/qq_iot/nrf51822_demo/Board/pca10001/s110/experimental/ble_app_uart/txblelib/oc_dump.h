#ifndef OC_DUMP
#define OC_DUMP
#include "ff_syshdr.h"
#include <stdio.h>

#define PRINTF(...) printf(__VA_ARGS__)
#define DUMP(x, y) stdio_dump((x), (y)), putchar('\r\n');

void stdio_dump(const uint8_t *buf, size_t len);

#endif