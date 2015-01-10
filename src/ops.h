#ifndef OPS_H
#define OPS_H

#include <stdint.h>

#include "struct.h"

#define OPCOUNT 68

#define DLL_EXPORT
#ifdef _WIN32
#ifdef BUILD_SHARED
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#endif

typedef struct {
    uint8_t size;
    uint8_t argcount;
    uint8_t arg_sizes[3];
    void (*execute)(machine*);
} opdata;

DLL_EXPORT opdata oplist[OPCOUNT];

#endif
