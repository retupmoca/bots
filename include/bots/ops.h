#ifndef BOTS_OPS_H
#define BOTS_OPS_H

#include <stdint.h>

#include <bots/struct.h>

#define BOTS_CPU_OPCOUNT 68

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
    void (*execute)(bots_cpu*);
} bots_cpu_opdata;

DLL_EXPORT bots_cpu_opdata bots_cpu_oplist[BOTS_CPU_OPCOUNT];

#endif
