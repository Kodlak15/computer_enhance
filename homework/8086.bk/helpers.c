#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

FileContents read_file(const char *path) {
    FileContents result = {NULL, 0};

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file %s\n", path);
        return result;
    }

    // Seek to the end of the file
    fseek(file, 0, SEEK_END);
    // Aske where we are
    long size = ftell(file);
    // Rewind back to the start of the file
    fseek(file, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "Couldn't find the size of file %s\n", path);
        fclose(file);
        return result;
    }

    uint8_t *buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Out of memory for file %s\n", path);
        fclose(file);
        return result;
    }

    size_t read_count = fread(buffer, 1, (size_t)size, file);
    fclose(file);

    if (read_count != (size_t)size) {
        fprintf(stderr, "Short read on %s\n", path);
        free(buffer);
        return result;
    }

    result.bytes = buffer;
    result.size = (size_t)size;
    return result;
}
