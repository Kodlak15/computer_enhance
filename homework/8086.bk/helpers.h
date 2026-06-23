#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t *bytes;
    size_t size;
} FileContents;

FileContents read_file(const char *path);
