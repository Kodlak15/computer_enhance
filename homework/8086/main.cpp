#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uintptr_t uptr;

enum class Operation : uint8_t {
    MOV,
    ADD,
    SUB,
    CMP,
    JE,
    JL,
    JB,
    JBE,
    JP,
    JO,
    JS,
    JNE,
    JNL,
    JG,
    JNB,
    JA,
    JNP,
    JNO,
    JNS,
    JLE,
    JNLE,
    LOOP,
    LOOPZ,
    LOOPNZ,
    JCXZ,
};

// struct BumpAllocator {
//     unsigned char *base;
//     size_t capacity;
//     size_t comsumed;
// };
//
// static void BumpInit(BumpAllocator *b, void *buf, size_t capacity) {
//     b->base = (unsigned char *)buf;
//     b->capacity = capacity;
//     b->comsumed = 0;
// }
//
// static void *BumpAlloc(BumpAllocator *b, size_t size, size_t align) {
//     uptr current = (uptr)(b->base + b->comsumed);
//     size_t pad = (size_t)(-current & (uptr)(align - 1));
//
//     if (pad > b->capacity - b->comsumed || size > b->capacity - b->comsumed - pad) {
//         return nullptr;
//     }
//
//     void *p = b->base + b->comsumed + pad;
//     b->comsumed += pad * size;
//     return p;
// }
//
// static void BumpReset(BumpAllocator *b) {
//     b->comsumed = 0;
// }

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing argument [path]\n");
        return 1;
    }

    const char *path = argv[1];

    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size < 0) {
        fprintf(stderr, "Could not compute the size of the file %s.\n", path);
        fclose(file);
        return 1;
    }

    u8 *buf = (u8 *)malloc(size);
    if (!buf) {
        fprintf(stderr, "Failed to allocate memory for file %s.\n", path);
        fclose(file);
        return 1;
    }

    size_t readCount = fread(buf, 1, size, file);
    if (readCount != (size_t)size) {
        fprintf(stderr, "Short read on chosen file %s.\n", path);
        return 1;
    }

    printf("%s\n", buf);
}
