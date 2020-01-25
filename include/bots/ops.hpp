#pragma once

#include <cstdint>

#include <bots/cpu.hpp>

#define BOTS_CPU_OPCOUNT 16

typedef uint8_t (*bots_cpu_op)(
        bots::Cpu &m,
        uint8_t cycle,
        uint8_t flags,
        uint8_t ra,
        uint8_t rb,
        uint16_t imm
);

extern bots_cpu_op bots_cpu_oplist[BOTS_CPU_OPCOUNT];
