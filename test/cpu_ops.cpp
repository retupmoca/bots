#include <criterion/criterion.h>

#include <bots/cpu.hpp>
#include <bots/ops.hpp>

Test(cpu_ops, nop) {
    bots::Cpu cpu;
    auto op_nop = bots_cpu_oplist[0];

    cr_assert(op_nop(cpu, 0, 0, 0, 0, 0) == true);
}
