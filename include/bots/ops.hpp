#ifndef BOTS_OPS_H
#define BOTS_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <bots/struct.hpp>
#include <bots/cpu.hpp>

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
        bots::Cpu &m,
        uint8_t cycle,
        uint8_t flags,
        uint8_t ra,
        uint8_t rb,
        uint16_t imm
);

DLL_EXPORT extern bots_cpu_op bots_cpu_oplist[BOTS_CPU_OPCOUNT];

#ifdef __cplusplus
}
#endif

#endif
