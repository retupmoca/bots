#ifndef BOTS_OPS_H
#define BOTS_OPS_H

#include <stdint.h>

#include <bots/struct.h>

#define BOTS_CPU_OPCOUNT 16

#ifndef DLL_EXPORT
# ifdef _WIN32
#  ifdef BOTS_BUILD_SHARED
#   define DLL_EXPORT __declspec(dllexport)
#  else
#   define DLL_EXPORT __declspec(dllimport)
#  endif
# else
#  define DLL_EXPORT
# endif
#endif

typedef uint8_t (*bots_cpu_op)(
        bots_cpu* m,
        uint8_t cycle,
        uint8_t flags,
        uint8_t ra,
        uint8_t rb,
        uint16_t imm
);

DLL_EXPORT extern bots_cpu_op bots_cpu_oplist[BOTS_CPU_OPCOUNT];

#endif
